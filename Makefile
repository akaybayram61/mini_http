CC=gcc
CFLAGS=-Wall -Wextra --std=c99 -g -I./inc
SRCS=$(wildcard src/*.c)
OBJS=$(patsubst %.o, %.c, $(SRCS))

all: main

%.o: %.c
	$(CC) -o $@ $<

main: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm -rf src/*.o main