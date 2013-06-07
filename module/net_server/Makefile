CC = gcc
CFLAGS = -g -Wall -pthread
TARGETS = server
OBJS = server.o \
	   command_extractor.o \
	   command_parser.o \
	   capture.o \
	   serial.o

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf *.o *.swp $(TARGETS)
