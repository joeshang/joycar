/**
 * File: video_container.c
 * Author: Joe Shang (shangchuanren@gmail.com)
 * Brief: The video data container with processed format.
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "video_container.h"

int video_container_init(VideoContainer *container,
                         int buf_capacity)
{
    assert(container != NULL);

    container->buf_capacity = buf_capacity; 

    container->raw_size = 0;
    container->raw_buffer = malloc(buf_capacity);
    if (container->raw_buffer == NULL)
    {
        video_container_destroy(container);
        return -1;
    }

    container->processed_size = 0;
    container->processed_buffer = malloc(buf_capacity);
    if (container->processed_buffer == NULL)
    {
        video_container_destroy(container);
        return -1;
    }
    
    pthread_mutex_init(&container->db, NULL);
    pthread_cond_init(&container->db_update, NULL);

    return 0;
}

void video_container_input_raw(VideoContainer *container,
                               unsigned char *raw_buffer, 
                               int raw_size)
{
    assert(container != NULL);

    if (raw_size > container->buf_capacity)
    {
        container->raw_size = container->buf_capacity;
    }
    else
    {
        container->raw_size = raw_size;
    }

    memcpy(container->raw_buffer, raw_buffer, container->raw_size);
}

void video_container_updatedb(VideoContainer *container,
                              FrameProcessFunc frame_process,
                              int raw_format)
{
    assert(container != NULL && frame_process != NULL);

    pthread_mutex_lock(&container->db);

    container->processed_size = frame_process(container->processed_buffer, 
                                              container->raw_buffer, 
                                              container->raw_size, 
                                              raw_format);

    pthread_cond_broadcast(&container->db_update);
    pthread_mutex_unlock(&container->db);
}

void video_container_requestdb(VideoContainer *container,
                               DataRequestFunc requestdb_callback,
                               void *ctx)
{
    assert(container != NULL);

    /* wait for new frame coming after updatedb */
    pthread_mutex_lock(&container->db);
    pthread_cond_wait(&container->db_update, &container->db);

    requestdb_callback(container->processed_buffer, container->processed_size, ctx);

    pthread_mutex_unlock(&container->db);
}

void video_container_releasedb(VideoContainer *container)
{
    pthread_mutex_unlock(&container->db);
}

void video_container_destroy(VideoContainer *container)
{
    assert(container != NULL);

    if (container->raw_buffer != NULL)
    {
        free(container->raw_buffer);
    }
    container->raw_buffer = NULL;

    if (container->processed_buffer != NULL)
    {
        free(container->processed_buffer);
    }
    container->raw_buffer = NULL;

    pthread_cond_destroy(&container->db_update);
    pthread_mutex_destroy(&container->db);
}
