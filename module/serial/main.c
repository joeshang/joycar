#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "serial.h"

#define BAUD_RATE   9600
#define DATA_BITS   8
#define STOP_BITS   1
#define PARITY_TYPE 'N'

#define BUF_SIZE    1024

int main(int argc, char *argv[])
{
    int fd;
    int nread;
    char read_buf[BUF_SIZE];
    char write_buf[BUF_SIZE];

    fd_set read_set;
    fd_set all_set;
    int max_fd;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s serial_name\n", argv[0]);
        exit(1);
    }

    if ((fd = serial_open_device(argv[1])) == -1)
    {
        exit(1);
    }

    serial_set_raw_mode(fd);
    serial_set_speed(fd, BAUD_RATE);
    serial_set_parity(fd, DATA_BITS, PARITY_TYPE, STOP_BITS);

    printf("serial initialization finished\n");

    FD_ZERO(&all_set);
    FD_SET(fileno(stdin), &all_set);
    FD_SET(fd, &all_set);
    max_fd = fd;

    for (;;)
    {
        read_set = all_set;

        if (select(max_fd + 1, &read_set, NULL, NULL, NULL)  == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("select error");
                exit(1);
            }
        }

        if (FD_ISSET(fileno(stdin), &read_set))
        {
            if (fgets(write_buf, BUF_SIZE, stdin) != NULL)
            {
                write(fd, write_buf, strlen(write_buf));
            }
            else
            {
                break;
            }
        }

        if (FD_ISSET(fd, &read_set))
        {
            while ((nread = read(fd, read_buf, BUF_SIZE)) > 0)
            {
                read_buf[nread] = '\0';

                printf("%s", read_buf);
            }
        }
    }

    close(fd);

    return 0;
}

