/**
 *  @file precedence/precedence.c
 *  @author Mario Klopan (xklopam00@stud.fit.vutbr.cz)
 */

#include <stdbool.h>

#include "stack.h"
#include "precedence.h"
#include "../lexer/token.h"
#include "../lexer/lexer.h"
#include "../tree/tree.h"
#include "../symtable/symtable.h"

enum PrecTable{
    TAKE_NEXT,
    REDUCTION,
    EQUAL_BR,
    ERROR,
    END,
};

int error_type = 0;
bool run_cycle = true;

/*															
	    ||	+   |   -	|   *   |	/	|   (   |	)   |	id	|	==  |	!=  |	<	|   >   |	<=  |	>=  |   $   |
    ====||===============================================================================================================
	+   ||	>   |	>   |	<   |	<   |	<   |	>   |	<   |	>   |	>   |	>   |	>   |	>   |	>   |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	-   ||	>   |	>   |	<   |	<   |	<   |	>   |	<   |	>   |	>   |	>   |	>   |	>   |	>   |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	*   ||	>   |	>   |	>   |	>   |	<   |	>   |	<   |	>   |	>   |	>   |	>   |	>   |	>   |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	/	||  >   |	>   |	>   |	>   |	<   |	>   |	<   |	>   |	>   |	>   |	>   |	>   |	>   |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	(   ||	<   |	<   |	<   |	<   |	<   |	=   |	<   |   <   |   <   |   <   |   <   |   <   |   <   |       |
    ----||---------------------------------------------------------------------------------------------------------------	
	)   ||	>   |	>   |	>   |	>   |	    |   >   |		|	>   |	>   |	>   |	>   |	>   |	>   |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	id  ||	>   |	>   |	>   |	>   |	    |	>   |	    |	>   |	>   |	>   |	>   |	>   |	>   |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	==  ||	<   |	<   |	<   |	<   |	<   |	>   |	<   |		|		|	    |       |       |       |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	!=  ||	<   |	<   |	<   |	<   |	<	|   >   |	<   |       |       |   	|		|		|		|   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	<   ||	<   |	<   |	<   |	<   |	<   |	>   |	<   |	    |	    |	    |	    |	    |	    |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	>   ||	<   |	<   |	<   |	<   |	<   |	>   |	<   |	    |	    |	    |	    |	    |	    |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	<=  ||	<   |	<   |	<   |	<   |	<   |	>   |	<   |	    |	    |	    |	    |	    |	    |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
	>=  ||	<   |	<   |	<   |	<   |	<   |	>   |	<   |	    |	    |	    |	    |	    |	    |   >   |
    ----||---------------------------------------------------------------------------------------------------------------
    $   ||	<   |	<   |	<   |	<   |	<   |	    |	<   |	<   |	<   |	<   |	<   |	<   |	<   |       |
    ----||---------------------------------------------------------------------------------------------------------------

*/

bool leaves_control(TreeElementPtr element){

    for(int i = 0; i < element->NodeCounter; i++){
        if(leaves_control(element->Node[i])){
            
        }
        else{
            return false;
        }
    }

    if(element->NodeCounter == 0){
        switch (element->Data.Token->type)
        {
        case ID:
            break;
        case I32_VAR:
            break;
        case F64_VAR:
            break;
        
        default:
            return false;
            break;
        }
        return true;
    }
}

int operator_reduction(StackBasePtr stack){

    TreeElementPtr ret_element;

    TreeElementPtr E_2 = Pop(stack);
    if(E_2 == NULL){
        goto error_pop;
    }

    TreeElementPtr operator = Pop(stack);
    if(operator == NULL){
        TreeNodeDelete(E_2);
        goto error_pop;
    }
    if((operator->Data.Token->type > MORE_OR_EQUAL) || ((operator->Data.Token->type > DIVIDE) && (operator->Data.Token->type < EQUAL))){
        TreeNodeDelete(E_2);
        error_type = 7;
        goto error_pop;
    }

    TreeElementPtr E_1 = Pop(stack);
    if(E_1 == NULL){
        TreeNodeDelete(E_2);
        TreeNodeDelete(operator);
        goto error_pop;
    }

    //typova kontrola
    if(E_1->Data.Type == I32_VAR && E_2->Data.Type == I32_VAR){
        operator->Data.Type = I32_VAR;
    }
    else if (E_1->Data.Type == I32_VAR && E_2->Data.Type == F64_VAR){
        operator->Data.Type = F64_VAR;
        E_1->Data.ChangeType = true;
    }
    else if (E_1->Data.Type == F64_VAR && E_2->Data.Type == I32_VAR){
        operator->Data.Type = F64_VAR;
        E_2->Data.ChangeType = true;
    }
    else if (E_1->Data.Type == F64_VAR && E_2->Data.Type == F64_VAR){
        operator->Data.Type = F64_VAR;
    }

    //vytvorenie podstromu
    ret_element = TreeElementConnect(operator, E_1);
    if(ret_element == NULL){
        goto error_connect;
    }

    ret_element = TreeElementConnect(operator, E_2);
    if(ret_element == NULL){
        goto error_connect;
    }

    ret_element = First(stack);
    if(ret_element == NULL){
        goto error_connect;
    }

    ret_element = Push(stack, operator);
    if(ret_element != operator){
        goto error_connect;
    }

    return REDUCTION;

error_pop:
    error_type = 99;
    return -1;

error_connect:
    error_type = 99;
    TreeNodeDelete(E_1);
    TreeNodeDelete(E_2);
    TreeNodeDelete(operator);
    return -1;

}

