#include "simplify.h"
#include "simplify_utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Позволяет понять, что за тип слагаемого перед нами: простое число или кэф*переменная
static bool extract_term(Node *n, double *coef, char *var_name, bool *is_const) {
    if (!n) return false;

    if (n->t.type == TOK_VAL) {
        *coef = n->t.data.val;
        *is_const = true;
        var_name = NULL;
        return true;
    }

    if (n->t.type == TOK_VAR) {
        *coef = 1.0;
        *is_const = false;
        strcpy(var_name, n->t.data.name);
        return true;
    }

    if (n->t.type == TOK_SYM && n->t.data.sym == '*') {
        Node *val = NULL;
        Node *var = NULL;

        if (n->l && n->l->t.type == TOK_VAL && n->r && n->r->t.type == TOK_VAR) {
            val = n->l;
            var = n->r;
        } else if (n->l && n->l->t.type == TOK_VAR && n->r && n->r->t.type == TOK_VAL) {
            val = n->r;
            var = n->l;
        }

        if (val && var) {
            *coef = val->t.data.val;
            *is_const = false;
            strcpy(var_name, var->t.data.name);
            return true;
        }
    }
    return false;
}

// Проталкивает слагаемое внутрь дерева – если нашло другое подходящее слагаемое (идёт вниз), то "расстворяет" первое там,
// т.е. выполняет сложение или вычитание
static bool absorb_term(Node *node, double const coef, const char *var_name, const bool is_const, const char sign) {
    if (!node) return false;

    double t_coef;
    char t_var_name[32];
    bool t_is_const;

    // Попали в слагаемое -> пробуем выполнить упрощение
    if (extract_term(node, &t_coef, t_var_name, &t_is_const)) {
        // Const + Const || 1X + 1X
        if ((is_const && t_is_const) || (!is_const && !t_is_const && strcmp(var_name, t_var_name) == 0)) {
            const double new_coef = sign == '+' ? t_coef + coef : t_coef - coef;

            free_tree(node->l);
            free_tree(node->r);
            node->l = NULL;
            node->r = NULL;

            if (is_const || new_coef == 0.0) {
                // 123 или 0*x => число
                node->t.type = TOK_VAL;
                node->t.data.val = new_coef;
            } else if (new_coef == 1.0) {
                // 1*x => просто переменная
                node->t.type = TOK_VAR;
                strcpy(node->t.data.name, var_name);
            } else {
                // 123 * x => 123 * x
                node->t.type = TOK_SYM;
                node->t.data.sym = '*';
                node->l = create_val_node(new_coef);
                node->r = create_var_node(var_name);
            }
            return true;
        }
        return false;
    }

    // Ищем слагаемые дальше через рекурсию
    if (node->t.type == TOK_SYM) {
        if (node->t.data.sym == '+') {
            // A + B. Пробуем добавить к A, если нет, то к B
            if (absorb_term(node->l, coef, var_name, is_const, sign)) return true;
            if (absorb_term(node->r, coef, var_name, is_const, sign)) return true;
        } else if (node->t.data.sym == '-') {
            // A - B. Пробуем добавить к A
            if (absorb_term(node->l, coef, var_name, is_const, sign)) return true;

            // Инвертируем знак, т.к. A - B + C = A - (B - С)
            if (absorb_term(node->r, coef, var_name, is_const, sign == '+' ? '-' : '+')) return true;
        }
    }
    return false;
}


static Node *try_combine_like_terms(Node *node, const char op, bool *changed) {
    if (op != '+' && op != '-') {
        return node;
    }

    double coef;
    char var[32];
    bool is_const;

    // (5x + 3) + 2x => 7x + 3
    // Сливаем ПРАВУЮ ноду влево
    if (extract_term(node->r, &coef, var, &is_const)) {
        if (absorb_term(node->l, coef, var, is_const, op)) {
            Node *new_root = node->l;
            free_tree(node->r);
            free(node);
            *changed = true;
            return new_root;
        }
    }

    // 5x + (2x + 3) => 7x + 3
    // Сливаем ЛЕВУЮ ноду вправо (можно только +, т.к. лень было обрабатывать 5x-2x-3)
    if (op == '+') {
        if (extract_term(node->l, &coef, var, &is_const)) {
            if (absorb_term(node->r, coef, var, is_const, '+')) {
                Node *new_root = node->r;
                free_tree(node->l);
                free(node);
                *changed = true;
                return new_root;
            }
        }
    }

    return node;
}


// Раньше была проблема – после каждого изменения приходилось спускаться с самой вершины дерева вниз.
// Теперь мы локализируем кусочки (и бегаем в пределах этого кусочка), и если они перестают изменяться – поднимаемся выше, увеличивая кусочек
// (увеличение кусочка происходит после return)
static Node *simplify_node_locally(Node *node) {
    bool local_changed = true;

    while (local_changed && node && node->t.type == TOK_SYM) {
        local_changed = false;
        const char op = node->t.data.sym;

        node = try_fold_consts(node, op, &local_changed);
        if (local_changed) {
            continue;
        }

        node = try_combine_like_terms(node, op, &local_changed);
        if (local_changed) {
            continue;
        }

        node = try_simplify_trash(node, op, &local_changed);
    }
    return node;
}


// ====== КОММЕНТАРИИ К СТАРОЙ ФУНКЦИИ ======
// СМ. комменты к simplify_node_locally – ниже устаревшые комменты к версии, когда после каждого изменения, упрощение начиналось заново сверху
// НАЧИНАЕТ СНИЗУ
// Свёртка констант: 1 + 1 = 2 (+ и / тоже работают)
// Приведение подобных членов
// Упрощение: x+0 = x, 0+x = x, x-0 = x, x*1 = x, 1*x = x, x*0 = 0
// static Node *simplify_expression(Node *node) {
// }

Node *simplify(Node *node) {
    if (!node) return NULL;

    node->l = simplify(node->l);
    node->r = simplify(node->r);

    return simplify_node_locally(node);
}
