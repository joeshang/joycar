CC = gcc
CFLAGS = -g -Wall
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0 gmodule-2.0`
LIBS = `pkg-config --libs gtk+-3.0 gmodule-2.0` -pthread -ljpeg
TARGET = control_platform
OBJS = main.o\
	   callbacks_gui.o\
	   callbacks_con.o\
	   connector_net.o\
	   connector_serial.o\
	   serial_driver.o\
	   color.o\
	   decoder_mjpeg.o\
	   decoder_yuv422.o

control_platform: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(GTK_CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.swp $(OBJS) $(TARGET)
