/*
 * File: command_parser.c
 * Author: Joe Shang
 * Description: The command parser program.
 */

#include <stdio.h>
#include "command.h"

enum
{
    CLASS_POS = 1,
    TYPE_POS,
    ARG_POS
};

int parse_command(char *buf, int command_len)
{
    int ret = COMMAND_OK;

    /* Buffer should not be NULL and command length should not be 0 */
    if (buf == NULL || command_len == 0)
    {
        return COMMAND_ERR;
    }

    switch (buf[CLASS_POS])
    {
        case 'm':   /* motor related */
        case 'h':   /* holder related */
            ret = COMMAND_PASS;
            break;
        case 's':   /* sensor related */
            if (TYPE_POS <= command_len - 1)
            {
                printf("sensor: ");
                switch (buf[TYPE_POS])
                {
                    default:
                        ret = COMMAND_ERR;
                        printf("unknown command: %s\n", buf);
                        break;
                }
            }
            else
            {
                ret = COMMAND_ERR;
                printf("too less command length\n");
            }

            break;
        case 'f':   /* function related */
            if (TYPE_POS <= command_len - 1)
            {
                printf("functional: ");
                switch (buf[TYPE_POS])
                {
                    default:
                        ret = COMMAND_ERR;
                        printf("unknown command: %s\n", buf);
                        break;
                }
            }
            else
            {
                ret = COMMAND_ERR;
                printf("too less command length\n");
            }

            break;
        default:
            ret = COMMAND_ERR;
            printf("unknown command: %s\n", buf);
            break;
    }

    return ret;
}
