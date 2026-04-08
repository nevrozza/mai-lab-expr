#ifndef EXPR_STACK_GENERIC_H
#define EXPR_STACK_GENERIC_H

#include "data.h"
#include <stddef.h>
#include <stdbool.h>

#define DECLARE_STACK(Type, Name) \
typedef struct { \
    Type *data; \
    size_t capacity; \
    size_t count; \
} Name; \
\
Name* Name##_create(); \
void Name##_push(Name *s, Type item); \
Type Name##_pop(Name *s); \
Type Name##_peek(Name *s); \
bool Name##_is_empty(Name *s); \
void Name##_destroy(Name *s);


DECLARE_STACK(Token, TokenStack)
DECLARE_STACK(Node*, NodeStack)

#endif //EXPR_STACK_GENERIC_H
