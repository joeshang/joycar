/**
 * File: main.c
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The main program of Joycar Control Platform.
 */

#include <gtk/gtk.h>

#define GLADE_FILE  "gui/joycar.glade"

int main(int argc, char *argv[])
{
    GtkBuilder *builder;
    GtkWidget *main_window;

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
