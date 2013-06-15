/**
 * File: key_control.c
 * Author: Joe Shang
 * Brief: The key(joystick + normal key) control module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "key_control.h"

#define STICK_INIT_VALUE    500
#define STICK_STANDBY_MIN   200
#define STICK_STANDBY_MAX   800

enum _JoystickState
{
    STICK_STATE_STANDBY,
    STICK_STATE_MOVE
};

enum _JoystickChange
{
    STICK_CHANGE_INC,
    STICK_CHANGE_DEC,
    STICK_CHANGE_MID
};

static const char *joystick_command_table[4][3] = 
{
    { "hu", "hd", "ht"},    /* channel 0: holder up/down */
    { "mf", "mb", "mt"},    /* channel 1: motor forward/backward */
    { "ml", "mr", "mt"},    /* channel 2: motor left/right */
    { "hl", "hr", "ht"}     /* channel 3: holder left/right */
};

void *key_control_thread(void *user_data)
{
    int i;
    int len;
    const char *command;
    char read_buf[30];
    char send_buf[30];
    int joystick_state[4];
    int joystick_value[4];

    KeyControlInfo *info = (KeyControlInfo *)user_data;
    int socket_fd = info->socket_fd;
    int joystick_fd = info->joystick_fd;
    //int buttons_fd = info->buttons_fd;

    pthread_detach(pthread_self());

    /* initialize joystick related infomations */
    for (i = 0; i < 4; i++)
    {
        joystick_value[i] = STICK_INIT_VALUE;
        joystick_state[i] = STICK_STATE_STANDBY;
    }

    for (;;)
    {
        /* read joystick value -- 4 channel(0-3) adc value */
        for (i = 0; i < 4; i++)
        {
            ioctl(joystick_fd, 0xc000fa01, i); 
            if ((len = read(joystick_fd, read_buf, sizeof(read_buf) - 1)) < 0)
            {
                perror("read adc device(joystick) failed");
                exit(EXIT_FAILURE);
            }

            read_buf[len] = '\0';
            sscanf(read_buf, "%d", &joystick_value[i]);

            command = NULL;

            if (joystick_state[i] == STICK_STATE_STANDBY)
            {
                if (joystick_value[i] > STICK_STANDBY_MAX)
                {
                    joystick_state[i] = STICK_STATE_MOVE;
                    command = joystick_command_table[i][STICK_CHANGE_INC];
                }
                else if (joystick_value[i] < STICK_STANDBY_MIN)
                {
                    joystick_state[i] = STICK_STATE_MOVE;
                    command = joystick_command_table[i][STICK_CHANGE_DEC];
                }
            }
            else if (joystick_state[i] == STICK_STATE_MOVE)
            {
                if (joystick_value[i] >= STICK_STANDBY_MIN
                    && joystick_value[i] <= STICK_STANDBY_MAX)
                {
                    joystick_state[i] = STICK_STATE_STANDBY;
                    command = joystick_command_table[i][STICK_CHANGE_MID];
                }
            }

            if (command != NULL)
            {
                printf("[%d] state:%d vlaue;%d\n", i, joystick_state[i], joystick_value[i]);
                sprintf(send_buf, "[%s]", command);

                printf("command: %s\n", send_buf);
                send(socket_fd, send_buf, strlen(send_buf), 0);
            }

            //usleep(500);
        }

        /* read normal buttons value */
    }

    return NULL;
}
