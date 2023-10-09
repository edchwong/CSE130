CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

all : httpserver

httpserver : httpserver.o connection.o requests.o process.o queue.o
	$(CC) $(CFLAGS) -g -pthread -o httpserver httpserver.o connection.o requests.o process.o queue.o 

httpserver.o : 
	$(CC) $(CFLAGS) -g -c httpserver.c

connection.o : 
	$(CC) $(CFLAGS) -g -c connection.c 

requests.o :
	$(CC) $(CFLAGS) -g -c requests.c 

process.o :
	$(CC) $(CFLAGS) -g -c process.c

queue.o :
	$(CC) $(CFLAGS) -g -c queue.c 

clean :
	rm -rf httpserver *.o

format :
	clang-format -i -style=file *.[ch]