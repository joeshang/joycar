CC = arm-linux-gcc
CFLAGS = -g -Wall -pthread -DDEBUG
TARGETS = car_master
OBJS = main.o \
	   capture.o \
	   serial.o \
	   command_extractor.o \
	   command_parser.o

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf *.o *.swp $(TARGETS)
