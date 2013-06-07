# if you want to cross-compile net_server, you should:
# 1. modify CC = arm-linux-gcc
# 2. add -DHAS_SERIAL
# 3. switch command_parser_stub.o to command_parser.o

CC = gcc
CFLAGS = -g -Wall -pthread #-DHAS_SERIAL
TARGETS = server
OBJS = server.o \
	   capture.o \
	   serial.o \
	   command_extractor.o \
	   command_parser_stub.o #command_parser.o

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf *.o *.swp $(TARGETS)
