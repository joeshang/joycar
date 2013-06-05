/**
 * File: construct_command.c
 * Author: Joe Shang
 * Brief: Extract real command from command protocol string used for interrupt
 *  handler(Do as less work as possible in interrupt handler).
 */

#include "header.h"

enum
{
    OUT_COMMAND = 0,
    IN_COMMAND
}state = OUT_COMMAND;

static int input_index;
static char input_buf[BUF_SIZE];

int construct_command(char c, char *command_buf)
{
    int i;
    int ret = 0;

    if (state == OUT_COMMAND)
    {
        if (c == '[')
        {
            input_index = 0;
            state = IN_COMMAND;
        }
    }
    else if (state == IN_COMMAND)
    {
        if (c == ']')
        {
            ret = input_index;
            for (i = 0; i < input_index; i++)
            {
                command_buf[i] = input_buf[i];
            }
            command_buf[input_index] = 0;

            input_index = 0;
            state = OUT_COMMAND;
        }
        else
        {
            input_buf[input_index] = c;
            if (input_index < BUF_SIZE - 1)
            {
                input_index++;
            }
        }
    }

    return ret;
}

