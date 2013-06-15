/**
 * File: key_control.h
 * Author: Joe Shang
 * Brief: The key(joystick + normal key) control module.
 */

#ifndef _KEY_CONTROL_H_
#define _KEY_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _KeyControlInfo
{
    int socket_fd;
    int joystick_fd;
    //int buttons_fd;
}KeyControlInfo;

void *key_control_thread(void *user_data);

#ifdef __cplusplus
}
#endif

#endif
