#include <stdbool.h>
#include <stdio.h>
#include "../syntax/syntax.h"
#include "../lexer/lexer.h"
#include "queue_fill.h"

bool t_pub_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == PUB){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}

bool t_fn_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == FN){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_cl_bracket_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == CLOSING_BRACKET){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_colon_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == COLON){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_comma_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == COMMA){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_op_bracket_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == OPENING_BRACKET){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_op_sq_bracket_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == OPENING_SQUARE_BRACKET){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_cl_sq_bracket_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == CLOSING_SQUARE_BRACKET){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_u8_q(TokenStoragePtr stoken, bool ret){
    if (stoken->SToken->type == U8){
        Elem_id *tmp;
        tmp = TableSearch(stoken->current_fn, 0, stoken->glob_table);

        if (ret == false){
            tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].type = U8;
        }
        else{
            tmp->FnVar.Fn_id.return_type.type = U8;
        }

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_void_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        Elem_id *tmp;
        tmp = TableSearch(stoken->current_fn, 0, stoken->glob_table);

        tmp->FnVar.Fn_id.return_type.type = VOID;

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}
bool t_id_func_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        if(TableSearch(stoken->SToken->value.str, 0, stoken->glob_table) != NULL){
            fprintf(stderr, "double fn def\n"); // Some kind of error
        }

        Elem_id elem; // Creation of an element
        elem.Type = FUNCTION;
        elem.name = stoken->SToken->value.str;
        elem.level = 0;
        elem.FnVar.Fn_id.num_of_params = 0;
        elem.FnVar.Fn_id.type_of_params = NULL;
        elem.FnVar.Fn_id.return_type.nullable = false; // Implicit nullable false
        elem.FnVar.Fn_id.LocalSymTable = TableInit();

        TableAdd(elem, stoken->glob_table); // Adding and element to the symtable

        if (stoken->current_fn != NULL){ // Setting new current fn
            free(stoken->current_fn);
        }
        stoken->current_fn = strdup(stoken->SToken->value.str);

        queue_add_token(stoken->queue, stoken->SToken); // adding token to queue
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}

bool t_id_param_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        Elem_id *tmp;
        tmp = TableSearch(stoken->current_fn, 0, stoken->glob_table);

        int count = ++tmp->FnVar.Fn_id.num_of_params;
        tmp->FnVar.Fn_id.type_of_params = realloc(tmp->FnVar.Fn_id.type_of_params, sizeof(Id_type) * count);
        tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].nullable = false; // Implicit false on nullable;

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}

bool t_nullable_q(TokenStoragePtr stoken, bool ret){
    if (stoken->SToken->type == NULLABLE){
        Elem_id *tmp;
        tmp = TableSearch(stoken->current_fn, 0, stoken->glob_table);
        if (ret == false){
            tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].nullable = true;
        }
        else{
            tmp->FnVar.Fn_id.return_type.nullable = true;
        }

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        return true;
    }
}
bool t_type_keyword_q(TokenStoragePtr stoken, bool ret){
    if (stoken->SToken->type == OPENING_SQUARE_BRACKET){
        return t_op_sq_bracket_q(stoken) &&
        t_cl_sq_bracket_q(stoken) &&
        t_u8_q(stoken, ret);
    }
    else if (stoken->SToken->type == F64){
        Elem_id *tmp;
        tmp = TableSearch(stoken->current_fn, 0, stoken->glob_table);

        if (ret == false){
            tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].type = F64;
        }
        else{
            tmp->FnVar.Fn_id.return_type.type = F64;
        }

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else if (stoken->SToken->type == I32){
        Elem_id *tmp;
        tmp = TableSearch(stoken->current_fn, 0, stoken->glob_table);

        if (ret == false){
            tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].type = I32;
        }
        else{
            tmp->FnVar.Fn_id.return_type.type = I32;
        }

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        syn_error(stoken);
        return false;
    }
}


bool l_type_q(TokenStoragePtr stoken, bool ret){
    return t_nullable_q(stoken, ret) &&
    t_type_keyword_q(stoken, ret);
}

bool l_type_fndef_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        return t_void_q(stoken);
    }
    else {
        return l_type_q(stoken, true);
    }
}

bool params_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        return t_id_param_q(stoken) &&
        t_colon_q(stoken) &&
        l_type_q(stoken, false) &&
        t_comma_q(stoken) &&
        params_q(stoken);
    }
    else {
        return true;
    }
}

bool fn_def_q(TokenStoragePtr stoken){
    return t_pub_q(stoken) &&
    t_fn_q(stoken) &&
    t_id_func_q(stoken) &&
    t_op_bracket_q(stoken) &&
    params_q(stoken) &&
    t_cl_bracket_q(stoken) &&
    l_type_fndef_q(stoken);
}
