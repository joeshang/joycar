CC = gcc
CFLAGS = -g -Wall -o
TARGETS = command_parser

all: $(TARGETS)

command_parser: main.c command_parser.c construct_command.c
	$(CC) $(CFLAGS) $@ $^

.PHONY: clean
clean:
	rm -rf *.o *.swp $(TARGETS)
