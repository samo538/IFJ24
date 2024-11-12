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

#include "../precedence/precedence.h"

#include "../tree/tree.h"

#include "../code_gen/code_gen.h"

#include <stdio.h>

// Debug output func
void print_tree(TreeElementPtr element){

    for(int i = 0; i < element->NodeCounter; i++){
        print_tree(element->Node[i]);
        printf("%d -> ", element->Node[i]->Data.NodeType);
    }
}


// errors and deallocs

void dealloc_stoken(TokenStoragePtr stoken){
    if (stoken != NULL){
        if (stoken->SToken != NULL){
            dealloc_token(stoken->SToken);
            stoken->SToken = NULL;
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
    fprintf(stderr, "syntax error\n");
    exit(2);
}

/*
 *  Variable definition
 */
bool ifj_call_var(TokenStoragePtr stoken, Elem_id *new){
    bool ret;
    ret = t_ifj(stoken) &&
    t_dot(stoken);

    if (!ret){
        syn_error(stoken);
    }

    if (stoken->SToken->type == ID && h_check_ifj_fn(stoken->SToken->value.str)){
        Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->ifj_table); // Search inside IFJ
        if (fn == NULL){
            fprintf(stderr, "Undefined IFJ function\n");
            exit(3);
        }
        if (!strcmp(fn->name, "write")){
            fprintf(stderr, "Assigning void\n");
            exit(7);
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
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
            if (fn->FnVar.Fn_id.return_type.nullable){
                if(!new->FnVar.Fn_id.return_type.nullable){
                    fprintf(stderr, "Wrong assign type\n");
                    exit(7);
                }
            }
        }
        ret = ret &&
        t_id_ifj(stoken) &&
        t_op_bracket(stoken) &&
        call_params(stoken, fn, 0) && // Checking the validity of params
        t_cl_bracket(stoken) &&
        t_semicolon(stoken);

        stoken->current_node = stoken->current_node->DadNode;

        return ret;
    }

    syn_error(stoken);
    return false;
}

bool e_var_exp_def(TokenStoragePtr stoken, Elem_id *new){
    Elem_id *ret_fn = NULL;
    Elem_id *ret_var = NULL;
    if (stoken->SToken->type == ID){ // If id search if its a function or variable
        ret_fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        ret_var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        // If both pointers not null
        if (ret_fn != NULL && ret_var != NULL){
            fprintf(stderr, "Function with the same name as variable\n");
            exit(10);
        }
        if (ret_fn == NULL && ret_var == NULL){
            fprintf(stderr, "Function or varable not defined\n");
            exit(3);
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
                fprintf(stderr, "Assigning void\n");
                exit(7);
            }
            new->FnVar.Var_id.type.type = ret_fn->FnVar.Fn_id.return_type.type;
            new->FnVar.Var_id.type.nullable = ret_fn->FnVar.Fn_id.return_type.nullable;
        }
        else { // Explicit type, check data types
            if (new->FnVar.Var_id.type.type != ret_fn->FnVar.Fn_id.return_type.type) {
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
            if (ret_fn->FnVar.Fn_id.return_type.nullable){ // Check nullables
                if(!new->FnVar.Fn_id.return_type.nullable){
                    fprintf(stderr, "Wrong assign type\n");
                    exit(7);
                }
            }
        }
        return fn_call(stoken);
    }

    // Assigning null
    else if(stoken->SToken->type == NULL_VALUE){
        if (new->FnVar.Var_id.type.type == END_OF_FILE){
            fprintf(stderr, "cannot determine type\n");
            exit(8);
        }
        else if(new->FnVar.Var_id.type.nullable == false){
            fprintf(stderr, "cannot assign null\n");
            exit(7);
        }
        // Copy the null pointer
        TreeElement *new = TreeInsert(stoken->current_node, NULL);
        new->Data.NodeType = EXPRESSION_NODE;
        new->Data.Token = copy_token(stoken->SToken);

        return t_null(stoken) &&
        t_semicolon(stoken);
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
                    fprintf(stderr, "Wrong assign type\n");
                    exit(7);
                }
                if (ret_var->FnVar.Var_id.type.nullable){ // Check nullables
                    if(!new->FnVar.Var_id.type.nullable){
                        fprintf(stderr, "Wrong assign type\n");
                        exit(7);
                    }
                }
            }
            // New tree node
            TreeElement *new = TreeInsert(stoken->current_node, NULL);
            new->Data.NodeType = EXPRESSION_NODE;
            new->Data.Token = copy_token(stoken->SToken);
            new->Data.TableElement = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);

            ret_var->FnVar.Var_id.used = true;
            return t_id(stoken) &&
            t_semicolon(stoken);
        }
        else {
            PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
            if (result == NULL){
                fprintf(stderr,"Strasne obrovska chyba\n");
                exit(99);
            }
            if (result->Error != 0){
                fprintf(stderr,"Exp error\n");
                exit(result->Error);
            }
            if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Implicit type
                new->FnVar.Var_id.type.type = result->Tree->Data.Type - 12;
                new->FnVar.Var_id.type.nullable = false;
            }
            else { // Explicit type, check data types
                if (new->FnVar.Var_id.type.type != result->Tree->Data.Type - 12) {
                    fprintf(stderr, "Wrong assign type\n");
                    exit(7);
                }
            }
            stoken->SToken = result->NextTotken;
            // Connecting expression to the main tree
            TreeElementConnect(stoken->current_node, result->Tree);
            return t_semicolon(stoken);
        }
    }

    // Assigning non-string literals
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
        if (result == NULL){
            fprintf(stderr,"Strasne obrovska chyba\n");
            exit(99);
        }
        if (result->Error != 0){
            fprintf(stderr,"Exp error\n");
            exit(result->Error);
        }
        if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Implicit type
            new->FnVar.Var_id.type.type = result->Tree->Data.Type - 12;
            new->FnVar.Var_id.type.nullable = false;
        }
        else { // Explicit type, check data types
            if (new->FnVar.Var_id.type.type != result->Tree->Data.Type - 12) {
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
        }
        stoken->SToken = result->NextTotken;
        // Connection expression to the main tree
        TreeElementConnect(stoken->current_node, result->Tree);
        return t_semicolon(stoken);
    }

    // Assigning string
    else if(stoken->SToken->type == STRING){
        fprintf(stderr, "Assigning string not allowd\n");
        exit(7);
    }
    fprintf(stderr, "Syntax Error\n");
    exit(2);
}

