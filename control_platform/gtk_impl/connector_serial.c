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

    int connect_status;
}PrivInfo;

static Ret connector_serial_open(Connector *thiz, void *arg, CallbackFunc cb_func, void *ctx)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;
    SerialArg *serial_arg = (SerialArg *)arg;

    return_val_if_fail(cb_func != NULL, RET_INVALID_PARAMS);

    priv->baud_rate = serial_arg->baud_rate;
    if (priv->serial_port != NULL)
    {
        free(priv->serial_port);
    }
    priv->serial_port = (char *)malloc(strlen(serial_arg->serial_port) + 1);
    strcpy(priv->serial_port, serial_arg->serial_port);

    printf("open serial: %s\n", priv->serial_port);
    if ((priv->fd = open(priv->serial_port, O_RDWR | O_NOCTTY)) == -1)
    {
        cb_func(strerror(errno), ctx);
        
        perror("open serial fail");

        return RET_FAIL;
    }

    serial_set_raw_mode(priv->fd);
    serial_set_speed(priv->fd, priv->baud_rate);
    serial_set_parity(priv->fd, DATA_BITS, PARITY_TYPE, STOP_BITS);

    priv->connect_status = 1;

    cb_func(NULL, ctx);
    
    return RET_OK;
}

static Ret connector_serial_close(Connector *thiz, CallbackFunc cb_func, void *ctx)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    return_val_if_fail(cb_func != NULL, RET_INVALID_PARAMS);

    priv->connect_status = 0;
    close(priv->fd); 
    
    cb_func(NULL, ctx);

    return RET_OK;
}

static int connector_serial_send(Connector *thiz, void *buf, size_t size)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    if (priv->connect_status)
    {
        return write(priv->fd, buf, size);
    }

    return -1;
}

static int connector_serial_recv(Connector *thiz, void *buf, size_t size)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    if (priv->connect_status)
    {
        return read(priv->fd, buf, size);
    }

    return -1;
}

static void connector_serial_destroy(Connector *thiz)
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
        priv->connect_status = 0;

        thiz->open = connector_serial_open;
        thiz->close = connector_serial_close;
        thiz->send = connector_serial_send;
        thiz->recv = connector_serial_recv;
        thiz->destroy = connector_serial_destroy;
    }

    return thiz;
}

