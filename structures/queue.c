#include "queue.h"
#include <stdlib.h>

#include "../utils/utils.h"

Queue *queue_create() {
    Queue *q = malloc(sizeof(Queue));

    if (!q) {
        fatal_memory_error();
        return NULL; // stfu IDE
    }

    q->head = q->tail = NULL;
    q->size = 0;

    return q;
}

bool queue_is_empty(const Queue *q) {
    return q == NULL || q->head == NULL;
}

void queue_push(Queue *q, const Token t) {
    if (!q) return;

    QueueItem *item = malloc(sizeof(QueueItem));
    if (!item) {
        fatal_memory_error();
        return; // stfu ide
    }

    item->data = t;
    item->next = NULL;

    if (q->tail == NULL) {
        q->head = q->tail = item;
    } else {
        q->tail->next = item;
        q->tail = item;
    }
    q->size++;
}

Token queue_pop(Queue *q) {
    if (queue_is_empty(q)) {
        fatal_error("can't pop from empty queue");
    }

    QueueItem *temp = q->head;
    Token data = temp->data;

    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }

    free(temp);
    q->size--;
    return data;
}

void queue_destroy(Queue *q) {
    if (!q) return;
    while (!queue_is_empty(q)) {
        queue_pop(q);
    }
    free(q);
}
