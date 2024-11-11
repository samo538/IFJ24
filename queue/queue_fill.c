#include <stdbool.h>
#include <stdio.h>
#include <string.h>
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
        tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

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
        tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

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
bool t_id_func_q(TokenStoragePtr stoken, Elem_id **TableElement){
    if (stoken->SToken->type == ID){
        if(TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table) != NULL){
            fprintf(stderr, "double fn def\n");
            exit(5);
        }

        Elem_id elem; // Creation of an element
        elem.Type = FUNCTION;
        elem.name = stoken->SToken->value.str;
        elem.level_stack = NULL;
        elem.stack_size = 0;
        elem.FnVar.Fn_id.num_of_params = 0;
        elem.FnVar.Fn_id.type_of_params = NULL;
        elem.FnVar.Fn_id.TableParams = NULL;
        elem.FnVar.Fn_id.return_type.nullable = false; // Implicit nullable false
        elem.FnVar.Fn_id.LocalSymTable = TableInit();

        TableAdd(elem, stoken->glob_table); // Adding and element to the symtable
        *TableElement = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);

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

bool t_id_param_q(TokenStoragePtr stoken, Elem_id *new){
    if (stoken->SToken->type == ID){
        Elem_id *tmp;
        tmp = TableSearch(stoken->current_fn, NULL,0, stoken->glob_table);

        new->name = strdup(stoken->SToken->value.str);

        // Adding the number of params and their type
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
        tmp = TableSearch(stoken->current_fn, NULL, 0, stoken->glob_table);
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
        tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

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
        tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

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

bool t_nullable_q_new(TokenStoragePtr stoken, bool ret, Elem_id *new){
    if (stoken->SToken->type == NULLABLE){
        Elem_id *tmp;
        new->FnVar.Var_id.type.nullable = true;
        tmp = TableSearch(stoken->current_fn, NULL, 0, stoken->glob_table);
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
bool t_type_keyword_q_new(TokenStoragePtr stoken, bool ret, Elem_id *new){
    if (stoken->SToken->type == OPENING_SQUARE_BRACKET){
        new->FnVar.Var_id.type.type = U8;
        return t_op_sq_bracket_q(stoken) &&
        t_cl_sq_bracket_q(stoken) &&
        t_u8_q(stoken, ret);
    }
    else if (stoken->SToken->type == F64){
        Elem_id *tmp;
        new->FnVar.Var_id.type.type = F64;
        tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

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
        new->FnVar.Var_id.type.type = I32;
        tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

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

bool l_type_q(TokenStoragePtr stoken, bool ret, Elem_id *new){
    if (new == NULL){
        return t_nullable_q(stoken, ret) &&
        t_type_keyword_q(stoken, ret);
    }
    else {
        return t_nullable_q_new(stoken, ret, new) &&
        t_type_keyword_q_new(stoken, ret, new);
    }
}

bool l_type_fndef_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        return t_void_q(stoken);
    }
    else {
        return l_type_q(stoken, true, NULL);
    }
}

bool params_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        Elem_id *tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);
        bool ret;
        Elem_id *new = malloc(sizeof(Elem_id)); // Creating a new local element
        new->Type = VARIABLE;
        new->stack_size = 1; // Setting the stack
        new->level_stack = malloc(sizeof(int));
        new->level_stack[0] = 1;
        new->FnVar.Var_id.type.type = END_OF_FILE; // Placeholder
        new->FnVar.Var_id.type.nullable = false; // Implicit false
        new->FnVar.Var_id.const_t = true;
        new->FnVar.Var_id.used = false; // Variable not used by default

        ret = t_id_param_q(stoken, new) &&
        t_colon_q(stoken) &&
        l_type_q(stoken, false, new) &&
        t_comma_q(stoken) &&
        params_q(stoken);

        // Adding to local sym table and to function parameters
        TableAdd(*new, tmp->FnVar.Fn_id.LocalSymTable);
        int *level = malloc(sizeof(int));
        level[0] = 1;
        tmp->FnVar.Fn_id.TableParams = realloc(tmp->FnVar.Fn_id.TableParams, sizeof(Elem_id *) * tmp->FnVar.Fn_id.num_of_params);
        tmp->FnVar.Fn_id.TableParams[tmp->FnVar.Fn_id.num_of_params - 1] = TableSearch(new->name, level, 1, tmp->FnVar.Fn_id.LocalSymTable);
        free(level);
        free(new->name);
        free(new->level_stack);
        free(new);

        return ret;
    }
    else {
        return true;
    }
}

bool fn_def_q(TokenStoragePtr stoken, Elem_id **table_element){
    return t_pub_q(stoken) &&
    t_fn_q(stoken) &&
    t_id_func_q(stoken, table_element) &&
    t_op_bracket_q(stoken) &&
    params_q(stoken) &&
    t_cl_bracket_q(stoken) &&
    l_type_fndef_q(stoken);
}