bool l_type_var(TokenStoragePtr stoken, Elem_id *new){
    return t_nullable_var(stoken, new) && // Set nullable
    t_type_keyword_var(stoken, new); // Set type
}

bool l_type_vardef(TokenStoragePtr stoken, Elem_id * new){
    if (stoken->SToken->type == COLON){ // Explicit definiton
        return t_colon(stoken) &&
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
        ret =  t_var(stoken);
    }
    else {
        new->FnVar.Var_id.const_t = true; // Setting the const flag
        ret = t_const(stoken);
    }
    return ret;
}

bool var_def(TokenStoragePtr stoken){
    bool ret;

    // New node in tree and moving deeper into the tree
    TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
    new_node->Data.NodeType = DEFINITION_NODE;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    Elem_id *new = malloc(sizeof(Elem_id)); // Creating a new local element
    new->Type = VARIABLE;
    new->stack_size = stoken->stack_size; // Setting the stack
    copy_levels(stoken->level_stack, &new->level_stack, stoken->stack_size);
    new->FnVar.Var_id.type.type = END_OF_FILE; // Placeholder
    new->FnVar.Var_id.type.nullable = false; // Implicit false
    new->FnVar.Var_id.used = false; // Variable not used by default

    // Tree creation
    TreeElement *tree_id = TreeInsert(stoken->current_node, NULL);
    tree_id->Data.NodeType = ID_NODE;

    // Tree node for ID created in t_id_var
    ret = l_var_const(stoken, new) && // Filling up new
    t_id_var(stoken, new) && // Filling up the name / checking for redef
    l_type_vardef(stoken, new) && // Implicit/Explicit definition
    t_eq(stoken) &&
    e_var_exp_def(stoken, new); // Adding value

    TableAdd(*new, stoken->local_table); // Adding the new element into local table
    tree_id->Data.TableElement = TableSearch(new->name, new->level_stack, new->stack_size, stoken->local_table);

    // Moving upper in the tree
    stoken->current_node = stoken->current_node->DadNode;

    free(new->name); // freeing the added element
    free(new->level_stack);
    free(new);
    return ret;
}

/*
 *  Function call
 */
bool call_params(TokenStoragePtr stoken, Elem_id *fn, int pos){
    if (stoken->SToken->type == STRING){
        if (pos == fn->FnVar.Fn_id.num_of_params){
            fprintf(stderr, "too many params\n");
            exit(4);
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].type == U8){
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
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
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
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
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
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
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
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
        if (tmp == NULL){
            fprintf(stderr, "undefined variable\n");
            exit(3);
        }
        if (fn->FnVar.Fn_id.type_of_params[pos].type == tmp->FnVar.Var_id.type.type){
            tmp->FnVar.Var_id.used = true; // Used this var

            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
            new_node->Data.TableElement = tmp;

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

bool fn_call(TokenStoragePtr stoken){
    bool ret;
    Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
    if (fn == NULL){
        fprintf(stderr, "Undefined function\n");
        exit(3);
    }

    // Creating a new tree node
    TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
    new_node->Data.NodeType = FUNCTION_NODE;
    new_node->Data.TableElement = fn;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    ret = t_id(stoken) &&
    t_op_bracket(stoken) &&
    call_params(stoken, fn, 0) && // Sematics action, check params validity
    t_cl_bracket(stoken) &&
    t_semicolon(stoken);

    stoken->current_node = stoken->current_node->DadNode;

    return ret;
}

/*
 *  IFJ function call
 */
bool ifj_call(TokenStoragePtr stoken){
    bool ret;
    ret = t_ifj(stoken) &&
    t_dot(stoken);

    if (stoken->SToken->type == ID && h_check_ifj_fn(stoken->SToken->value.str)){
        Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->ifj_table);
        if (fn == NULL){
            fprintf(stderr, "Undefined IFJ function\n");
            exit(3);
        }
        if (strcmp(fn->name, "write")){
            fprintf(stderr, "Return type dumped\n");
            exit(4);
        }
        ret = ret &&
        t_id_ifj(stoken) &&
        t_op_bracket(stoken);

        if (ret == false){
            syn_error(stoken);
        }

        // Creating a new tree element and going deeper
        TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = IFJ_FUNCTION_NODE;
        new_node->Data.TableElement = fn;
        stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

        if (stoken->SToken->type == F64_VAR){
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);

            ret = ret && t_float(stoken);
            if (ret == false){
                syn_error(stoken);
            }
        }
        else if (stoken->SToken->type == I32_VAR){
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);

            ret = ret && t_int(stoken);
            if (ret == false){
                syn_error(stoken);
            }

        }
        else if (stoken->SToken->type == STRING){
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);

            ret = ret && t_string(stoken);
            if (ret == false){
                syn_error(stoken);
            }
        }
        else if (stoken->SToken->type == NULL_VALUE){
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);

            ret = ret && t_null(stoken);
            if (ret == false){
                syn_error(stoken);
            }
        }
        else if (stoken->SToken->type == ID){
        Elem_id *var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
            if (var == NULL){
                fprintf(stderr, "Undefined variable\n");
                exit(3);
            }
            var->FnVar.Var_id.used = true; // Used this variable
            TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = ARG_NODE;
            new_node->Data.Token = copy_token(stoken->SToken);
            new_node->Data.TableElement = var;
            ret = ret && t_id(stoken);
            if (ret == false){
                syn_error(stoken);
            }

        }
        else {
            syn_error(stoken);
        }

        //Moving up in the tree
        stoken->current_node = stoken->current_node->DadNode;

        return ret &&
        t_comma(stoken) &&
        t_cl_bracket(stoken) &&
        t_semicolon(stoken);
    }

    syn_error(stoken);
    return false;
}

