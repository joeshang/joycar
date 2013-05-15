CC = gcc
CFLAGS = -g -Wall
GTK_CFLAGS = `pkg-config --cflags --libs gtk+-2.0`
TARGETS = control

all: $(TARGETS)

control: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(GTK_CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.swp $(TARGETS)
