/**
 * @File: connector_net.h
 * @Author: Joe Shang
 * @Brief: The network verison communication module.
 */

#ifndef _CONNECTOR_NET_H_
#define _CONNECTOR_NET_H_

DECLS_BEGIN

typedef struct _NetworkArg
{
    int net_port;
    char *net_addr;
}NetworkArg;

Connector *connector_net_create();

DECLS_END

#endif
