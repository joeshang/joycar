/**
 * File: callbacks_con.c
 * Author: Joe Shang
 * Brief: The callback handlers of communication programs.
 */

#include <gtk/gtk.h>
#include <stdio.h>

#include "callbacks_con.h"

void connector_open_callback(void *data, void *ctx)
{
    GtkBuilder *builder = (GtkBuilder *)ctx;
    GtkWidget *connect_btn;
    GtkWidget *disconnect_btn;
    GtkImage *status_img;

    printf("connector open callback:\n");

    if (data == NULL) /* open success */
    {
        fprintf(stderr, "open success\n");

        disconnect_btn = GTK_WIDGET(gtk_builder_get_object(builder, "disconnect_btn"));
        gtk_widget_set_sensitive(disconnect_btn, TRUE);

        connect_btn = GTK_WIDGET(gtk_builder_get_object(builder, "connect_btn"));
        gtk_widget_set_sensitive(connect_btn, FALSE);

        status_img = GTK_IMAGE(gtk_builder_get_object(builder, "connect_status_img"));
        //gtk_image_set_from_file(status_img, "res/circle_green.png");
        gtk_image_set_from_icon_name(status_img, "dialog-ok", GTK_ICON_SIZE_MENU);
    }
    else
    {
        /* TODO: handle open failed with error message in data */
        fprintf(stderr, "%s\n", (char *)data);
    }
}
