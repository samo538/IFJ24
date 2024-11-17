/**
 *  @file syntax.c
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "syntax.h"
#include "../lexer/token.h"
#include "../lexer/lexer.h"
#include "../queue/queue.h"
#include "../symtable/symtable.h"
#include "../precedence/precedence.h"
#include "../tree/tree.h"
#include "../code_gen/code_gen.h"
#include "../errors/error.h"

// expect function for simple syntax checks
bool t_expect(TokenStoragePtr stoken, enum TokenType type){
    if (stoken->SToken->type == type){
        dealloc_token(stoken->SToken);
        if (type != END_OF_FILE){
            stoken->SToken = queue_next_token(stoken->queue);
        }
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}

/*
 *  Variable definition
 */
bool ifj_call_var(TokenStoragePtr stoken, Elem_id *new){
    bool ret;
    ret = t_expect(stoken, IFJ) &&
    t_expect(stoken, DOT);

    check_ret(stoken, ret);

    if (stoken->SToken->type == ID){
        Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->ifj_table); // Search inside IFJ
        if (fn == NULL){
            throw_error(stoken, 3); // Undefined IFJ function
        }
        if (!strcmp(fn->name, "write")){
            throw_error(stoken, 7); // Assigning void
        }

        // Creating a new tree node
        TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = IFJ_FUNCTION_NODE;
        new_node->Data.TableElement = fn;
        stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

        if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Implicit type
            new->FnVar.Var_id.type.type = fn->FnVar.Fn_id.return_type.type;
            new->FnVar.Var_id.type.nullable = fn->FnVar.Fn_id.return_type.nullable;
        }
        else { // Explicit type, check data types
            if (new->FnVar.Var_id.type.type != fn->FnVar.Fn_id.return_type.type) {
                throw_error(stoken, 7); // Wrong assign type
            }
            if (fn->FnVar.Fn_id.return_type.nullable){
                if(!new->FnVar.Fn_id.return_type.nullable){
                    throw_error(stoken, 7); // Wrong assign type
                }
            }
        }
        ret = ret &&
        t_expect(stoken, ID) &&
        t_expect(stoken, OPENING_BRACKET) &&
        call_params(stoken, fn, 0) && // Checking the validity of params
        t_expect(stoken, CLOSING_BRACKET) &&
        t_expect(stoken, SEMICOLON);

        stoken->current_node = stoken->current_node->DadNode;

        return ret;
    }
    throw_error(stoken, 2);
}

bool def_preced(TokenStoragePtr stoken, Elem_id *new){
    PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
    if (result == NULL){
        throw_error(NULL, 99);
    }
    if (result->Error != 0){
        throw_error(stoken, result->Error); // Expression error
    }
    if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Implicit type
        new->FnVar.Var_id.type.type = result->Tree->Data.Type - 12;
        new->FnVar.Var_id.type.nullable = false;
    }
    else { // Explicit type, check data types
        if (new->FnVar.Var_id.type.type != result->Tree->Data.Type - 12) {
            throw_error(stoken, 7); // Wrong assign type
        }
    }
    stoken->SToken = result->NextTotken;
    // Connecting expression to the main tree
    TreeElementConnect(stoken->current_node, result->Tree);
    return t_expect(stoken, SEMICOLON);
}

