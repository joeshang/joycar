/**
 * @File: connector.h
 * @Author: Joe Shang
 * @Brief: The commnunication module of JoyCar Control Platform.
 */

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "typedef.h"

DECLS_BEGIN

typedef enum _EventType
{
    EVENT_TYPE_OPEN = 0,
    EVENT_TYPE_CLOSE,
    EVENT_TYPE_READ,
    EVENT_TYPE_END
}EventType;

struct _Connector;
typedef struct _Connector Connector;

typedef void (*CallBackFuncPtr)(void *data, void *ctx);
typedef Ret (*ConnectorOpen)(Connector *thiz, void *arg);
typedef Ret (*ConnectorClose)(Connector *thiz);
typedef void (*ConnectorSend)(Connector *thiz, void *buf, size_t size);
typedef void (*ConnectorDestroy)(Connector *thiz);

typedef struct _EventListener
{
    CallBackFuncPtr cb_func;
    void *ctx;
}EventListener;


struct _Connector
{
    ConnectorOpen open;
    ConnectorClose close;
    ConnectorSend send;
    ConnectorDestroy destroy;

    EventListener open_listener;
    EventListener close_listener;
    EventListener read_listener;

    char priv[];
};

static inline Ret connector_open(Connector *thiz, void *arg)
{
    return_val_if_fail(thiz != NULL && thiz->open != NULL, RET_INVALID_PARAMS);

    return (thiz->open(thiz, arg));
}

static inline Ret connector_close(Connector *thiz)
{
    return_val_if_fail(thiz != NULL && thiz->close != NULL, RET_INVALID_PARAMS);

    return (thiz->close(thiz));
}

static inline Ret connector_send(Connector *thiz, void *buf, size_t size)
{
    return_val_if_fail(thiz != NULL && thiz->send != NULL, RET_INVALID_PARAMS);

    thiz->send(thiz, buf, size);

    return RET_OK;
}

static inline Ret connector_destroy(Connector *thiz)
{
    return_val_if_fail(thiz != NULL && thiz->destroy != NULL, RET_INVALID_PARAMS);

    thiz->destroy(thiz);

    return RET_OK;
}

static inline Ret connector_event_listener_set(Connector *thiz, EventType event, EventListener *listener)
{
    Ret ret;
    return_val_if_fail(thiz != NULL && event < EVENT_TYPE_END, RET_INVALID_PARAMS);
    
    switch (event)
    {
        case EVENT_TYPE_OPEN:
            thiz->open_listener.cb_func = listener->cb_func;
            thiz->open_listener.ctx = listener->ctx;
            ret = RET_OK;
            break;
        case EVENT_TYPE_CLOSE:
            thiz->close_listener.cb_func = listener->cb_func;
            thiz->close_listener.ctx = listener->ctx;
            ret = RET_OK;
            break;
        case EVENT_TYPE_READ:
            thiz->read_listener.cb_func = listener->cb_func;
            thiz->read_listener.ctx = listener->ctx;
            ret = RET_OK;
            break;
        default:
            ret = RET_INVALID_PARAMS;
            break;
    }

    return ret;
}

DECLS_END

#endif
