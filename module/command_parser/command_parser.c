/*
 * File: command_parser.c
 * Author: Joe Shang
 * Description: The command parser program.
 */

#include <stdio.h>

enum
{
    CLASS_POS = 0,
    TYPE_POS
};

int parse_command(char *buf, int command_len)
{
    int ret = 0;

    /* Buffer should not be NULL and command length should not be 0 */
    if (buf == NULL || command_len == 0)
    {
        return -1;
    }

    switch (buf[CLASS_POS])
    {
        case 'm':   /* motor related */
            if (TYPE_POS <= command_len - 1)
            {
                switch (buf[TYPE_POS])
                {
                    case 'f':   /* Move farward */
                        break;
                    case 'b':   /* Move backward */
                        break;
                    case 'l':   /* Turn left */
                        break;
                    case 'r':   /* Turn right */
                        break;
                    case 'u':   /* Speed up */
                        break;
                    case 'd':   /* Speed down */
                        break;
                    case 's':   /* Stop */
                        break;
                    default:
                        ret = -1;
                        printf("unknown command: %s\n", buf);
                        break;
                }
            }
            else
            {
                ret = -1;
                printf("too less command length\n");
            }

            break;
        case 'h':   /* holder related */
            if (TYPE_POS <= command_len - 1)
            {
                switch (buf[TYPE_POS])
                {
                    case 'u':   /* Turn up */
                        break;
                    case 'd':   /* Turn down */
                        break;
                    case 'l':   /* Turn left */
                        break;
                    case 'r':   /* Turn right */
                        break;
                    case 's':   /* Stop */
                        break;
                    default:
                        ret = -1;
                        printf("unknown command: %s\n", buf);
                        break;
                }
            }
            else
            {
                ret = -1;
                printf("too less command length\n");
            }

            break;
        case 's':   /* sensor related */
            if (TYPE_POS <= command_len - 1)
            {
                switch (buf[TYPE_POS])
                {
                    default:
                        ret = -1;
                        printf("unknown command: %s\n", buf);
                        break;
                }
            }
            else
            {
                ret = -1;
                printf("too less command length\n");
            }

            break;
        case 'f':   /* function related */
            if (TYPE_POS <= command_len - 1)
            {
                switch (buf[TYPE_POS])
                {
                    default:
                        ret = -1;
                        printf("unknown command: %s\n", buf);
                        break;
                }
            }
            else
            {
                ret = -1;
                printf("too less command length\n");
            }

            break;
        default:
            ret = -1;
            printf("unknown command: %s\n", buf);
            break;
    }

    return ret;
}
