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
    char *arg;

    /* Buffer should not be NULL and command length should not be 0 */
    if (buf == NULL || command_len == 0)
    {
        return COMMAND_ERR;
    }

    switch (buf[CLASS_POS])
    {
        case 'm':   /* motor related */
            if (TYPE_POS <= command_len - 1)
            {
                printf("motor: ");
                switch (buf[TYPE_POS])
                {
                    case 'f':   /* Move farward */
                        printf("forward\n");
                        break;
                    case 'b':   /* Move backward */
                        printf("backward\n");
                        break;
                    case 'l':   /* Turn left */
                        printf("turn left\n");
                        break;
                    case 'r':   /* Turn right */
                        printf("turn right\n");
                        break;
                    case 's':   /* Set Speed */
                        if (buf[ARG_POS] != 0)
                        {
                            arg = buf + ARG_POS;
                            printf("set speed %s\n", arg);
                        }
                        else
                        {
                            ret = COMMAND_ERR;
                            printf("no speed argument\n");
                        }
                        break;
                    case 't':   /* Stop */
                        printf("stop\n");
                        break;
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
        case 'h':   /* holder related */
            if (TYPE_POS <= command_len - 1)
            {
                printf("holder: ");
                switch (buf[TYPE_POS])
                {
                    case 'u':   /* Turn up */
                        printf("turn up\n");
                        break;
                    case 'd':   /* Turn down */
                        printf("turn down\n");
                        break;
                    case 'l':   /* Turn left */
                        printf("turn left\n");
                        break;
                    case 'r':   /* Turn right */
                        printf("turn right\n");
                        break;
                    case 's':   /* Set Speed */
                        if (buf[ARG_POS] != 0)
                        {
                            arg = buf + ARG_POS;
                            printf("set speed %s\n", arg);
                        }
                        else
                        {
                            ret = COMMAND_ERR;
                            printf("no speed argument\n");
                        }
                        break;
                    case 't':   /* Stop */
                        printf("stop\n");
                        break;
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
