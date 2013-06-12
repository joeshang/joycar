/**
 * File: client.c
 * Author: Joe Shang
 * Brief:
 */

#include <gtk/gtk.h>
#include <cairo.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "decoder.h"
#include "decoder_mjpeg.h"
#include "decoder_yuv422.h"

//#define DEBUG

#define DEV_WIDTH       320
#define DEV_HEIGHT      240
#define DISP_WIDTH      DEV_WIDTH * 2
#define DISP_HEIGHT     DEV_HEIGHT * 2
#define WIN_WIDTH       DISP_WIDTH
#define WIN_HEIGHT      DISP_HEIGHT

#define BUF_SIZE        4096

int sockfd;
pthread_t capture_tid;

Decoder *frame_decoder = NULL;

unsigned char rgb_buf[DEV_WIDTH * DEV_HEIGHT * 3];

static void refresh_video_area(GtkWidget *drawing_area)
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
    unsigned char *frame_buf = NULL;
    int frame_size;

    int pos;
    int left_size;
    int recv_size;
    int recv_buf_size;
    char recv_buf[BUF_SIZE];
    
    /* get the frame data size */
    recv(sockfd, (void *)&frame_size, sizeof(int), 0);

#ifdef DEBUG
    printf("data size: %d\n", frame_size);
#endif

    if (frame_size > 0)
    {
        frame_buf = calloc(frame_size + 10, 1);
        
        pos = 0;
        left_size = frame_size;
        recv_buf_size = BUF_SIZE;
        while (left_size > 0)
        {
            /* ensure the real receive frame data size == frame_size */
            if (left_size - BUF_SIZE < 0)
            {
                recv_buf_size = left_size;
            }

            if ((recv_size = recv(sockfd, recv_buf, recv_buf_size, 0)) <= 0)
            {
                return FALSE;
            }

            memcpy(frame_buf + pos, recv_buf, recv_size);
            
            left_size -= recv_size;
            pos += recv_size;
        }

#ifdef DEBUG
        printf("finish receiving data\n");
#endif
        decoder_decode(frame_decoder, rgb_buf, frame_buf, frame_size);
        free(frame_buf);
        frame_buf = NULL;

#ifdef DEBUG
        printf("finish formating data\n");
#endif
        refresh_video_area((GtkWidget *)user_data);

#ifdef DEBUG
        printf("finish refreshing data\n");
#endif
    }
    else
    {
        fprintf(stderr, "invalid frame size\n");
    }

    return TRUE;
}

static void *recv_handler(void *user_data)
{
    int type;
    gboolean ret = TRUE;

    pthread_detach(pthread_self());

    for (;;)
    {
        if (recv(sockfd, (void *)&type, sizeof(int), 0) <= 0)
        {
            break;
        }

#ifdef DEBUG
        printf("data type: %d\n", type);
#endif

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

void destroy_handler(GtkWidget *widget, gpointer data)
{
    close(sockfd);
    pthread_cancel(capture_tid);
    decoder_destroy(frame_decoder);

    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *drawing_area;

    struct sockaddr_in serv_addr;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s addr port format\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[3], "yuyv") == 0)
    {
        frame_decoder = decoder_yuv422_create();
    }
    else if (strcmp(argv[3], "mjpeg") == 0)
    {
        frame_decoder = decoder_mjpeg_create();
    }
    else
    {
        fprintf(stderr, "unsupported format, support yuv and mjpeg\n");
        exit(EXIT_FAILURE);
    }

    if (!g_thread_supported())
    {
        g_thread_init(NULL);
    }
    gdk_threads_init();
    gdk_threads_enter();

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

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    pthread_create(&capture_tid, NULL, recv_handler, drawing_area);

    gtk_main();
    gdk_threads_leave();

    return 0;
}
