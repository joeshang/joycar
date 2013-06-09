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
#include "capture.h"
#include "serial.h"

#define CAM_WIDTH       320
#define CAM_HEIGHT      240
#define CAM_FORMAT      PIX_FMT_YUYV
#define CAM_REQ_BUF_CNT 4

#define BAUD_RATE           9600
#define DATA_BITS           8
#define STOP_BITS           1
#define PARITY_TYPE         'N'

#define BACKLOG             5
#define BUF_SIZE            1024

static pthread_t capture_tid;
CameraDevice *camera;

static void process_image(void *ctx, void *buf_start, int buf_size)
{
    int client_socket = (int)ctx;
    int data_type = 1;

    send(client_socket, (void *)&data_type, sizeof(int), 0);
    send(client_socket, (void *)&buf_size, sizeof(int), 0);
    send(client_socket, buf_start, buf_size, 0);
}

static void *capture_thread(void *user_data)
{
    pthread_detach(pthread_self());

    /* video capturing module */
    for (;;)
    {
        if (camera_is_read_ready(camera))
        {
            camera_read_frame(camera, process_image, user_data);
        }
    }

    return NULL;
}

static void command_callback(int status, void *command, void *ctx)
{
    int serial_fd = (int)ctx;

    if (status == COMMAND_PASS)
    {
        write(serial_fd, command, strlen(command));

#ifdef DEBUG
        printf("pass command: %s\n", (char *)command);
#endif
    }
}

int main(int argc, char **argv)
{
    int serial_fd;
    int listen_socket;
    int connect_socket;
    socklen_t client_addr_len;
    socklen_t disp_addr_len;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    struct sockaddr_in disp_addr;

    int recv_size;
    char command_buf[BUF_SIZE];

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s port camera_name serial_name\n", argv[0]);
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
#ifdef DEBUG
    printf("server is listening at %s:%d\n", disp_addr_str, ntohs(disp_addr.sin_port));
#endif

    /* camera device init */
    camera = camera_create();
    camera_open_device(camera, argv[2]);
    camera_query_cap(camera);
    camera_query_stream(camera);
    camera_query_support_format(camera);
    camera_set_format(camera, CAM_WIDTH, CAM_HEIGHT, CAM_FORMAT);
    camera_req_buf_and_mmap(camera, CAM_REQ_BUF_CNT);
    camera_start_capture(camera);

    /* serial device init */
    if ((serial_fd = serial_open_device(argv[3])) == -1)
    {
        perror("open serial failed");
        exit(EXIT_FAILURE);
    }
    serial_set_raw_mode(serial_fd);
    serial_set_speed(serial_fd, BAUD_RATE);
    serial_set_parity(serial_fd, DATA_BITS, PARITY_TYPE, STOP_BITS);

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
#ifdef DEBUG
        printf("client(%s) connected\n", disp_addr_str);
#endif

        pthread_create(&capture_tid, NULL, capture_thread, (void *)connect_socket);

        /* command parsing module */
        for (;;)
        {
            if ((recv_size = recv(connect_socket, command_buf, BUF_SIZE, 0)) <= 0)
            {
                if (recv_size == 0)
                {
                    printf("client exit\n");

                    pthread_cancel(capture_tid);
                
                    break;
                }
                else
                {
                    perror("recv failed");
                    exit(EXIT_FAILURE);
                }
            }
            command_buf[recv_size] = '\0';

            extract_command(command_buf, command_callback, (void *)serial_fd);
        }
    }

    camera_stop_capture(camera);
    camera_deinit_device(camera);
    camera_close_device(camera);

    camera_destroy(camera);

    close(listen_socket);
    close(serial_fd);

    return 0;
}