int E_reduction(StackBasePtr stack, int *level, int level_size, SymTable *Table){

    TreeElementPtr ret_element;

    switch (stack->ActiveElement->Tree->Data.Token->type)
    {
    case ID:
        Elem_id *id = TableSearch(stack->ActiveElement->Tree->Data.Token->value.str, level, level_size, Table);
        if(!(id->FnVar.Var_id.type.nullable)){
            if(id->FnVar.Var_id.type.type == I32){
                stack->ActiveElement->Tree->Data.Type = I32_VAR;
            }
            else if(id->FnVar.Var_id.type.type == F64){
                stack->ActiveElement->Tree->Data.Type = F64_VAR;
            }
            else{
                error_type = 7;
                return -1;
            }
        }
        else{
            error_type = 7;
            return -1;
        }
        break;
    case I32_VAR:
        stack->ActiveElement->Tree->Data.Type = I32_VAR;
        break;
    case F64_VAR:
        stack->ActiveElement->Tree->Data.Type = F64_VAR;
        break;
    
    default:
        break;
    }

    ret_element = Down(stack);
    if(ret_element == NULL){
        error_type = 99;
        return -1;
    }

    return REDUCTION;
}


int reduction(StackBasePtr stack, int *level, int level_size, SymTable *Table){

    int ret = 0;

    //ak je aktivny element id alebo cislo tak sa len posunie aktivita dole
    if((stack->ActiveElement->Tree->Data.Token->type == ID) ||
        (stack->ActiveElement->Tree->Data.Token->type == I32_VAR) ||
        (stack->ActiveElement->Tree->Data.Token->type == F64_VAR))
    {
        ret = E_reduction(stack, level, level_size, Table);
        if(ret == -1){
            return -1;
        }
        return REDUCTION;
    }

    ret = operator_reduction(stack);
    if(ret == -1){
        return -1;
    }
    
    return REDUCTION;
}

int take_next(StackBasePtr stack, TokenPtr n_token){
    
    TreeElementPtr element = TreeElementCreate(n_token);
    if(element == NULL){
        error_type = 99;
        return -1;
    }

    TreeElementPtr ret_element = Push(stack, element);
    if(ret_element != element){
        TreeNodeDelete(element);
        error_type = 99;
        return -1;
    }

    ret_element = First(stack);
    if(ret_element != element){
        error_type = 99;
        return -1;
    }

    return TAKE_NEXT;
}

int equal(StackBasePtr stack){

    TreeElementPtr ret_element;

    TreeElementPtr cl_bracket = Pop(stack);
    if(cl_bracket == NULL){
        goto error_pop;
    }
    if(cl_bracket->Data.Token->type != CLOSING_BRACKET){
        error_type = 7;
        return -1;
    }

    TreeElementPtr E = Pop(stack);
    if(E == NULL){
        TreeNodeDelete(cl_bracket);
        goto error_pop;
    }

    TreeElementPtr op_bracket = Pop(stack);
    if(op_bracket == NULL){
        TreeNodeDelete(E);
        TreeNodeDelete(cl_bracket);
        goto error_pop;
    }
    if(op_bracket->Data.Token->type != OPENING_BRACKET){
        TreeNodeDelete(E);
        TreeNodeDelete(cl_bracket);
        error_type = 7;
        return -1;
    }

    ret_element = First(stack);
    if(ret_element == NULL){
        goto error_connect;
    }

    ret_element = Push(stack, E);
    if(ret_element != E){
        goto error_connect;
    }

    return EQUAL_BR;

error_pop:
    error_type = 99;
    return -1;

error_connect:
    error_type = 99;
    TreeNodeDelete(cl_bracket);
    TreeNodeDelete(op_bracket);
    TreeNodeDelete(E);
    return -1;

}

int error(StackBasePtr stack){
    
    if(stack->StackCounter == 2){
        return END;
    }

    return ERROR;
}

int search_for_rule(int prec_table[14][14], TokenPtr next_token, StackBasePtr stack){

    int top_index = stack->ActiveElement->Tree->Data.Token->type;
    int next_token_index = next_token->type;

    if((next_token_index == I32_VAR) || (next_token_index == F64_VAR)){
        next_token_index = ID;
    }

    if((top_index == I32_VAR) || (top_index == F64_VAR)){
        top_index = ID;
    }

    if(next_token_index > MORE_OR_EQUAL){
        next_token_index = 13;
    }

    if(top_index == 100){
        top_index = 13;
    }

    if((top_index == 13) && (next_token_index == 13) && (stack->StackCounter == 1)){
        return END;
    }

    int rule = prec_table[top_index][next_token_index];

    return rule;
}

