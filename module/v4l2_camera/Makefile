CC = gcc
CFLAGS = -g -Wall
LIBS = -ljpeg
GTK_FLAGS = `pkg-config gtk+-2.0 --cflags --libs`
OBJS = main.o \
	   capture.o \
	   decoder_mjpeg.o \
	   decoder_yuv422.o \
	   color.o
TARGET = camera

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(GTK_FLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS) $(GTK_FLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.swp $(TARGET)
