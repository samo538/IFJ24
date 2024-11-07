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

#include <stdio.h>

bool fn_call(TokenStoragePtr stoken);

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


bool l_type_fndef(TokenStoragePtr stoken){
    if (stoken->SToken->type == VOID){
        return t_void(stoken);
    }
    else {
        return l_type(stoken);
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

/*
 *  Variable definition
 */
bool ifj_call_var(TokenStoragePtr stoken, Elem_id *new){
    bool ret;
    ret = t_ifj(stoken) &&
    t_dot(stoken);

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
        return ret &&
        t_id_ifj(stoken) &&
        t_op_bracket(stoken) &&
        call_params(stoken, fn, 0) && // Checking the validity of params
        t_cl_bracket(stoken) &&
        t_semicolon(stoken);
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
            if (ret_fn->FnVar.Fn_id.return_type.nullable){
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
                if (ret_var->FnVar.Var_id.type.nullable){
                    if(!new->FnVar.Var_id.type.nullable){
                        fprintf(stderr, "Wrong assign type\n");
                        exit(7);
                    }
                }
            }
            return t_id(stoken) &&
            t_semicolon(stoken);
        }
        else {
            PrecResultPtr result = preced_analysis(stoken->SToken, NULL, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
            if (result == NULL){
                fprintf(stderr,"Strasne obrovska chyba\n");
                exit(99);
            }
            if (result->Error != 0){
                fprintf(stderr,"Exp error\n");
                exit(result->Error);
            }
            if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Implicit type
                new->FnVar.Var_id.type.type = result->Tree->Data.Type;
                new->FnVar.Var_id.type.nullable = false;
            }
            else { // Explicit type, check data types
                if (new->FnVar.Var_id.type.type != result->Tree->Data.Type) {
                    fprintf(stderr, "Wrong assign type\n");
                    exit(7);
                }
            }
            stoken->SToken = result->NextTotken;
            return t_semicolon(stoken);
        }
    }

    // Assigning non-string literals
    else if(stoken->SToken->type == F64_VAR || stoken->SToken->type == I32_VAR || stoken->SToken->type == OPENING_BRACKET){
        PrecResultPtr result = preced_analysis(stoken->SToken, NULL, false, stoken->level_stack, stoken->stack_size, stoken->local_table, stoken->queue);
        if (result == NULL){
            fprintf(stderr,"Strasne obrovska chyba\n");
            exit(99);
        }
        if (result->Error != 0){
            fprintf(stderr,"Exp error\n");
            exit(result->Error);
        }
        if (new->FnVar.Var_id.type.type == END_OF_FILE){ // Implicit type
            new->FnVar.Var_id.type.type = result->Tree->Data.Type;
            new->FnVar.Var_id.type.nullable = false;
        }
        else { // Explicit type, check data types
            if (new->FnVar.Var_id.type.type != result->Tree->Data.Type) {
                fprintf(stderr, "Wrong assign type\n");
                exit(7);
            }
        }
        stoken->SToken = result->NextTotken;
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
    Elem_id *new = malloc(sizeof(Elem_id)); // Creating a new local element
    new->Type = VARIABLE;
    new->stack_size = stoken->stack_size; // Setting the stack
    copy_levels(stoken->level_stack, &new->level_stack, stoken->stack_size);
    new->FnVar.Var_id.type.type = END_OF_FILE; // Placeholder
    new->FnVar.Var_id.type.nullable = false; // Implicit false
    new->FnVar.Var_id.used = false; // Variable not used by default

    ret = l_var_const(stoken, new) && // Filling up new
    t_id_var(stoken, new) && // Filling up the name / checking for redef
    l_type_vardef(stoken, new) && // Implicit/Explicit definition
    t_eq(stoken) &&
    e_var_exp_def(stoken, new); // Adding value

    TableAdd(*new, stoken->local_table); // Adding the new element into local table

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
    Elem_id *fn = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
    if (fn == NULL){
        fprintf(stderr, "Undefined function\n");
        exit(3);
    }
    return t_id(stoken) &&
    t_op_bracket(stoken) &&
    call_params(stoken, fn, 0) && // Sematics action, check params validity
    t_cl_bracket(stoken) &&
    t_semicolon(stoken);
}

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
            return false;
        }

        if (stoken->SToken->type == F64_VAR){
            ret = ret && t_float(stoken);
            if (ret == false){
                return false;
            }
        }
        else if (stoken->SToken->type == I32_VAR){
            ret = ret && t_int(stoken);
            if (ret == false){
                return false;
            }

        }
        else if (stoken->SToken->type == STRING){
            ret = ret && t_string(stoken);
            if (ret == false){
                return false;
            }

        }
        else if (stoken->SToken->type == NULL_VALUE){
            ret = ret && t_null(stoken);
            if (ret == false){
                return false;
            }

        }
        else if (stoken->SToken->type == ID){
        Elem_id *var = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
            if (var == NULL){
                fprintf(stderr, "Undefined variable\n");
                exit(3);
            }
            ret = ret && t_id(stoken);
            if (ret == false){
                return false;
            }

        }
        else {
            fprintf(stderr, "syntax error\n");
            exit(2);
        }
        return ret &&
        t_comma(stoken) &&
        t_cl_bracket(stoken) &&
        t_semicolon(stoken);
    }

    syn_error(stoken);
    return false;
}

