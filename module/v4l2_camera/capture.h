/**
 * File: capture.h
 * Author: Joe Shang
 * Brief: The video capture program with v4l2 framework
 * Capturing Process:
 *  1. video_open_device(): Open video device.
 *  2. video_query_cap(): Query device's capability.
 *  3. video_query_format(): Query device's output format.
 *  4. video_init_device(): Initialize device, incluing:
 *      - video_set_format(): Set the output format of device.
 *      - video_req_buf_and_mmap(): Request video data buffers in kernel space and mmap buffers to user space.
 *  5. video_start_capture(): Device begin to capture video.
 *  6. video_is_read_ready(): Query whether the device data is ready or not.
 *  7. video_read_frame(): Read frame from device buffer and handle it in callback function(VideoCallBack).
 *  8. video_stop_capture(): Device stop to capture video.
 *  9. video_deinit_device(): Deinitialize device.
 *  10. video_close_device(): Close video device.
 */

#ifndef _CAPTURE_H_
#define _CAPTURE_H_

#define DEV_WIDTH       320
#define DEV_HEIGHT      240
#define DEV_FORMAT      V4L2_PIX_FMT_YUYV
#define BYTES_PIXEL     2
#define VIDEO_BUF_SIZE  ((DEV_WIDTH) * (DEV_HEIGHT) * (BYTES_PIXEL))
#define REQ_BUF_CNT     4

typedef void (*VideoCallBack)(void *ctx, void *buf_start, int buf_size);

void video_open_device(char *dev_name);
void video_query_cap();
void video_query_format();
void video_init_device();
void video_start_capture();
void video_read_frame(void *ctx, VideoCallBack video_data_callback);
int  video_is_read_ready();
void video_stop_capture();
void video_deinit_device();
void video_close_device();

#endif
