CC=gcc 
NAME=social_sir


.PHONY: all clean

all: $(NAME)

clean:
	rm $(NAME) $(NAME).o

$(NAME): $(NAME).o
	$(CC) -Wall -o $@ $< -lm

%.o: %.c
	$(CC) -Wall -c $< -o $@

