#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

/**
 * Checks if its already running
 *
 * Checks for processes called NAME, if there are 2 or
 * more (you + 1) then we stop the program
 *
 * Returns - OK or ERROR
*/
int already_running();

/**
 * Install on the system
 *
 * Installs itself on the system
 *
 * Returns - OK or ERROR
*/
int install();

/**
 * Wrapper for popen
 *
 * Should be able to execute basic commands on the shell. Same as if you used them directly on popen.
 *
 * Command - String with the command to execute
 * Output - String where the output of the command is saved. Memory is allocated inside.
 *
 * Returns - OK or ERROR
*/
int exec(char *command, char **output);

#endif