/*
 *  Assign
 */
bool ifj_call_assign(TokenStoragePtr stoken, bool underscore ,Elem_id *assign_to){
    bool ret;
    ret = t_ifj(stoken) &&
    t_dot(stoken);

    if (!ret){
        syn_error(stoken);
    }

    if (stoken->SToken->type == ID && h_check_ifj_fn(stoken->SToken->value.str)){
        Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->ifj_table);
        if (fn == NULL){
            fprintf(stderr, "Undefined IFJ function\n");
            exit(3);
        }
        if (!strcmp(fn->name, "write")){
            fprintf(stderr, "Assigning void\n");
        exit(7);
        }
        if (!underscore && assign_to->FnVar.Var_id.type.type != fn->FnVar.Fn_id.return_type.type) {
            fprintf(stderr, "Wrong assign type\n");
            exit(7);
        }
        if (!underscore && fn->FnVar.Fn_id.return_type.nullable){
            if(!assign_to->FnVar.Fn_id.return_type.nullable){
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
        }
        // Creating a new tree node
        TreeElement* new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = IFJ_FUNCTION_NODE;
        new_node->Data.TableElement = fn;
        stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

        ret = ret &&
        t_id_ifj(stoken) &&
        t_op_bracket(stoken) &&
        call_params(stoken, fn, 0) && // Checking the validity of params
        t_cl_bracket(stoken) &&
        t_semicolon(stoken);

        stoken->current_node = stoken->current_node->DadNode;

        return ret;
    }

    syn_error(stoken);
    return false;
}
bool e_var_exp_assign(TokenStoragePtr stoken, bool underscore, Elem_id *assign_to){
    Elem_id *ret_fn = NULL;
    Elem_id *ret_var = NULL;
    if (stoken->SToken->type == ID){ // If id search if its a function or variable
        ret_fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        ret_var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        // If both pointers not null
        if (ret_fn != NULL && ret_var != NULL){
            fprintf(stderr, "Function with the same name as variable\n");
            exit(10);
        }
        if (ret_fn == NULL && ret_var == NULL){
            fprintf(stderr, "Function or varable not defined\n");
            exit(3);
        }
    }

    // Assigning IFJ func
    if (stoken->SToken->type == IFJ){
        return ifj_call_assign(stoken, underscore, assign_to);
    }

    // Assigning func
    else if (ret_fn != NULL){ 
        if (ret_fn->FnVar.Fn_id.return_type.type == VOID){
            fprintf(stderr, "Assigning void\n");
            exit(7);
        }
        if (!underscore && assign_to->FnVar.Var_id.type.type != ret_fn->FnVar.Fn_id.return_type.type) {
            fprintf(stderr, "Wrong assign type\n");
            exit(7);
        }
        if (!underscore && ret_fn->FnVar.Fn_id.return_type.nullable){
            if(!assign_to->FnVar.Fn_id.return_type.nullable){
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
        }
        return fn_call(stoken);
    }

    // Assigning null
    else if(stoken->SToken->type == NULL_VALUE){
        if(!underscore && assign_to->FnVar.Var_id.type.nullable == false){
            fprintf(stderr, "cannot assign null\n");
            exit(7);
        }
        // Copy the null pointer
        TreeElement *new = TreeInsert(stoken->current_node, NULL);
        new->Data.NodeType = EXPRESSION_NODE;
        new->Data.Token = copy_token(stoken->SToken);

        return t_null(stoken) &&
        t_semicolon(stoken);
    }

    // Assigning id expression or string variable
    else if(ret_var != NULL){
        if (ret_var->FnVar.Var_id.type.type == U8){
            if (!underscore && assign_to->FnVar.Var_id.type.type != ret_var->FnVar.Var_id.type.type) {
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
            if (!underscore && ret_var->FnVar.Var_id.type.nullable){
                if(!assign_to->FnVar.Var_id.type.nullable){
                    fprintf(stderr, "Wrong assign type\n");
                    exit(7);
                }
            }
            ret_var->FnVar.Var_id.used = true; // Used this var
            // New tree node
            TreeElement *new = TreeInsert(stoken->current_node, NULL);
            new->Data.NodeType = EXPRESSION_NODE;
            new->Data.Token = copy_token(stoken->SToken);
            new->Data.TableElement = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);

            return t_id(stoken) &&
            t_semicolon(stoken);
        }
        else {
            PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
            if (result == NULL){
                fprintf(stderr,"Strasne obrovska chyba\n");
                exit(99);
            }
            if (result->Error != 0){
                fprintf(stderr,"Exp error\n");
                exit(result->Error);
            }
            if (!underscore && assign_to->FnVar.Var_id.type.type != result->Tree->Data.Type - 12) {
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
            // Connecting expression to the main tree
            TreeElementConnect(stoken->current_node, result->Tree);
            stoken->SToken = result->NextTotken;
            return t_semicolon(stoken);
        }
    }

    // Assigning non-string literals
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
        if (result == NULL){
            fprintf(stderr,"Strasne obrovska chyba\n");
            exit(99);
        }
        if (result->Error != 0){
            fprintf(stderr,"Exp error\n");
            exit(result->Error);
        }
        if (!underscore && assign_to->FnVar.Var_id.type.type != result->Tree->Data.Type - 12) {
            fprintf(stderr, "Wrong assign type\n");
            exit(7);
        }
        stoken->SToken = result->NextTotken;
        // Connecting expression to the main tree
        TreeElementConnect(stoken->current_node, result->Tree);
        return t_semicolon(stoken);
    }

    // Assigning string
    else if(stoken->SToken->type == STRING){
        fprintf(stderr, "Assigning string not allowd\n");
        exit(7);
    }
    fprintf(stderr, "Syntax Error\n");
    exit(2);
}

bool l_id_assign(TokenStoragePtr stoken, bool *underscore, Elem_id **assign_to){
    if (stoken->SToken->type == UNDERSCORE){
        TreeElement *tree_id = TreeInsert(stoken->current_node, NULL);
        tree_id->Data.NodeType = ID_NODE;
        tree_id->Data.Token = copy_token(stoken->SToken);
        *underscore = true;
        return t_underline(stoken);
    }
    else {
        *underscore = false;
        *assign_to = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        Elem_id *tmp = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        if (*assign_to == NULL){
            fprintf(stderr, "undefined variable\n");
            exit(3);
        }
        if (*assign_to != NULL && tmp != NULL){
            fprintf(stderr, "Variable and function with the same name\n");
            exit(10);
        }
        if ((*assign_to)->FnVar.Var_id.const_t == true){
            fprintf(stderr, "Assigning to const\n");
            exit(5);
        }
        TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = ID_NODE;
        new_node->Data.TableElement = *assign_to;
        new_node->Data.Token = copy_token(stoken->SToken);

        return t_id(stoken);
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
    t_eq(stoken) &&
    e_var_exp_assign(stoken, underscore, assign_to); // Assigment

    stoken->current_node = stoken->current_node->DadNode;

    return ret;
}

/*
 *  IfElse
 */

bool if_while_body(TokenStoragePtr stoken){
    return t_op_cr_bracket(stoken) &&
    fn_body(stoken) &&
    t_cl_cr_bracket(stoken);
}

bool if_else(TokenStoragePtr stoken){
    bool ret;
    ret = t_if(stoken) &&
    t_op_bracket(stoken);
    if (ret == false){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // Check the variant of the condition (null or exp)
    if (stoken->SToken->type == ID){
        Elem_id * var_id = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (var_id == NULL){
            fprintf(stderr, "undefined variable");
            exit(3);
        }
        if (var_id->FnVar.Var_id.type.nullable){
            var_id->FnVar.Var_id.used = true;
            Elem_id *new = malloc(sizeof(Elem_id)); // Creating a new local element
            new->Type = VARIABLE;
            new->FnVar.Var_id.type.type = var_id->FnVar.Var_id.type.type; // Placeholder
            new->FnVar.Var_id.type.nullable = false; // Implicit false
            new->FnVar.Var_id.used = false; // Variable not used by default
            new->FnVar.Var_id.const_t = false; // Variable not used by default
            new->FnVar.Var_id.used = false;

            //Changind the position is stack
            stoken->stack_size++;
            stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);
            stoken->level_stack[stoken->stack_size - 1] = stoken->last_poped + 1; // Adding values to stack

            if (stoken->level_stack == NULL){
                free(new);
                fprintf(stderr,"Internal error\n");
                exit(99);
            }

            new->stack_size = stoken->stack_size;
            copy_levels(stoken->level_stack, &(new->level_stack), stoken->stack_size);

            // New tree node
            TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = IF_NODE;
            new_node->Data.isNullCond = true;
            stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

            ret = ret &&
            t_id(stoken) &&
            t_cl_bracket(stoken) &&
            t_vertical_bar(stoken);

            if (ret == false){
                free(new->level_stack);
                free(new);
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }
            if (stoken->SToken->type == ID){
                new->name = strdup(stoken->SToken->value.str);
                TableAdd(*new, stoken->local_table); // Adding the new element into Table
                // Is nullable, Unwrapped variable goes here
                TreeElement *Unwrapped = TreeInsert(stoken->current_node, NULL);
                Unwrapped->Data.NodeType = ID_NODE;
                Unwrapped->Data.TableElement = TableSearch(new->name, new->level_stack, new->stack_size, stoken->local_table);
                free(new->name);
                free(new->level_stack);
                free(new);
            }
            // Adding the condition into the tree
            TreeElement *Cond_node = TreeInsert(stoken->current_node, NULL);
            Cond_node->Data.NodeType = EXPRESSION_NODE;
            Cond_node->Data.TableElement = var_id;


            ret = ret &&
            t_id(stoken) && 
            t_vertical_bar(stoken) &&
            if_while_body(stoken);

            if (ret == false){
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }

            stoken->level_stack[stoken->stack_size - 1]++; // Moving into else
            TreeElement *else_node = TreeInsert(stoken->current_node, NULL);
            else_node->Data.NodeType = ELSE_NODE;

            ret = ret &&
            t_else(stoken) &&
            if_while_body(stoken);

            if (ret == false){
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }


            stoken->last_poped = stoken->level_stack[stoken->stack_size - 1]; // Pop last item from the stack
            stoken->stack_size--;
            stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);

            // Going up in the tree
            stoken->current_node = stoken->current_node->DadNode;

            if (stoken->level_stack == NULL){
                fprintf(stderr,"Internal error\n");
                exit(99);
            }
            return ret;
        }
        else {
            PrecResultPtr result = preced_analysis(stoken->SToken, true, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
            if (result == NULL){
                fprintf(stderr,"Strasne obrovska chyba\n");
                exit(99);
            }
            if (result->Error != 0){
                fprintf(stderr,"Exp error\n");
                exit(result->Error);
            }
            stoken->SToken = result->NextTotken;

            //Changind the position is stack
            stoken->stack_size++;
            stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);
            stoken->level_stack[stoken->stack_size - 1] = stoken->last_poped + 1; // Adding values to stack

            // New tree node
            TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = IF_NODE;
            new_node->Data.isNullCond = false;
            stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

            // Inserting Empty Node, because null cond is false
            TreeInsert(stoken->current_node, NULL);

            // Inserting the expression
            TreeElementConnect(stoken->current_node, result->Tree);


            if (stoken->level_stack == NULL){
                fprintf(stderr,"Internal error\n");
                exit(99);
            }

            ret = ret &&
            t_cl_bracket(stoken) &&
            if_while_body(stoken);

            if (ret == false){
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }

            // going into else
            stoken->level_stack[stoken->stack_size - 1]++; // Moving into else
            TreeElement *else_node = TreeInsert(stoken->current_node, NULL);
            else_node->Data.NodeType = ELSE_NODE;



            ret = ret &&
            t_else(stoken) &&
            if_while_body(stoken);

            if (ret == false){
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }

            // stepping out of else
            stoken->last_poped = stoken->level_stack[stoken->stack_size - 1]; // Pop last item from the stack
            stoken->stack_size--;
            stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);

            stoken->current_node = stoken->current_node->DadNode;

            if (stoken->level_stack == NULL){
                fprintf(stderr,"Internal error\n");
                exit(99);
            }

            return ret;
        }
    }
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        PrecResultPtr result = preced_analysis(stoken->SToken, true, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
        if (result == NULL){
            fprintf(stderr,"Strasne obrovska chyba\n");
            exit(99);
        }
        if (result->Error != 0){
            fprintf(stderr,"Exp error\n");
            exit(result->Error);
        }
        stoken->SToken = result->NextTotken;

        //Changind the position is stack
        stoken->stack_size++;
        stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);
        stoken->level_stack[stoken->stack_size - 1] = stoken->last_poped + 1; // Adding values to stack

        // New tree node
        TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = IF_NODE;
        new_node->Data.isNullCond = false;
        stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

        // Inserting Empty Node, because null cond is false
        TreeInsert(stoken->current_node, NULL);

        // Inserting the expression
        TreeElementConnect(stoken->current_node, result->Tree);

        if (stoken->level_stack == NULL){
            fprintf(stderr,"Internal error\n");
            exit(99);
        }

        ret = ret &&
        t_cl_bracket(stoken) &&
        if_while_body(stoken);

        if (ret == false){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }

        stoken->level_stack[stoken->stack_size - 1]++; // Moving into else
        TreeElement *else_node = TreeInsert(stoken->current_node, NULL);
        else_node->Data.NodeType = ELSE_NODE;

        ret = ret &&
        t_else(stoken) &&
        if_while_body(stoken);

        if (ret == false){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }

        stoken->last_poped = stoken->level_stack[stoken->stack_size - 1]; // Pop last item from the stack
        stoken->stack_size--;
        stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);

        stoken->current_node = stoken->current_node->DadNode;

        if (stoken->level_stack == NULL){
            fprintf(stderr,"Internal error\n");
            exit(99);
        }

        return ret;
    }
    else {
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
}

