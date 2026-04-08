#include "data.h"


#include <ctype.h>
#include <stdio.h>

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
        while (isalnum(c)) {
            t->data.var.name[i++] = (char) c;
            c = getchar();
        }
        t->data.var.name[i] = '\0';
        ungetc(c, stdin);
        t->type = TOK_VAR;
        t->data.var.coef = 1.0;
        t->data.var.degree = 1;
    } else {
        t->data.sym = (char) c;
        t->type = TOK_SYM;
    }
    return 1;
}
