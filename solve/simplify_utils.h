#ifndef EXPR_SIMPLIFY_UTILS_H
#define EXPR_SIMPLIFY_UTILS_H
#include <stdbool.h>
#include "../structures/data.h"


Node *try_fold_consts(Node *node, char op, bool *changed);

Node *try_simplify_trash(Node *node, char op, bool *changed);

#endif //EXPR_SIMPLIFY_UTILS_H
