#include <stdio.h>
#include <stdlib.h>

#include "command.h"

#define BUF_SIZE    512

static void command_callback(int status, void *command, void *ctx)
{
    if (status == COMMAND_PASS)
    {
        fprintf(stdout, "pass command: %s\n", (char *)command);
    }
}

int main(int argc, char *argv[])
{
    char buf[BUF_SIZE];

    while (fgets(buf, BUF_SIZE, stdin) != NULL)
    {
        extract_command(buf, command_callback, NULL);
    }

    return 0;
}