PrecResultPtr preced_analysis(TokenPtr first_token, TokenPtr second_token, bool rel_op, int *level, int level_size, SymTable *Table){

    int prec_table[14][14] = {{REDUCTION, REDUCTION, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, REDUCTION, TAKE_NEXT, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION},
                                {REDUCTION, REDUCTION, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, REDUCTION, TAKE_NEXT, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION},
                                {REDUCTION, REDUCTION, REDUCTION, REDUCTION, TAKE_NEXT, REDUCTION, TAKE_NEXT, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION},
                                {REDUCTION, REDUCTION, REDUCTION, REDUCTION, TAKE_NEXT, REDUCTION, TAKE_NEXT, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION},
                                {TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, EQUAL_BR, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, ERROR},
                                {REDUCTION, REDUCTION, REDUCTION, REDUCTION, ERROR, REDUCTION, ERROR, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION},
                                {REDUCTION, REDUCTION, REDUCTION, REDUCTION, ERROR, REDUCTION, ERROR, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION, REDUCTION},
                                {TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, REDUCTION, TAKE_NEXT, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, REDUCTION},
                                {TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, REDUCTION, TAKE_NEXT, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, REDUCTION},
                                {TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, REDUCTION, TAKE_NEXT, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, REDUCTION},
                                {TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, REDUCTION, TAKE_NEXT, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, REDUCTION},
                                {TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, REDUCTION, TAKE_NEXT, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, REDUCTION},
                                {TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, REDUCTION, TAKE_NEXT, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, REDUCTION},
                                {TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, ERROR, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, TAKE_NEXT, ERROR},
    };

    StackBasePtr stack = StackInit();

    TokenPtr dollar_token = malloc(sizeof(Token));
    if(dollar_token == NULL){
        error_type = 99;
        return NULL;
    }
    
    dollar_token->type = 100;
    dollar_token->value.i = 0;

    TreeElementPtr dollar_tree = TreeElementCreate(dollar_token);
    Push(stack, dollar_tree);
    First(stack);

    TokenPtr n_token = first_token;

    int counter = 0;

    int ret = 0;

    int rel_op_counter = 0;

    while(run_cycle){

        //overuje ci sa moze vyskytovat relacny operator vo vyraze
        if((rel_op == false) && (n_token->type >= 7 && n_token->type <= 12)){
            error_type = 7;
            goto error;
        }

        //overuje ci nie je realcny operator prvy vo vyraze
        if((counter == 0) && (n_token->type >= 7 && n_token->type <= 12)){
            error_type = 7;
            goto error;
        }

                

        //vratenie adresy funkcie z tabulky a jej nasledne volanie
        int rule = search_for_rule(prec_table, n_token, stack);
        
        switch (rule)
        {
        case REDUCTION:
            ret = reduction(stack, level, level_size, Table);
            if(ret == -1){
                goto error;
            }
            break;

        case TAKE_NEXT:
            ret = take_next(stack, n_token);
            if(ret == -1){
                goto error;
            }

            if(n_token->type >= 7 && n_token->type <= 12){
                rel_op_counter++;

                if(rel_op_counter > 1){
                    error_type = 7;
                    goto error;
                }
            }

            if((counter == 0) && (rel_op == true)){
                n_token = second_token;
            }
            else{
                n_token = next_token();
            }
        
            break;

        case EQUAL_BR:
            ret = take_next(stack, n_token);
            if(ret == -1){
                goto error;
            }

            if((counter == 0) && (rel_op == true)){
                n_token = second_token;
            }
            else{
                n_token = next_token();
            }

            ret = equal(stack);
            if(ret == -1){
                goto error;
            }
            break;

        case ERROR:
            ret = error(stack);
            if(ret == END){
                run_cycle = false;
            }
            else{
                goto error;
            }
            break;
        case END:
            run_cycle = false;
            break;

        default:
            goto error;
            break;
        }    
        
        counter ++;
    }

    if(leaves_control(stack->TopElement->Tree)){

        PrecResultPtr result = malloc(sizeof(PrecResult));
        if(result == NULL){
            error_type = 99;
            goto error;
        }
        
        result->Tree = Pop(stack);
        if(result->Tree == NULL){
            error_type = 99;
            goto error;
        }
        TreeNodeDelete(Pop(stack));
        StackDestroy(stack);

        result->NextTotken = n_token;
        result->Error = error_type;
        return result;
    }
    else{
        error_type = 7;
        goto error;
    }


error:
    for(int i = 0; i < stack->StackCounter; i++){
        TreeNodeDelete(Pop(stack));
    }
    StackDestroy(stack);

    PrecResultPtr result = malloc(sizeof(PrecResult));
    if(result == NULL){
        return NULL;
    }

    result->NextTotken = n_token;
    result->Tree = NULL;
    result->Error = error_type;

    return result;
    
}