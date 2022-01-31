CC=gcc
CFLAGS=-Wall -Wextra --std=c99 -g -I./inc -Werror=div-by-zero
SRCS=$(wildcard src/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))

all: main

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJS)
	$(CC) $^ $(CFLAGS) -o $@
	
clean:
	rm -rf src/*.o main