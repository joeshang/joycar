/**
 * @File: capture.c
 * @Author: Joe Shang
 * @Brief: The camera capturing module with V4L2 framework
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#include "capture.h"

/* record for mmap memory address */
struct buffer
{
    void    *start;
    size_t  length;
};

static int fd                   = -1;
static struct buffer *buffers   = NULL;
static int n_buffers            = 0;

static int xioctl(int fd, int request, void *arg)
{
    int ret;

    while ((ret = ioctl(fd, request, arg)) == -1 && errno == EINTR);

    return ret;
}

static void video_set_format()
{
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));

    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = DEV_WIDTH;
    fmt.fmt.pix.height      = DEV_HEIGHT;
    fmt.fmt.pix.pixelformat = DEV_FORMAT;

    if (xioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
    {
        perror("set format failed");
        exit(EXIT_FAILURE);
    }
}

static void video_req_buf_and_mmap()
{
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    
    /* request buffers */
    req.count  = REQ_BUF_CNT;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd, VIDIOC_REQBUFS, &req) == -1)
    {
        perror("request buffers failed");
        exit(EXIT_FAILURE);
    }

    if (req.count < 2)
    {
        fprintf(stderr, "insufficient buffer memory on %s\n", DEV_NAME);
        exit(EXIT_FAILURE);
    }

    /* alloc mmap memory address of device */
    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers)
    {
        fprintf(stderr, "out of memory\n");
        exit(EXIT_FAILURE);
    }

    /* query buffers in device and mmap buffers */
    struct v4l2_buffer buf;
    for (n_buffers = 0; n_buffers < req.count; n_buffers++)
    {
        memset(&buf, 0, sizeof(buf));

        buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory  = V4L2_MEMORY_MMAP;
        buf.index   = n_buffers;

        /* query buffers */
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            perror("query buffers failed");
            free(buffers);
            exit(EXIT_FAILURE);
        }

        /* mmap buffers */
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start  = mmap(NULL,
                buf.length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                buf.m.offset);

        if (buffers[n_buffers].start == MAP_FAILED)
        {
            perror("mmap buffers failed");
            free(buffers);
            exit(EXIT_FAILURE);
        }
    }
}

void video_open_device()
{
    if ((fd = open(DEV_NAME, O_RDWR | O_NONBLOCK, 0)) == -1)
    {
        fprintf(stderr, "can't open %s: %s\n", DEV_NAME, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void video_init_device()
{
    video_set_format();
    video_req_buf_and_mmap();
}

void video_start_capture()
{
    int i;
    enum v4l2_buf_type type;
    struct v4l2_buffer buf;

    /* queue buffers */
    for (i = 0; i < n_buffers; i++)
    {
        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if (xioctl(fd, VIDIOC_QBUF, &buf) == -1)
        {
            perror("queue buffers failed");
            exit(EXIT_FAILURE);
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* device stream on */
    if (xioctl(fd, VIDIOC_STREAMON, &type) == -1)
    {
        perror("stream on failed");
        exit(EXIT_FAILURE);
    }
}

void video_read_frame(void *ctx, VideoCallBack video_data_callback)
{
    struct v4l2_buffer buf;

    memset(&buf, 0, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd, VIDIOC_DQBUF, &buf) == -1)
    {
        perror("de-queue buffers failed");
        exit(EXIT_FAILURE);
    }

    if (video_data_callback != NULL)
    {
        video_data_callback(ctx, buffers[buf.index].start, buf.bytesused);
    }

    if (xioctl(fd, VIDIOC_QBUF, &buf) == -1)
    {
        perror("queue buffers failed");
        exit(EXIT_FAILURE);
    }
}

int video_is_read_ready()
{
    int sel_ret;
    fd_set fds;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    tv.tv_sec = 2;
    tv.tv_usec = 0;

    sel_ret = select(fd + 1, &fds, NULL, NULL, &tv);

    if (sel_ret == -1)
    {
        if (errno == EINTR)
        {
            return 0;
        }

        exit(EXIT_FAILURE);
    }

    if (sel_ret == 0)
    {
        fprintf(stderr, "select time out\n");
        return 0;
    }

    return 1;
}

void video_stop_capture()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (xioctl(fd, VIDIOC_STREAMOFF, &type) == -1)
    {
        perror("stream off failed");
        exit(EXIT_FAILURE);
    }
}

void video_deinit_device()
{
    int i;
    for (i = 0; i < n_buffers; i++)
    {
        if (munmap(buffers[i].start, buffers[i].length) == -1)
        {
            perror("munmap failed");
            exit(EXIT_FAILURE);
        }
    }

    free(buffers);
}

void video_close_device()
{
    if (close(fd) == -1)
    {
        perror("close failed");
        exit(EXIT_FAILURE);
    }
}

