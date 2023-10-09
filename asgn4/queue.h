#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    int size;
    int head;
    int tail;
    int *buffer;
    int capacity;
} BoundedQueue;

BoundedQueue queue_new(int capacity);

void enqueue(BoundedQueue *q, int x);

void dequeue(BoundedQueue *q, int *x);

bool queue_full(BoundedQueue *q);

bool queue_empty(BoundedQueue *q);

void queue_print(BoundedQueue *q);
