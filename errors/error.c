/**
 *  @file error.c
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../queue/queue.h"
#include "../lexer/lexer.h"
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

void throw_error(TokenStoragePtr stoken, int type){
    if (type == 99){
        fprintf(stderr,"%s",err_message[10]);
        exit(type);
    }

    fprintf(stderr,"%s",err_message[type - 1]);
    garbage_collector(stoken, true);
    exit(type);
}

void check_ret(TokenStoragePtr stoken,bool ret){
    if (!ret){
        throw_error(stoken ,2);
    }
}

void garbage_collector(TokenStoragePtr stoken, bool err){
    if (stoken != NULL){
        TokenPtr tmp;
        if (err){
            dealloc_token(stoken->SToken);
            stoken->SToken = NULL;
            while ((tmp = queue_next_token(stoken->queue))->type != END_OF_FILE){
                dealloc_token(tmp);
            }
        }
        queue_free(stoken->queue);
        TableClear(stoken->glob_table, FUNCTION);
        TableClear(stoken->ifj_table, FUNCTION);
        free(stoken->current_fn);
        free(stoken->level_stack);
        free(stoken);
        stoken = NULL;
    }
}
