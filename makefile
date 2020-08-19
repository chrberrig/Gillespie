CC=gcc 
NAME=social_sir


.PHONY: all clean

all: $(NAME)

clean:
	rm $(NAME) 
	#rm $(NAME).o

%: %.c
	$(CC) -Wall -o $@ $< -lm

# $(NAME): $(NAME).o
# 	$(CC) -Wall -o $@ $< -lm
# 
# %.o: %.c
# 	$(CC) -Wall -o $@ -c $<

