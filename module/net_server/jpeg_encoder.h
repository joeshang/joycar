/**
 * File: jpeg_encoder.h
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The encoder program which convert other formats to jpeg.
 */

#ifndef _JPEG_ENCODER_H_
#define _JPEG_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

int jpeg_encoder_yuv422(unsigned char *out_buf, 
                        unsigned char *in_buf, 
                        int in_size,
                        int width,
                        int height,
                        int quality);

int jpeg_encoder_mjpeg(unsigned char *out_buf, 
                       unsigned char *in_buf, 
                       int in_size);

#ifdef __cplusplus
}
#endif

#endif
