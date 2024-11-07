/*
* File: syntax.c
* Project: IFJ24
* Authors: Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "syntax_types.h"
#include "syntax.h"

#include "../lexer/token.h"
#include "../lexer/lexer.h"

#include "../queue/queue.h"

#include "../symtable/symtable.h"

#include <stdio.h>

bool fn_call(TokenStoragePtr stoken);

// errors and deallocs

void dealloc_stoken(TokenStoragePtr stoken){
    if (stoken != NULL){
        if (stoken->SToken != NULL){
            dealloc_token(stoken->SToken);
            stoken->SToken = NULL;
        }
        if (stoken->SStoredToken != NULL){
            dealloc_token(stoken->SStoredToken);
            stoken->SStoredToken = NULL;
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
}

void syn_error(TokenStoragePtr stoken){
    dealloc_stoken(stoken);
    exit(2);
}

// Expression rules

bool e_var_exp(TokenStoragePtr stoken, Elem_id *new){
    Elem_id *ret = NULL;
    if (stoken->SToken->type == ID){
        ret = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
    }
    if (stoken->SToken->type == IFJ){
        return ifj_call(stoken);
    } 
    else if (stoken->SToken->type == ID && ret != NULL){
        if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Auto assign type
            new->FnVar.Var_id.type.type = ret->FnVar.Fn_id.return_type.type;
            new->FnVar.Var_id.type.nullable = ret->FnVar.Fn_id.return_type.nullable;
        }
        else { // Check for correct type
            if (
                new->FnVar.Var_id.type.type != ret->FnVar.Fn_id.return_type.type ||
                new->FnVar.Var_id.type.nullable != ret->FnVar.Fn_id.return_type.nullable) {
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
        }
        return fn_call(stoken);
    }
    /*else {
        call precedence
    }

    */
    return false;
}

bool e_return_exp(TokenStoragePtr stoken){
    /*if (stoken->SToken->type == EXP){
        call precedence
    }*/
    return true;
}

bool e_null_exp(TokenStoragePtr stoken){
    return t_cl_bracket(stoken);
}

// Lower rules

bool l_type(TokenStoragePtr stoken){
    return t_nullable(stoken) &&
    t_type_keyword(stoken);
}

bool l_var_const(TokenStoragePtr stoken, Elem_id *new){
    bool ret;
    if (stoken->SToken->type == VAR){
        new->FnVar.Var_id.const_t = false;
        ret =  t_var(stoken);
    }
    else {
        new->FnVar.Var_id.const_t = true;
        ret = t_const(stoken);
    }
    return ret;
}

bool l_type_fndef(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        return t_void(stoken);
    }
    else {
        return l_type(stoken);
    }
}

bool l_type_var(TokenStoragePtr stoken, Elem_id *new){
    return t_nullable_var(stoken, new) &&
    t_type_keyword_var(stoken, new);
}

bool l_type_vardef(TokenStoragePtr stoken, Elem_id * new){
    if (stoken->SToken->type == COLON){
        return t_colon(stoken) &&
        l_type_var(stoken, new);
    }
    else {
        return true;
    }
}

bool l_id_assign(TokenStoragePtr stoken){
    if (stoken->SToken->type == UNDERSCORE){
        return t_underline(stoken);
    }
    else {
        return t_id(stoken);
    }
}

