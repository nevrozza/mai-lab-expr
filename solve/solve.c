#include "solve.h"

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

// shunting yard (Deikstra)
Queue *postfix_queue_from_input_expr() {
    TokenStack *ops = TokenStack_create();
    Queue *output = queue_create();
    Token t;

    while (read_next_token(&t)) {
        switch (t.type) {
            case TOK_VAL:
            case TOK_NAME:
                queue_push(output, t);
                break;

            case TOK_SYM:
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

        if (t.type == TOK_VAL || t.type == TOK_NAME) {
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
