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

bool ifj_call(TokenStoragePtr stoken);
bool fn_body(TokenStoragePtr stoken);

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
        free(stoken);
        stoken = NULL;
    }
}

void syn_error(TokenStoragePtr stoken){
    dealloc_stoken(stoken);
    exit(2);
}

// Helper functions

bool h_check_ifj_fn(char *fn_name){
    for (int i = 0; i < 13; i++){
        if (!strcmp(fn_name, embededfunctions[i])){
            return true;
        }
    }
    return false;
}

// Token rules

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
bool t_string_prolog(TokenStoragePtr stoken){
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
bool t_dot(TokenStoragePtr stoken){
    if (stoken->SToken->type == DOT){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_return(TokenStoragePtr stoken){
    if (stoken->SToken->type == RETURN){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_if(TokenStoragePtr stoken){
    if (stoken->SToken->type == IF){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_else(TokenStoragePtr stoken){
    if (stoken->SToken->type == ELSE){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_vertical_bar(TokenStoragePtr stoken){
    if (stoken->SToken->type == VERTICAL_BAR){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_while(TokenStoragePtr stoken){
    if (stoken->SToken->type == WHILE){
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
bool t_var(TokenStoragePtr stoken){
    if (stoken->SToken->type == VAR){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_void(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
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
bool t_string(TokenStoragePtr stoken){
    if (stoken->SToken->type == STRING){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_int(TokenStoragePtr stoken){
    if (stoken->SToken->type == I32_VAR){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_float(TokenStoragePtr stoken){
    if (stoken->SToken->type == F64_VAR){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_null(TokenStoragePtr stoken){
    if (stoken->SToken->type == NULL_VALUE){
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
bool t_id_ifj(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID && h_check_ifj_fn(stoken->SToken->value.str)){
        dealloc_token(stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_underline(TokenStoragePtr stoken){
    if (stoken->SToken->type == UNDERSCORE){
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

// Expression rules

bool e_var_exp(TokenStoragePtr stoken){
    if (stoken->SToken->type == IFJ){
        return ifj_call(stoken);
    }
    /*else if (stoken->SToken->type == ID && ID -> function){
        return fn_call(stoken);
    }
    else {
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

bool l_var_const(TokenStoragePtr stoken){
    if (stoken->SToken->type == VAR){
        return t_var(stoken);
    }
    else {
        return t_const(stoken);
    }
}

bool l_type_fndef(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        return t_void(stoken);
    }
    else {
        return l_type(stoken);
    }
}

bool l_type_vardef(TokenStoragePtr stoken){
    if (stoken->SToken->type == COLON){
        return t_colon(stoken) &&
        l_type(stoken);
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
bool call_params(TokenStoragePtr stoken){
    if (stoken->SToken->type == STRING){
        t_string(stoken) &&
        t_comma(stoken) &&
        call_params(stoken);
    }
    else if(stoken->SToken->type == I32_VAR) {
        t_int(stoken) &&
        t_comma(stoken) &&
        call_params(stoken);
    }
    else if(stoken->SToken->type == F64_VAR) {
        t_float(stoken) &&
        t_comma(stoken) &&
        call_params(stoken);
    }
    else if(stoken->SToken->type == NULL_VALUE) {
        t_null(stoken) &&
        t_comma(stoken) &&
        call_params(stoken);
    }
    else if(stoken->SToken->type == ID) {
        t_id(stoken) &&
        t_comma(stoken) &&
        call_params(stoken);
    }

    return true;
}

bool if_while_body(TokenStoragePtr stoken){
    return t_op_cr_bracket(stoken) &&
    fn_body(stoken) &&
    t_cl_cr_bracket(stoken);
}

bool var_def(TokenStoragePtr stoken){
    return l_var_const(stoken) &&
    t_id(stoken) &&
    l_type_vardef(stoken) &&
    t_eq(stoken) &&
    e_var_exp(stoken);
}

bool assign(TokenStoragePtr stoken){
    return l_id_assign(stoken) &&
    t_eq(stoken) &&
    e_var_exp(stoken);
}

bool fn_call(TokenStoragePtr stoken){
    return t_id(stoken) &&
    t_op_bracket(stoken) &&
    call_params(stoken) &&
    t_cl_bracket(stoken) &&
    t_semicolon(stoken);
}

bool ifj_call(TokenStoragePtr stoken){
    return t_ifj(stoken) &&
    t_dot(stoken) &&
    t_id_ifj(stoken) &&
    t_op_bracket(stoken) &&
    call_params(stoken) &&
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
    /*else if(stoken->SToken->type == ID && ID -> var){
        return assign(stoken) &&
        fn_body(stoken);
    }
    else if(stoken->SToken->type == ID && ID -> function){
        return fn_call(stoken) &&
        fn_body(stoken);
    }*/
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
    t_id(stoken) &&
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

