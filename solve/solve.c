#include "solve.h"

#include "../structures/stack_generic.h"

static int get_priority(const char sym) {
    if (sym == '*' || sym == '/') return 2;
    if (sym == '+' || sym == '-') return 1;
    return 0;
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
