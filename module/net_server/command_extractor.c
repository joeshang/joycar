/**
 * File: command_extractor.c
 * Author: Joe Shang
 * Brief: Extract real command from command protocol string.
 */

#include "command.h"

#define BUF_SIZE    256

enum
{
    OUT_COMMAND = 0,
    IN_COMMAND
}state = OUT_COMMAND;

static int index;
static char command_buf[BUF_SIZE];

void extract_command(const char *string, CommandCallbackFunc cb_func, void *ctx)
{
    int parse_status;
    const char *p;

    for (p = string; *p != '\0'; p++)
    {
        if (state == OUT_COMMAND)
        {
            if (*p == '[')
            {
                state = IN_COMMAND;

                index = 0;
                command_buf[index++] = *p;
            }
        }
        else if (state == IN_COMMAND)
        {
            command_buf[index] = *p;
            if (index < BUF_SIZE - 1)
            {
                index++;
            }

            if (*p == ']')
            {
                state = OUT_COMMAND;
                command_buf[index] = 0;

                parse_status = parse_command(command_buf, index);
                cb_func(parse_status, command_buf, ctx);

                index = 0;
            }
        }
    }
}
