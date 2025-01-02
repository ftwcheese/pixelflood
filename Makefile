CFLAGS = -std=c11 -pedantic -Wall -D_XOPEN_SOURCE=700
CC=gcc

.PHONY: all clean

all: main lzw

clean:
	rm -f main
	rm -f lzw

%.o: %.c
	$(CC) $(CFLAGS) -c $<
	

main: main.o
	$(CC) -o main main.o 
lzw: lzw.o
	$(CC) -o lzw lzw.o