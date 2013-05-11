#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connector.h"
#include "connector_serial.h"

static void open_listener_cb(void *data, void *ctx)
{
    printf("open callback\n");
}

int main(int argc, char *argv[])
{
    int c;
    char *msg_buf;
    Connector *ser_con;
    EventListener open_listener;
    EventListener read_listener;
    SerialArg ser_arg;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s serial_port\n", argv[0]);
        exit(1);
    }

    ser_con = connector_serial_create();

    open_listener.cb_func = open_listener_cb;
    open_listener.ctx = NULL;
    connector_event_listener_set(ser_con, EVENT_TYPE_OPEN, &open_listener);

    read_listener.cb_func = NULL;
    read_listener.ctx = NULL;
    connector_event_listener_set(ser_con, EVENT_TYPE_READ, &read_listener);

    ser_arg.baud_rate = 9600;
    ser_arg.serial_port = argv[1];
    connector_open(ser_con, &ser_arg);

    msg_buf = "[ml]";
    connector_send(ser_con, msg_buf, strlen(msg_buf));

    while ((c = getchar()) != EOF)
    {
        connector_send(ser_con, &c, 1);
    }

    connector_destroy(ser_con);

    return 0;
}