/*
 *  While
 */

bool cycle(TokenStoragePtr stoken){
    bool ret;
    ret = t_while(stoken) &&
    t_op_bracket(stoken);

    if (ret == false){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // Check the variant of the condition (null or exp)
    if (stoken->SToken->type == ID){
        Elem_id * var_id = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (var_id == NULL){
            fprintf(stderr, "undefined variable");
            exit(3);
        }
        if (var_id->FnVar.Var_id.type.nullable){ // Nullable path
            var_id->FnVar.Var_id.used = true;
            Elem_id *new = malloc(sizeof(Elem_id)); // Creating a new local element
            new->Type = VARIABLE;
            new->FnVar.Var_id.type.type = var_id->FnVar.Var_id.type.type; // Placeholder
            new->FnVar.Var_id.type.nullable = false; // Implicit false
            new->FnVar.Var_id.used = false; // Variable not used by default
            new->FnVar.Var_id.const_t = false; // Variable not used by default

            //Changind the position is stack
            stoken->stack_size++;
            stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);
            stoken->level_stack[stoken->stack_size - 1] = stoken->last_poped + 1; // Adding values to stack

            if (stoken->level_stack == NULL){
                free(new);
                fprintf(stderr,"Internal error\n");
                exit(99);
            }

            new->stack_size = stoken->stack_size; // Creating the new variable
            copy_levels(stoken->level_stack, &(new->level_stack), stoken->stack_size);

            // New tree node
            TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = WHILE_NODE;
            new_node->Data.isNullCond = true;
            stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];


            ret = ret &&
            t_id(stoken) &&
            t_cl_bracket(stoken) &&
            t_vertical_bar(stoken);

            if (ret == false){
                free(new->level_stack);
                free(new);
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }
            if (stoken->SToken->type == ID){
                new->name = strdup(stoken->SToken->value.str);
                TableAdd(*new, stoken->local_table); // Adding the new element into Table
                // Is nullable, Unwrapped variable goes here
                TreeElement *Unwrapped = TreeInsert(stoken->current_node, NULL);
                Unwrapped->Data.NodeType = ID_NODE;
                Unwrapped->Data.TableElement = TableSearch(new->name, new->level_stack, new->stack_size, stoken->local_table);

                free(new->name);
                free(new->level_stack);
                free(new);
            }
            // Adding the condition into the tree
            TreeElement *Cond_node = TreeInsert(stoken->current_node, NULL);
            Cond_node->Data.NodeType = EXPRESSION_NODE;
            Cond_node->Data.TableElement = var_id;

            ret = ret &&
            t_id(stoken) && 
            t_vertical_bar(stoken) &&
            if_while_body(stoken);

            if (ret == false){
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }

            //Changing the position back
            stoken->last_poped = stoken->level_stack[stoken->stack_size - 1]; // Pop last item from the stack
            stoken->stack_size--;
            stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);

            // Going up in the tree
            stoken->current_node = stoken->current_node->DadNode;

            if (stoken->level_stack == NULL){
                fprintf(stderr,"Internal error\n");
                exit(99);
            }
            return ret;
 
        }
        else {
            PrecResultPtr result = preced_analysis(stoken->SToken, true, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
            if (result == NULL){
                fprintf(stderr,"Strasne obrovska chyba\n");
                exit(99);
            }
            if (result->Error != 0){
                fprintf(stderr,"Exp error\n");
                exit(result->Error);
            }
            stoken->SToken = result->NextTotken;

            //Changind the position is stack
            stoken->stack_size++;
            stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);
            stoken->level_stack[stoken->stack_size - 1] = stoken->last_poped + 1; // Adding values to stack

            // New tree node
            TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
            new_node->Data.NodeType = WHILE_NODE;
            new_node->Data.isNullCond = false;
            stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

            // Inserting Empty Node, because null cond is false
            TreeInsert(stoken->current_node, NULL);

            // Inserting the expression
            TreeElementConnect(stoken->current_node, result->Tree);

            if (stoken->level_stack == NULL){
                fprintf(stderr,"Internal error\n");
                exit(99);
            }

            ret = ret &&
            t_cl_bracket(stoken) &&
            if_while_body(stoken);

            if (ret == false){
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }

            stoken->last_poped = stoken->level_stack[stoken->stack_size - 1]; // Pop last item from the stack
            stoken->stack_size--;
            stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);

            stoken->current_node = stoken->current_node->DadNode;

            if (stoken->level_stack == NULL){
                fprintf(stderr,"Internal error\n");
                exit(99);
            }

            return ret;
        }
    }
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        PrecResultPtr result = preced_analysis(stoken->SToken, true, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
        if (result == NULL){
            fprintf(stderr,"Strasne obrovska chyba\n");
            exit(99);
        }
        if (result->Error != 0){
            fprintf(stderr,"Exp error\n");
            exit(result->Error);
        }
        stoken->SToken = result->NextTotken;

        //Changind the position is stack
        stoken->stack_size++;
        stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);
        stoken->level_stack[stoken->stack_size - 1] = stoken->last_poped + 1; // Adding values to stack

        // New tree node
        TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
        new_node->Data.NodeType = WHILE_NODE;
        new_node->Data.isNullCond = false;
        stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

        // Inserting Empty Node, because null cond is false
        TreeInsert(stoken->current_node, NULL);

        // Inserting the expression
        TreeElementConnect(stoken->current_node, result->Tree);

        if (stoken->level_stack == NULL){
            fprintf(stderr,"Internal error\n");
            exit(99);
        }

        ret = ret &&
        t_cl_bracket(stoken) &&
        if_while_body(stoken);

        if (ret == false){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }

        stoken->last_poped = stoken->level_stack[stoken->stack_size - 1]; // Pop last item from the stack
        stoken->stack_size--;
        stoken->level_stack = realloc(stoken->level_stack, sizeof(int) * stoken->stack_size);

        stoken->current_node = stoken->current_node->DadNode;

        if (stoken->level_stack == NULL){
            fprintf(stderr,"Internal error\n");
            exit(99);
        }
        return ret;
    }
    else {
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
}


