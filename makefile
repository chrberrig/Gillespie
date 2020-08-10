CC=gcc 

.PHONY: all clean

all: test

clean:
	rm test

test: gillespie.c
	$(CC) -o $@ gillespie.c
