UNAME=$(shell uname)

CCFLAGS=-Wall -O3 -std=gnu99
CC=gcc

all: sjf

sjf: main.o queue.o sjf.o ctest.h sjftests.o
	$(CC) $(LDFLAGS) main.o queue.o sjf.o sjftests.o -o shortestjobfirst -lm

remake: clean all

%.o: %.c ctest.h
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f shortestjobfirst *.o
