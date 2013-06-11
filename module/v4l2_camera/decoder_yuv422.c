/**
 * File: decoder_yuv422.c
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The decoder of yuv422 format.
 */

#include <stdlib.h>

#include "color.h"

#include "decoder.h"
#include "decoder_yuv422.h"

static void decoder_yuv422_decode(Decoder *thiz, 
        unsigned char *out_buf, 
        unsigned char *in_buf,
        int buf_size)
{
	unsigned int i;
    unsigned int size;
	unsigned char Y, Y1, U, V;
	unsigned char *buff = in_buf;
	unsigned char *outp = out_buf;

	size = buf_size / 4;
	for (i = size; i > 0; i--) 
    {
		Y = buff[0] ;
		U = buff[1] ;
		Y1 = buff[2];
		V = buff[3];

		buff += 4;

		*outp++ = R_FROMYV(Y, V);
		*outp++ = G_FROMYUV(Y, U, V);
		*outp++ = B_FROMYU(Y, U);
			
		*outp++ = R_FROMYV(Y1, V);
		*outp++ = G_FROMYUV(Y1, U, V);
		*outp++ = B_FROMYU(Y1, U);
	}
	
}

static void decoder_yuv422_destroy(Decoder *thiz)
{
    freeLut();

    if (thiz != NULL)
    {
        free(NULL);
    }
}

Decoder *decoder_yuv422_create()
{
    Decoder *thiz = malloc(sizeof(Decoder));

    if (thiz != NULL)
    {
        initLut();

        thiz->decode = decoder_yuv422_decode;
        thiz->destroy = decoder_yuv422_destroy;
    }

    return thiz;
}
