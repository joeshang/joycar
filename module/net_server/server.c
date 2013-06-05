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

#define BACKLOG             5
#define BUF_SIZE            1024

static pthread_t capture_tid;

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
        if (video_is_read_ready())
        {
            video_read_frame(user_data, process_image);
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    int listen_socket;
    int connect_socket;
    socklen_t client_addr_len;
    socklen_t disp_addr_len;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    struct sockaddr_in disp_addr;

    int recv_size;
    char command_buf[BUF_SIZE];

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s device port\n", argv[0]);
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
    serv_addr.sin_port = htons(atoi(argv[2]));

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

    video_open_device(argv[1]);
    video_init_device();
    video_start_capture();

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

            printf("command(length = %d): %s\n", recv_size, command_buf);
            extract_command(command_buf);
        }
    }

    video_stop_capture();
    video_deinit_device();
    video_close_device();

    close(listen_socket);

    return 0;
}
