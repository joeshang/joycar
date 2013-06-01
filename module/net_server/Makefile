CC = gcc
CFLAGS = -g -Wall
TARGETS = server
OBJS = server.o command_extractor.o command_parser.o

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf *.o *.swp $(TARGETS)
