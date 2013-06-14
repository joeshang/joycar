CC = gcc
CFLAGS = -g -Wall -pthread
LIBS = -ljpeg
TARGETS = server
OBJS = server.o \
	   capture.o \
	   command_extractor.o \
	   command_parser_stub.o \
	   video_container.o \
	   jpeg_encoder_mjpeg.o \
	   jpeg_encoder_yuv422.o

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf *.o *.swp $(TARGETS)
