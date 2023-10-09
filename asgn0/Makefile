CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

all : split

split : split.o
	$(CC) $(CFLAGS) -g -o split split.o

split.o : 
	$(CC) $(CFLAGS) -g -c split.c

clean :
	rm -rf split split.o