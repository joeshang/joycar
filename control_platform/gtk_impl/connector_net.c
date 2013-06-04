/**
 * @File: connector_net.c
 * @Author: Joe Shang
 * @Brief: The network verison communication module.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "connector.h"
#include "connector_net.h"

typedef struct _PrivInfo
{
    int socket;
    struct sockaddr_in serv_addr;

    int connect_status;
}PrivInfo;

static Ret connector_net_open(Connector *thiz, void *arg, CallbackFunc cb_func, void *ctx)
{
    NetworkArg *net_arg = (NetworkArg *)arg;
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    return_val_if_fail(cb_func != NULL, RET_INVALID_PARAMS);

    if ((priv->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cb_func(strerror(errno), ctx);

        perror("socket failed");
        return RET_FAIL;
    }

    priv->serv_addr.sin_family = AF_INET;
    priv->serv_addr.sin_port = htons(net_arg->net_port);
    inet_pton(AF_INET, net_arg->net_addr, &priv->serv_addr.sin_addr);

    if (connect(priv->socket, (struct sockaddr *)&priv->serv_addr, sizeof(priv->serv_addr)) == -1)
    {
        cb_func(strerror(errno), ctx);

        perror("connect failed");
        return RET_FAIL;
    }

    priv->connect_status = 1;

    cb_func(NULL, ctx);

    return RET_OK;
}

static Ret connector_net_close(Connector *thiz, CallbackFunc cb_func, void *ctx)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    return_val_if_fail(cb_func != NULL, RET_INVALID_PARAMS);

    priv->connect_status = 0;
    close(priv->socket);

    cb_func(NULL, ctx);

    return RET_OK;
}

static int connector_net_send(Connector *thiz, void *buf, size_t size)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    if (priv->connect_status)
    {
        return send(priv->socket, buf, size, 0);
    }

    return -1;
}

static int connector_net_recv(Connector *thiz, void *buf, size_t size)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    if (priv->connect_status)
    {
        return recv(priv->socket, buf, size, 0);
    }

    return -1;
}

static void connector_net_destroy(Connector *thiz)
{
    free(thiz);
}

Connector *connector_net_create()
{
    Connector *thiz = (Connector *)malloc(sizeof(Connector) + sizeof(PrivInfo));

    if (thiz != NULL)
    {
        PrivInfo *priv = (PrivInfo *)thiz->priv;
        priv->connect_status = 0;
        
        thiz->open = connector_net_open;
        thiz->close = connector_net_close;
        thiz->send = connector_net_send;
        thiz->recv = connector_net_recv;
        thiz->destroy = connector_net_destroy;
    }

    return thiz;
}
