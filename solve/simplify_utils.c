#include "simplify_utils.h"

#include <stddef.h>
#include <stdlib.h>

Node *try_fold_consts(Node *node, const char op, bool *changed) {
    if (!(
        node->l && node->l->t.type == TOK_VAL &&
        node->r && node->r->t.type == TOK_VAL
    )) {
        return node;
    }

    const double a = node->l->t.data.val;
    const double b = node->r->t.data.val;
    double res = 0;
    bool valid_op = true;

    if (op == '+') {
        res = a + b;
    } else if (op == '-') {
        res = a - b;
    } else if (op == '*') {
        res = a * b;
    } else if (op == '/' && b != 0) {
        res = a / b;
    } else {
        valid_op = false;
    }

    if (valid_op) {
        free_tree(node->l);
        free_tree(node->r);
        node->t.type = TOK_VAL;
        node->t.data.val = res;
        node->l = NULL;
        node->r = NULL;
        *changed = true;
    }
    return node;
}

// Заменяет одну ноду на другую (удалив первую)
static Node *rebase_node(Node *node, Node *keep_child, Node *free_child, bool *changed) {
    free_tree(free_child);
    const Node temp = *keep_child;
    *node = temp;
    free(keep_child);
    *changed = true;
    return node;
}

Node *try_simplify_trash(Node *node, const char op, bool *changed) {
    // x + 0 -> x
    if (op == '+' && node->r && node->r->t.type == TOK_VAL && node->r->t.data.val == 0.0) {
        return rebase_node(node, node->l, node->r, changed);
    }
    // 0 + x -> x
    if (op == '+' && node->l && node->l->t.type == TOK_VAL && node->l->t.data.val == 0.0) {
        return rebase_node(node, node->r, node->l, changed);
    }
    // x - 0 -> x
    if (op == '-' && node->r && node->r->t.type == TOK_VAL && node->r->t.data.val == 0.0) {
        return rebase_node(node, node->l, node->r, changed);
    }
    // x * 1 -> x
    if (op == '*' && node->r && node->r->t.type == TOK_VAL && node->r->t.data.val == 1.0) {
        return rebase_node(node, node->l, node->r, changed);
    }
    // 1 * x -> x
    if (op == '*' && node->l && node->l->t.type == TOK_VAL && node->l->t.data.val == 1.0) {
        return rebase_node(node, node->r, node->l, changed);
    }
    // x * 0 -> 0  ||  0 * x -> 0
    if (op == '*') {
        const bool is_left_zero = node->l && node->l->t.type == TOK_VAL && node->l->t.data.val == 0.0;
        const bool is_right_zero = node->r && node->r->t.type == TOK_VAL && node->r->t.data.val == 0.0;
        if (is_left_zero || is_right_zero) {
            free_tree(node->l);
            free_tree(node->r);
            node->t.type = TOK_VAL;
            node->t.data.val = 0.0;
            node->l = NULL;
            node->r = NULL;
            *changed = true;
            return node;
        }
    }

    return node;
}
