/**
 * File: video_container.h
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The video data container with processed format.
 */

#ifndef _VIDEO_CONTAINER_H_
#define _VIDEO_CONTAINER_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*FrameProcessFunc)(unsigned char *out_buf,
                                unsigned char *in_buf,
                                int in_size,
                                int in_format,
                                int out_format);

typedef void (*DataRequestFunc)(unsigned char *buf, int buf_size, void *ctx);

typedef struct _VideoContainer
{
    int buf_capacity;
    
    int raw_size;
    unsigned char *raw_buffer;

    int processed_size;
    unsigned char *processed_buffer;

    pthread_mutex_t db;
    pthread_cond_t  db_update;
}VideoContainer;

int video_container_init(VideoContainer *container,
                         int buf_capacity);
void video_container_destroy(VideoContainer *container);

void video_container_input_raw(VideoContainer *container,
                               unsigned char *raw_buffer, 
                               int raw_size);
void video_container_requestdb(VideoContainer *container,
                               DataRequestFunc requesedb_callback,
                               void *ctx);
void video_container_updatedb(VideoContainer *container,
                              FrameProcessFunc frame_process,
                              int in_format,
                              int out_format); 
void video_container_releasedb(VideoContainer *container);

#ifdef __cplusplus
}
#endif

#endif
