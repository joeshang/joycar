/**
 * File: main.c
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The main program of Joycar Control Platform.
 */

#include <gtk/gtk.h>

#include "connector.h"
#include "connector_serial.h"
#include "con_callbacks.h"

Connector *con;

int main(int argc, char *argv[])
{
    GtkBuilder *builder;
    GtkWidget *main_window;

    EventListener con_listener;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "joycar_serial.glade", NULL);
    gtk_builder_connect_signals(builder, builder);

    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    
    con = connector_serial_create();

    con_listener.cb_func = connector_open_callback;
    con_listener.ctx = builder;
    connector_event_listener_set(con, EVENT_TYPE_OPEN, &con_listener);

    gtk_widget_show(main_window);

    gtk_main();

    return 0;
}
