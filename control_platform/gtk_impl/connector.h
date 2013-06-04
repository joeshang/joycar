/**
 * @File: connector.h
 * @Author: Joe Shang
 * @Brief: The commnunication module of JoyCar Control Platform.
 */

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <stdio.h>

#include "typedef.h"

DECLS_BEGIN

typedef enum _EventType
{
    EVENT_TYPE_OPEN = 0,
    EVENT_TYPE_CLOSE,
    EVENT_TYPE_END
}EventType;

struct _Connector;
typedef struct _Connector Connector;

typedef void (*CallbackFunc)(void *data, void *ctx);

typedef Ret (*ConnectorOpen)(Connector *thiz, void *arg, CallbackFunc cb_func, void *ctx);
typedef Ret (*ConnectorClose)(Connector *thiz, CallbackFunc cb_func, void *ctx);
typedef int (*ConnectorSend)(Connector *thiz, void *buf, size_t size);
typedef int (*ConnectorRecv)(Connector *thiz, void *buf, size_t size);
typedef void (*ConnectorDestroy)(Connector *thiz);

struct _Connector
{
    ConnectorOpen open;
    ConnectorClose close;
    ConnectorSend send;
    ConnectorRecv recv;
    ConnectorDestroy destroy;

    char priv[];
};

static inline Ret connector_open(Connector *thiz, void *arg, CallbackFunc cb_func, void *ctx)
{
    return_val_if_fail(thiz != NULL && thiz->open != NULL, RET_INVALID_PARAMS);

    return (thiz->open(thiz, arg, cb_func, ctx));
}

static inline Ret connector_close(Connector *thiz, CallbackFunc cb_func, void *ctx)
{
    return_val_if_fail(thiz != NULL && thiz->close != NULL, RET_INVALID_PARAMS);

    return (thiz->close(thiz, cb_func, ctx));
}

static inline int connector_send(Connector *thiz, void *buf, size_t size)
{
    return_val_if_fail(thiz != NULL && thiz->send != NULL, -1);

    printf("%s\n", (char *)buf);
    
    return thiz->send(thiz, buf, size);
}

static inline int connector_recv(Connector *thiz, void *buf, size_t size)
{
    return_val_if_fail(thiz != NULL && thiz->recv != NULL, -1);

    return thiz->recv(thiz, buf, size);
}

static inline Ret connector_destroy(Connector *thiz)
{
    return_val_if_fail(thiz != NULL && thiz->destroy != NULL, RET_INVALID_PARAMS);

    thiz->destroy(thiz);

    return RET_OK;
}

DECLS_END

#endif
