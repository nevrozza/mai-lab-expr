#include "data.h"
#include "../utils/utils.h"


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_next_token(Token *t) {
    int c = getchar();
    while (isspace(c) && c != '\n') c = getchar();

    if (c == EOF || c == '\n') return 0;

    if (isdigit(c) || c == '.') {
        ungetc(c, stdin);
        scanf("%lf", &t->data.val);
        t->type = TOK_VAL;
    } else if (isalpha(c)) {
        int i = 0;
        while (isalnum(c) || c == '^' || (c == '-' && i > 0 && t->data.name[i - 1] == '^')) {
            if (i < 31) {
                t->data.name[i++] = (char) c;
            }
            c = getchar();
        }

        t->data.name[i] = '\0';
        ungetc(c, stdin);
        t->type = TOK_VAR;
    } else {
        t->data.sym = (char) c;
        t->type = TOK_SYM;
    }
    return 1;
}


void free_tree(Node *node) {
    if (!node) return;
    free_tree(node->l);
    free_tree(node->r);
    free(node);
}

Node *create_node(Token t, Node *r, Node *l) {
    Node *n = malloc(sizeof(Node));
    if (!n) {
        fatal_memory_error();
        return NULL;
    }
    n->t = t;
    n->l = l;
    n->r = r;
    return n;
}


Node *create_val_node(double val) {
    Token t = {.type = TOK_VAL, .data.val = val};
    return create_node(t, NULL, NULL);
}

Node *create_var_node(const char name[]) {
    Token t = {.type = TOK_VAR };
    strcpy(t.data.name, name);
    return create_node(t, NULL, NULL);
}