/*
 *  Return
 */
bool e_return_exp(TokenStoragePtr stoken){
    // Search for the current function
    Elem_id *curr_func = NULL;
    curr_func = TableSearch(stoken->current_fn, NULL, 0, stoken->glob_table);
    if (curr_func == NULL){
        fprintf(stderr, "Something went wrong\n");
        exit(99);
    }

    if (curr_func->FnVar.Fn_id.return_type.type == VOID){
        if (stoken->SToken->type != SEMICOLON){
            fprintf(stderr, "Expression void return statement\n");
            exit(4);
        }
        return t_semicolon(stoken);
    }

    Elem_id *ret_fn = NULL;
    Elem_id *ret_var = NULL;
    if (stoken->SToken->type == ID){ // If id search if its a function or variable
        ret_fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        ret_var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        // If both pointers not null
        if (ret_fn != NULL && ret_var != NULL){
            fprintf(stderr, "Function with the same name as variable\n");
            exit(10);
        }
        if (ret_fn == NULL && ret_var == NULL){
            fprintf(stderr, "Function or variable not defined\n");
            exit(3);
        }
    }

    // Returning IFJ func
    if (stoken->SToken->type == IFJ){
        fprintf(stderr, "cannot return function\n");
        exit(4);
    }

    // Trying to return void in non-void fn
    else if(stoken->SToken->type == SEMICOLON){
        fprintf(stderr, "Returning void in non-void func\n");
        exit(4);
    }

    // Returning func
    else if (ret_fn != NULL){ 
        fprintf(stderr, "cannot return function\n");
        exit(4);
    }

    // Returning null
    else if(stoken->SToken->type == NULL_VALUE){
        if(curr_func->FnVar.Fn_id.return_type.nullable == false){
            fprintf(stderr, "cannot return null\n");
            exit(4);
        }
        // Copy the null pointer
        TreeElement *new = TreeInsert(stoken->current_node, NULL);
        new->Data.NodeType = EXPRESSION_NODE;
        new->Data.Token = copy_token(stoken->SToken);
        stoken->returned = true;
        return t_null(stoken) &&
        t_semicolon(stoken);
    }

    // returning id expression or string variable
    else if(ret_var != NULL){
        if (ret_var->FnVar.Var_id.type.type == U8){
            if (curr_func->FnVar.Fn_id.return_type.type != U8) {
                fprintf(stderr, "Wrong return type\n");
                exit(4);
            }
            if (ret_var->FnVar.Var_id.type.nullable) {
                if (!curr_func->FnVar.Fn_id.return_type.nullable){
                    fprintf(stderr, "Wrong return type\n");
                    exit(4);
                }
            }
            // New tree node
            TreeElement *new = TreeInsert(stoken->current_node, NULL);
            new->Data.NodeType = EXPRESSION_NODE;
            new->Data.Token = copy_token(stoken->SToken);
            new->Data.TableElement = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);

            stoken->returned = true;
            ret_var->FnVar.Var_id.used = true;
            return t_id(stoken) &&
            t_semicolon(stoken);
        }
        else {
            PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
            if (result == NULL){
                fprintf(stderr,"Strasne obrovska chyba\n");
                exit(99);
            }
            if (result->Error != 0){
                fprintf(stderr,"Exp error\n");
                exit(result->Error);
            }
            if (curr_func->FnVar.Fn_id.return_type.type != result->Tree->Data.Type - 12) {
                fprintf(stderr, "Wrong return type\n");
                exit(4);
            }
            stoken->SToken = result->NextTotken;
            stoken->returned = true;
            // Connecting expression to the main tree
            TreeElementConnect(stoken->current_node, result->Tree);
            return t_semicolon(stoken);
        }
    }

    // Assigning non-string literals
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        PrecResultPtr result = preced_analysis(stoken->SToken, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
        if (result == NULL){
            fprintf(stderr,"Strasne obrovska chyba\n");
            exit(99);
        }
        if (result->Error != 0){
            fprintf(stderr,"Exp error\n");
            exit(result->Error);
        }
        if (curr_func->FnVar.Fn_id.return_type.type != result->Tree->Data.Type - 12) {
            fprintf(stderr, "Wrong return type\n");
            exit(4);
        }
        stoken->SToken = result->NextTotken;
        stoken->returned = true;
        // Connecting expression to the main tree
        TreeElementConnect(stoken->current_node, result->Tree);
        return t_semicolon(stoken);
    }

    // Assigning string
    else if(stoken->SToken->type == STRING){
        fprintf(stderr, "Returning string not allowd\n");
        exit(4);
    }
    fprintf(stderr, "Syntax Error\n");
    exit(2);
}

