#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "command_parser.h"

extern int construct_command(char c);

int main(int argc, char *argv[])
{
    int c;
    int flag = 0;

    while ((c = getchar()) != EOF)
    {
        flag = construct_command(c);

        if (flag)
        {
            flag = 0;
            parse_command(command_buf, command_len);
        }
    }

    return 0;
}