// Main rules
bool call_params(TokenStoragePtr stoken, Elem_id *fn, int pos){
    if (stoken->SToken->type == STRING){
        if (pos == fn->FnVar.Fn_id.num_of_params){
            fprintf(stderr, "too many params\n");
            exit(4);
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].type == U8){
            return t_string(stoken) &&
            t_comma(stoken) &&
            call_params(stoken, fn, pos + 1);
        }
        else {
            fprintf(stderr, "wrong type in fn\n");
            exit(4);
        }
    }
    else if(stoken->SToken->type == I32_VAR) {
        if (pos == fn->FnVar.Fn_id.num_of_params){
            fprintf(stderr, "too many params\n");
            exit(4);
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].type == I32){
            return t_int(stoken) &&
            t_comma(stoken) &&
            call_params(stoken, fn, pos + 1);
        }
        else {
            fprintf(stderr, "wrong type in fn\n");
            exit(4);
        }
    }
    else if(stoken->SToken->type == F64_VAR) {
        if (pos == fn->FnVar.Fn_id.num_of_params){
            fprintf(stderr, "too many params\n");
            exit(4);
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].type == F64){
            return t_float(stoken) &&
            t_comma(stoken) &&
            call_params(stoken, fn, pos + 1);
        }
        else {
            fprintf(stderr, "wrong type in fn\n");
            exit(4);
        }
    }
    else if(stoken->SToken->type == NULL_VALUE) {
        if (pos == fn->FnVar.Fn_id.num_of_params){
            fprintf(stderr, "too many params\n");
            exit(4);
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].nullable == true){
            return t_null(stoken) &&
            t_comma(stoken) &&
            call_params(stoken, fn, pos + 1);
        }
        else {
            fprintf(stderr, "wrong type in fn\n");
            exit(4);
        }
    }
    else if(stoken->SToken->type == ID) {
        if (pos == fn->FnVar.Fn_id.num_of_params){
            fprintf(stderr, "too many params\n");
            exit(4);
        }
        Elem_id *tmp = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (fn->FnVar.Fn_id.type_of_params[pos].type == tmp->FnVar.Var_id.type.type){
            return t_id(stoken) &&
            t_comma(stoken) &&
            call_params(stoken, fn, pos + 1);
        }
        else {
            fprintf(stderr, "wrong type in fn\n");
            exit(4);
        }
    }
    if (fn->FnVar.Fn_id.num_of_params != pos){
        fprintf(stderr, "not enough params\n");
        exit(4);
    }
    return true;
}

bool if_while_body(TokenStoragePtr stoken){
    return t_op_cr_bracket(stoken) &&
    fn_body(stoken) &&
    t_cl_cr_bracket(stoken);
}

bool var_def(TokenStoragePtr stoken){
    bool ret;
    Elem_id *new = malloc(sizeof(Elem_id));
    new->Type = VARIABLE;
    new->stack_size = stoken->stack_size;
    copy_levels(stoken->level_stack, &new->level_stack, stoken->stack_size);
    new->FnVar.Var_id.type.type = END_OF_FILE; // Placeholder
    new->FnVar.Var_id.type.nullable = false; // Implicit false

    ret = l_var_const(stoken, new) &&
    t_id_var(stoken, new) &&
    l_type_vardef(stoken, new) &&
    t_eq(stoken) &&
    e_var_exp(stoken, new);

    TableAdd(*new, stoken->local_table);

    free(new->name);
    free(new->level_stack);
    free(new);
    return ret;
}

bool assign(TokenStoragePtr stoken){
    return l_id_assign(stoken) &&
    t_eq(stoken) &&
    e_var_exp(stoken, NULL);
}

bool fn_call(TokenStoragePtr stoken){
    Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
    return t_id(stoken) &&
    t_op_bracket(stoken) &&
    call_params(stoken, fn, 0) &&
    t_cl_bracket(stoken) &&
    t_semicolon(stoken);
}

bool ifj_call(TokenStoragePtr stoken){
    return t_ifj(stoken) &&
    t_dot(stoken) &&
    t_id_ifj(stoken) &&
    t_op_bracket(stoken) &&
    //call_params(stoken) &&
    t_cl_bracket(stoken) &&
    t_semicolon(stoken);
}

bool if_else(TokenStoragePtr stoken){
    return t_if(stoken) &&
    t_op_bracket(stoken) &&
    e_null_exp(stoken) &&
    if_while_body(stoken) &&
    t_else(stoken) &&
    if_while_body(stoken);
}

bool cycle(TokenStoragePtr stoken){
    return t_while(stoken) &&
    t_op_bracket(stoken) &&
    e_null_exp(stoken) &&
    if_while_body(stoken);
}

bool fn_return(TokenStoragePtr stoken){
    return t_return(stoken) &&
    e_return_exp(stoken) &&
    t_semicolon(stoken);
}

bool fn_body(TokenStoragePtr stoken){ // TODO 
    if (stoken->SToken->type == VAR || stoken->SToken->type == CONST){
        return var_def(stoken) &&
        fn_body(stoken);
    }
    else if(stoken->SToken->type == ID){
        Elem_id *tmp = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        if (tmp != NULL){
            if (tmp->FnVar.Fn_id.return_type.type != VOID){
                fprintf(stderr, "return type dumped\n");
                exit(4);
            }
            return fn_call(stoken) &&
            fn_body(stoken);
        }
        tmp = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (tmp != NULL){
            return assign(stoken) &&
            fn_body(stoken);
        }
        //Error
        return -1;
    }
    else if(stoken->SToken->type == IFJ){
        return ifj_call(stoken) &&
        fn_body(stoken);
    }
    else if(stoken->SToken->type == IF){
        return if_else(stoken) &&
        fn_body(stoken);
    }
    else if(stoken->SToken->type == WHILE){
        return cycle(stoken) &&
        fn_body(stoken);
    }
    else if(stoken->SToken->type == RETURN){
        return fn_return(stoken) &&
        fn_body(stoken);
    }
    else {
        return true;
    }
}

