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
    Token t;
    struct Node *l;
    struct Node *r;
} Node;

void free_tree(Node *node);
Node *create_node(Token t, Node *r, Node *l);

Node *create_val_node(double val);
Node *create_var_node(const char name[]);

#endif //EXPR_DATA_H