bool if_while_body(TokenStoragePtr stoken){
    return t_op_cr_bracket(stoken) &&
    fn_body(stoken) &&
    t_cl_cr_bracket(stoken);
}


bool assign(TokenStoragePtr stoken){
    return l_id_assign(stoken) &&
    t_eq(stoken);// &&
    //e_var_exp_assign(stoken, NULL);
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
    else if(stoken->SToken->type == ID){ // Or _
        Elem_id *tmp = TableSearch(stoken->SToken->value.str, NULL, 0, stoken->glob_table);
        if (tmp != NULL){
            if (tmp->FnVar.Fn_id.return_type.type != VOID){
                fprintf(stderr, "return type dumped\n");
                exit(4);
            }
            return fn_call(stoken) &&
            fn_body(stoken);
        }
        tmp = TableSearch(stoken->SToken->value.str, stoken->level_stack, stoken->stack_size, stoken->local_table);
        if (tmp != NULL){
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
        return fn_return(stoken) && // Todo sematics
        fn_body(stoken);
    }
    /*
    *   End of the current function
    */
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
    t_id_fn(stoken) && // Setting stoken context
    t_op_bracket(stoken) &&
    params(stoken) && // Does just syntactic checks in the second walkthrough
    t_cl_bracket(stoken) &&
    l_type_fndef(stoken); // Just syntactic check
}

bool functions(TokenStoragePtr stoken){
    if (stoken->SToken->type == END_OF_FILE) {
        return t_eof(stoken); // End of file
    }
    else {
        if (stoken->current_fn != NULL){ // Freeing stoken context
            free(stoken->current_fn);
            stoken->current_fn = NULL;
            free(stoken->level_stack);
            stoken->level_stack = NULL;
            stoken->stack_size = 0;
        }
        return fn_def(stoken) && // Setting stoke context
        t_op_cr_bracket(stoken) &&
        fn_body(stoken) && // Main syntax and sematics validation
        t_cl_cr_bracket(stoken) &&
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
        return 99;
    }

    Queue *queue;
    SymTable *glob_table;
    SymTable *ifj_table;
    queue = queue_init();
    glob_table = TableInit();
    ifj_table = TableInit();

    stoken->queue = queue;
    stoken->glob_table = glob_table;
    stoken->ifj_table = ifj_table;
    stoken->current_fn = NULL;

    queue_fill(stoken);
    ifj_table_fill(stoken);

    free(stoken->current_fn);
    stoken->current_fn = NULL;
    stoken->SStoredToken = NULL;
    stoken->local_table = NULL;
    stoken->level_stack = NULL;
    stoken->stack_size = 0;
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
        printf("%s\n",elem->name);
        printf("%d\n",elem->FnVar.Fn_id.return_type);
        printf("%d\n",elem->FnVar.Fn_id.return_type.nullable);
        printf("%d\n",elem->FnVar.Fn_id.num_of_params);
        for (int i = 0; i < elem->FnVar.Fn_id.num_of_params; i++){
            printf("%d\n",elem->FnVar.Fn_id.type_of_params[i].type);
            printf("%d\n",elem->FnVar.Fn_id.type_of_params[i].nullable);
        }
        Elem_id *elem_loc;
        for (int j = 0; j < 1001; j++){
        elem_loc = elem->FnVar.Fn_id.LocalSymTable[j];
        if (elem_loc == NULL){
            continue;
        }
        printf("--Locals--\n");
        printf("%s\n",elem_loc->name);
        printf("%d\n",elem_loc->stack_size);
        for (int y = 0; y < elem_loc->stack_size; y++){
            printf("%d ",elem_loc->level_stack[y]);
        }
        printf("\n");
        printf("%d\n",elem_loc->FnVar.Var_id.type.type);
        printf("%d\n",elem_loc->FnVar.Var_id.type.nullable);
        printf("%d\n",elem_loc->FnVar.Var_id.const_t);
        }
    }
    printf("%d\n", result);
    // !! Debug output
    TableClear(ifj_table, FUNCTION);
    dealloc_stoken(stoken);
}

