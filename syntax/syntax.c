/*
* File: syntax.c
* Project: IFJ24
* Authors: Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "syntax.h"
#include "../lexer/token.h"
#include "../lexer/lexer.h"

#include <stdio.h>

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
        free(stoken);
        stoken = NULL;
    }
}

void syn_error(TokenStoragePtr stoken){
    dealloc_stoken(stoken);
    exit(2);
}

bool t_semicolon(TokenStoragePtr stoken){
    if (stoken->SToken->type == SEMICOLON){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_cl_bracket(TokenStoragePtr stoken){
    if (stoken->SToken->type == CLOSING_BRACKET){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_op_bracket(TokenStoragePtr stoken){
    if (stoken->SToken->type == OPENING_BRACKET){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_op_sq_bracket(TokenStoragePtr stoken){
    if (stoken->SToken->type == OPENING_SQUARE_BRACKET){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_cl_sq_bracket(TokenStoragePtr stoken){
    if (stoken->SToken->type == CLOSING_SQUARE_BRACKET){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_op_cr_bracket(TokenStoragePtr stoken){
    if (stoken->SToken->type == OPENING_CURLY_BRACKET){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_cl_cr_bracket(TokenStoragePtr stoken){
    if (stoken->SToken->type == CLOSING_CURLY_BRACKET){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool string_prolog(TokenStoragePtr stoken){
    if (stoken->SToken->type == STRING && !strcmp(stoken->SToken->value.str, "ifj24.zig")){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_at_import(TokenStoragePtr stoken){
    if (stoken->SToken->type == IMPORT){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_eq(TokenStoragePtr stoken){
    if (stoken->SToken->type == ASSIGN){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_ifj(TokenStoragePtr stoken){
    if (stoken->SToken->type == IFJ){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_const(TokenStoragePtr stoken){
    if (stoken->SToken->type == CONST){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}

bool t_eof(TokenStoragePtr stoken){
    if (stoken->SToken->type == END_OF_FILE){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_pub(TokenStoragePtr stoken){
    if (stoken->SToken->type == PUB){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_fn(TokenStoragePtr stoken){
    if (stoken->SToken->type == FN){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_id(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_comma(TokenStoragePtr stoken){
    if (stoken->SToken->type == COMMA){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_colon(TokenStoragePtr stoken){
    if (stoken->SToken->type == COLON){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}

bool t_u8(TokenStoragePtr stoken){
    if (stoken->SToken->type == U8){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}

bool t_type_keyword(TokenStoragePtr stoken){
    if (stoken->SToken->type == OPENING_SQUARE_BRACKET){
        return t_op_sq_bracket(stoken) &&
        t_cl_sq_bracket(stoken) &&
        t_u8(stoken);
    }
    else if (stoken->SToken->type == F64){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else if (stoken->SToken->type == I32){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}

bool t_nullable(TokenStoragePtr stoken){
    if (stoken->SToken->type == NULLABLE){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        return true;
    }
}

bool t_type(TokenStoragePtr stoken){
    return t_nullable(stoken) &&
    t_type_keyword(stoken);
}

bool t_type_fndef(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else {
        return t_type(stoken);
    }
}

//--------------------------------------------------------

bool params(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        return t_id(stoken) &&
        t_colon(stoken) &&
        t_type(stoken) &&
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
    t_id(stoken) &&
    t_op_bracket(stoken) &&
    params(stoken) &&
    t_cl_bracket(stoken) &&
    t_type_fndef(stoken);
}

bool fn_body(TokenStoragePtr stoken){
    return true;
}

bool functions(TokenStoragePtr stoken){
    if (stoken->SToken->type == END_OF_FILE) {
        return t_eof(stoken);
    }
    else {
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
    string_prolog(stoken) &&
    t_cl_bracket(stoken) &&
    t_semicolon(stoken);
}

bool start(TokenStoragePtr stoken){
    return import_def(stoken) && functions(stoken);
}

int main(){
    bool result;
    TokenStoragePtr stoken = malloc(sizeof(TokenStorage));
    if (stoken == NULL){
        return 99;
    }

    stoken->SToken = next_token();
    stoken->SStoredToken = NULL;

    result = start(stoken);
    printf("%d\n", result); //debug output
    dealloc_stoken(stoken);
}

