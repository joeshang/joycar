#include "header.h"

enum
{
    OUT_COMMAND = 0,
    IN_COMMAND
}state = OUT_COMMAND;

static int input_index;
static char input_buf[BUF_SIZE];

int construct_command(char c)
{
    int i;

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
            command_len = input_index;
            for (i = 0; i < input_index; i++)
            {
                command_buf[i] = input_buf[i];
            }
            command_buf[input_index] = 0;

            input_index = 0;
            state = OUT_COMMAND;

            return 1;
        }
        else
        {
            input_buf[input_index++] = c;
        }
    }

    return 0;
}