bool e_var_exp_def(TokenStoragePtr stoken, Elem_id *new){
    Elem_id *ret_fn = NULL;
    Elem_id *ret_var = NULL;
    if (stoken->SToken->type == ID){ // If id search if its a function or variable
        ret_fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        ret_var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        // If both pointers not null
        if (ret_fn != NULL && ret_var != NULL){
            throw_error(stoken, 10);
        }
        if (ret_fn == NULL && ret_var == NULL){
            throw_error(stoken, 3);
        }
    }

    // Assigning IFJ func
    if (stoken->SToken->type == IFJ){
        return ifj_call_var(stoken, new);
    }

    // Assigning func
    else if (ret_fn != NULL){ 
        if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Implicit type
            if (ret_fn->FnVar.Fn_id.return_type.type == VOID){
                throw_error(stoken, 7); // Assigning void
            }
            new->FnVar.Var_id.type.type = ret_fn->FnVar.Fn_id.return_type.type;
            new->FnVar.Var_id.type.nullable = ret_fn->FnVar.Fn_id.return_type.nullable;
        }
        else { // Explicit type, check data types
            if (new->FnVar.Var_id.type.type != ret_fn->FnVar.Fn_id.return_type.type) {
                throw_error(stoken, 7); // wrong assign type
            }
            if (ret_fn->FnVar.Fn_id.return_type.nullable){ // Check nullables
                if(!new->FnVar.Fn_id.return_type.nullable){
                    throw_error(stoken, 7); // wrong assign type
                }
            }
        }
        return fn_call(stoken);
    }

    // Assigning null
    else if(stoken->SToken->type == NULL_VALUE){
        if (new->FnVar.Var_id.type.type == END_OF_FILE){
            throw_error(stoken, 8); // Cannot determine type
        }
        else if(new->FnVar.Var_id.type.nullable == false){
            throw_error(stoken, 7); // Cannot assign null
        }
        // Copy the null pointer
        TreeElement *new = TreeInsert(stoken->current_node, NULL);
        new->Data.NodeType = EXPRESSION_NODE;
        new->Data.Token = copy_token(stoken->SToken);

        return t_expect(stoken, NULL_VALUE) &&
        t_expect(stoken, SEMICOLON);
    }

    // Assigning id expression or string variable
    else if(ret_var != NULL){
        if (ret_var->FnVar.Var_id.type.type == U8){
            if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Implicit type
                new->FnVar.Var_id.type.type = U8;
                new->FnVar.Var_id.type.nullable = ret_var->FnVar.Var_id.type.nullable;
            }
            else { // Explicit type, check data types
                if (new->FnVar.Var_id.type.type != ret_var->FnVar.Var_id.type.type) {
                    throw_error(stoken, 7); // Wrong assign type
                }
                if (ret_var->FnVar.Var_id.type.nullable){ // Check nullables
                    if(!new->FnVar.Var_id.type.nullable){
                        throw_error(stoken, 7); // Wrong assign type
                    }
                }
            }
            // New tree node
            TreeElement *new = TreeInsert(stoken->current_node, NULL);
            new->Data.NodeType = EXPRESSION_NODE;
            new->Data.Token = copy_token(stoken->SToken);
            new->Data.TableElement = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);

            ret_var->FnVar.Var_id.used = true;
            return t_expect(stoken, ID) &&
            t_expect(stoken, SEMICOLON);
        }
        else {
            return def_preced(stoken, new);
        }
    }

    // Assigning non-string literals
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        return def_preced(stoken, new);
    }

    // Assigning string
    else if(stoken->SToken->type == STRING){
        throw_error(stoken, 8); // Assigning string not allowed
    }
    throw_error(stoken, 2);
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
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else if (stoken->SToken->type == I32){
        if (new != NULL){
            new->FnVar.Var_id.type.type = I32;
        }
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}

bool l_type_var(TokenStoragePtr stoken, Elem_id *new){
    return t_nullable_var(stoken, new) && // Set nullable
    t_type_keyword(stoken, new); // Set type
}

bool l_type_vardef(TokenStoragePtr stoken, Elem_id * new){
    if (stoken->SToken->type == COLON){ // Explicit definiton
        return t_expect(stoken, COLON) &&
        l_type_var(stoken, new);
    }
    else { // Implicit definition
        return true;
    }
}

bool l_var_const(TokenStoragePtr stoken, Elem_id *new){
    bool ret;
    if (stoken->SToken->type == VAR){
        new->FnVar.Var_id.const_t = false; // Setting the const flag
        ret =  t_expect(stoken, VAR);
    }
    else {
        new->FnVar.Var_id.const_t = true; // Setting the const flag
        ret = t_expect(stoken, CONST);
    }
    return ret;
}

