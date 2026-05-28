#include "tree_from_expr.h"

#include <stdlib.h>
#include "../utils/utils.h"
#include "../structures/stack_generic.h"

static int get_priority(const char sym) {
    if (sym == '*' || sym == '/') return 2;
    if (sym == '+' || sym == '-') return 1;
    return 0;
}

static Node *create_node(Token t, Node *r, Node *l) {
    Node *n = malloc(sizeof(Node));
    if (!n) {
        fatal_memory_error();
        return NULL;
    }
    n->tok = t;
    n->left = l;
    n->right = r;
    return n;
}

static void process_sym_token(Token t, TokenStack *ops, Queue *output) {
    if (t.data.sym == '(') {
        TokenStack_push(ops, t);
    } else if (t.data.sym == ')') {
        while (!TokenStack_is_empty(ops) &&
               (TokenStack_peek(ops).type != TOK_SYM || TokenStack_peek(ops).data.sym != '(')) {
            queue_push(output, TokenStack_pop(ops));
        }
        if (!TokenStack_is_empty(ops)) TokenStack_pop(ops);
    } else {
        while (!TokenStack_is_empty(ops) &&
               TokenStack_peek(ops).type == TOK_SYM &&
               TokenStack_peek(ops).data.sym != '(' &&
               get_priority(TokenStack_peek(ops).data.sym) >= get_priority(t.data.sym)) {
            queue_push(output, TokenStack_pop(ops));
        }
        TokenStack_push(ops, t);
    }
}

// shunting yard (Deikstra)
Queue *postfix_queue_from_input_expr() {
    TokenStack *ops = TokenStack_create();
    Queue *output = queue_create();
    Token t;


    int prev_type = -1;
    char prev_sym = '\0';

    while (read_next_token(&t)) {
        // Неявное умножение
        if (prev_type != -1) {
            bool prev_before_mul = (prev_type == TOK_VAL ||
                                    prev_type == TOK_VAR ||
                                    (prev_type == TOK_SYM && prev_sym == ')'));

            bool curr_after_mul = (t.type == TOK_VAL ||
                                   t.type == TOK_VAR ||
                                   (t.type == TOK_SYM && t.data.sym == '('));

            if (prev_before_mul && curr_after_mul) {
                const Token mul_t = {TOK_SYM, '*'};
                process_sym_token(mul_t, ops, output);
            }
        }
        prev_type = t.type;
        prev_sym = t.type == TOK_SYM ? t.data.sym : '\0';


        switch (t.type) {
            case TOK_VAL:
            case TOK_VAR:
                queue_push(output, t);
                break;

            case TOK_SYM:
                process_sym_token(t, ops, output);
                break;
            default:
                break;
        }
    }
    while (!TokenStack_is_empty(ops)) queue_push(output, TokenStack_pop(ops));
    TokenStack_destroy(ops);
    return output;
}


Node *build_tree_from_queue(Queue *q) {
    if (!q) return NULL;
    NodeStack *s = NodeStack_create();

    while (!queue_is_empty(q)) {
        const Token t = queue_pop(q);

        if (t.type == TOK_VAL || t.type == TOK_VAR) {
            NodeStack_push(s, create_node(t, NULL, NULL));
        } else {
            Node *parent = create_node(t, NodeStack_pop(s), NodeStack_pop(s));
            NodeStack_push(s, parent);
        }
    }

    Node *root = NodeStack_pop(s);
    NodeStack_destroy(s);
    return root;
}
