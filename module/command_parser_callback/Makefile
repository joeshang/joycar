CC = gcc
CFLAGS = -g -Wall
TARGETS = command
OBJS = command_extractor.o \
	   command_parser.o \
	   main.o

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf *.o *.swp $(TARGETS)
