/**
 * File: main.c
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The main program of Joycar Control Platform.
 */

#include <gtk/gtk.h>
#include <string.h>

#include "decoder.h"
#include "decoder_mjpeg.h"
#include "decoder_yuv422.h"
#include "connector.h"

#define GLADE_FILE  "gui/joycar.glade"

Decoder *frame_decoder = NULL;
Connector *active_connector = NULL;

int main(int argc, char *argv[])
{
    GtkBuilder *builder;
    GtkWidget *main_window;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s format\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* initialize frame decoder */
    if (strcmp(argv[1], "yuyv") == 0)
    {
        frame_decoder = decoder_yuv422_create();
    }
    else if (strcmp(argv[1], "mjpeg") == 0)
    {
        frame_decoder = decoder_mjpeg_create();
    }
    else
    {
        fprintf(stderr, "unsupport format, support yuyv and mjpeg\n");
        exit(EXIT_FAILURE);
    }

    if (!g_thread_supported())
    {
        g_thread_init(NULL);
    }
    gdk_threads_init();
    gdk_threads_enter();

    gtk_init(&argc, &argv);

    /* load glade gui file and connect signal-callbacks */
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, GLADE_FILE, NULL);
    gtk_builder_connect_signals(builder, builder);

    /* show the main window */
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show(main_window);

    gtk_main();
    gdk_threads_leave();

    return 0;
}
