CC = gcc
CFLAGS = -g -Wall
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0 gmodule-2.0`
LIBS = `pkg-config --libs gtk+-3.0 gmodule-2.0` -pthread
OBJS = main.o gui_callbacks.o con_callbacks.o connector_serial.o serial_driver.o

joycar_serial: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(GTK_CFLAGS)

gui_callbacks.o: gui_callbacks.c
	$(CC) $(CFLAGS) -c $< -o $@ $(GTK_CFLAGS)

con_callbacks.o: con_callbacks.c con_callbacks.h
	$(CC) $(CFLAGS) -c $< -o $@ $(GTK_CFLAGS)

connector_serial.o: connector_serial.c connector_serial.h connector.h typedef.h
	$(CC) $(CFLAGS) -c $< -o $@

serial_driver.o: serial_driver.c serial_driver.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o *.swp $(OBJS) joycar_serial
