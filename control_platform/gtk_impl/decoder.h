/**
 * File: decoder.h
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The decoder interface.
 */

#ifndef _DECODER_H_
#define _DECODER_H_

#include <stdlib.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _Decoder;
typedef struct _Decoder Decoder;

struct _Decoder
{
    void (*decode)(Decoder *thiz,
            unsigned char *out_buf,
            unsigned char *in_buf,
            int buf_size);
    void (*destroy)(Decoder *thiz);

    char priv[];
};

static inline void decoder_decode(Decoder *thiz,
        unsigned char *out_buf,
        unsigned char *in_buf,
        int buf_size)
{
    assert(thiz != NULL && thiz->decode != NULL);

    thiz->decode(thiz, out_buf, in_buf, buf_size);
}

static inline void decoder_destroy(Decoder *thiz)
{
    assert(thiz != NULL && thiz->destroy != NULL);

    thiz->destroy(thiz);
}

#ifdef __cplusplus
}
#endif

#endif
