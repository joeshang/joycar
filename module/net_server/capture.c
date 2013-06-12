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
#include <assert.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#include "capture.h"

#define CAMERA_DEBUG
#define REQ_BUF_CNT     4

static int xioctl(int fd, int request, void *arg)
{
    int ret;

    while ((ret = ioctl(fd, request, arg)) == -1 && errno == EINTR);

    return ret;
}

static void camera_free_resource(CameraDevice *thiz)
{
    if (thiz->req_buf_info != NULL)
    {
        free(thiz->req_buf_info);
    }
    thiz->req_buf_info = NULL;

    if (thiz->dev_name != NULL)
    {
        free(thiz->dev_name);
    }
    thiz->dev_name = NULL;
}

/* open camera device */
static void camera_open_device(CameraDevice *thiz)
{
    if ((thiz->fd = open(thiz->dev_name, O_RDWR | O_NONBLOCK, 0)) == -1)
    {
        fprintf(stderr, "can't open %s: %s\n", thiz->dev_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

#ifdef CAMERA_DEBUG
    printf("open camera %s successfully\n", thiz->dev_name);
#endif
}

/* close camera device */
static void camera_close_device(CameraDevice *thiz)
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

static void camera_query_cap(CameraDevice *thiz)
{
    struct v4l2_capability cap; 
    memset(&cap, 0, sizeof(struct v4l2_capability));
    if (xioctl(thiz->fd, VIDIOC_QUERYCAP, &cap) == -1)
    {
        perror("query capabilities failed");
        exit(EXIT_FAILURE);
    }

    printf("driver:\t\t%s\n", cap.driver);
    printf("card:\t\t%s\n", cap.card);
    printf("buf info:\t%s\n", cap.bus_info);
    printf("version:\t%d\n", cap.version);
    printf("capabilities:\t%x\n", cap.capabilities);

    if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
    {
        printf("capabilities:\tsupport capture\n");
    }

    if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
    {
        printf("capabilities:\tsupport streaming\n");
    }
}

static void camera_query_cur_fps(CameraDevice *thiz)
{
    struct v4l2_streamparm streamparm;
    memset(&streamparm, 0, sizeof(struct v4l2_streamparm));
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (xioctl(thiz->fd, VIDIOC_G_PARM, &streamparm) == -1)
    {
        perror("get stream parameters failed");
        exit(EXIT_FAILURE);
    }

    printf("current fps:\t%u frames per %u second\n",
            streamparm.parm.capture.timeperframe.denominator,
            streamparm.parm.capture.timeperframe.numerator);

    if ((streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) == V4L2_CAP_TIMEPERFRAME)
    {
        printf("capabilities:\t support pragrammable frame rates\n");
    }

}

static int enum_frame_intervals(int fd, int pixfmt, int width, int height)
{
    int ret;
    struct v4l2_frmivalenum fival;

    memset(&fival, 0, sizeof(struct v4l2_frmivalenum));
    fival.index = 0;
    fival.pixel_format = pixfmt;
    fival.width = width;
    fival.height = height;

    printf("\tTime interval between frame: ");
    while ((ret = ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fival)) == 0) 
    {
        if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE) 
        {
            printf("%u/%u, ",
                   fival.discrete.numerator, fival.discrete.denominator);
        } 
        else if (fival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) 
        {
            printf("{min { %u/%u } .. max { %u/%u } }, ",
                   fival.stepwise.min.numerator, fival.stepwise.min.numerator,
                   fival.stepwise.max.denominator, fival.stepwise.max.denominator);
            break;
        } 
        else if (fival.type == V4L2_FRMIVAL_TYPE_STEPWISE) 
        {
            printf("{min { %u/%u } .. max { %u/%u } / "
                   "stepsize { %u/%u } }, ",
                   fival.stepwise.min.numerator, fival.stepwise.min.denominator,
                   fival.stepwise.max.numerator, fival.stepwise.max.denominator,
                   fival.stepwise.step.numerator, fival.stepwise.step.denominator);
            break;
        }
        fival.index++;
    }
    printf("\n");

    if (ret != 0 && errno != EINVAL) 
    {
        printf("ERROR enumerating frame intervals: %d\n", errno);
        return errno;
    }

    return 0;
}

