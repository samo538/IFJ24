/**
 *  @file queue_fill.c
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "../errors/error.h"
#include "queue_fill.h"

bool q_expect(TokenStoragePtr stoken, enum TokenType type){
    if (stoken->SToken->type == END_OF_FILE){
        throw_error(stoken, 2);
    }
    if (stoken->SToken->type == type){
        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}

bool q_u8(TokenStoragePtr stoken, Elem_id *new){
    if (stoken->SToken->type == U8){
        Elem_id *tmp;
        tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

        if (new != NULL){
            tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].type = U8;
            new->FnVar.Var_id.type.type = U8;
        }
        else{
            tmp->FnVar.Fn_id.return_type.type = U8;
        }

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}
bool q_void(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        Elem_id *tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

        tmp->FnVar.Fn_id.return_type.type = VOID;

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}



bool q_nullable(TokenStoragePtr stoken, Elem_id *new){
    if (stoken->SToken->type == NULLABLE){
        Elem_id *tmp = TableSearch(stoken->current_fn, NULL, 0, stoken->glob_table);

        if (new != NULL){
            tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].nullable = true;
            new->FnVar.Var_id.type.nullable = true;
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
bool q_type_keyword(TokenStoragePtr stoken, Elem_id *new){
    Elem_id *tmp = TableSearch(stoken->current_fn,NULL, 0, stoken->glob_table);

    if (stoken->SToken->type == OPENING_SQUARE_BRACKET){
        if (new != NULL){
            new->FnVar.Var_id.type.type = U8;
        }
        return q_expect(stoken, OPENING_SQUARE_BRACKET) &&
        q_expect(stoken, CLOSING_SQUARE_BRACKET) &&
        q_u8(stoken, new);
    }
    else if (stoken->SToken->type == F64){
        if (new != NULL){
            tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].type = F64;
            new->FnVar.Var_id.type.type = F64;
        }
        else{
            tmp->FnVar.Fn_id.return_type.type = F64;
        }

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else if (stoken->SToken->type == I32){
        if (new != NULL){
            tmp->FnVar.Fn_id.type_of_params[tmp->FnVar.Fn_id.num_of_params - 1].type = I32;
            new->FnVar.Var_id.type.type = I32;
        }
        else{
            tmp->FnVar.Fn_id.return_type.type = I32;
        }

        queue_add_token(stoken->queue, stoken->SToken);
        stoken->SToken = next_token();
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}

bool q_type_param(TokenStoragePtr stoken, Elem_id *new){
    return q_nullable(stoken, new) &&
    q_type_keyword(stoken, new);
}

bool l_type_fndef_q(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        return q_void(stoken);
    }
    else {
        return q_type_param(stoken, NULL);
    }
}

bool q_id_param(TokenStoragePtr stoken, Elem_id *new){
    if (stoken->SToken->type == ID){
        Elem_id *tmp = TableSearch(stoken->current_fn, NULL,0, stoken->glob_table);

        // Setting the name of the variable
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
        throw_error(stoken, 2);
    }
}

bool q_params(TokenStoragePtr stoken){
    if (stoken->SToken->type == ID){
        Elem_id *tmp = TableSearch(stoken->current_fn, NULL, 0, stoken->glob_table);
        bool ret;

        // storing info about the variable here
        Elem_id *info_var = malloc(sizeof(Elem_id)); // Creating a new local element

        ret = q_id_param(stoken, info_var) &&
        q_expect(stoken, COLON) &&
        q_type_param(stoken, info_var) &&
        q_expect(stoken, COMMA);

        check_ret(stoken, ret);

        int *level_stack = malloc(sizeof(int));
        level_stack[0] = 1;
        Elem_id *new_var = TableAdd(info_var->name, level_stack, 1,tmp->FnVar.Fn_id.LocalSymTable);

        new_var->name = strdup(info_var->name);
        new_var->level_stack = level_stack;
        new_var->stack_size = 1;
        new_var->Type = VARIABLE;
        new_var->FnVar = info_var->FnVar;
        new_var->FnVar.Var_id.const_t = true; // function parameters are const
        new_var->FnVar.Var_id.used = false; // function parameters are const

        // Adding to the TableParams
        tmp->FnVar.Fn_id.TableParams = realloc(tmp->FnVar.Fn_id.TableParams, sizeof(Elem_id *) * tmp->FnVar.Fn_id.num_of_params);
        tmp->FnVar.Fn_id.TableParams[tmp->FnVar.Fn_id.num_of_params - 1] = new_var;

        // Freeing the allocated memory
        free(info_var->name);
        free(info_var);

        return ret &&
        q_params(stoken);
    }
    else {
        return true;
    }
}

bool q_id_func(TokenStoragePtr stoken, Elem_id **TableElement){
    if (stoken->SToken->type == ID){
        Elem_id *new_fn; // Creation of an element

        // TableAdd also checks for redefinitions
        new_fn = TableAdd(stoken->SToken->value.str, NULL, 0, stoken->glob_table); // Adding and element to the symtable

        // Fills the new element
        new_fn->Type = FUNCTION;
        new_fn->name = strdup(stoken->SToken->value.str);
        new_fn->level_stack = NULL;
        new_fn->stack_size = 0;
        new_fn->FnVar.Fn_id.num_of_params = 0;
        new_fn->FnVar.Fn_id.type_of_params = NULL;
        new_fn->FnVar.Fn_id.TableParams = NULL;
        new_fn->FnVar.Fn_id.return_type.nullable = false; // Implicit nullable false
        new_fn->FnVar.Fn_id.LocalSymTable = TableInit();

        // Adding to tree
        *TableElement = new_fn;

        free(stoken->current_fn); // Free on NULL does nothing
        stoken->current_fn = strdup(stoken->SToken->value.str);

        queue_add_token(stoken->queue, stoken->SToken); // adding token to queue
        stoken->SToken = next_token();
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}

bool fn_def_q(TokenStoragePtr stoken, Elem_id **table_element){
    return q_expect(stoken, PUB) &&
    q_expect(stoken, FN) &&
    q_id_func(stoken, table_element) &&
    q_expect(stoken, OPENING_BRACKET) &&
    q_params(stoken) &&
    q_expect(stoken, CLOSING_BRACKET) &&
    l_type_fndef_q(stoken);
}
