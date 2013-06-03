/**
 * File: yuv422_rgb.h
 * Author: Joe Shang
 * Brief: Convert YUV422 to RGB24 format.
 */

#ifndef _YUV422_RGB_H_
#define _YUV422_RGB_H_

int  yuv422_rgb24(
        unsigned char *yuv_buf, 
        unsigned char *rgb_buf, 
        unsigned int width, 
        unsigned int height);

#endif
