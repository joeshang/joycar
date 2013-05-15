/**
 * @File: Connector.c
 * @Author: Joe Shang
 * @Brief: The communication module of JoyCar Control Platform.
 */

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "connector.h"
#include "serial_driver.h"
#include "connector_serial.h"

#define DATA_BITS       8
#define STOP_BITS       1
#define PARITY_TYPE     'N'

#define PORT_NAME_MAX   100

typedef struct _PrivInfo
{
    int fd;
    int baud_rate;
    char *serial_port;

    int status;
}PrivInfo;

static pthread_t serial_pid;

static void *serial_thread_handler(void *arg)
{
    Connector *thiz = (Connector *)arg;
    EventListener listener = thiz->open_listener;
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    if ((priv->fd = open(priv->serial_port, O_RDWR | O_NOCTTY)) == -1)
    {
        if (listener.cb_func != NULL)
        {
            listener.cb_func(strerror(errno), listener.ctx);
        }
        perror("open serial fail");
    }

    serial_set_raw_mode(priv->fd);
    serial_set_speed(priv->fd, priv->baud_rate);
    serial_set_parity(priv->fd, DATA_BITS, PARITY_TYPE, STOP_BITS);

    priv->status = 1;

    if (listener.cb_func != NULL)
    {
        listener.cb_func(NULL, listener.ctx);
    }

    pthread_exit(NULL);
}

Ret connector_serial_open(Connector *thiz, void *arg)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;
    SerialArg *serial_arg = (SerialArg *)arg;

    priv->baud_rate = serial_arg->baud_rate;
    if (priv->serial_port != NULL)
    {
        free(priv->serial_port);
    }
    priv->serial_port = (char *)malloc(strlen(serial_arg->serial_port) + 1);
    strcpy(priv->serial_port, serial_arg->serial_port);

    pthread_create(&serial_pid, NULL, serial_thread_handler, (void *)thiz);
    
    return RET_OK;
}

Ret connector_serial_close(Connector *thiz)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;
    priv->status = 0;
    close(priv->fd); 

    return RET_OK;
}

void connector_serial_send(Connector *thiz, void *buf, size_t size)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    if (priv->status)
    {
        write(priv->fd, buf, size);
    }
}

void connector_serial_destroy(Connector *thiz)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;
    free(priv->serial_port);
    free(thiz);
}

Connector *connector_serial_create()
{
    Connector *thiz =  (Connector *)malloc(sizeof(Connector) + sizeof(PrivInfo));

    if (thiz != NULL)
    {
        PrivInfo *priv = (PrivInfo *)thiz->priv;
        priv->serial_port = NULL;
        priv->baud_rate = 9600;
        priv->status = 0;

        thiz->open = connector_serial_open;
        thiz->close = connector_serial_close;
        thiz->send = connector_serial_send;
        thiz->destroy = connector_serial_destroy;
    }

    return thiz;
}

