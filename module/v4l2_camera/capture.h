/**
 * File: capture.h
 * Author: Joe Shang
 * Brief: The camera capture program with v4l2 framework
*/

#ifndef _CAPTURE_H_
#define _CAPTURE_H_

typedef enum _StreamingState
{
    STREAMING_OFF,
    STREAMING_ON,
    STREAMING_PAUSED
}StreamingState;

typedef enum _PixelFormat
{
    PIX_FMT_MJPEG,
    PIX_FMT_YUYV
}PixelFormat;

/* record for mmap memory address */
typedef struct _ReqBufInfo
{
    void    *start;
    size_t  length;
}ReqBufInfo;

typedef struct _CameraDevice
{
    /* input attributes */
    char *dev_name;
    int width;
    int height;
    int fps;
    int in_size;
    int format;

    /* internal status */
    int fd;
    ReqBufInfo *req_buf_info;
    StreamingState streaming_state;
}CameraDevice;

typedef void (*VideoCallBack)(void *ctx, void *buf_start, int buf_size);

/* initialize camera device structure for operation */
int camera_init(CameraDevice *thiz,
                char *dev_name,
                int width,
                int height,
                int fps,
                PixelFormat format);

/* open camera and query informations of camera device */
void camera_open_query(CameraDevice *thiz);

/* open camera and set attributes of camera */
void camera_open_set(CameraDevice *thiz);

/* deinitialize and close camera */
void camera_close(CameraDevice *thiz);

/* read frame of camera with callback function */
void camera_read_frame(CameraDevice *thiz, 
                       VideoCallBack video_data_callback,
                       void *ctx);

#endif
