/**
 * File: jpeg_encoder_mjpeg.c
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The encoder program which convert mjpeg to jpeg.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman.h"
#include "jpeg_encoder.h"

static int is_huffman(unsigned char *buf)
{
    int i = 0;
    unsigned char *pbuf = buf;

    while (((pbuf[0] << 8) | pbuf[1]) != 0xffda)
    {
        if (i++ > 2048)
        {
            return 0;
        }
        
        if (((pbuf[0] << 8) | pbuf[1]) == 0xffc4)
        {
            return 1;
        }

        pbuf++;
    }

    return 0;
}

int jpeg_encoder_mjpeg(unsigned char *out_buf, 
                       unsigned char *in_buf, 
                       int in_size)
{
    int pos = 0;
    int size_start = 0;
    int processed_size = 0;
    unsigned char *pdeb = in_buf;
    unsigned char *pcur = in_buf;
    unsigned char *plimit = in_buf + in_size;

    if (is_huffman(in_buf))
    {
        memcpy(out_buf, in_buf, in_size);
        processed_size = in_size;
    }
    else
    {
        /* find the SOF0(Start Of Frame 0) of JPEG */
        while ( (((pcur[0] << 8) | pcur[1]) != 0xffc0) && (pcur < plimit) )
        {
            pcur++;
        }

        /* SOF0 of JPEG exist */
        if (pcur >= plimit)
        {
            return 0;
        }

        /* insert huffman table after SOF0 */
        size_start = pcur - pdeb;
        memcpy(out_buf, in_buf, size_start);
        pos += size_start;

        memcpy(out_buf + pos, dht_data, sizeof(dht_data));
        pos += sizeof(dht_data);

        memcpy(out_buf + pos, pcur, in_size - size_start);
        pos += in_size - size_start;

        processed_size = pos;
    }

    return processed_size;
}
