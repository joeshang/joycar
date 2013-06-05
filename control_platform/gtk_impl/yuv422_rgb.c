/**
 * File: yuv422_rgb.c
 * Author: Joe Shang
 * Brief: Convert YUV422 to RGB24 format.
 */

/*
 * YUV422 也称 YUYV格式，在内存中存储方式为 Y0 U0 Y1 V0	
 * 这四个字节代表两个像素：Y0 U0 VO 和 Y1 U0 V0
 * 	
 * R = Y + 1.4075*(V-128)
 * G = Y - 0.3455*(U-128) - 0.7169*(V-128)
 * B = Y +1.779 *(U-128)

 * 快速算法
 * U' = U -128
 * V' = V - 128
 * R = Y + V' + ((V'*104) >> 8))
 * G = Y - ((U'*89) >> 8) - ((V' * 183) >> 8)
 * B = Y + U' + ((U'*199) >> 8)
 */ 

#define Y0	0
#define U	1
#define Y1	2
#define	V	3

#define R	0
#define G	1
#define	B	2

int yuv422_rgb24(unsigned char *yuv_buf, unsigned char *rgb_buf, unsigned int width, unsigned int height)
{
	int yuvdata[4];
	int rgbdata[3];
	unsigned char *rgb_temp;
 	unsigned int i, j;

	rgb_temp = rgb_buf;
	for (i = 0; i < height * 2; i++) 
    {
		for (j = 0; j < width; j += 4)
        {
			/* get Y0 U Y1 V */
			yuvdata[Y0] = *(yuv_buf + i * width + j + 0);
			yuvdata[U]  = *(yuv_buf + i * width + j + 1);
			yuvdata[Y1] = *(yuv_buf + i * width + j + 2);
			yuvdata[V]  = *(yuv_buf + i * width + j + 3);

			/* the first pixel */
			rgbdata[R] = yuvdata[Y0] + (yuvdata[V] - 128) + (((yuvdata[V] - 128) * 104 ) >> 8);
			rgbdata[G] = yuvdata[Y0] - (((yuvdata[U] - 128) * 89) >> 8) - (((yuvdata[V] - 128) * 183) >> 8);
			rgbdata[B] = yuvdata[Y0] + (yuvdata[U] - 128) + (((yuvdata[U] - 128) * 199) >> 8);


			if (rgbdata[R] > 255)  rgbdata[R] = 255;		 
			if (rgbdata[R] < 0) rgbdata[R] = 0;			
			if (rgbdata[G] > 255)  rgbdata[G] = 255;		 
			if (rgbdata[G] < 0) rgbdata[G] = 0;			
			if (rgbdata[B] > 255)  rgbdata[B] = 255;		 
			if (rgbdata[B] < 0) rgbdata[B] = 0;			
			
			*(rgb_temp++) = rgbdata[R] ;
			*(rgb_temp++) = rgbdata[G];
			*(rgb_temp++) = rgbdata[B];
			
			/* the second pix */ 
			rgbdata[R] = yuvdata[Y1] + (yuvdata[V] - 128) + (((yuvdata[V] - 128) * 104 ) >> 8);
			rgbdata[G] = yuvdata[Y1] - (((yuvdata[U] - 128) * 89) >> 8) - (((yuvdata[V] - 128) * 183) >> 8);
			rgbdata[B] = yuvdata[Y1] + (yuvdata[U] - 128) + (((yuvdata[U] - 128) * 199) >> 8);

			
			if (rgbdata[R] > 255)  rgbdata[R] = 255;		 
			if (rgbdata[R] < 0) rgbdata[R] = 0;			
			if (rgbdata[G] > 255)  rgbdata[G] = 255;		 
			if (rgbdata[G] < 0) rgbdata[G] = 0;			
			if (rgbdata[B] > 255)  rgbdata[B] = 255;		 
			if (rgbdata[B] < 0) rgbdata[B] = 0;		
	
			*(rgb_temp++) = rgbdata[R];
			*(rgb_temp++) = rgbdata[G];
			*(rgb_temp++) = rgbdata[B];
		
		}
	}
	return 0;
}

