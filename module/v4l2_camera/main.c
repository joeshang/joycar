/**
 * File: client.c
 * Author: Joe Shang
 * Brief:
 */

#include <stdio.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <cairo.h>

#include "capture.h"
#include "yuv422_rgb.h"

#define DISP_WIDTH      DEV_WIDTH * 2
#define DISP_HEIGHT     DEV_HEIGHT * 2
#define WIN_WIDTH       DISP_WIDTH
#define WIN_HEIGHT      DISP_HEIGHT

unsigned char rgb_buf[DEV_WIDTH * DEV_HEIGHT * 3];

static void process_image(void *ctx, void *buf_start, int buf_size)
{
    cairo_t *cr;
    GtkWidget *drawing_area = (GtkWidget *)ctx;
    
    yuv422_rgb24(buf_start, rgb_buf, DEV_WIDTH, DEV_HEIGHT);

    cr = gdk_cairo_create(drawing_area->window);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((const guchar *)rgb_buf, 
            GDK_COLORSPACE_RGB, 
            FALSE,
            8,
            DEV_WIDTH,
            DEV_HEIGHT,
            DEV_WIDTH * 3,
            NULL,
            NULL);
    if (pixbuf != NULL)
    {
        GdkPixbuf *scale_pixbuf = gdk_pixbuf_scale_simple(pixbuf,
                DISP_WIDTH,
                DISP_HEIGHT,
                GDK_INTERP_BILINEAR);
        gdk_cairo_set_source_pixbuf(cr, scale_pixbuf, 0, 0);
        cairo_paint(cr);

        g_object_unref(pixbuf);
        g_object_unref(scale_pixbuf);
    }

    cairo_destroy(cr);
}

gboolean refresh_ui(gpointer user_data)
{
    if (video_is_read_ready())
    {
        video_read_frame(user_data, process_image);
    }

    return TRUE;
}

void destroy_handler(GtkWidget *widget, gpointer data)
{
    printf("enter destroy handler\n");

    video_stop_capture();
    video_deinit_device();
    video_close_device();

    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *drawing_area;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window, WIN_WIDTH, WIN_HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "Camera");

    g_signal_connect(G_OBJECT(window), "destroy",
            G_CALLBACK(destroy_handler), NULL);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    drawing_area = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);
    gtk_widget_set_size_request(drawing_area, DISP_WIDTH, DISP_HEIGHT);

    gtk_widget_show_all(window);

    video_open_device();
    video_init_device();
    video_start_capture();

    gtk_idle_add((GSourceFunc)refresh_ui, drawing_area);

    gtk_main();

    return 0;
}
