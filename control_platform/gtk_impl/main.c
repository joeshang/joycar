/**
 * File: main.c
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The main program of Joycar Control Platform.
 */

#include <gtk/gtk.h>

#include "connector.h"
#include "connector_net.h"
#include "connector_serial.h"
#include "callbacks_con.h"

#define GLADE_FILE  "gui/joycar.glade"

Connector *serial_connector = NULL;
Connector *net_connector = NULL;

int main(int argc, char *argv[])
{
    GtkBuilder *builder;
    GtkWidget *main_window;
    EventListener con_listener;

    gtk_init(&argc, &argv);

    /* load glade gui file and connect signal-callbacks */
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, GLADE_FILE, NULL);
    gtk_builder_connect_signals(builder, builder);

    /* initialize connectors */
    serial_connector = connector_serial_create();
    net_connector = connector_net_create();
    /* open listener */
    con_listener.cb_func = connector_open_callback;
    con_listener.ctx = builder;
    connector_event_listener_set(serial_connector, EVENT_TYPE_OPEN, &con_listener);
    connector_event_listener_set(net_connector, EVENT_TYPE_OPEN, &con_listener);
    /* read listener */

    /* show the main window */
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show(main_window);

    gtk_main();

    return 0;
}