bool t_id_var(TokenStoragePtr stoken, Elem_id *new){
    if (stoken->SToken->type == ID){
        Elem_id *tmp = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (tmp != NULL){
            throw_error(stoken, 5); //Redefinition shadowing not allowed
        }
        tmp = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        if (tmp != NULL){
            throw_error(stoken, 5); // Function with the same name as variable not defined
        }
        new->name = strdup(stoken->SToken->value.str);

        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}

bool var_def(TokenStoragePtr stoken){
    bool ret;

    // New node in tree and moving deeper into the tree
    TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
    new_node->Data.NodeType = DEFINITION_NODE;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    Elem_id *new = malloc(sizeof(Elem_id)); // Creating a new local element
    new->stack_size = stoken->stack_size; // Setting the stack
    copy_levels(stoken->level_stack, &new->level_stack, stoken->stack_size);
    new->FnVar.Var_id.type.type = END_OF_FILE; // Placeholder
    new->FnVar.Var_id.type.nullable = false; // Implicit false

    // Tree creation
    TreeElement *tree_id = TreeInsert(stoken->current_node, NULL);
    tree_id->Data.NodeType = ID_NODE;

    // Tree node for ID created in t_id_var
    ret = l_var_const(stoken, new) && // Filling up new
    t_id_var(stoken, new) && // Filling up the name / checking for redef
    l_type_vardef(stoken, new) && // Implicit/Explicit definition
    t_expect(stoken, ASSIGN) &&
    e_var_exp_def(stoken, new); // Adding value

    check_ret(stoken, ret);

    Elem_id *inserted =  TableAdd(new->name, new->level_stack, new->stack_size, stoken->local_table);
    inserted->name = strdup(new->name);
    inserted->Type = VARIABLE;
    inserted->stack_size = stoken->stack_size; // Setting the stack
    copy_levels(stoken->level_stack, &inserted->level_stack, stoken->stack_size);
    inserted->FnVar.Var_id.type.type = new->FnVar.Var_id.type.type; // Placeholder
    inserted->FnVar.Var_id.type.nullable = new->FnVar.Var_id.type.nullable; // Implicit false
    inserted->FnVar.Var_id.const_t = new->FnVar.Var_id.const_t;
    inserted->FnVar.Var_id.used = false; // Variable not used by default*/
    tree_id->Data.TableElement = inserted;

    // Free the helper variable
    free(new->name); // freeing the added element
    free(new->level_stack);
    free(new);

    // Moving upper in the tree
    stoken->current_node = stoken->current_node->DadNode;

    return ret;
}

/*
 *  Function call
 */
bool call_params(TokenStoragePtr stoken, Elem_id *fn, int pos){
    enum TokenType type = stoken->SToken->type;
    if (type == STRING || type == I32_VAR || type == F64_VAR){
        enum TokenType type_def;
        switch (type) {
            case STRING:
                type_def = U8;
                break;
            case I32_VAR:
                type_def = I32;
                break;
            case F64_VAR:
                type_def = F64;
                break;
        }
        if (pos == fn->FnVar.Fn_id.num_of_params){
            throw_error(stoken, 4); // Too many params
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].type == type_def){
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
            return t_expect(stoken, type) &&
            t_expect(stoken, COMMA) &&
            call_params(stoken, fn, pos + 1);
        }
        else {
            throw_error(stoken, 4); // Wrong type in fn
        }
    }
    else if(stoken->SToken->type == NULL_VALUE) {
        if (pos == fn->FnVar.Fn_id.num_of_params){
            throw_error(stoken, 4); // Too many params
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].nullable == true){
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
            return t_expect(stoken, NULL_VALUE) &&
            t_expect(stoken, COMMA) &&
            call_params(stoken, fn, pos + 1);
        }
        else {
            throw_error(stoken, 4); // Wrong type in fn
        }
    }
    else if(stoken->SToken->type == ID) {
        if (pos == fn->FnVar.Fn_id.num_of_params){
            throw_error(stoken, 4); // Too many params
        }
        Elem_id *tmp = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (tmp == NULL){
            throw_error(stoken, 3); // Undefined variable
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].type == tmp->FnVar.Var_id.type.type){
            tmp->FnVar.Var_id.used = true; // Used this var

            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
            new_node->Data.TableElement = tmp;

            return t_expect(stoken, ID) &&
            t_expect(stoken, COMMA) &&
            call_params(stoken, fn, pos + 1);
        }
        else {
            throw_error(stoken, 4); // Wrong type in fn
        }
    }
    else if (stoken->SToken->type == CLOSING_BRACKET){
        if (fn->FnVar.Fn_id.num_of_params != pos){
            throw_error(stoken, 4); // Not enough params
        }
        return true;
    }
    else {
        throw_error(stoken, 2);
    }
}

bool fn_call(TokenStoragePtr stoken){
    bool ret;
    Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
    if (fn == NULL){
        throw_error(stoken, 3); // Undefined function
    }

    // Creating a new tree node
    TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
    new_node->Data.NodeType = FUNCTION_NODE;
    new_node->Data.TableElement = fn;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    ret = t_expect(stoken, ID) &&
    t_expect(stoken, OPENING_BRACKET) &&
    call_params(stoken, fn, 0) && // Sematics action, check params validity and fill tree
    t_expect(stoken, CLOSING_BRACKET) &&
    t_expect(stoken, SEMICOLON);

    stoken->current_node = stoken->current_node->DadNode;

    return ret;
}

/*
 *  IFJ function call
 */
bool ifj_call(TokenStoragePtr stoken){
    bool ret;
    ret = t_expect(stoken, IFJ) &&
    t_expect(stoken, DOT);

    if (stoken->SToken->type == ID){
        Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->ifj_table);
        if (fn == NULL){
            throw_error(stoken, 3); // undefined IFJ function
        }
        if (strcmp(fn->name, "write")){
            throw_error(stoken, 4); // return type dumped
        }
        ret = ret &&
        t_expect(stoken, ID) &&
        t_expect(stoken, OPENING_BRACKET);

        check_ret(stoken, ret);

        // Creating a new tree element and going deeper
        TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = IFJ_FUNCTION_NODE;
        new_node->Data.TableElement = fn;
        stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

        enum TokenType type = stoken->SToken->type;
        if (type == F64_VAR || type == I32_VAR || type == STRING || type == NULL_VALUE){
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);

            ret = ret && t_expect(stoken, type);
            check_ret(stoken, ret);
        }
        else if (stoken->SToken->type == ID){
            Elem_id *var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
            if (var == NULL){
                throw_error(stoken, 3); // undefined variable
            }

            var->FnVar.Var_id.used = true; // Used this variable
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
            new_node->Data.TableElement = var;
            ret = ret && t_expect(stoken, ID);

            check_ret(stoken, ret);
        }
        else {
            throw_error(stoken, 2);
        }
        //Moving up in the tree
        stoken->current_node = stoken->current_node->DadNode;

        return ret &&
        t_expect(stoken, COMMA) &&
        t_expect(stoken, CLOSING_BRACKET) &&
        t_expect(stoken, SEMICOLON);
    }
    throw_error(stoken, 2);
}

