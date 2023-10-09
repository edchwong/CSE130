#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>

#define BUF_SIZE       4096
#define Method_length  8
#define Max_URI_length 19

#define OPTIONS              "t:l:"
#define DEFAULT_THREAD_COUNT 4

enum type { none, get, put, append };

enum state { E, MUV, H, processing, done };

typedef struct Request {
    int fd;
    char URI[BUF_SIZE];
    int URI_len;
    int content_length;
    char is_content_length_set;
    int v1;
    int v2;
    int method;
    int state;
    int id;
    FILE *logfile;
} Request;

Request newRequest(int connfd);