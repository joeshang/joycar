CC = gcc
CFLAGS = -g -Wall
TARGETS = connector_test

all: $(TARGETS)

connector_test: main.c serial_driver.c connector_serial.c
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

.PHONY: clean
clean:
	rm -rf *.o *.swp $(TARGETS)
