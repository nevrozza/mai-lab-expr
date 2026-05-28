#ifndef EXPR_DATA_H
#define EXPR_DATA_H

// ------------- TOKENS -------------


typedef enum TokenType {
    TOK_VAL,
    TOK_SYM,
    TOK_VAR
} TokenType;


typedef struct Token {
    TokenType type;

    union {
        double val;
        char sym;
        char name[32];
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