/*
 *  Assign
 */
bool ifj_call_assign(TokenStoragePtr stoken, bool underscore ,Elem_id *assign_to){
    bool ret;
    ret = t_expect(stoken, IFJ) &&
    t_expect(stoken, DOT);

    check_ret(stoken, ret);

    if (stoken->SToken->type == ID){
        Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->ifj_table);
        if (fn == NULL){
            throw_error(stoken, 3); // Undefined IFJ function
        }
        if (!strcmp(fn->name, "write")){
            throw_error(stoken, 7); // Assigning void
        }
        if (!underscore && assign_to->FnVar.Var_id.type.type != fn->FnVar.Fn_id.return_type.type) {
            throw_error(stoken, 7); // Wrong assign type
        }
        if (!underscore && fn->FnVar.Fn_id.return_type.nullable){
            if(!assign_to->FnVar.Fn_id.return_type.nullable){
                throw_error(stoken, 7); // Wrong assign type
            }
        }
        // Creating a new tree node
        TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = IFJ_FUNCTION_NODE;
        new_node->Data.TableElement = fn;
        stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

        ret = ret &&
        t_expect(stoken, ID) &&
        t_expect(stoken, OPENING_BRACKET) &&
        call_params(stoken, fn, 0) && // Checking the validity of params
        t_expect(stoken, CLOSING_BRACKET) &&
        t_expect(stoken, SEMICOLON);

        stoken->current_node = stoken->current_node->DadNode;

        return ret;
    }
    throw_error(stoken, 2);
}
bool assign_prec(TokenStoragePtr stoken, bool underscore, Elem_id * assign_to){
    PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
    if (result == NULL){
        throw_error(NULL, 99);
    }
    if (result->Error != 0){
        throw_error(stoken, result->Error); // Expression error
    }
    if (!underscore && assign_to->FnVar.Var_id.type.type != result->Tree->Data.Type - 12) {
        throw_error(stoken, 7); // Wrong assign type
    }
    stoken->SToken = result->NextTotken;
    // Connecting expression to the main tree
    TreeElementConnect(stoken->current_node, result->Tree);
    return t_expect(stoken, SEMICOLON);
}

bool e_var_exp_assign(TokenStoragePtr stoken, bool underscore, Elem_id *assign_to){
    Elem_id *ret_fn = NULL;
    Elem_id *ret_var = NULL;
    if (stoken->SToken->type == ID){ // If id search if its a function or variable
        ret_fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        ret_var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        // If both pointers not null
        if (ret_fn != NULL && ret_var != NULL){
            throw_error(stoken, 10); // Function with the same name as variable
        }
        if (ret_fn == NULL && ret_var == NULL){
            throw_error(stoken, 3); // Function or variable not defined
        }
    }

    // Assigning IFJ func
    if (stoken->SToken->type == IFJ){
        return ifj_call_assign(stoken, underscore, assign_to);
    }

    // Assigning func
    else if (ret_fn != NULL){ 
        if (ret_fn->FnVar.Fn_id.return_type.type == VOID){
            throw_error(stoken, 7); // Assigning void
        }
        if (!underscore && assign_to->FnVar.Var_id.type.type != ret_fn->FnVar.Fn_id.return_type.type) {
            throw_error(stoken, 7); // Wrong assign type
        }
        if (!underscore && ret_fn->FnVar.Fn_id.return_type.nullable){
            if(!assign_to->FnVar.Fn_id.return_type.nullable){
                throw_error(stoken, 7); // Wrong assign type
            }
        }
        return fn_call(stoken);
    }

    // Assigning null
    else if(stoken->SToken->type == NULL_VALUE){
        if(!underscore && assign_to->FnVar.Var_id.type.nullable == false){
            throw_error(stoken, 7); // cannot assign null
        }
        // Copy the null pointer
        TreeElement *new = TreeInsert(stoken->current_node, NULL);
        new->Data.NodeType = EXPRESSION_NODE;
        new->Data.Token = copy_token(stoken->SToken);

        return t_expect(stoken, NULL_VALUE) &&
        t_expect(stoken, SEMICOLON);
    }

    // Assigning id expression or string variable
    else if(ret_var != NULL){
        if (ret_var->FnVar.Var_id.type.type == U8){
            if (!underscore && assign_to->FnVar.Var_id.type.type != ret_var->FnVar.Var_id.type.type) {
                throw_error(stoken, 7); // Wrong assign type
            }
            if (!underscore && ret_var->FnVar.Var_id.type.nullable){
                if(!assign_to->FnVar.Var_id.type.nullable){
                    throw_error(stoken, 7); // Wrong assign type
                }
            }
            ret_var->FnVar.Var_id.used = true; // Used this var
            // New tree node
            TreeElement *new = TreeInsert(stoken->current_node, NULL);
            new->Data.NodeType = EXPRESSION_NODE;
            new->Data.Token = copy_token(stoken->SToken);
            new->Data.TableElement = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);

            return t_expect(stoken, ID) &&
            t_expect(stoken, SEMICOLON);
        }
        else {
            return assign_prec(stoken, underscore, assign_to);
        }
    }

    // Assigning non-string literals
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        return assign_prec(stoken, underscore, assign_to);
    }

    // Assigning string
    else if(stoken->SToken->type == STRING){
        throw_error(stoken, 7); // Assigning string not allowed
    }
    throw_error(stoken, 2);
}

