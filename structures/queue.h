#ifndef EXPR_QUEUE_H
#define EXPR_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include "data.h"

typedef struct QueueItem {
    Token data;
    struct QueueItem *next;
} QueueItem;

typedef struct Queue {
    QueueItem *head;
    QueueItem *tail;
    size_t size;
} Queue;

Queue* queue_create();
void queue_push(Queue *q, Token t);
Token queue_pop(Queue *q);
bool queue_is_empty(const Queue *q);
void queue_destroy(Queue *q);

#endif //EXPR_QUEUE_H