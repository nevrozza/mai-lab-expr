#include "stack_generic.h"

#include <stdlib.h>
#include "../utils/utils.h"

#define INITIAL_CAPACITY 4

#define DEFINE_STACK(Type, Name) \
Name* Name##_create() { \
    Name *s = malloc(sizeof(Name)); \
    if (!s) { \
        fatal_memory_error(); \
    } else { \
        s->data = malloc(INITIAL_CAPACITY * sizeof(Type)); \
        s->capacity = INITIAL_CAPACITY; \
        s->count = 0; \
    } \
    return s; \
} \
\
void Name##_push(Name *s, Type item) { \
    if (s->count == s->capacity) { \
        size_t new_cap = s->capacity * 2; \
        Type *temp = realloc(s->data, new_cap * sizeof(Type)); \
        if (!temp) { \
            fatal_memory_error(); \
        } \
        s->data = temp; \
        s->capacity = new_cap; \
    } \
        s->data[s->count++] = item; \
    } \
\
Type Name##_pop(Name *s) { \
    if (Name##_is_empty(s)) { \
        fatal_error("can't pop from empty stack (" #Name ")"); \
    } \
    return s->data[--s->count]; \
} \
\
Type Name##_peek(Name *s) { \
    if (Name##_is_empty(s)) { \
        fatal_error("can't peek in empty stack (" #Name ")"); \
    } \
    return s->data[s->count - 1]; \
} \
\
bool Name##_is_empty(Name *s) { \
    return s == NULL || s->count == 0; \
} \
void Name##_destroy(Name *s) { if (s) { free(s->data); free(s); } }

DEFINE_STACK(Token, TokenStack)
DEFINE_STACK(Node*, NodeStack)
