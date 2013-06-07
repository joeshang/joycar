/*
 * File: command.h
 * Author: Joe Shang
 * Description: The command constructor and parser program.
 */

#ifndef _COMMAND_H_
#define _COMMAND_H_

#define COMMAND_BUF_SIZE	30

extern int command_len;
extern char command[COMMAND_BUF_SIZE];

int construct_command(char c, char *command_buf);
int parse_command(char *buf, int command_len);

#endif
