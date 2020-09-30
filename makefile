CC=cc 
NAME=social_sir


.PHONY: all clean

all: $(NAME)

clean:
	rm $(NAME) 
	#rm $(NAME).o
	rm sir_data.csv

%: %.c
	$(CC) -Wall -o $@ $< -lm

# $(NAME): $(NAME).o
# 	$(CC) -Wall -o $@ $< -lm
# 
# %.o: %.c
# 	$(CC) -Wall -o $@ -c $<

