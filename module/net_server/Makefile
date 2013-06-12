CC = gcc
CFLAGS = -g -Wall -pthread
TARGETS = server
OBJS = server.o \
	   capture.o \
	   command_extractor.o \
	   command_parser_stub.o

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf *.o *.swp $(TARGETS)
