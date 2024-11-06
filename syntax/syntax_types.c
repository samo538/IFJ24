
#include <stdbool.h>
#include "syntax.h"
#include "../lexer/token.h"
#include "../lexer/lexer.h"


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


bool t_ifj(TokenStoragePtr stoken){
    if (stoken->SToken->type == IFJ){
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = NULL;
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else if (stoken->SToken->type == I32){
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
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
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        return true;
    }
}
