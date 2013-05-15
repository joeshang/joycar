/**
 * File: typedef.h
 * Author: Joe Shang
 * Brief: Common type definition.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

typedef enum _Ret
{
    RET_OK,
    RET_OOM,
    RET_STOP,
    RET_INVALID_PARAMS,
    RET_FAIL
}Ret;

#ifdef __cplusplus
#define DECLS_BEGIN extern "C" {
#define DECLS_END   }
#else
#define DECLS_BEGIN
#define DECLS_END  
#endif

#define return_if_fail(p) if(!(p)) \
    { printf("%s:%d Warning: "#p" failed.\n", \
        __func__, __LINE__); return; }

#define return_val_if_fail(p, ret) if(!(p)) \
    { printf("%s:%d Warning: "#p" failed.\n", \
        __func__, __LINE__); return (ret); }


#endif
