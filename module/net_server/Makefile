CC = gcc
CFLAGS = -g -Wall
TARGETS = server
OBJS = server.o

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $<

%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf *.o *.swp $(TARGETS)
