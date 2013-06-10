/**
 * File: main.c
 * Author: Joe Shang
 * Brief:
 */

#include <stdio.h>
#include <stdlib.h>

#include "capture.h"

#define CAM_WIDTH       320
#define CAM_HEIGHT      240

int main(int argc, char *argv[])
{
    CameraDevice camera;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s dev_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    camera_init(&camera, argv[1], CAM_WIDTH, CAM_HEIGHT, 15, PIX_FMT_YUYV);
    camera_open_query(&camera);
    camera_close(&camera);

    return 0;
}
