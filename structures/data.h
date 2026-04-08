#ifndef EXPR_DATA_H
#define EXPR_DATA_H

// ------------- TOKENS -------------


typedef enum TokenType {
    TOK_VAL,
    TOK_SYM,
    TOK_VAR
} TokenType;

// Слагаемое вида 3x^2
typedef struct Var {
    double coef;
    int degree;
    char name[32];
} Var;


typedef struct Token {
    TokenType type;

    union {
        double val;
        char sym;
        Var var;
    } data;
} Token;

int read_next_token(Token *t);


// ------------- TREE -------------
typedef struct Node {
    Token tok;
    struct Node *left;
    struct Node *right;
} Node;

#endif //EXPR_DATA_H