static int enum_frame_sizes(int fd, int pixfmt)
{
    int ret;
    struct v4l2_frmsizeenum fsize;

    memset(&fsize, 0, sizeof(struct v4l2_frmsizeenum));
    fsize.index = 0;
    fsize.pixel_format = pixfmt;
    while ((ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fsize)) == 0) 
    {
        if (fsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) 
        {
            printf("{ discrete: width = %u, height = %u }\n",
                   fsize.discrete.width, fsize.discrete.height);

            ret = enum_frame_intervals(fd, pixfmt,
                                       fsize.discrete.width, 
                                       fsize.discrete.height);
            if (ret != 0)
            {
                printf("  Unable to enumerate frame sizes.\n");
            }
        } 
        else if (fsize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS) 
        {
            printf("{ continuous: min { width = %u, height = %u } .. "
                   "max { width = %u, height = %u } }\n",
                   fsize.stepwise.min_width, fsize.stepwise.min_height,
                   fsize.stepwise.max_width, fsize.stepwise.max_height);
            printf("  Refusing to enumerate frame intervals.\n");
            break;
        } 
        else if (fsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) 
        {
            printf("{ stepwise: min { width = %u, height = %u } .. "
                   "max { width = %u, height = %u } / "
                   "stepsize { width = %u, height = %u } }\n",
                   fsize.stepwise.min_width, fsize.stepwise.min_height,
                   fsize.stepwise.max_width, fsize.stepwise.max_height,
                   fsize.stepwise.step_width, fsize.stepwise.step_height);
            printf("  Refusing to enumerate frame intervals.\n");
            break;
        }
        fsize.index++;
    }

    if (ret != 0 && errno != EINVAL) 
    {
        printf("ERROR enumerating frame sizes: %d\n", errno);
        return errno;
    }

    return 0;
}

static int camera_query_support_format(CameraDevice *thiz)
{
    int ret;
    struct v4l2_fmtdesc fmt;

    memset(&fmt, 0, sizeof(struct v4l2_fmtdesc));
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    printf("camera format ->\n");
    while ((ret = ioctl(thiz->fd, VIDIOC_ENUM_FMT, &fmt)) == 0) 
    {
        fmt.index++;
        printf("{ pixelformat = '%c%c%c%c', description = '%s' }\n",
               fmt.pixelformat & 0xFF, 
               (fmt.pixelformat >> 8) & 0xFF,
               (fmt.pixelformat >> 16) & 0xFF, 
               (fmt.pixelformat >> 24) & 0xFF,
               fmt.description);

        ret = enum_frame_sizes(thiz->fd, fmt.pixelformat);
        if (ret != 0)
        {
            printf("  Unable to enumerate frame sizes.\n");
        }
    }

    if (errno != EINVAL) 
    {
        printf("ERROR enumerating frame formats: %d\n", errno);
        return errno;
    }

    return 0;
}

