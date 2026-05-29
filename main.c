#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures/data.h"
#include "structures/queue.h"
#include "solve/tree_from_expr.h"
#include "solve/simplify.h"
#include "solve/print.h"
#include "utils/utils.h"

int main(void) {
    printf("quit – выход; tree – спрятать/отобразить дерево");

    bool show_tree = true;
    char input_buffer[1024];

    while (true) {
        printf("\n> ");

        if (!fgets(input_buffer, sizeof(input_buffer), stdin)) {
            break;
        }
        input_buffer[strcspn(input_buffer, "\n")] = '\0';
        if (strcmp(input_buffer, "quit") == 0 || strcmp(input_buffer, "exit") == 0) {
            printf("Пока-пока!\n");
            break;
        }
        if (strcmp(input_buffer, "tree") == 0) {
            show_tree = !show_tree;
            printf("Отображение дерева %s", show_tree ? "ВКЛЮЧЕНО" : "ВЫКЛЮЧЕНО");
            continue;
        }

        // Возвращаём всё в stdint для ввода в `postfix_queue_from_input_expr`
        ungetc('\n', stdin);
        for (int i = strlen(input_buffer) - 1; i >= 0; i--) {
            ungetc(input_buffer[i], stdin);
        }


        Queue *q = postfix_queue_from_input_expr();

        if (!q) {
            continue;
        }

        Node *root = build_tree_from_queue(q);
        queue_destroy(q);

        if (!root) {
            soft_error("Что-то пошло не так =/");
            continue;
        }
        root = simplify(root);

        printf("Ответ: ");
        print_expr_from_node(root);

        if (show_tree) {
            printf("\nДерево:\n");
            print_tree_from_node(root);
        }

        free_tree(root);
    }

    return 0;
}