bool fn_return(TokenStoragePtr stoken){
    bool ret;

    // New node in tree and moving deeper into the tree
    TreeElement *new_node = TreeInsert(stoken->current_node, NULL);
    new_node->Data.NodeType = RETURN_NODE;
    stoken->current_node = stoken->current_node->Node[stoken->current_node->NodeCounter - 1];

    ret = t_return(stoken) &&
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
        if (tmp_var != NULL && tmp_fn != NULL){
            fprintf(stderr, "function and variable with the same name defined\n");
            exit(10);
        }
        else if(tmp_fn != NULL){
            if (tmp_fn->FnVar.Fn_id.return_type.type != VOID){
                fprintf(stderr, "return type dumped\n");
                exit(4);
            }
            return fn_call(stoken) &&
            fn_body(stoken);
        }
        else if (tmp_var != NULL || stoken->SToken->type == UNDERSCORE){
            return assign(stoken) &&
            fn_body(stoken);
        }
        fprintf(stderr, "nondef func or variable\n");
        exit(3);
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
        return if_else(stoken) && // TODO sematics and syntax
        fn_body(stoken);
    }
    /*
    *   Cycle block
    */
    else if(stoken->SToken->type == WHILE){
        return cycle(stoken) && // TODO sematics and syntax
        fn_body(stoken);
    }
    /*
    *   Return from function
    */
    else if(stoken->SToken->type == RETURN){
        return fn_return(stoken) && // Sematics
        fn_body(stoken);
    }
    /*
    *   End of the current function
    */
    else {
        return true;
    }
}

