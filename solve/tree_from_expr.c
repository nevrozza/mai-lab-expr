#include "tree_from_expr.h"

#include <stdio.h>
#include <stdlib.h>
#include "../utils/utils.h"
#include "../structures/stack_generic.h"


// ======= ДЛЯ ВАЛИДАЦИИ =======
static void flush_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static Queue *error(TokenStack *ops, Queue *output, const char msg[]) {
    soft_error(msg);
    queue_destroy(output);
    TokenStack_destroy(ops);
    flush_input();
    fflush(stdin);
    return NULL;
}

static bool is_binary_op(char sym) {
    return sym == '+' || sym == '-' || sym == '*' || sym == '/';
}

static bool is_math_sym(char sym) {
    return sym == '+' || sym == '-' || sym == '*' || sym == '/' || sym == '(' || sym == ')';
}


// ======= АЛГОРИТМ =======
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
// Ещё и валидирует это всё
Queue *postfix_queue_from_input_expr() {
    TokenStack *ops = TokenStack_create();
    Queue *output = queue_create();
    Token t;


    int prev_type = -1;
    char prev_sym = '\0';

    int bracket_balance = 0;

    while (read_next_token(&t)) {
        // Валидация скобок и знаков
        if (t.type == TOK_SYM) {
            if (t.data.sym == '(') bracket_balance++;
            else if (t.data.sym == ')') bracket_balance--;

            if (bracket_balance < 0) {
                return error(ops, output, "Закрывающая скобка без открывающей");
            }
            if (!is_math_sym(t.data.sym)) {
                return error(ops, output, "Не математический знак");
            }
        }

        // Ещё больше валидации + поддержка неявного умножения
        if (prev_type != -1) {
            // Валидация: 2 оператора подряд "2 + * 3"
            if (prev_type == TOK_SYM && is_binary_op(prev_sym) &&
                t.type == TOK_SYM && is_binary_op(t.data.sym)) {
                return error(ops, output, "Два оператора подряд");
            }

            // Валидация: Оператор перед закрывающей скобкой "(2 + )"
            if (prev_type == TOK_SYM && is_binary_op(prev_sym) &&
                t.type == TOK_SYM && t.data.sym == ')') {
                return error(ops, output, "Оператор перед закрывающей скобкой");
            }

            // Валидация: Открывающая скобка перед бинарным оператором "( * 2)"
            if (prev_type == TOK_SYM && prev_sym == '(' &&
                t.type == TOK_SYM && is_binary_op(t.data.sym)) {
                return error(ops, output, "Оператор сразу после открывающей скобки");
            }

            // Неявное умножение
            bool prev_is_operand = prev_type == TOK_VAL || prev_type == TOK_VAR || (
                                       prev_type == TOK_SYM && prev_sym == ')');
            bool curr_is_operand = t.type == TOK_VAL || t.type == TOK_VAR || (t.type == TOK_SYM && t.data.sym == '(');

            if (prev_is_operand && curr_is_operand) {
                // Валидация: нельзя, чтобы после переменной шло число "x 3"
                if (t.type == TOK_VAR || (t.type == TOK_SYM && t.data.sym == '(')) {
                    const Token mul_t = {.type = TOK_SYM, .data.sym = '*'};
                    process_sym_token(mul_t, ops, output);
                } else {
                    return error(ops, output, "Пропущен знак");
                }
            }
        } else if (t.type == TOK_SYM) {
            // Валидация на запрет оператора в начале, но пропускаем +, а из унарного минуса делаем 0 - x
            if (t.data.sym == '-') {
                Token zero_t = {.type = TOK_VAL, .data.val = 0};
                queue_push(output, zero_t);
            } else if (t.data.sym == '+') {
                prev_type = TOK_SYM;
                prev_sym = '+';
                continue;
            } else {
                return error(ops, output, "Выражение не может начинаться с операторов '*' или '/'");
            }
        }
        prev_type = t.type;
        prev_sym = t.type == TOK_SYM ? t.data.sym : '\0';

        // Логика =)
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

    // Валидация (последний раз, правда)
    // Выражение не должно заканчиваться на оператор "3x - 2 +"
    if (prev_type == TOK_SYM && is_binary_op(prev_sym)) {
        return error(ops, output, "Выражение не может заканчиваться на оператор");
    }

    if (bracket_balance != 0) {
        return error(ops, output, "Не все открытые скобки были закрыты");
    }

    // Логика
    while (!TokenStack_is_empty(ops)) queue_push(output, TokenStack_pop(ops));

    // я обманул. это точно последний
    if (queue_is_empty(output)) {
        return error(ops, output, "Пустой ввод");
    }

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
