/**
 * File: command.h
 * Author: Joe Shang
 * Brief: command related functions.
 */

#ifndef _COMMAND_H_
#define _COMMAND_H_

void extract_command(const char *string);
int parse_command(char *buf, int command_len);

#endif
