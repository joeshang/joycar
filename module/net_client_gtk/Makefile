CC = gcc
CFLAGS = -g -Wall
GTK_FLAGS = `pkg-config gtk+-2.0 --cflags --libs`
TARGET = client
OBJS = yuv422_rgb.o\
	   client.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(GTK_FLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(GTK_FLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.swp $(OBJS) $(TARGET)
