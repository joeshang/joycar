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

void extract_command(const char *string)
{
    const char *p;

    for (p = string; *p != '\0'; p++)
    {
        if (state == OUT_COMMAND)
        {
            if (*p == '[')
            {
                index = 0;
                state = IN_COMMAND;
            }
        }
        else if (state == IN_COMMAND)
        {
            if (*p == ']')
            {
                command_buf[index] = 0;

                parse_command(command_buf, index);

                index = 0;
                state = OUT_COMMAND;
            }
            else
            {
                command_buf[index] = *p;
                if (index < BUF_SIZE - 1)
                {
                    index++;
                }
            }
        }
    }
}
