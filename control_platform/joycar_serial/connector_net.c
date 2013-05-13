/**
 * @File: connector_net.c
 * @Author: Joe Shang
 * @Brief: The network verison communication module.
 */

#include <stdio.h>

#include "connector.h"
#include "connector_net.h"

typedef struct _PrivInfo
{
    int fd;
}PrivInfo;

static Ret connector_net_open(Connector *thiz, void *arg)
{
    return RET_OK;
}

static Ret connector_net_close(Connector *thiz)
{
    return RET_OK;
}

static void connector_net_send(Connector *thiz, void *buf, size_t size)
{
}

static void connector_net_destroy(Connector *thiz)
{
}

Connector *connector_net_create()
{
    Connector *thiz = (Connector *)malloc(sizeof(Connector) + sizeof(PrivInfo));

    if (thiz != NULL)
    {
        //PrivInfo *priv = (PrivInfo *)thiz->priv;
        
        thiz->open = connector_net_open;
        thiz->close = connector_net_close;
        thiz->send = connector_net_send;
        thiz->destroy = connector_net_destroy;
    }

    return thiz;
}
