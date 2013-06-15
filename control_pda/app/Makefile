CC = arm-linux-gcc
CFLAGS = -g -Wall -pthread# -DDEBUG -DDECODER_DEBUG
LIBS = -ljpeg
TARGET = control_pda
OBJS = main.o\
	   color.o\
	   decoder_mjpeg.o\
	   decoder_yuv422.o\
	   key_control.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o *.swp $(TARGET)