bool l_id_assign(TokenStoragePtr stoken, bool *underscore, Elem_id **assign_to){
    if (stoken->SToken->type == UNDERSCORE){
        TreeElement *tree_id = TreeInsert(stoken->current_node, NULL);
        tree_id->Data.NodeType = ID_NODE;
        tree_id->Data.TableElement = NULL;
        tree_id->Data.Token = copy_token(stoken->SToken);
        *underscore = true;
        return t_expect(stoken, UNDERSCORE);
    }
    else {
        *underscore = false;
        *assign_to = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        Elem_id *tmp = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        if (*assign_to == NULL){
            throw_error(stoken, 3); // undefined variable
        }
        if (*assign_to != NULL && tmp != NULL){
            throw_error(stoken, 3); // Variable and function with the same name
        }
        if ((*assign_to)->FnVar.Var_id.const_t == true){
            throw_error(stoken, 5); // Assigning to const
        }
        TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = ID_NODE;
        new_node->Data.TableElement = *assign_to;
        new_node->Data.Token = copy_token(stoken->SToken);

        return t_expect(stoken, ID);
    }
}
bool assign(TokenStoragePtr stoken){
    bool ret;
    bool underscore;
    Elem_id *assign_to;

    TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
    new_node->Data.NodeType = ASSIGN_NODE;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    ret = l_id_assign(stoken, &underscore, &assign_to) && // Assign_to
    t_expect(stoken, ASSIGN) &&
    e_var_exp_assign(stoken, underscore, assign_to); // Assigment

    stoken->current_node = stoken->current_node->DadNode;

    return ret;
}

/*
 *  IfElse // While
 */

bool if_while_body(TokenStoragePtr stoken){
    return t_expect(stoken, OPENING_CURLY_BRACKET) &&
    fn_body(stoken) &&
    t_expect(stoken, CLOSING_CURLY_BRACKET);
}

bool if_while_preced(TokenStoragePtr stoken, bool ifelse){
    bool ret;
    PrecResultPtr result = preced_analysis(stoken->SToken, true, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
    if (result == NULL){
        throw_error(NULL, 99);
    }
    if (result->Error != 0){
        throw_error(stoken, result->Error); // Expression error
    }
    stoken->SToken = result->NextTotken;

    //Changind the position is stack
    stoken->stack_size++;
    stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);
    stoken->level_stack[stoken->stack_size - 1] = stoken->last_poped + 1; // Adding values to stack

    if (stoken->level_stack == NULL){
        throw_error(NULL, 99);
    }

    // New tree node
    TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
    if (ifelse){
        new_node->Data.NodeType = IF_NODE;
    }
    else{
        new_node->Data.NodeType = WHILE_NODE;
    }
    new_node->Data.isNullCond = false;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    // Inserting Empty Node, because null cond is false
    TreeElement *emptynode = TreeInsert(stoken->current_node, NULL);
    emptynode->Data.NodeType = ID_NODE;

    // Inserting the expression
    TreeElementConnect(stoken->current_node, result->Tree);


    ret = t_expect(stoken, CLOSING_BRACKET) &&
    if_while_body(stoken);

    check_ret(stoken, ret);

    if (ifelse){
        // going into else
        stoken->level_stack[stoken->stack_size - 1]++; // Moving into else
        TreeElement *else_node = TreeInsert(stoken->current_node, NULL);
        else_node->Data.NodeType = ELSE_NODE;

        ret = ret &&
        t_expect(stoken, ELSE) &&
        if_while_body(stoken);

        check_ret(stoken, ret);
    }

    stoken->last_poped = stoken->level_stack[stoken->stack_size - 1]; // Pop last item from the stack
    stoken->stack_size--;
    stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);

    stoken->current_node = stoken->current_node->DadNode;

    if (stoken->level_stack == NULL){
        throw_error(NULL, 99);
    }

    return ret;

}

