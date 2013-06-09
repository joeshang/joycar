/**
 * File: jpeg_rgb24.h
 * Author: Joe Shang
 * Brief: Convert JPEG to RGB24 format.
 */

#ifndef _JPEG_RGB24_H_
#define _JPEG_RGB24_H_

#ifdef __cplusplus
extern "C" {
#endif

void jpeg_rgb24(
        unsigned char *rgb_buf,
        unsigned char *jpeg_buf,
        unsigned int jpeg_size);

#ifdef __cplusplus
}
#endif

#endif
