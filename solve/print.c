#include "print.h"

#include <stdbool.h>
#include <stdio.h>

#include "tree_from_expr.h"


void print_expr_from_node(const Node *node) {
    if (!node) return;

    if (node->t.type == TOK_VAL) {
        // Если int, то без x.yyy
        if (node->t.data.val == (int) node->t.data.val) {
            printf("%d", (int) node->t.data.val);
        } else {
            printf("%g", node->t.data.val);
        }
    } else if (node->t.type == TOK_VAR) {
        printf("%s", node->t.data.name);
    } else if (node->t.type == TOK_SYM) {
        const char op = node->t.data.sym;

        // Левое поддерево
        bool l_brackets = false;
        if (node->l && node->l->t.type == TOK_SYM) {
            if (get_priority(node->l->t.data.sym) < get_priority(op)) {
                l_brackets = true;
            }
        }
        if (l_brackets) printf("(");
        print_expr_from_node(node->l);
        if (l_brackets) printf(")");

        // Знак
        printf(" %c ", op);

        // Правое поддерево
        bool r_brackets = false;
        if (node->r && node->r->t.type == TOK_SYM) {
            // Если приоритет меньше, или приоритеты равны, но это некоммутативная/левоассоциативная операция (типа минус или деление)
            const int p_right = get_priority(node->r->t.data.sym);
            const int p_curr = get_priority(op);

            // По факту: 5 - (2 - 3)  = 6
            // Если не поставить скобки: 5 - 2 - 3 = 0
            // Поэтому не смотря на одинаковые приоритеты, необходимо поставить скобки
            // Аналогично с /
            if (p_right < p_curr || (p_right == p_curr && (op == '-' || op == '/'))) {
                r_brackets = true;
            }
        }
        if (r_brackets) printf("(");
        print_expr_from_node(node->r);
        if (r_brackets) printf(")");
    }
}

static void print_node_data(const Node *node) {
    if (node->t.type == TOK_VAL) {
        if (node->t.data.val == (int) node->t.data.val) printf("[%d]\n", (int) node->t.data.val);
        else printf("[%g]\n", node->t.data.val);
    } else if (node->t.type == TOK_VAR) {
        printf("[%s]\n", node->t.data.name);
    } else if (node->t.type == TOK_SYM) {
        printf("%c\n", node->t.data.sym);
    }
}

static void print_tree_helper(const Node *node, const char *prefix, bool is_tail) {
    if (!node) return;

    printf("%s%s", prefix, is_tail ? "└── " : "├── ");
    print_node_data(node);

    char new_prefix[1024];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_tail ? "    " : "│   ");

    if (node->l || node->r) {
        if (node->l) print_tree_helper(node->l, new_prefix, node->r == NULL);
        if (node->r) print_tree_helper(node->r, new_prefix, true);
    }
}

void print_tree_from_node(const Node *node) {
    if (!node) {
        printf("Нечего принтить =(\n");
        return;
    }
    print_node_data(node);
    if (node->l) print_tree_helper(node->l, "", node->r == NULL);
    if (node->r) print_tree_helper(node->r, "", true);
}