bool l_type(TokenStoragePtr stoken){
    return t_nullable(stoken) &&
    t_type_keyword(stoken);
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

bool l_type_fndef(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        return t_void(stoken);
    }
    else {
        return l_type(stoken);
    }
}

bool fn_def(TokenStoragePtr stoken){
    return t_pub(stoken) &&
    t_fn(stoken) &&
    t_id_fn(stoken) && // Setting stoken
    t_op_bracket(stoken) &&
    params(stoken) && // Does just syntactic checks in the second walkthrough
    t_cl_bracket(stoken) &&
    l_type_fndef(stoken); // Just syntactic check
}

bool functions(TokenStoragePtr stoken){
    static int node_counter = 0;
    if (stoken->SToken->type == END_OF_FILE) {
        return t_eof(stoken); // End of file
    }
    else {
        bool ret;
        if (stoken->current_fn != NULL){ // Reseting stoken
            free(stoken->current_fn);
            stoken->current_fn = NULL;
            free(stoken->level_stack);
            stoken->local_table = NULL;
            stoken->returned = false;
            stoken->level_stack = NULL;
            stoken->stack_size = 0;
            stoken->last_poped = 0;
        }
        // Setting current tree node
        stoken->current_node = stoken->current_node->Node[node_counter];
        node_counter++;
        ret = fn_def(stoken) && // Setting stoke context
        t_op_cr_bracket(stoken) &&
        fn_body(stoken) && // Main syntax and sematics validation
        t_cl_cr_bracket(stoken);

        if (ret == false){ // Syntax check
            syn_error(stoken);
        }

        stoken->current_node = stoken->current_node->DadNode;

        if (stoken->returned == false){ // Return check
            fprintf(stderr, "missing return\n");
            exit(6);
        }

        // Checking for variable usage

        Elem_id *elem_loc;
        for (int j = 0; j < TABLE_SIZE; j++){
            elem_loc = stoken->local_table[j];
            if (elem_loc == NULL){
                continue;
            }
            if (elem_loc->FnVar.Var_id.used == 0){
                fprintf(stderr, "Unused variable!\n");
                exit(9);
            }
        }


        return ret &&
        functions(stoken); // Recursive calling
    }
}

