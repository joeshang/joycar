/**
 * File: communication.c
 * Author: Joe Shang
 * Brief: The callback handlers of communication programs.
 */

#include <gtk/gtk.h>
#include <cairo.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "decoder.h"
#include "connector.h"
#include "callbacks_con.h"

#define DEV_WIDTH       320
#define DEV_HEIGHT      240
#define DISP_WIDTH      320
#define DISP_HEIGHT     240

#define BUF_SIZE        4096
#define STATUS_BAR_CTX  1

extern Connector *active_connector;
extern Decoder *frame_decoder;

static pthread_t recv_tid;

unsigned char video_buf[DEV_WIDTH * DEV_HEIGHT * 2];
unsigned char rgb_buf[DEV_WIDTH * DEV_HEIGHT * 3];

static void refresh_drawing_area(GtkWidget *drawing_area)
{
    gdk_threads_enter();

    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(drawing_area));

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

    gdk_threads_leave();
}

static gboolean video_data_handler(gpointer user_data)
{
    int pos;
    int video_size;

    int left_size;
    int recv_size;
    int recv_buf_size;
    char recv_buf[BUF_SIZE];

    GtkBuilder *builder;
    GtkWidget *drawing_area;

    /* get the video data size */
    connector_recv(active_connector, (void *)&video_size, sizeof(int));

    if (video_size > 0)
    {
        pos = 0;
        left_size = video_size;
        recv_buf_size = BUF_SIZE;

        /* tcp receive video data */
        while (left_size > 0)
        {
            /* ensure the real receive video data size == video_size */
            if (left_size - BUF_SIZE < 0)
            {
                recv_buf_size = left_size;
            }

            if ((recv_size = connector_recv(active_connector, recv_buf, recv_buf_size)) <= 0)
            {
                return FALSE;
            }

            memcpy(video_buf + pos, recv_buf, recv_size);

            pos += recv_size;
            left_size -= recv_size;
        }

        /* process image */
        decoder_decode(frame_decoder, rgb_buf, video_buf, video_size);

        builder = (GtkBuilder *)user_data;
        drawing_area = GTK_WIDGET(gtk_builder_get_object(builder, "video_area"));
        refresh_drawing_area(drawing_area); 
    }
    else
    {
        fprintf(stderr, "invalid video size\n");
    }

    return TRUE;
}

static void *recv_handler(void *user_data)
{
    /* parse receive data type */
    int type;
    gboolean ret = TRUE;

    pthread_detach(pthread_self());

    for (;;)
    {
        if (connector_recv(active_connector, (void *)&type, sizeof(int)) <= 0)
        {
            break;
        }

        switch (type)
        {
            case 1:
                ret = video_data_handler(user_data);
                break;
            default:
                fprintf(stderr, "unknown data type\n");
                break;
        }

        if (!ret)
        {
            break;
        }
    }

    return NULL;
}

static void update_status_bar(GtkWidget *status_bar, char *msg)
{
    gtk_statusbar_pop(GTK_STATUSBAR(status_bar), STATUS_BAR_CTX);
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), STATUS_BAR_CTX, msg);
    printf("%s\n", msg);
}

void connector_open_callback(void *data, void *ctx)
{
    StatusMessage *status_msg = (StatusMessage *)data;
    GtkBuilder *builder = (GtkBuilder *)ctx;
    GtkWidget *connect_btn;
    GtkWidget *disconnect_btn;
    GtkImage *status_img;
    GtkWidget *status_bar = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));

    printf("connector open callback: ");

    update_status_bar(status_bar, status_msg->msg);

    if (status_msg->status == STATUS_TYPE_OK)
    {
        disconnect_btn = GTK_WIDGET(gtk_builder_get_object(builder, "disconnect_btn"));
        gtk_widget_set_sensitive(disconnect_btn, TRUE);

        connect_btn = GTK_WIDGET(gtk_builder_get_object(builder, "connect_btn"));
        gtk_widget_set_sensitive(connect_btn, FALSE);

        status_img = GTK_IMAGE(gtk_builder_get_object(builder, "connect_status_img"));
        gtk_image_set_from_icon_name(status_img, "dialog-ok", GTK_ICON_SIZE_MENU);

        pthread_create(&recv_tid, NULL, recv_handler, ctx);
    }
}

void connector_close_callback(void *data, void *ctx)
{
    GtkBuilder *builder = (GtkBuilder *)ctx;
    StatusMessage *status_msg = (StatusMessage *)data;
    GtkWidget *status_bar = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));

    update_status_bar(status_bar, status_msg->msg);

    pthread_cancel(recv_tid);
}

