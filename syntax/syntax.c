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
        return false;
    }
}
bool string_prolog(TokenStoragePtr stoken){
    if (stoken->SToken->type == STRING && strcmp(stoken->SToken->value.str, "ifj24.zig")){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
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
        return false;
    }
}

//--------------------------------------------------------

bool functions(){
    return true;
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
    return import_def(stoken) && functions();
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

