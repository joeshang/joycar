#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "command.h"
#include "serial.h"
#include "capture.h"
#include "video_container.h"
#include "jpeg_encoder.h"

/************************************************************
 * Macro Definitions
 ************************************************************/
#define CAM_WIDTH       320
#define CAM_HEIGHT      240
#define CAM_FPS         5
#define JPEG_QUALITY    80

#define BAUD_RATE       9600
#define DATA_BITS       8
#define STOP_BITS       1
#define PARITY_TYPE     'N'

#define BACKLOG         5
#define BUF_SIZE        1024

enum _OutputFormat
{
    OUT_FMT_RAW,
    OUT_FMT_JPEG
};

/************************************************************
 * Global Variables
 ************************************************************/
CameraDevice camera;
VideoContainer container;

pthread_t camera_tid;
pthread_t send_tid;

int send_frame_size = 0;
unsigned char *send_frame_buf[CAM_WIDTH * CAM_HEIGHT * 2];

/************************************************************
 * Callback Function Area
 ************************************************************/
static int process_image(unsigned char *out_buf,
                         unsigned char *in_buf,
                         int in_size,
                         int in_format,
                         int out_format)
{
    int processed_size = 0;

    if (out_format == OUT_FMT_RAW)
    {
        memcpy(out_buf, in_buf, in_size);
        processed_size = in_size;
    }
    else if (out_format == OUT_FMT_JPEG)
    {
        switch (in_format)
        {
            case PIX_FMT_YUYV:
                processed_size = jpeg_encoder_yuv422(out_buf, 
                                                     in_buf, 
                                                     in_size,
                                                     CAM_WIDTH, 
                                                     CAM_HEIGHT, 
                                                     JPEG_QUALITY);
                break;
            case PIX_FMT_MJPEG:
                processed_size = jpeg_encoder_mjpeg(out_buf, in_buf, in_size);
                break;
            default:
                break;
        }
    }

    return processed_size;
}

static void requestdb_callback(unsigned char *buf, int buf_size, void *ctx)
{
    memcpy(send_frame_buf, buf, buf_size);
    send_frame_size = buf_size;
}

static void frame_callback(void *ctx, void *buf_start, int buf_size)
{
    video_container_input_raw(&container, buf_start, buf_size);
}

static void command_callback(int status, void *command, void *ctx)
{
    int serial_fd = (int)ctx;

    if (status == COMMAND_PASS)
    {
        printf("pass command: %s\n", (char *)command);

        write(serial_fd, command, strlen(command));
    }
}

static void send_thread_cleanup(void *arg)
{
    printf("enter send thread cleanup\n");
    video_container_releasedb(&container);
}

/************************************************************
 * Thread Function Area
 ************************************************************/
static void *send_thread(void *user_data)
{
    int data_type = 1;
    int client_socket = (int)user_data;

    printf("enter into send thread\n");

    pthread_cleanup_push(send_thread_cleanup, NULL);
    pthread_detach(pthread_self());

    for (;;)
    {
        video_container_requestdb(&container, requestdb_callback, NULL);

        send(client_socket, (void *)&data_type, sizeof(int), 0);
        send(client_socket, (void *)&send_frame_size, sizeof(int), 0);
        send(client_socket, send_frame_buf, send_frame_size, 0);
    }

    pthread_cleanup_pop(1);

    return NULL;
}

static void *camera_thread(void *user_data)
{
    int out_format = (int)user_data;

    pthread_detach(pthread_self());

    /* camera capturing module */
    for (;;)
    {
        camera_read_frame(&camera, frame_callback, NULL);

        video_container_updatedb(&container, process_image, camera.format, out_format);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    int in_format;
    int out_format;

    int recv_size;
    char command_buf[BUF_SIZE];

    int serial_fd;

    int listen_socket;
    int connect_socket;
    socklen_t client_addr_len;
    socklen_t disp_addr_len;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    struct sockaddr_in disp_addr;

    if (argc != 6)
    {
        fprintf(stderr, "Usage: %s port camera_name camera_format output_format serial_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[3], "yuyv") == 0)
    {
        in_format = PIX_FMT_YUYV;
    }
    else if (strcmp(argv[3], "mjpeg") == 0)
    {
        in_format = PIX_FMT_MJPEG;
    }
    else
    {
        fprintf(stderr, "unsupported input format, support \"yuyv\" and \"mjpeg\"\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[4], "raw") == 0)
    {
        out_format = OUT_FMT_RAW;
    }
    else if (strcmp(argv[4], "jpeg") == 0)
    {
        out_format = OUT_FMT_JPEG;
    }
    else
    {
        fprintf(stderr, "unsupported output format, support \"raw\" and \"jpeg\"\n");
        exit(EXIT_FAILURE);
    }

    if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(listen_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_socket, BACKLOG) == -1)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    disp_addr_len = sizeof(disp_addr);
    if (getsockname(listen_socket, (struct sockaddr *)&disp_addr, &disp_addr_len) == -1)
    {
        perror("getsockaddr failed");
        exit(EXIT_FAILURE);
    }
    char disp_addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &disp_addr.sin_addr, disp_addr_str, INET_ADDRSTRLEN);
    printf("server is listening at %s:%d\n", disp_addr_str, ntohs(disp_addr.sin_port));

    /* serial device init */
    if ((serial_fd = serial_open_device(argv[5])) == -1)
    {
        perror("open serial failed");
        exit(EXIT_FAILURE);
    }
    serial_set_raw_mode(serial_fd);
    serial_set_speed(serial_fd, BAUD_RATE);
    serial_set_parity(serial_fd, DATA_BITS, PARITY_TYPE, STOP_BITS);

    /* video container init */
    video_container_init(&container,  CAM_WIDTH * CAM_HEIGHT * 2);

    /* camera device init */
    camera_init(&camera, argv[2], CAM_WIDTH, CAM_HEIGHT, CAM_FPS, in_format);
    camera_open_set(&camera);
    pthread_create(&camera_tid, NULL, camera_thread, (void *)out_format);

    for (;;)
    {
        client_addr_len = sizeof(client_addr);
        if ((connect_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }
        }
        inet_ntop(AF_INET, &client_addr.sin_addr, disp_addr_str, INET_ADDRSTRLEN);
        printf("client(%s) connected\n", disp_addr_str);

        pthread_create(&send_tid, NULL, send_thread, (void *)connect_socket);

        /* command parsing module */
        for (;;)
        {
            if ((recv_size = recv(connect_socket, command_buf, BUF_SIZE, 0)) <= 0)
            {
                if (recv_size == 0)
                {
                    printf("client exit\n");

                    pthread_cancel(send_tid);

                    break;
                }
                else
                {
                    perror("recv failed");
                    exit(EXIT_FAILURE);
                }
            }
            command_buf[recv_size] = '\0';

            printf("command(length = %d): %s\n", recv_size, command_buf);

            extract_command(command_buf, command_callback, (void *)serial_fd); 
        }
    }


    pthread_cancel(camera_tid);

    camera_close(&camera);
    video_container_destroy(&container);

    close(listen_socket);
    close(serial_fd);

    return 0;
}
