/**
 * File: control_pda.c
 * Author: Joe Shang
 * Brief: The main program of PDA control platform with framebuffer displaying.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <assert.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "decoder.h"
#include "decoder_mjpeg.h"
#include "decoder_yuv422.h"

//#define DEBUG

#define CAM_WIDTH       320
#define CAM_HEIGHT      240

#define BUF_SIZE        4096

int sockfd;
pthread_t capture_tid;

int fb_width;
int fb_height;

Decoder *frame_decoder = NULL;

unsigned char rgb_buf[CAM_WIDTH * CAM_HEIGHT * 3];
unsigned char rgb565_buf[CAM_WIDTH * CAM_HEIGHT * 2];

static void rgb888_to_rgb565(unsigned char *out_buf, unsigned char *in_buf, int width, int height)
{
    int i;
    int j;
    unsigned char *in_dot;
    unsigned short *out_dot;
    unsigned char *in_line;
    unsigned char *out_line;

    assert((out_buf != NULL) && (in_buf != NULL));

    in_line = in_buf;
    out_line = out_buf;
    for (i = 0; i < height; i++)
    {
        in_dot = in_line;
        out_dot = (unsigned short *)out_line;

        for (j = 0; j < width; j++)
        {
            *out_dot = (((in_dot[0] >> 3) & 0x1f) << 0)     /* R */
                       | (((in_dot[1] >> 2) & 0x3f) << 5)   /* G */
                       | (((in_dot[2] >> 3) & 0x1f) << 11); /* B */

            in_dot += 3;
            out_dot++;
        }

        in_line += width * 3;
        out_line += width * 2;
    }
}

static void refresh_video_area(void *ctx)
{
    int i;

    /* find start point of middle-display */
    unsigned char *src_line_start = rgb565_buf;
    unsigned char *fb_line_start = ctx + fb_width * (fb_height - CAM_HEIGHT) 
        + (fb_width - CAM_WIDTH);


    for (i = 0; i < CAM_HEIGHT; i++)
    {
        memcpy(fb_line_start, src_line_start, CAM_WIDTH * 2);

        fb_line_start += fb_width * 2;
        src_line_start += CAM_WIDTH * 2;
    }
}

static int video_data_handler(void *user_data)
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
                return -1;
            }

            memcpy(frame_buf + pos, recv_buf, recv_size);
            
            left_size -= recv_size;
            pos += recv_size;
        }

#ifdef DEBUG
        printf("finish receiving data\n");
#endif
        decoder_decode(frame_decoder, rgb_buf, frame_buf, frame_size);
        rgb888_to_rgb565(rgb565_buf, rgb_buf, CAM_WIDTH, CAM_HEIGHT);

        free(frame_buf);
        frame_buf = NULL;

#ifdef DEBUG
        printf("finish formating data\n");
#endif
        refresh_video_area(user_data);

#ifdef DEBUG
        printf("finish refreshing data\n");
#endif
    }
    else
    {
        fprintf(stderr, "invalid frame size\n");
    }

    return 0;
}

static void *recv_handler(void *user_data)
{
    int type;
    int ret = 0;

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

        if (ret == -1)
        {
            break;
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int fb_fd;
    char *fb_addr = NULL;
    struct fb_var_screeninfo vinfo;
    unsigned long screen_size = 0;
    struct sockaddr_in serv_addr;

    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s addr port format framebuffer\n", argv[0]);
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

    /* network setting */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    /* framebuffer setting */
    /* open framebuffer */
    fb_fd = open(argv[4], O_RDWR);
    if (fb_fd == -1)
    {
        fprintf(stderr, "cannot open framebuffer device\n");
        exit(EXIT_FAILURE);
    }

    /* get variable screen infomation */
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo))
    {
    }

    if (vinfo.bits_per_pixel != 16)
    {
        fprintf(stderr, "not supported bits_per_pixel, it only supports 16 bit color\n");
        exit(EXIT_FAILURE);
    }

    /* figure out the size of screen in bytes */
    fb_width = vinfo.xres;
    fb_height = vinfo.yres;
    screen_size = vinfo.xres * vinfo.yres * 2;
    printf("frame buffer: width = %d, height = %d\n", vinfo.xres, vinfo.yres);

    /* mmap framebuffer device to memory */
    fb_addr = (char *)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0); 
    if ((int)fb_addr == -1)
    {
        fprintf(stderr, "failed to map framebuffer device to memory\n");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    pthread_create(&capture_tid, NULL, recv_handler, fb_addr);

    for (;;)
    {
    }

    return 0;
}
