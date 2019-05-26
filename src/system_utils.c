#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/system_utils.h"
#include "../include/types.h"

int already_running()
{
	char *output = NULL;

	exec("ps -C "NAME" | wc -l", &output);

	if (atoi(output) > 2)
		return OK;

	free(output);

	return ERROR;
}

int install()
{
	FILE *y_service = NULL;
	char service_data[] =
		"[Unit]\nDescription="NAME" intelligent thermostat\nAfter=network.target\nStartLimitIntervalSec=0\n\n[Service]\nType=simple\nRestart=always\nRestartSec=1\nExecStart="BIN"\n\n[Install]\nWantedBy=multi-user.target\n";

	if (access(BIN, F_OK) != -1)
		return ALT;

	y_service = fopen("/etc/systemd/system/"NAME".service", "w");
	if (!y_service)
		return ERROR;

	// Create home and move there
	if (mkdir(HOME, S_IRWXU) != 0)
		return ERROR;

	if (mkdir(HOME"/updates", S_IRWXU) != 0)
		return ERROR;

	if (mkdir(HOME"/plugins", S_IRWXU) != 0)
		return ERROR;

	if (rename(NAME, BIN) != 0)
		return ERROR;

	// Make a daemon
	fputs(service_data, y_service);
	fclose(y_service);
	printf("Systemd > %d",
	       system("systemctl daemon-reload; systemctl start "NAME"; systemctl enable "NAME));

	return OK;
}

int exec(char *command, char **output)
{
	int i = 0, stream_size = STD_SIZE;
	char buffer = 0;
	FILE *output_file = NULL;

	(*output) = (char *)calloc(STD_SIZE, sizeof(char));
	if (!(*output)) {
#ifdef DEBUG
		printf("Error allocating memory\n");
#endif
		return ERROR;
	}

	output_file = popen(command, "r");
	if (!output_file) {
#ifdef DEBUG
		printf("Error running command\n");
#endif
		free(*output);
		return ERROR;
	}

	i = 0;
	while ((buffer = fgetc(output_file)) != EOF) {
		(*output)[i] = buffer;
		i++;

		if (i == stream_size - 1) {
			stream_size += STD_SIZE;
			(*output) = (char *)realloc(*output,
						    stream_size * sizeof(char));

			if (!(*output)) {
#ifdef DEBUG
				printf("Error reallocating memory\n");
#endif
				return ERROR;
			}
		}
	}

	pclose(output_file);

	return OK;
}
