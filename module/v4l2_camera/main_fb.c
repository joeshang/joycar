/**
 * File: main_fb.c
 * Author: Joe Shang
 * Brief: Display camera data using framebuffer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "capture.h"
#include "decoder.h"
#include "decoder_mjpeg.h"
#include "decoder_yuv422.h"

#define CAM_WIDTH       320
#define CAM_HEIGHT      240

int fb_width;
int fb_height;

Decoder *decoder = NULL;
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

static void draw_image(void *ctx, void *buf_start, int buf_size)
{
    int i;

    /* find start point of middle-display */
    unsigned char *fb_line_start = ctx + fb_width * (fb_height - CAM_HEIGHT) + (fb_width - CAM_WIDTH);
    unsigned char *src_line_start = rgb565_buf;

    decoder_decode(decoder, rgb_buf, buf_start, buf_size);
    rgb888_to_rgb565(rgb565_buf, rgb_buf, CAM_WIDTH, CAM_HEIGHT);

    for (i = 0; i < CAM_HEIGHT; i++)
    {
        memcpy(fb_line_start, src_line_start, CAM_WIDTH * 2);

        fb_line_start += fb_width * 2;
        src_line_start += CAM_WIDTH * 2;
    }
}

int main(int argc, char *argv[])
{
    int fbfd;
    int format;
    char *fbp = NULL;
    struct fb_var_screeninfo vinfo;
    unsigned long screen_size = 0;

    CameraDevice camera;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s camera camera_fomat framebuffer\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[2], "yuyv") == 0)
    {
        format = PIX_FMT_YUYV;
        decoder = decoder_yuv422_create();
    }
    else if (strcmp(argv[2], "mjpeg") == 0)
    {
        format = PIX_FMT_MJPEG;
        decoder = decoder_mjpeg_create();
    }
    else
    {
        fprintf(stderr, "unsupported format, support yuyv and mjpeg\n");
        exit(EXIT_FAILURE);
    }

    /* camera setting */
    camera_init(&camera, argv[1], CAM_WIDTH, CAM_HEIGHT, 5, format);
    camera_open_set(&camera);

    /* framebuffer setting */
    /* open framebuffer */
    fbfd = open(argv[3], O_RDWR);
    if (fbfd == -1)
    {
        fprintf(stderr, "cannot open framebuffer device\n");
        exit(EXIT_FAILURE);
    }

    /* get variable screen infomation */
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
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
    fbp = (char *)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0); 
    if ((int)fbp == -1)
    {
        fprintf(stderr, "failed to map framebuffer device to memory\n");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        camera_read_frame(&camera, draw_image, fbp);
    }

    munmap(fbp, screen_size);
    close(fbfd);
    camera_close(&camera);
    decoder_destroy(decoder);

    return 0;
}
