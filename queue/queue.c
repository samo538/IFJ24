/**
 *  @file queue.c
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "queue_fill.h"
#include "../tree/tree.h"
#include "../lexer/lexer.h"
#include "../errors/error.h"

#define SIZE_IFJ_FN 13

void queue_add_token(Queue *queue, TokenPtr token){
    queue->Queue_act = realloc(queue->Queue_act, sizeof(TokenPtr) * (queue->Queue_size + 1));
    if (queue->Queue_act == NULL){
        throw_error(NULL, 99);
    }
    queue->Queue_act[queue->Queue_size] = token;
    queue->Queue_size++;
}

TokenPtr queue_next_token(Queue *queue){
    static int counter = 0;
    if (queue->Queue_size <= counter){
        TokenPtr tmp = malloc(sizeof(Token));
        tmp->type = END_OF_FILE;
        return tmp;
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
        exit(99);
    }
    new->Queue_act = NULL;
    new->Queue_size = 0;
    return new;
}

void queue_fill(TokenStoragePtr stoken, TreeElement *tree_node){
    TokenPtr tmp;
    bool ret;
    while ((tmp = next_token())->type != END_OF_FILE){
        if (tmp->type == PUB){
            stoken->SToken = tmp;

            // New tree element representing the top function
            TreeElement *new_node = TreeInsert(tree_node, NULL);
            if (new_node == NULL){
                throw_error(NULL, 99);
            }
            new_node->Data.NodeType = TOP_FUNCTION_NODE;

            ret = fn_def_q(stoken, &new_node->Data.TableElement); // First go through
            check_ret(stoken, ret);
            queue_add_token(stoken->queue, stoken->SToken); // Add the last token

            if (stoken->SToken->type == END_OF_FILE){
                throw_error(stoken, 2);
            }
        }
        else{
            queue_add_token(stoken->queue, tmp);
        }
    }
    queue_add_token(stoken->queue, tmp); // Adds the END_OF_FILE token
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
    2,
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

    // Entering the ifj functions into the special table
    for (int i = 0; i < SIZE_IFJ_FN; i++){
        Elem_id *new_ifj = TableAdd(embededfunctions[i], NULL, 0, stoken->ifj_table);
        new_ifj->Type = FUNCTION;
        new_ifj->stack_size = 0;
        new_ifj->level_stack = NULL;
        new_ifj->FnVar.Fn_id.LocalSymTable = NULL;
        new_ifj->name = strdup(embededfunctions[i]);
        new_ifj->FnVar.Fn_id.return_type.type = ret_type[i];
        new_ifj->FnVar.Fn_id.return_type.nullable = ret_type_null[i];
        new_ifj->FnVar.Fn_id.num_of_params = num_param[i];
        new_ifj->FnVar.Fn_id.TableParams = NULL;
        new_ifj->FnVar.Fn_id.type_of_params = malloc(sizeof(Id_type) * num_param[i]);
        for (int j = 0; j < num_param[i]; j++){
            new_ifj->FnVar.Fn_id.type_of_params[j].nullable = false;
            new_ifj->FnVar.Fn_id.type_of_params[j].type = param_type[i][j];
        }
    }
}
