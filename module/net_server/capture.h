/**
 * File: capture.h
 * Author: Joe Shang
 * Brief: The video capture program with v4l2 framework
 */

#ifndef _CAPTURE_H_
#define _CAPTURE_H_

#define DEV_NAME        "/dev/video0"
#define DEV_WIDTH       320
#define DEV_HEIGHT      240
#define DEV_FORMAT      V4L2_PIX_FMT_YUYV

#define BYTES_PIXEL     2
#define VIDEO_BUF_SIZE  ((DEV_WIDTH) * (DEV_HEIGHT) * (BYTES_PIXEL))
#define REQ_BUF_CNT     4

typedef void (*VideoCallBack)(void *ctx, void *buf_start, int buf_size);

void video_open_device();
void video_init_device();
void video_start_capture();
void video_read_frame(void *ctx, VideoCallBack video_data_callback);
int  video_is_read_ready();
void video_stop_capture();
void video_deinit_device();
void video_close_device();

#endif
