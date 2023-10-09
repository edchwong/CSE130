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
#include <err.h>
#include <errno.h>
#include "requests.h"

void processURI(Request *R);

void processGET(int connfd, Request *R);

void processPUT(int connfd, Request *R, char *remaining_buffer, int remaining_buf_size);

void processAPPEND(int connfd, Request *R, char *remaining_buffer, int remaining_buf_size);

void processLog(Request *R, int statuscode);

void processReply(int connfd, Request *R, int statuscode);