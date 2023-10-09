#include "queue.h"
#include <stdio.h>

BoundedQueue queue_new(int capacity) {
    BoundedQueue q;
    q.size = 0;
    q.head = 0;
    q.tail = 0;
    q.buffer = malloc(capacity * sizeof(int));
    q.capacity = capacity;
    return q;
}

void enqueue(BoundedQueue *q, int x) {
    q->buffer[q->tail] = x;
    q->tail = (q->tail + 1) % q->capacity;
    q->size += 1;
}

void dequeue(BoundedQueue *q, int *x) {
    *x = q->buffer[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size -= 1;
}

bool queue_full(BoundedQueue *q) {
    return q->size == q->capacity;
}

bool queue_empty(BoundedQueue *q) {
    return q->size == 0;
}

void queue_print(BoundedQueue *q) {
    printf("[");
    for (int i = 0; i < q->size; i += 1) {
        printf("%d", q->buffer[(q->head + i) % q->capacity]);
        if (i + 1 != q->size) {
            printf(", ");
        }
    }
    printf("]\n");
}