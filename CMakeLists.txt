cmake_minimum_required(VERSION 4.1)
project(expr C)

set(CMAKE_C_STANDARD 17)

add_executable(expr main.c
        structures/data.h
        structures/data.c
        structures/stack_generic.h
        structures/stack_generic.c
        utils/utils.c
        utils/utils.h
        structures/queue.c
        structures/queue.h
        solve/tree_from_expr.c
        solve/tree_from_expr.h
        solve/simplify.c
        solve/simplify.h)