bool import_def(TokenStoragePtr stoken){ // Only syntax analysis
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
        exit(99);
    }

    Queue *queue;
    SymTable *glob_table;
    SymTable *ifj_table;
    // Tables init
    queue = queue_init();
    glob_table = TableInit();
    ifj_table = TableInit();

    // Tree init
    TreeElement *tree_root;
    TreeRootPtr x = TreeInit();
    tree_root = x->Root;
    tree_root->Data.NodeType = ROOT_NODE;

    stoken->queue = queue;
    stoken->glob_table = glob_table;
    stoken->ifj_table = ifj_table;
    stoken->current_fn = NULL;

    queue_fill(stoken, tree_root);

    // Searching for main, if not defined exit
    Elem_id *tmp = TableSearch("main", NULL, 0, stoken->glob_table);
    if (tmp == NULL){
        fprintf(stderr, "Main not defined\n");
        exit(3);
    }
    else if (tmp->FnVar.Fn_id.return_type.type != VOID || tmp->FnVar.Fn_id.num_of_params != 0){
        fprintf(stderr, "Main has wrong params\n");
        exit(4);
    }

    ifj_table_fill(stoken);

    free(stoken->current_fn);
    stoken->current_fn = NULL;
    stoken->returned = false;
    stoken->local_table = NULL;
    stoken->level_stack = NULL;
    stoken->stack_size = 0;
    stoken->last_poped = 0;
    stoken->current_node = tree_root;
    stoken->SToken = queue_next_token(queue);

    result = start(stoken);

    // !! Debug output
    Elem_id *elem;
    for (int i = 0; i < 1001; i++){
        elem = stoken->glob_table[i];
        if (elem == NULL){
            continue;
        }
        printf("--Globals--\n");
        printf("fn_name: %s\n",elem->name);
        printf("fn_return: %d\n",elem->FnVar.Fn_id.return_type.type);
        printf("fn_return_null: %d\n",elem->FnVar.Fn_id.return_type.nullable);
        printf("fn_num_of_params: %d\n",elem->FnVar.Fn_id.num_of_params);
        for (int i = 0; i < elem->FnVar.Fn_id.num_of_params; i++){
            printf("fn_param: %d\n",elem->FnVar.Fn_id.type_of_params[i].type);
            printf("fn_param_null: %d\n",elem->FnVar.Fn_id.type_of_params[i].nullable);
            printf("fn_name_of_param: %s\n", elem->FnVar.Fn_id.TableParams[i]->name);
            printf("fn_stack_len_of_param: %d\n", elem->FnVar.Fn_id.TableParams[i]->stack_size);
            printf("fn_stack_cont: ");
            for (int y = 0; y < elem->FnVar.Fn_id.TableParams[i]->stack_size; y++){
                printf("%d,",elem->FnVar.Fn_id.TableParams[i]->level_stack[y]);
            }
            printf("\n");
        }
        Elem_id *elem_loc;
        for (int j = 0; j < 1001; j++){
        elem_loc = elem->FnVar.Fn_id.LocalSymTable[j];
        if (elem_loc == NULL){
            continue;
        }
        printf("--Locals--\n");
        printf("name: %s\n",elem_loc->name);
        printf("stack_size: %d\n",elem_loc->stack_size);
        printf("stack_cont: ");
        for (int y = 0; y < elem_loc->stack_size; y++){
            printf("%d,",elem_loc->level_stack[y]);
        }
        printf("\n");
        printf("type: %d\n",elem_loc->FnVar.Var_id.type.type);
        printf("nullable: %d\n",elem_loc->FnVar.Var_id.type.nullable);
        printf("const: %d\n",elem_loc->FnVar.Var_id.const_t);
        }
    }
    print_tree(tree_root);
    printf("%d\n", tree_root->Data.NodeType);
    // !! Debug output
    gen_code(tree_root);

    if (result == false){
        fprintf(stderr, "syntax error\n");
        exit(2);
    }
    TableClear(ifj_table, FUNCTION);
    dealloc_stoken(stoken);
}