/* set the format of camera device */
static void camera_set_format(CameraDevice *thiz)
{
    struct v4l2_format fmt;

    memset(&fmt, 0, sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = thiz->width;
    fmt.fmt.pix.height      = thiz->height;
    fmt.fmt.pix.pixelformat = thiz->format;

    if (xioctl(thiz->fd, VIDIOC_S_FMT, &fmt) == -1)
    {
        perror("set format failed");
        exit(EXIT_FAILURE);
    }

    if (thiz->fps != 0)
    {
        struct v4l2_streamparm streamparm;

        memset(&streamparm, 0, sizeof(struct v4l2_streamparm));
        streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        streamparm.parm.capture.timeperframe.numerator = 1;
        streamparm.parm.capture.timeperframe.denominator = thiz->fps;

        if (xioctl(thiz->fd, VIDIOC_S_PARM, &streamparm) == -1)
        {
            perror("set fps failed");
            exit(EXIT_FAILURE);
        }
    }

#ifdef CAMERA_DEBUG
    printf("camera set format successfully\n");
#endif
}

/* Request camera data buffers in kernel space and mmap buffers to user space */
static void camera_req_buf_and_mmap(CameraDevice *thiz)
{
    int i;

    /* request buffers in kernel space */
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(struct v4l2_requestbuffers));
    req.count  = REQ_BUF_CNT;
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
    for (i = 0; i < req.count; i++)
    {
        memset(&buf, 0, sizeof(struct v4l2_buffer));

        buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory  = V4L2_MEMORY_MMAP;
        buf.index   = i;

        /* query buffers */
        if (xioctl(thiz->fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            perror("query buffers failed");
            exit(EXIT_FAILURE);
        }

        /* mmap buffers */
        thiz->req_buf_info[i].length = buf.length;
        thiz->req_buf_info[i].start  = mmap(NULL,
                buf.length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                thiz->fd,
                buf.m.offset);

        if (thiz->req_buf_info[i].start == MAP_FAILED)
        {
            perror("mmap buffers failed");
            exit(EXIT_FAILURE);
        }
    }

    /* queue buffers */
    for (i = 0; i < req.count; i++)
    {
        memset(&buf, 0, sizeof(struct v4l2_buffer));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if (xioctl(thiz->fd, VIDIOC_QBUF, &buf) == -1)
        {
            perror("queue buffers failed");
            exit(EXIT_FAILURE);
        }
    }

#ifdef CAMERA_DEBUG
    printf("camera request buffers and mmap successfully\n");
#endif
}

/* deinitialize camera device */
static void camera_deinit_device(CameraDevice *thiz)
{
    int i;

    for (i = 0; i < REQ_BUF_CNT; i++)
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

/* device begin to capture */
static void camera_start_capture(CameraDevice *thiz)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* device stream on */
    if (xioctl(thiz->fd, VIDIOC_STREAMON, &type) == -1)
    {
        perror("stream on failed");
        exit(EXIT_FAILURE);
    }

    thiz->streaming_state = STREAMING_ON;

#ifdef CAMERA_DEBUG
    printf("camera begin to capture(stream on)\n");
#endif
}

/* device stop to capture */
static void camera_stop_capture(CameraDevice *thiz)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (xioctl(thiz->fd, VIDIOC_STREAMOFF, &type) == -1)
    {
        perror("stream off failed");
        exit(EXIT_FAILURE);
    }

    thiz->streaming_state = STREAMING_OFF;

#ifdef CAMERA_DEBUG
    printf("camera stop to capture(stream off)\n");
#endif
}

/* whether the device can read or not */
static int camera_is_read_ready(CameraDevice *thiz)
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

int camera_init(CameraDevice *thiz,
                char *dev_name,
                int width,
                int height,
                int fps,
                PixelFormat format)
{
    int ret = 0;

    assert(thiz != NULL);

    thiz->dev_name = strdup(dev_name);
    thiz->width = width;
    thiz->height = height;
    thiz->fps = fps;
    thiz->in_size = width * height * 2;

    switch(format)
    {
        case PIX_FMT_MJPEG:
            thiz->format = V4L2_PIX_FMT_MJPEG;
            break;
        case PIX_FMT_YUYV:
            thiz->format = V4L2_PIX_FMT_YUYV;
            break;
        default:
            fprintf(stderr, "unsupported video format\n");
            camera_free_resource(thiz);
            return -1;
            break;
    }

    thiz->fd = -1;
    thiz->req_buf_info = NULL;
    thiz->streaming_state = STREAMING_OFF;

    return ret;
}

/* query informations of camera device */
void camera_open_query(CameraDevice *thiz)
{
    assert(thiz != NULL);

    camera_open_device(thiz);
    camera_query_cap(thiz);
    camera_query_cur_fps(thiz);
    camera_query_support_format(thiz);
}

void camera_open_set(CameraDevice *thiz)
{
    assert(thiz != NULL);

    camera_open_query(thiz);
    camera_set_format(thiz);
    camera_req_buf_and_mmap(thiz);
}

void camera_close(CameraDevice *thiz)
{
    assert(thiz != NULL);

    if (thiz->streaming_state == STREAMING_ON)
    {
        camera_stop_capture(thiz);
    }

    if (thiz->req_buf_info != NULL)
    {
        camera_deinit_device(thiz);
    }

    camera_close_device(thiz);
    camera_free_resource(thiz);
}

/* read the frame of camera from device buffer and handle it in callback function */
void camera_read_frame(CameraDevice *thiz, 
                       VideoCallBack camera_data_callback,
                       void *ctx)
{
    assert(thiz != NULL);

    if (thiz->streaming_state == STREAMING_OFF)
    {
        camera_start_capture(thiz);
    }

    if (camera_is_read_ready(thiz))
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
}