bool if_while_nullable(TokenStoragePtr stoken, bool ifelse, Elem_id *var_id){
    bool ret;
    var_id->FnVar.Var_id.used = true;

    //Changind the position is stack
    stoken->stack_size++;
    stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);
    stoken->level_stack[stoken->stack_size - 1] = stoken->last_poped + 1; // Adding values to stack

    if (stoken->level_stack == NULL){
        throw_error(NULL, 99);
    }

    // New tree node
    TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
    if (ifelse){
        new_node->Data.NodeType = IF_NODE;
    }
    else {
        new_node->Data.NodeType = WHILE_NODE;
    }
    new_node->Data.isNullCond = true;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    ret = t_expect(stoken, ID) &&
    t_expect(stoken, CLOSING_BRACKET) &&
    t_expect(stoken, VERTICAL_BAR);

    check_ret(stoken, ret);

    if (stoken->SToken->type == ID){
        Elem_id *table_elem = TableAdd(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        table_elem->name = strdup(stoken->SToken->value.str);
        table_elem->Type = VARIABLE;
        table_elem->stack_size = stoken->stack_size;
        copy_levels(stoken->level_stack, &table_elem->level_stack, stoken->stack_size);
        table_elem->FnVar.Var_id.type.type = var_id->FnVar.Var_id.type.type; // Placeholder
        table_elem->FnVar.Var_id.type.nullable = false; // Implicit false
        table_elem->FnVar.Var_id.used = false; // Variable not used by default
        table_elem->FnVar.Var_id.const_t = false; // Variable not used by default

        // Is nullable, Unwrapped variable goes here
        TreeElement *Unwrapped = TreeInsert(stoken->current_node, NULL);
        Unwrapped->Data.NodeType = ID_NODE;
        Unwrapped->Data.TableElement = TableSearch(table_elem->name, table_elem->level_stack, table_elem->stack_size, stoken->local_table);
    }
    else{
        throw_error(stoken, 2);
    }

    // Adding the condition into the tree
    TreeElement *Cond_node = TreeInsert(stoken->current_node, NULL);
    Cond_node->Data.NodeType = EXPRESSION_NODE;
    Cond_node->Data.TableElement = var_id;

    ret = ret &&
    t_expect(stoken, ID) && 
    t_expect(stoken, VERTICAL_BAR) &&
    if_while_body(stoken);

    check_ret(stoken, ret);

    if (ifelse){
        stoken->level_stack[stoken->stack_size - 1]++; // Moving into else
        TreeElement *else_node = TreeInsert(stoken->current_node, NULL);
        else_node->Data.NodeType = ELSE_NODE;

        ret = ret &&
        t_expect(stoken, ELSE) &&
        if_while_body(stoken);

        check_ret(stoken, ret);
    }

    //Changing the position back
    stoken->last_poped = stoken->level_stack[stoken->stack_size - 1]; // Pop last item from the stack
    stoken->stack_size--;
    stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);

    // Going up in the tree
    stoken->current_node = stoken->current_node->DadNode;

    if (stoken->level_stack == NULL){
        throw_error(NULL, 99);
    }
    return ret;
}

bool while_ifelse(TokenStoragePtr stoken, bool ifelse){
    bool ret;
    if (ifelse){
        ret = t_expect(stoken, IF) &&
        t_expect(stoken, OPENING_BRACKET);
    }
    else{
        ret = t_expect(stoken, WHILE) &&
        t_expect(stoken, OPENING_BRACKET);
    }

    check_ret(stoken, ret);

    // Check the variant of the condition (null or exp)
    if (stoken->SToken->type == ID){
        Elem_id * var_id = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (var_id == NULL){
            throw_error(stoken, 3); // undefined variable
        }
        if (var_id->FnVar.Var_id.type.nullable){ // ID but nullable
            return if_while_nullable(stoken, ifelse, var_id);
        }
        else { // ID but not nullable, go into precedence analysis
            return if_while_preced(stoken, ifelse);
        }
    }
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        return if_while_preced(stoken, ifelse);
    }
    else {
        throw_error(stoken, 2);
    }
}


/*
 *  Return
 */
bool return_preced(TokenStoragePtr stoken ,Elem_id * curr_func){
        PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
        if (result == NULL){
            throw_error(NULL, 99);
        }
        if (result->Error != 0){
            throw_error(stoken, result->Error); // Expression error
        }
        if (curr_func->FnVar.Fn_id.return_type.type != result->Tree->Data.Type - 12) {
            throw_error(stoken, 4); // Wrong return type
        }
        stoken->SToken = result->NextTotken;
        stoken->returned = true;
        // Connecting expression to the main tree
        TreeElementConnect(stoken->current_node, result->Tree);
        return t_expect(stoken, SEMICOLON);
}

