CC = gcc
CFLAGS = -g -Wall -o
TARGETS = serial

all: $(TARGETS)

serial: main.c serial.c
	$(CC) $(CFLAGS) $@ $^

.PHONY: clean
clean:
	rm -rf *.o *.swp $(TARGETS)
