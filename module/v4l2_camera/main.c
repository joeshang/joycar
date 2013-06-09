/**
 * File: main.c
 * Author: Joe Shang
 * Brief:
 */

#include <gtk/gtk.h>
#include <cairo.h>

#include <stdio.h>
#include <stdlib.h>

#include "capture.h"
#include "yuv422_rgb.h"

#define CAM_WIDTH       320
#define CAM_HEIGHT      240
#define CAM_REQ_BUF_CNT 4
#define DISP_WIDTH      CAM_WIDTH * 2
#define DISP_HEIGHT     CAM_HEIGHT * 2
#define WIN_WIDTH       DISP_WIDTH
#define WIN_HEIGHT      DISP_HEIGHT

CameraDevice *camera;
unsigned char rgb_buf[CAM_WIDTH * CAM_HEIGHT * 3];

static void process_image(void *ctx, void *buf_start, int buf_size)
{
    cairo_t *cr;
    GtkWidget *drawing_area = (GtkWidget *)ctx;

    yuv422_rgb24(buf_start, rgb_buf, CAM_WIDTH, CAM_HEIGHT);

    cr = gdk_cairo_create(gtk_widget_get_window(drawing_area));
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((const guchar *)rgb_buf, 
            GDK_COLORSPACE_RGB, 
            FALSE,
            8,
            CAM_WIDTH,
            CAM_HEIGHT,
            CAM_WIDTH * 3,
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

static gboolean refresh_ui(gpointer user_data)
{
    if (camera_is_read_ready(camera))
    {
        camera_read_frame(camera, process_image, user_data);
    }

    return TRUE;
}

static void destroy_handler(GtkWidget *widget, gpointer data)
{
    printf("enter destroy handler\n");

    camera_stop_capture(camera);
    camera_deinit_device(camera);
    camera_close_device(camera);

    camera_destroy(camera);

    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *drawing_area;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s dev_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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

    camera = camera_create();
    camera_open_device(camera, argv[1]);
    camera_query_cap(camera);
    camera_query_stream(camera);
    camera_query_support_format(camera);
    camera_set_format(camera, CAM_WIDTH, CAM_HEIGHT, PIX_FMT_YUYV);
    camera_req_buf_and_mmap(camera, CAM_REQ_BUF_CNT);
    camera_start_capture(camera);

    g_idle_add((GSourceFunc)refresh_ui, drawing_area);

    gtk_main();

    return 0;
}
