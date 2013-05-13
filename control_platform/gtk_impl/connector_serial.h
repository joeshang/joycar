/**
 * @File: connector_serial.h
 * @Author: Joe Shang
 * @Brief: The serial version of communication module.
 */

#ifndef _CONNECTOR_SERIAL_H_
#define _CONNECTOR_SERIAL_H_

DECLS_BEGIN

typedef struct _SerialArg
{
    int baud_rate;
    char *serial_port;
}SerialArg;

Connector *connector_serial_create();

DECLS_END

#endif
