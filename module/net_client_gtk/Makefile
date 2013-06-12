CC = gcc
CFLAGS = -g -Wall #-DDEBUG
LIBS = -ljpeg
GTK_FLAGS = `pkg-config gtk+-2.0 --cflags --libs`
TARGET = client
OBJS = client.o \
	   color.o \
	   decoder_mjpeg.o \
	   decoder_yuv422.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(GTK_FLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(GTK_FLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.swp $(TARGET)
