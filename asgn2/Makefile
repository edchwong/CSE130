CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

all : httpserver

httpserver : httpserver.o connection.o requests.o process.o
	$(CC) $(CFLAGS) -g -o httpserver httpserver.o connection.o requests.o process.o

httpserver.o : 
	$(CC) $(CFLAGS) -g -c httpserver.c

connection.o : 
	$(CC) $(CFLAGS) -g -c connection.c 

requests.o :
	$(CC) $(CFLAGS) -g -c requests.c 

process.o :
	$(CC) $(CFLAGS) -g -c process.c

clean :
	rm -rf httpserver httpserver.o connection.o requests.o process.o

format :
	clang-format -i -style=file *.[ch]