#ifndef EXPR_SOLVE_H
#define EXPR_SOLVE_H

#include "../structures/queue.h"

Queue *postfix_queue_from_input_expr();

Node *build_tree_from_queue(Queue *q);

#endif //EXPR_SOLVE_H
