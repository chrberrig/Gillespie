CC=gcc 

.PHONY: all clean

all: gillespie

clean:
	rm gillespie

%: %.c
	$(CC) -o $@ $>

# % : %.o
# 	$(CC) -c -o $@ $>
