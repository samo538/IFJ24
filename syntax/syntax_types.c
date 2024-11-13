
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "syntax.h"
#include "../lexer/token.h"
#include "../lexer/lexer.h"
#include "../errors/error.h"


const char* embededfunctions[] = {
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
    "chr"
};

// Helper functions

bool h_check_ifj_fn(char *fn_name){
    for (int i = 0; i < 13; i++){
        if (!strcmp(fn_name, embededfunctions[i])){
            return true;
        }
    }
    return false;
}

// expect function for simple sematics checks
bool t_expect(TokenStoragePtr stoken, enum TokenType type){
    if (stoken->SToken->type == type){
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(2);
    }
}

bool t_string_prolog(TokenStoragePtr stoken){
    if (stoken->SToken->type == STRING && !strcmp(stoken->SToken->value.str, "ifj24.zig")){
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(2);
    }
}

bool t_id_ifj(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID && h_check_ifj_fn(stoken->SToken->value.str)){
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(2);
    }
}

bool t_type_keyword(TokenStoragePtr stoken, Elem_id * new){
    if (stoken->SToken->type == OPENING_SQUARE_BRACKET){
        if (new != NULL){
            new->FnVar.Var_id.type.type = U8;
        }
        return t_expect(stoken, OPENING_SQUARE_BRACKET) &&
        t_expect(stoken, CLOSING_SQUARE_BRACKET) &&
        t_expect(stoken, U8);
    }
    else if (stoken->SToken->type == F64){
        if (new != NULL){
            new->FnVar.Var_id.type.type = F64;
        }
        new->FnVar.Var_id.type.type = F64;
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else if (stoken->SToken->type == I32){
        if (new != NULL){
            new->FnVar.Var_id.type.type = I32;
        }
        new->FnVar.Var_id.type.type = I32;
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(2);
    }
}
bool t_id_var(TokenStoragePtr stoken, Elem_id *new){
    if (stoken->SToken->type == ID){
        Elem_id *tmp = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (tmp != NULL){
            fprintf(stderr,"Redefinition/shadowing not allowed\n");
            exit(5);
        }
        tmp = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        if (tmp != NULL){
            fprintf(stderr,"Function/Variable with the same name not allowed\n");
            exit(5);
        }
        new->name = strdup(stoken->SToken->value.str);

        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(2);
    }
}
bool t_id_fn(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        Elem_id * tmp = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table); // Searching for the curr function
        stoken->current_fn = strdup(stoken->SToken->value.str);
        if (tmp->FnVar.Fn_id.return_type.type == VOID){ // If void, return must not be present
            stoken->returned = true;
        }
        stoken->local_table = tmp->FnVar.Fn_id.LocalSymTable; // Settin local table
        stoken->stack_size = 1; // Initialising stack
        stoken->level_stack = malloc(sizeof(int));
        stoken->level_stack[0] = 1;

        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(2);
    }
}
bool t_nullable_var(TokenStoragePtr stoken, Elem_id *new){
    if (stoken->SToken->type == NULLABLE){
        new->FnVar.Var_id.type.nullable = true;

        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        return true;
    }
}
