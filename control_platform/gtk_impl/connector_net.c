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
#include <pthread.h>

#include "connector.h"
#include "connector_net.h"

typedef struct _PrivInfo
{
    int socket;
    struct sockaddr_in serv_addr;
}PrivInfo;

static pthread_t net_pid;

static void *net_thread_handler(void *arg)
{
    Connector *thiz = (Connector *)arg;
    EventListener listener = thiz->open_listener;
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    if (connect(priv->socket, (struct sockaddr *)&priv->serv_addr, sizeof(priv->serv_addr)) == -1)
    {
        listener.cb_func(strerror(errno), listener.ctx);

        perror("connect failed");
        pthread_exit(NULL);
    }

    listener.cb_func(NULL, listener.ctx);
    
    pthread_exit(NULL);
}

static Ret connector_net_open(Connector *thiz, void *arg)
{
    NetworkArg *net_arg = (NetworkArg *)arg;
    PrivInfo *priv = (PrivInfo *)thiz->priv;

    if ((priv->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    priv->serv_addr.sin_family = AF_INET;
    priv->serv_addr.sin_port = htons(net_arg->net_port);
    inet_pton(AF_INET, net_arg->net_addr, &priv->serv_addr.sin_addr);

    pthread_create(&net_pid, NULL, net_thread_handler, (void *)thiz);

    return RET_OK;
}

static Ret connector_net_close(Connector *thiz)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;
    close(priv->socket);
    return RET_OK;
}

static void connector_net_send(Connector *thiz, void *buf, size_t size)
{
    PrivInfo *priv = (PrivInfo *)thiz->priv;
    send(priv->socket, buf, size, 0);
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

        connector_event_init(thiz);
    }

    return thiz;
}
