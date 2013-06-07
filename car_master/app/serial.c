#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#include "serial.h"

int name_array[] =
{
    9600,
    19200,
    38400,
    57600,
    115200
};

static int speed_array[] =
{
    B9600,
    B19200,
    B38400,
    B57600,
    B115200
};

int serial_open_device(char *dev_name)
{
    int fd;

    if ((fd = open(dev_name, O_RDWR | O_NOCTTY)) == -1)
    {
        perror("open serial error");
    }

    return fd;
}

int serial_set_raw_mode(int fd)
{
    struct termios options;

    if (isatty(fd) == 0)
    {
        fprintf(stderr, "file descriptor %d is not a terminal\n", fd);
        return -1;
    }

    if (tcgetattr(fd, &options) != 0)
    {
        perror("tcgetattr error");
        return -1;
    }
    
    /* serial mode */
    /* options.c_lflag &= ~(ICANON | ECHO | ECHOE | IEXTEN | ISIG); */
    options.c_lflag = 0;

    /* input setting */
    options.c_iflag = IGNBRK;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* output setting */
    /* options.c_oflag &= ~OPOST; */
    options.c_oflag = 0;

    /* flow control */
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CLOCAL | CREAD;

    /* read setting */
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 20;

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("tcsetattr error");
        return -1;
    }

    return 0;
}

int serial_set_speed(int fd, int speed)
{
    int i;
    struct termios options;

    if (isatty(fd) == 0)
    {
        fprintf(stderr, "file descriptor %d is not a terminal\n", fd);
        return -1;
    }

    if (tcgetattr(fd, &options) != 0)
    {
        perror("tcgetattr error");
        return -1;
    }

    for (i = 0; i < sizeof(speed_array) / sizeof(speed_array[0]); i++)
    {
        if (speed == name_array[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&options, speed_array[i]);
            cfsetospeed(&options, speed_array[i]);

            if (tcsetattr(fd, TCSANOW, &options) != 0)
            {
                perror("tcsetattr error");
                return -1;
            }
            tcflush(fd, TCIOFLUSH);

            break;
        }
    }

    return 0;
}

int serial_set_parity(int fd, int data_bits, char parity_type, int stop_bits)
{
    struct termios options;

    if (isatty(fd) == 0)
    {
        fprintf(stderr, "file descriptor %d is not a terminal\n", fd);
        return -1;
    }

    /* get attributes of terminal */
    if (tcgetattr(fd, &options) != 0)
    {
        perror("tcgetattr error");
        return -1;
    }

    /* data bits setting */
    options.c_cflag &= ~CSIZE;
    switch (data_bits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "unsupported data bits: %d\n", data_bits);
            return -1;
    }

    /* parity setting */
    switch (toupper(parity_type))
    {
        case 'N':   /* no parity */
            options.c_cflag &= ~PARENB; /* disable parity checking */
            options.c_iflag &= ~INPCK;  /* disable input parity checking */
            break;
        case 'E':   /* even parity */
            options.c_cflag |= PARENB;  /* enable parity checking */
            options.c_cflag &= ~PARODD; /* even parity checking */
            options.c_iflag |= INPCK;   /* enable input parity checking */
            break;
        case 'O':   /* odd parity */
            options.c_cflag |= PARENB;  /* enable parity checking */
            options.c_cflag |= PARODD;  /* odd parity checking */
            options.c_iflag |= INPCK;   /* enable input parity checking */
            break;
        case 'S':   /* space parity */
            options.c_cflag &= ~PARENB; /* disable parity checking */
            options.c_iflag |= INPCK;   /* enable input parity checking */
            break;
        default:
            fprintf(stderr, "unsupported parity: %c\n", parity_type);
            return -1;
    }

    /* stop bits setting */
    switch (stop_bits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "unsupported stop bits: %d\n", stop_bits);
            return -1;
    }

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("tcsetattr error");
        return -1;
    }

    return 0;
}

