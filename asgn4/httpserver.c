#include <err.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include "connection.h"
#include "process.h"
#include "queue.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
//pthread_t * workers;
BoundedQueue *q;

bool cancel_threads = false;
int threads = DEFAULT_THREAD_COUNT;
static FILE *logfile;
#define LOG(...) fprintf(logfile, __VA_ARGS__)

// Converts a string to an 16 bits unsigned integer.
// Returns 0 if the string is malformed or out of the range.
static size_t strtouint16(char number[]) {
    char *last;
    long num = strtol(number, &last, 10);
    if (num <= 0 || num > UINT16_MAX || *last != '\0') {
        return 0;
    }
    return num;
}

// Creates a socket for listening for connections.
// Closes the program and prints an error message on error.
static int create_listen_socket(uint16_t port) {
    struct sockaddr_in addr;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        err(EXIT_FAILURE, "socket error");
    }
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *) &addr, sizeof addr) < 0) {
        err(EXIT_FAILURE, "bind error");
    }
    if (listen(listenfd, 128) < 0) {
        err(EXIT_FAILURE, "listen error");
    }
    return listenfd;
}

static void sigterm_handler(int sig) {
    if (sig == SIGTERM) {
        warnx("received SIGTERM");
        cancel_threads = true;
        /*for (int i=0; i<threads;i++){
            pthread_join(workers[i],NULL);
        }*/
        fclose(logfile);
        exit(EXIT_SUCCESS);
    }
}

static void usage(char *exec) {
    fprintf(stderr, "usage: %s [-t threads] [-l logfile] <port>\n", exec);
}

void *worker(void *arg) {
    while (cancel_threads == false) {
        q = (BoundedQueue *) arg;
        int x;

        pthread_mutex_lock(&lock);

        while (queue_empty(q)) {
            pthread_cond_wait(&fill, &lock);
        }

        dequeue(q, &x);

        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&empty);
        handle_connection(x, logfile);
        close(x);
    }

    if (q->buffer != NULL) {
        free(q->buffer);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int opt = 0;

    logfile = stderr;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't':
            threads = strtol(optarg, NULL, 10);
            if (threads <= 0) {
                errx(EXIT_FAILURE, "bad number of threads");
            }
            break;
        case 'l':
            logfile = fopen(optarg, "w");
            if (!logfile) {
                errx(EXIT_FAILURE, "bad logfile");
            }
            break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        warnx("wrong number of arguments");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port = strtouint16(argv[optind]);
    if (port == 0) {
        errx(EXIT_FAILURE, "bad port number: %s", argv[1]);
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, sigterm_handler);

    int listenfd = create_listen_socket(port);
    //LOG("port=%" PRIu16 ", threads=%d\n", port, threads);

    pthread_t worker_threads[threads];

    BoundedQueue q = queue_new(BUF_SIZE);

    for (int i = 0; i < threads; i++) {
        if (pthread_create(&worker_threads[i], NULL, worker, &q) != 0) {
            err(1, "pthread_create failed");
        }
    }
    //workers = &worker_threads;

    for (;;) {

        int connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) {
            warn("accept error\n");
            continue;
        }
        pthread_mutex_lock(&lock);
        while (queue_full(&q)) {
            pthread_cond_wait(&empty, &lock);
        }

        enqueue(&q, connfd);
        //queue_print(&q);
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&lock);
    }

    return EXIT_SUCCESS;
}
