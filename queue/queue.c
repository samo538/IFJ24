#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "queue_fill.h"

#include "../lexer/lexer.h"

#define SIZE_IFJ_FN 13

void queue_add_token(Queue *queue, TokenPtr token){
    queue->Queue_act = realloc(queue->Queue_act, sizeof(TokenPtr) * (queue->Queue_size + 1));
    if (queue->Queue_act == NULL){
        //TODO error
    }
    queue->Queue_act[queue->Queue_size] = token;
    queue->Queue_size++;
}

TokenPtr queue_next_token(Queue *queue){
    static int counter = 0;
    if (queue->Queue_size <= counter){
        printf("invalid read\n"); //TODO real error
        return NULL;
    }
    TokenPtr tmp = queue->Queue_act[counter];
    counter++;
    return tmp;
}

void queue_free(Queue *queue){
    free(queue->Queue_act);
    free(queue);
}

Queue *queue_init(){
    Queue * new = malloc(sizeof(Queue));
    if (new == NULL){
        //TODO error
    }
    new->Queue_act = NULL;
    new->Queue_size = 0;
    return new;
}

void queue_fill(TokenStoragePtr stoken){
    TokenPtr tmp;
    while ((tmp = next_token())->type != END_OF_FILE){
        if (tmp->type == PUB){
            stoken->SToken = tmp;
            fn_def_q(stoken);
            queue_add_token(stoken->queue, stoken->SToken); // Add the last token
            if (stoken->SToken->type == END_OF_FILE){
                fprintf(stderr, "syntax error\n");
                exit(2);
            }
        }
        else{
            queue_add_token(stoken->queue, tmp);
        }
    }
    queue_add_token(stoken->queue, tmp);
}

void ifj_table_fill(TokenStoragePtr stoken){
    char* embededfunctions[] = {
    "readstr",
    "readi32",
    "readf64",
    "write",
    "i2f",
    "f2i",
    "string",
    "length",
    "concat",
    "substring",
    "strcmp",
    "ord",
    "chr"};

    bool ret_type_null[] = {
    true,
    true,
    true,
    false,
    false,
    false,
    false,
    false,
    false,
    true,
    false,
    false,
    false};

    enum TokenType ret_type[] = {
    U8,
    I32,
    F64,
    VOID,
    F64,
    I32,
    U8,
    I32,
    U8,
    U8,
    I32,
    I32,
    U8};

    int num_param[] = {
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    3,
    2,
    2,
    1};

    enum TokenType param_type[][5] = {
    {},
    {},
    {},
    {STRING}, // Special kind of param, it accepts everything
    {I32},
    {F64},
    {U8},
    {U8},
    {U8, U8},
    {U8, I32, I32},
    {U8, U8},
    {U8, I32},
    {I32}};

    Elem_id tmp;
    tmp.Type = FUNCTION;
    tmp.stack_size = 0;
    tmp.level_stack = NULL;
    tmp.FnVar.Fn_id.LocalSymTable = NULL;
    for (int i = 0; i < SIZE_IFJ_FN; i++){
        tmp.name = embededfunctions[i];
        tmp.FnVar.Fn_id.return_type.type = ret_type[i];
        tmp.FnVar.Fn_id.return_type.nullable = ret_type_null[i];
        tmp.FnVar.Fn_id.num_of_params = num_param[i];
        tmp.FnVar.Fn_id.type_of_params = malloc(sizeof(Id_type) * num_param[i]);
        for (int j = 0; j < num_param[i]; j++){
            tmp.FnVar.Fn_id.type_of_params[j].nullable = false;
            tmp.FnVar.Fn_id.type_of_params[j].type = param_type[i][j];
        }
        TableAdd(tmp, stoken->ifj_table);
        free(tmp.FnVar.Fn_id.type_of_params);
    }
}
