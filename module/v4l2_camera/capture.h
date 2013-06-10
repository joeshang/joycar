/**
 * File: capture.h
 * Author: Joe Shang
 * Brief: The camera capture program with v4l2 framework
 * Capturing Process:
 *  1. camera_open_device(): Open camera
 *  2. camera_query_cap(): Query camera's capability.
 *  3. camera_query_stream(): Query camera's output format.
 *  4. camera_query_support_format(): Query camera's output format.
 *  5. camera_set_format(): Set the output format of camera.
 *  6. camera_req_buf_and_mmap(): Request camera data buffers in kernel space and mmap buffers to user space.
 *  7. camera_start_capture(): Device begin to capture camera.
 *  8. camera_is_read_ready(): Query whether the camera data is ready or not.
 *  9. camera_read_frame(): Read frame of camera and handle it in callback function(VideoCallBack).
 *  10.camera_stop_capture(): Camera stop to capture video.
 *  11.camera_deinit_device(): Deinitialize camera.
 *  12.camera_close_device(): Close camera.
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
    char *dev_name;
    int width;
    int height;
    int fps;
    int format;

    StreamingState streaming_state;

    int fd;
    ReqBufInfo *req_buf_info;
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
