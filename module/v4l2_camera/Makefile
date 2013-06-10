CC = gcc
CFLAGS = -g -Wall
LIBS = -ljpeg
GTK_FLAGS = `pkg-config gtk+-2.0 --cflags --libs`
OBJS = main.o \
	   capture.o \
	   yuv422_rgb24.o \
	   jpeg_rgb24.o
TARGET = camera

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(GTK_FLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS) $(GTK_FLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.swp $(TARGET)
