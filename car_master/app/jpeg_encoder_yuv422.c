/**
 * File: jpeg_encoder_yuv422.c
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The encoder program which convert yuv422 to jpeg. Available for libjpeg version >= 8 which has jpeg_mem_src and jpeg_mem_dest interface.
 */

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include <jpeglib.h>
#include <jerror.h>

int jpeg_encoder_yuv422(unsigned char *out_buf, 
                        unsigned char *in_buf, 
                        int in_size,
                        int width,
                        int height,
                        int quality)
{
    int z;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    unsigned char *rgb_line_buffer = NULL;
    unsigned long out_size = in_size;

    /* step 1: allocate and initialize JPEG compression object */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    /* step 2: specify the data source */
    jpeg_mem_dest(&cinfo, &out_buf, &out_size);

    /* step 3: set parameters for compression */
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    /* step 4: start compressor */
    (void)jpeg_start_compress(&cinfo, TRUE);

    /* step 5: scan lines of input buffer and compress lines with writing compressed data to data source which is set at step 2 */
    rgb_line_buffer = calloc(width * 3, 1);
    z = 0;
    while (cinfo.next_scanline < height)
    {
        int i;
        unsigned char *ptr = rgb_line_buffer;

        /* convert YUV422 to RGB888 */
        for (i = 0; i < width; i++)
        {
            int r, g, b;
            int y, u, v;

            /* get YUV value from input buffer */
            if (!z)
            {
                y = in_buf[0] << 8;
            }
            else
            {
                y = in_buf[2] << 8;
            }
            u = in_buf[1] - 128;
            v = in_buf[3] - 128;

            /* generate RGB value from YUV value */
            r = (y + (359 * v)) >> 8;
            g = (y - (88 * u) - (183 * v)) >> 8;
            b = (y + (454 * u)) >> 8;

            *(ptr++) = (r > 255) ? 255 : ((r < 0) ? 0 : r);
            *(ptr++) = (g > 255) ? 255 : ((g < 0) ? 0 : g);
            *(ptr++) = (b > 255) ? 255 : ((b < 0) ? 0 : b);

            if (z++)
            {
                z = 0;
                in_buf += 4;
            }
        }

        /* specify input line start point */
        row_pointer[0] = rgb_line_buffer;
        /* compress this line and write result to data source */
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    /* step 6: finish compression */
    jpeg_finish_compress(&cinfo);

    /* step 7: release JPEG compression object */
    jpeg_destroy_compress(&cinfo);

    free(rgb_line_buffer);

    return out_size;
}
