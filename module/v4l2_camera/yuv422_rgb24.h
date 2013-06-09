/**
 * File: yuv422_rgb24.h
 * Author: Joe Shang
 * Brief: Convert YUV422 to RGB24 format.
 */

#ifndef _YUV422_RGB24_H_
#define _YUV422_RGB24_H_

#ifdef __cplusplus
extern "C" {
#endif

int  yuv422_rgb24(
        unsigned char *rgb_buf, 
        unsigned char *yuv_buf, 
        unsigned int width, 
        unsigned int height);

#ifdef __cplusplus
}
#endif

#endif
