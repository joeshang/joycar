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

#define CAMERA_DEBUG

/* record for mmap memory address */
typedef struct _ReqBufInfo
{
    void    *start;
    size_t  length;
}ReqBufInfo;

struct _CameraDevice
{
    int fd;
    int req_buf_cnt;
    ReqBufInfo *req_buf_info;

    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_streamparm streamparm;
};

static int xioctl(int fd, int request, void *arg)
{
    int ret;

    while ((ret = ioctl(fd, request, arg)) == -1 && errno == EINTR);

    return ret;
}

CameraDevice *camera_create(void)
{
    CameraDevice *thiz = malloc(sizeof(CameraDevice));

    return thiz;
}

void camera_destroy(CameraDevice *thiz)
{
    if (thiz->req_buf_info != NULL)
    {
        free(thiz->req_buf_info);
    }

    free(thiz);
}

/* open camera device */
void camera_open_device(CameraDevice *thiz, char *dev_name)
{
    if ((thiz->fd = open(dev_name, O_RDWR | O_NONBLOCK, 0)) == -1)
    {
        fprintf(stderr, "can't open %s: %s\n", dev_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

#ifdef CAMERA_DEBUG
    printf("open camera %s successfully\n", dev_name);
#endif
}

void camera_query_cap(CameraDevice *thiz)
{
    /* struct v4l2_capabilities */
    if (xioctl(thiz->fd, VIDIOC_QUERYCAP, &thiz->cap) == -1)
    {
        perror("query capabilities failed");
        exit(EXIT_FAILURE);
    }

    printf("camera thiz->capabilities ->\n");
    printf("driver:\t\t%s\n", thiz->cap.driver);
    printf("card:\t\t%s\n", thiz->cap.card);
    printf("buf info:\t%s\n", thiz->cap.bus_info);
    printf("version:\t%d\n", thiz->cap.version);
    printf("capabilities:\t%x\n", thiz->cap.capabilities);

    if ((thiz->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
    {
        printf("capabilities:\tsupport capture\n");
    }

    if ((thiz->cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
    {
        printf("capabilities:\tsupport streaming\n");
    }
}

void camera_query_stream(CameraDevice *thiz)
{
    /* struct v4l2_streamparm */
    memset(&thiz->streamparm, 0, sizeof(thiz->streamparm));
    thiz->streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (xioctl(thiz->fd, VIDIOC_G_PARM, &thiz->streamparm) == -1)
    {
        perror("get stream parameters failed");
        exit(EXIT_FAILURE);
    }

    printf("camera streaming attributes ->\n");
    printf("%u frames per %u second\n",
            thiz->streamparm.parm.capture.timeperframe.denominator,
            thiz->streamparm.parm.capture.timeperframe.numerator);
}

void camera_query_support_format(CameraDevice *thiz)
{
    /* struct v4l2_fmtdesc */
    memset(&thiz->fmtdesc, 0, sizeof(thiz->fmtdesc));
    thiz->fmtdesc.index = 0;
    thiz->fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    printf("camera format ->\n");
    while (xioctl(thiz->fd, VIDIOC_ENUM_FMT, &thiz->fmtdesc) != -1)
    {
        printf("support format:\t%s\n", thiz->fmtdesc.description);
        thiz->fmtdesc.index++;
    }
}

/* set the format of camera device */
void camera_set_format(CameraDevice *thiz, int width, int height, CameraFormat format)
{
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = width;
    fmt.fmt.pix.height      = height;

    switch (format)
    {
        case PIX_FMT_YUYV:
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
            break;
        case PIX_FMT_MJPEG:
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
            break;
        default:
            fprintf(stderr, "unsupported video format\n");
            exit(EXIT_FAILURE);
    }

    if (xioctl(thiz->fd, VIDIOC_S_FMT, &fmt) == -1)
    {
        perror("set format failed");
        exit(EXIT_FAILURE);
    }

#ifdef CAMERA_DEBUG
    printf("camera set format successfully\n");
#endif
}

/* Request camera data buffers in kernel space and mmap buffers to user space */
void camera_req_buf_and_mmap(CameraDevice *thiz, int req_buf_cnt)
{
    /* request buffers in kernel space */
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count  = req_buf_cnt;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (xioctl(thiz->fd, VIDIOC_REQBUFS, &req) == -1)
    {
        perror("request buffers failed");
        exit(EXIT_FAILURE);
    }

    if (req.count < 2)
    {
        fprintf(stderr, "insufficient buffer memory\n");
        exit(EXIT_FAILURE);
    }

    /* alloc mmap memory address of device */
    thiz->req_buf_info = calloc(req.count, sizeof(ReqBufInfo));
    if (thiz->req_buf_info == NULL)
    {
        fprintf(stderr, "out of memory\n");
        exit(EXIT_FAILURE);
    }

    /* query buffers in device and mmap buffers */
    struct v4l2_buffer buf;
    for (thiz->req_buf_cnt = 0; thiz->req_buf_cnt < req.count; thiz->req_buf_cnt++)
    {
        memset(&buf, 0, sizeof(buf));

        buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory  = V4L2_MEMORY_MMAP;
        buf.index   = thiz->req_buf_cnt;

        /* query buffers */
        if (xioctl(thiz->fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            perror("query buffers failed");
            exit(EXIT_FAILURE);
        }

        /* mmap buffers */
        thiz->req_buf_info[thiz->req_buf_cnt].length = buf.length;
        thiz->req_buf_info[thiz->req_buf_cnt].start  = mmap(NULL,
                buf.length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                thiz->fd,
                buf.m.offset);

        if (thiz->req_buf_info[thiz->req_buf_cnt].start == MAP_FAILED)
        {
            perror("mmap buffers failed");
            exit(EXIT_FAILURE);
        }
    }

#ifdef CAMERA_DEBUG
    printf("camera request buffers and mmap successfully\n");
#endif
}

/* device begin to capture */
void camera_start_capture(CameraDevice *thiz)
{
    int i;
    enum v4l2_buf_type type;

    /* queue buffers */
    struct v4l2_buffer buf;
    for (i = 0; i < thiz->req_buf_cnt; i++)
    {
        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if (xioctl(thiz->fd, VIDIOC_QBUF, &buf) == -1)
        {
            perror("queue buffers failed");
            exit(EXIT_FAILURE);
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* device stream on */
    if (xioctl(thiz->fd, VIDIOC_STREAMON, &type) == -1)
    {
        perror("stream on failed");
        exit(EXIT_FAILURE);
    }

#ifdef CAMERA_DEBUG
    printf("camera begin to capture(stream on)\n");
#endif
}

/* read the frame of camera from device buffer and handle it in callback function */
void camera_read_frame(CameraDevice *thiz, VideoCallBack camera_data_callback, void *ctx)
{
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (xioctl(thiz->fd, VIDIOC_DQBUF, &buf) == -1)
    {
        perror("de-queue buffers failed");
        exit(EXIT_FAILURE);
    }

    if (camera_data_callback != NULL)
    {
        camera_data_callback(ctx, 
                thiz->req_buf_info[buf.index].start, 
                buf.bytesused);
    }

    if (xioctl(thiz->fd, VIDIOC_QBUF, &buf) == -1)
    {
        perror("queue buffers failed");
        exit(EXIT_FAILURE);
    }
}

/* whether the device can read or not */
int camera_is_read_ready(CameraDevice *thiz)
{
    int sel_ret;
    fd_set fds;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(thiz->fd, &fds);

    tv.tv_sec = 2;
    tv.tv_usec = 0;

    sel_ret = select(thiz->fd + 1, &fds, NULL, NULL, &tv);

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

/* device stop to capture */
void camera_stop_capture(CameraDevice *thiz)
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (xioctl(thiz->fd, VIDIOC_STREAMOFF, &type) == -1)
    {
        perror("stream off failed");
        exit(EXIT_FAILURE);
    }

#ifdef CAMERA_DEBUG
    printf("camera stop to capture(stream on)\n");
#endif
}

/* deinitialize camera device */
void camera_deinit_device(CameraDevice *thiz)
{
    int i;
    for (i = 0; i < thiz->req_buf_cnt; i++)
    {
        if (munmap(thiz->req_buf_info[i].start, thiz->req_buf_info[i].length) == -1)
        {
            perror("munmap failed");
            exit(EXIT_FAILURE);
        }
    }

#ifdef CAMERA_DEBUG
    printf("deinitialize camera successfully\n");
#endif
}

/* close camera device */
void camera_close_device(CameraDevice *thiz)
{
    if (close(thiz->fd) == -1)
    {
        perror("close failed");
        exit(EXIT_FAILURE);
    }

#ifdef CAMERA_DEBUG
    printf("close camera successfully\n");
#endif
}

