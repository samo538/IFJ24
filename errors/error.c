/**
 *  @file error.c
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "error.h"

const char *err_message[] = {
    "lexer error\n",
    "syntax error\n",
    "undef fn/var\n",
    "wrong num of params // wrong return type // fn return dumping\n",
    "fn/var redef // assigning to const\n",
    "return expression in void function // no return expression in non-void function\n",
    "error in expression // wrong type assignment\n",
    "cannot derive var type\n",
    "unsed variable\n",
    "other sematic error\n",
    "internal failure\n"
};

void throw_error(int type){
    if (type == 99){
        fprintf(stderr,"%s",err_message[10]);
        exit(type);
    }

    fprintf(stderr,"%s",err_message[type - 1]);
    exit(type);
}

void check_ret(bool ret){
    if (!ret){
        throw_error(2);
    }
}

/*void garbage_collector(TokenStoragePtr stoken){
    if (stoken != NULL){
        if (stoken->SToken != NULL){
            dealloc_token(stoken->SToken);
            stoken->SToken = NULL;
        }
        if (stoken->queue != NULL){
            queue_free(stoken->queue);
        }
        if (stoken->glob_table != NULL){
            TableClear(stoken->glob_table, FUNCTION);
        }
        if (stoken->current_fn != NULL){
            free(stoken->current_fn);
        }
        if (stoken->level_stack != NULL){
            free(stoken->level_stack);
        }
        free(stoken);
        stoken = NULL;
    }
} */
