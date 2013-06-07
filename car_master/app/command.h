/**
 * File: command.h
 * Author: Joe Shang
 * Brief: command related functions.
 */

#ifndef _COMMAND_H_
#define _COMMAND_H_

enum _CommandStatus
{
    COMMAND_OK,
    COMMAND_PASS,
    COMMAND_ERR
};

typedef void (*CommandCallbackFunc)(int status, void *command, void *ctx);

void extract_command(const char *string, 
        CommandCallbackFunc cb_func,
        void *ctx);
int parse_command(char *buf, int command_len);

#endif
