#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "command_parser.h"

extern int construct_command(char c, char *command_buf);

int main(int argc, char *argv[])
{
    int c;

    while ((c = getchar()) != EOF)
    {
        command_len = construct_command(c, command_buf);

        if (command_len)
        {
            parse_command(command_buf, command_len);
            command_len = 0;
        }
    }

    return 0;
}
