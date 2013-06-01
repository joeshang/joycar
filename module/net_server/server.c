#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "command.h"

#define BACKLOG             5
#define BUF_SIZE            1024

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

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
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

        for (;;)
        {
            if ((recv_size = recv(connect_socket, command_buf, BUF_SIZE, 0)) <= 0)
            {
                if (recv_size == 0)
                {
                    printf("client exit\n");
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

    close(listen_socket);

    return 0;
}