bool params(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        return t_id(stoken) &&
        t_colon(stoken) &&
        l_type(stoken) &&
        t_comma(stoken) &&
        params(stoken);
    }
    else {
        return true;
    }
}

bool fn_def(TokenStoragePtr stoken){
    return t_pub(stoken) &&
    t_fn(stoken) &&
    t_id_fn(stoken) &&
    t_op_bracket(stoken) &&
    params(stoken) &&
    t_cl_bracket(stoken) &&
    l_type_fndef(stoken);
}

bool functions(TokenStoragePtr stoken){
    if (stoken->SToken->type == END_OF_FILE) {
        return t_eof(stoken);
    }
    else {
        if (stoken->current_fn != NULL){
            free(stoken->current_fn);
            stoken->current_fn = NULL;
            free(stoken->level_stack);
            stoken->level_stack = NULL;
            stoken->stack_size = 0;
        }
        return fn_def(stoken) &&
        t_op_cr_bracket(stoken) &&
        fn_body(stoken) &&
        t_cl_cr_bracket(stoken) &&
        functions(stoken);
    }
}

bool import_def(TokenStoragePtr stoken){
    return t_const(stoken) &&
    t_ifj(stoken) &&
    t_eq(stoken) &&
    t_at_import(stoken) &&
    t_op_bracket(stoken) &&
    t_string_prolog(stoken) &&
    t_cl_bracket(stoken) &&
    t_semicolon(stoken);
}

bool start(TokenStoragePtr stoken){
    return import_def(stoken) && functions(stoken);
}

int main(){
    bool result = false;
    TokenStoragePtr stoken = malloc(sizeof(TokenStorage));
    if (stoken == NULL){
        return 99;
    }

    Queue *queue;
    SymTable *glob_table;
    queue = queue_init();
    glob_table = TableInit();

    stoken->queue = queue;
    stoken->glob_table = glob_table;
    stoken->current_fn = NULL;

    queue_fill(stoken);

    free(stoken->current_fn);
    stoken->current_fn = NULL;
    stoken->SStoredToken = NULL;
    stoken->local_table = NULL;
    stoken->level_stack = NULL;
    stoken->stack_size = 0;
    stoken->SToken = queue_next_token(queue);

    result = start(stoken);

    Elem_id *elem;
    for (int i = 0; i < 1001; i++){
        elem = stoken->glob_table[i];
        if (elem == NULL){
            continue;
        }
        printf("--Globals--\n");
        printf("%s\n",elem->name);
        printf("%d\n",elem->FnVar.Fn_id.return_type);
        printf("%d\n",elem->FnVar.Fn_id.return_type.nullable);
        printf("%d\n",elem->FnVar.Fn_id.num_of_params);
        for (int i = 0; i < elem->FnVar.Fn_id.num_of_params; i++){
            printf("%d\n",elem->FnVar.Fn_id.type_of_params[i].type);
            printf("%d\n",elem->FnVar.Fn_id.type_of_params[i].nullable);
        }
        Elem_id *elem_loc;
        for (int j = 0; j < 1001; j++){
        elem_loc = elem->FnVar.Fn_id.LocalSymTable[j];
        if (elem_loc == NULL){
            continue;
        }
        printf("--Locals--\n");
        printf("%s\n",elem_loc->name);
        printf("%d\n",elem_loc->stack_size);
        for (int y = 0; y < elem_loc->stack_size; y++){
            printf("%d ",elem_loc->level_stack[y]);
        }
        printf("\n");
        printf("%d\n",elem_loc->FnVar.Var_id.type.type);
        printf("%d\n",elem_loc->FnVar.Var_id.type.nullable);
        printf("%d\n",elem_loc->FnVar.Var_id.const_t);
        }
    }

    printf("%d\n", result); //debug output
    dealloc_stoken(stoken);
}