bool e_return_exp(TokenStoragePtr stoken){
    // Search for the current function
    Elem_id *curr_func = NULL;
    curr_func = TableSearch(stoken->current_fn, NULL, 0, stoken->glob_table);
    if (curr_func == NULL){
        throw_error(NULL, 99);
    }

    if (curr_func->FnVar.Fn_id.return_type.type == VOID){
        if (stoken->SToken->type != SEMICOLON){
            throw_error(stoken, 4); // Expression void return statement
        }
        return t_expect(stoken, SEMICOLON);
    }

    Elem_id *ret_fn = NULL;
    Elem_id *ret_var = NULL;
    if (stoken->SToken->type == ID){ // If id search if its a function or variable
        ret_fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        ret_var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        // If both pointers not null
        if (ret_fn != NULL && ret_var != NULL){
            throw_error(stoken, 10); // Function with the same name as variable
        }
        if (ret_fn == NULL && ret_var == NULL){
            throw_error(stoken, 3); // Function or variable not defined
        }
    }

    // Returning null
    if(stoken->SToken->type == NULL_VALUE){
        if(curr_func->FnVar.Fn_id.return_type.nullable == false){
            throw_error(stoken, 4); // Cannot return null
        }
        // Copy the null pointer
        TreeElement *new = TreeInsert(stoken->current_node, NULL);
        new->Data.NodeType = EXPRESSION_NODE;
        new->Data.Token = copy_token(stoken->SToken);
        stoken->returned = true;
        return t_expect(stoken, NULL_VALUE) &&
        t_expect(stoken, SEMICOLON);
    }

    // returning id expression or string variable
    else if(ret_var != NULL){
        if (ret_var->FnVar.Var_id.type.type == U8){
            if (curr_func->FnVar.Fn_id.return_type.type != U8) {
                throw_error(stoken, 4); // Wrong return type
            }
            if (ret_var->FnVar.Var_id.type.nullable) {
                if (!curr_func->FnVar.Fn_id.return_type.nullable){
                    throw_error(stoken, 4); // Wrong return type
                }
            }
            // New tree node
            TreeElement *new = TreeInsert(stoken->current_node, NULL);
            new->Data.NodeType = EXPRESSION_NODE;
            new->Data.Token = copy_token(stoken->SToken);
            new->Data.TableElement = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);

            stoken->returned = true;
            ret_var->FnVar.Var_id.used = true;
            return t_expect(stoken, ID) &&
            t_expect(stoken, SEMICOLON);
        }
        else {
            return return_preced(stoken, curr_func);
        }
    }

    // Assigning non-string literals
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        return return_preced(stoken, curr_func);
    }

    // Assigning string
    else if(stoken->SToken->type == STRING || stoken->SToken->type == SEMICOLON || stoken->SToken->type == IFJ || ret_fn != NULL){
        throw_error(stoken, 4); // returning string not allowed
    }
    throw_error(stoken, 2);
}

bool fn_return(TokenStoragePtr stoken){
    bool ret;

    // New node in tree and moving deeper into the tree
    TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
    new_node->Data.NodeType = RETURN_NODE;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    ret = t_expect(stoken, RETURN) &&
    e_return_exp(stoken);

    stoken->current_node = stoken->current_node->DadNode;

    return ret;
}

/*
 *  Main switchboard
 */
bool fn_body(TokenStoragePtr stoken){ // Main switchboard
    /*
    *   Variable definition
    */
    if (stoken->SToken->type == VAR || stoken->SToken->type == CONST){
        return var_def(stoken) &&
        fn_body(stoken);
    }
    /*
    *   Function calling or assignment
    */
    else if(stoken->SToken->type == ID || stoken->SToken->type == UNDERSCORE){ // Or _
        Elem_id *tmp_fn = NULL;
        Elem_id *tmp_var = NULL;
        if (stoken->SToken->type == ID){
            tmp_fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
            tmp_var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        }

        if (tmp_var != NULL && tmp_fn != NULL){ // Error
            throw_error(stoken, 10); // Function and variable with the same name
        }
        else if(tmp_fn != NULL){ // Function call
            if (tmp_fn->FnVar.Fn_id.return_type.type != VOID){
                throw_error(stoken, 4); // return type dumped
            }

            return fn_call(stoken) &&
            fn_body(stoken);
        } // Assingment
        else if (tmp_var != NULL || stoken->SToken->type == UNDERSCORE){
            return assign(stoken) &&
            fn_body(stoken);
        }
        else{
            throw_error(stoken, 3); // non defined function or variable
        }
    }
    /*
    *   Ifj function calling
    */
    else if(stoken->SToken->type == IFJ){
        return ifj_call(stoken) && // Checks the sematics of ifj functions, only write can be called here
        fn_body(stoken);
    }
    /*
    *   If else block
    */
    else if(stoken->SToken->type == IF){
        return while_ifelse(stoken, true) &&
        fn_body(stoken);
    }
    /*
    *   Cycle block
    */
    else if(stoken->SToken->type == WHILE){
        return while_ifelse(stoken, false) &&
        fn_body(stoken);
    }
    /*
    *   Return from function
    */
    else if(stoken->SToken->type == RETURN){
        return fn_return(stoken) &&
        fn_body(stoken);
    }
    /*
    *   End of the current function
    */
    else {
        return true;
    }
}

/*
 * Prolog and function definition
 */
