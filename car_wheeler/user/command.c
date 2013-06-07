/*
 * File: command.c
 * Author: Joe Shang
 * Description: The command constructor and parser program.
 */

#include <stdio.h>
#include "command.h"
#include "config.h"
#include "usart.h"
#include "motor.h"
#include "holder.h"

/* Command syntax position */
enum
{
    CLASS_POS = 0,
    TYPE_POS,
    ARG_POS
};

/* Command divide state */
enum
{
    OUT_COMMAND = 0,
    IN_COMMAND
}state = OUT_COMMAND;

int command_len = 0;
char command[COMMAND_BUF_SIZE];

/* Input index and buffer used for interrupt */
static int input_index;
static char input_buf[COMMAND_BUF_SIZE];

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

			if (input_index < COMMAND_BUF_SIZE - 1)
			{
				input_index++;
			}
        }
    }

    return ret;
}


int parse_command(char *buf, int command_len)
{
    int ret = 0;
    char *arg;

    /* buffer should not be NULL and command length should not be 0 */
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
                    case 'f':   /* move farward */
						Motor_Forward();
                        break;
                    case 'b':   /* move backward */
						Motor_Backward();
                        break;
                    case 'l':   /* turn left */
                        Motor_TurnLeft();
                        break;
                    case 'r':   /* turn right */
                        Motor_TurnRight();
                        break;
                    case 's':   /* set speed */
                        if (buf[ARG_POS] != 0)
                        {
                            arg = buf + ARG_POS;
                            Motor_SetSpeed(arg);
                        }
                        else
                        {
                            ret = -1;
                        #ifdef DEBUG
                            printf("no speed argument\n");
                        #endif
                        }
                        break;
                    case 't':   /* stop */
                        Motor_Stop();
                        break;
                    default:
                        ret = -1;
					#ifdef DEBUG
                        printf("unknown command: %s\n", buf);
					#endif
                        break;
                }
            }
            else
            {
                ret = -1;
			#ifdef DEBUG
                printf("too less command length\n");
			#endif
            }
            break;
        case 'h':   /* holder related */
            if (TYPE_POS <= command_len - 1)
            {
                switch (buf[TYPE_POS])
                {
                    case 'u':   /* turn up */
                        Holder_TurnUp();
                        break;
                    case 'd':   /* turn down */
                        Holder_TurnDown();
                        break;
                    case 'l':   /* turn left */
                        Holder_TurnLeft();
                        break;
                    case 'r':   /* turn right */
                        Holder_TurnRight();
                        break;
                    case 's':   /* set speed */
                        if (buf[ARG_POS] != 0)
                        {
                            arg = buf + ARG_POS;
                            Holder_SetSpeed(arg);
                        }
                        else
                        {
                            ret = -1;
                        #ifdef DEBUG
                            printf("no speed argument\n");
                        #endif
                        }
                        break;
                    case 't':   /* Stop */
                        Holder_Stop();
                        break;
                    default:
                        ret = -1;
					#ifdef DEBUG
                        printf("unknown command: %s\n", buf);
					#endif
                        break;
                }
            }
            else
            {
                ret = -1;
			#ifdef DEBUG
                printf("too less command length\n");
            #endif
			}

            break;
        case 's':   /* sensor related */
            if (TYPE_POS <= command_len - 1)
            {
                switch (buf[TYPE_POS])
                {
                    default:
                        ret = -1;
					#ifdef DEBUG
                        printf("unknown command: %s\n", buf);
                    #endif
						break;
                }
            }
            else
            {
                ret = -1;
			#ifdef DEBUG
                printf("too less command length\n");
            #endif
			}

            break;
        case 'f':   /* function related */
            if (TYPE_POS <= command_len - 1)
            {
                switch (buf[TYPE_POS])
                {
                    default:
                        ret = -1;
					#ifdef DEBUG
                        printf("unknown command: %s\n", buf);
                    #endif
						break;
                }
            }
            else
            {
                ret = -1;
			#ifdef DEBUG
                printf("too less command length\n");
            #endif
			}
            break;
        default:
            ret = -1;
		#ifdef DEBUG
            printf("unknown command: %s\n", buf);
		#endif
            break;
    }

    return ret;
}
