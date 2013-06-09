/**
 * File: jpeg_rgb24.c
 * Author: Joe Shang
 * Brief: Convert JPEG to RGB24 format.
 */

#include <stdio.h>
#include <setjmp.h>

#include "jpeglib.h"
#include "jerror.h"

static struct jpeg_decompress_struct cinfo;
static struct jpeg_error_mgr jerr;

void jpeg_rgb24(unsigned char *rgb_buf, unsigned char *jpeg_buf, unsigned int jpeg_size)
{
    unsigned char *rgb_line;

    /* step 1: allocate and initialize JPEG decompression object */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    /* step 2: specify data source */
    jpeg_mem_src(&cinfo, jpeg_buf, jpeg_size);

    /* step 3: read file parameter of jpeg */
    (void)jpeg_read_header(&cinfo, TRUE);

    /* step 4: set parameters for decompression(optional) */
    /* output 1/2 scale image 
     * cinfo.scale_num = 1;
     * cinfo.scale_denom = 2; */

    /* step 5: begin to decompress */
    (void)jpeg_start_decompress(&cinfo);


    //printf("\n");
    //printf("width: %d\n", cinfo.output_width);
    //printf("height: %d\n", cinfo.output_height);
    //printf("components: %d\n", cinfo.output_components);

    /* step 6: read scan lines */
    int cur_line = 0;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        rgb_line = rgb_buf + cur_line * cinfo.output_width * cinfo.output_components;
        (void)jpeg_read_scanlines(&cinfo, 
                &rgb_line,
                1);
        cur_line++;
    }

    /* step 7: finish decompression */
    (void)jpeg_finish_decompress(&cinfo);

    /* step 8: release JPEG decompression object */
    jpeg_destroy_decompress(&cinfo);
}