bool set_fn_context(TokenStoragePtr stoken){
    Elem_id * tmp = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table); // Searching for the curr function
    stoken->current_fn = strdup(stoken->SToken->value.str);
    if (tmp->FnVar.Fn_id.return_type.type == VOID){ // If void, return must not be present
        stoken->returned = true;
    }

    // Setting stoken context
    stoken->local_table = tmp->FnVar.Fn_id.LocalSymTable; // Setting local table
    stoken->stack_size = 1; // Initialising stack
    stoken->level_stack = malloc(sizeof(int));
    stoken->level_stack[0] = 1;
    return true;
}

bool fn_def(TokenStoragePtr stoken){ // Syntax and sematics chces were alredy done in the first go through
    int count = 0;
    while(stoken->SToken->type != OPENING_CURLY_BRACKET){
        if (count == 2){
            set_fn_context(stoken);
        }
        count++;
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
    }
    return true;
}

bool functions(TokenStoragePtr stoken){
    static int node_counter = 0;
    if (stoken->SToken->type == END_OF_FILE) {
        return t_expect(stoken, END_OF_FILE); // End of program
    }
    else { // Setting the context for the function body
        bool ret;

        // Reseting the context for the next function
        free(stoken->current_fn);
        stoken->current_fn = NULL;
        free(stoken->level_stack);
        stoken->level_stack = NULL;
        stoken->local_table = NULL;
        stoken->returned = false;
        stoken->level_stack = NULL;
        stoken->stack_size = 0;
        stoken->last_poped = 0;

        // Setting current tree node
        stoken->current_node = stoken->current_node->Node[node_counter];
        node_counter++;

        ret = fn_def(stoken) && // Setting stoke context
        t_expect(stoken, OPENING_CURLY_BRACKET) &&
        fn_body(stoken) && // Main syntax and sematics validation
        t_expect(stoken, CLOSING_CURLY_BRACKET);

        check_ret(stoken, ret);

        // Going up the tree
        stoken->current_node = stoken->current_node->DadNode;

        if (stoken->returned == false){ // Return check
            throw_error(stoken, 6); // Missing return
        }

        // Checking for variable usage
        Elem_id *elem_loc;
        for (int j = 0; j < TABLE_SIZE; j++){
            elem_loc = stoken->local_table[j];
            if (elem_loc == NULL){
                continue;
            }
            if (elem_loc->FnVar.Var_id.used == 0){
                throw_error(stoken, 9); // Unsed variable
            }
        }

        return ret &&
        functions(stoken); // Recursive calling
    }
}

bool t_string_prolog(TokenStoragePtr stoken){
    if (stoken->SToken->type == STRING && !strcmp(stoken->SToken->value.str, "ifj24.zig")){
        dealloc_token(stoken->SToken);
        stoken->SToken = queue_next_token(stoken->queue);
        return true;
    }
    else{
        throw_error(stoken, 2);
    }
}

bool import_def(TokenStoragePtr stoken){ // Only syntax checks TODO remove
    return t_expect(stoken, CONST) &&
    t_expect(stoken, IFJ) &&
    t_expect(stoken, ASSIGN) &&
    t_expect(stoken, IMPORT) &&
    t_expect(stoken, OPENING_BRACKET) &&
    t_string_prolog(stoken) &&
    t_expect(stoken, CLOSING_BRACKET) &&
    t_expect(stoken, SEMICOLON);
}

bool start(TokenStoragePtr stoken){ // Starting point
    return import_def(stoken) && functions(stoken);
}

int main(){
    bool ret = false;
    TokenStoragePtr stoken = malloc(sizeof(TokenStorage));
    if (stoken == NULL){
        throw_error(NULL, 99);
    }

    // Table and queue init
    stoken->queue = queue_init();
    stoken->glob_table = TableInit();
    stoken->ifj_table = TableInit();
    stoken->current_fn = NULL;

    // Tree init
    TreeElement *tree_root;
    TreeRootPtr t_init = TreeInit();
    tree_root = t_init->Root;
    tree_root->Data.NodeType = ROOT_NODE;

    // filling the queue and the first go through
    queue_fill(stoken, tree_root);

    // Searching for main, if not defined exit
    Elem_id *tmp = TableSearch("main", NULL, 0, stoken->glob_table);
    if (tmp == NULL){ 
        throw_error(stoken, 3); // Main is not defined
    }
    else if (tmp->FnVar.Fn_id.return_type.type != VOID || tmp->FnVar.Fn_id.num_of_params != 0){
        throw_error(stoken, 4); // Main has wrong params
    }

    // Filling the ifj table
    ifj_table_fill(stoken);

    // Setting fresh context for the second go through
    free(stoken->current_fn);
    stoken->current_fn = NULL;
    stoken->returned = false;
    stoken->local_table = NULL;
    stoken->level_stack = NULL;
    stoken->stack_size = 0;
    stoken->last_poped = 0;
    stoken->current_node = tree_root;
    stoken->SToken = queue_next_token(stoken->queue);

    // Second go through
    ret = start(stoken);

    check_ret(stoken, ret);

    // Generate code
    gen_code(tree_root);

    // Freeing everything
    TreeDestroy(t_init);
    garbage_collector(stoken, false);
}
