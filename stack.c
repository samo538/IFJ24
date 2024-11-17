/**
 *  @file stack.c
 *  @author Mario Klopan (xklopam00@stud.fit.vutbr.cz)
 */

#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"

StackBasePtr StackInit(void){
    
    StackBasePtr stack_base = (StackBasePtr) malloc(sizeof(StackBase));
    if(stack_base == NULL){
        return NULL;
    }

    stack_base->BottomElement = NULL;
    stack_base->TopElement = NULL;
    stack_base->ActiveElement = NULL;
    stack_base->StackCounter = 0;

    return stack_base;
}

bool StackDestroy(StackBasePtr stack){

    TreeElementPtr ret = NULL;

    do
    {
        ret = Pop(stack);
    } while (ret != NULL);

    free(stack);

    return true;
}

TreeElementPtr Top(StackBasePtr stack){

    return stack->TopElement->Tree;
}

TreeElementPtr Push(StackBasePtr stack, TreeElementPtr tree){

    StackElementPtr new_element = (StackElementPtr) malloc(sizeof(StackElement));
    if(new_element == NULL){
        return NULL;
    }

    new_element->Tree = tree;
    new_element->NextElement = stack->TopElement;
    stack->TopElement = new_element;

    if(stack->BottomElement == NULL){
        stack->BottomElement = new_element;
    }

    stack->StackCounter++;

    return new_element->Tree;
}

TreeElementPtr Pop(StackBasePtr stack){

    if(stack->BottomElement != NULL){
        StackElementPtr element_to_pop = stack->TopElement;
        
        if(element_to_pop->NextElement == NULL){

            stack->TopElement = NULL;
            stack->BottomElement = NULL;
        }
        else{

            stack->TopElement = element_to_pop->NextElement;
        }

        TreeElementPtr tree = element_to_pop->Tree;

        if(stack->ActiveElement == element_to_pop){
            
            stack->ActiveElement = NULL;
        }

        free(element_to_pop);

        stack->StackCounter--;

        return tree;
    }
    return NULL;
}

TreeElementPtr First(StackBasePtr stack){
    
    if(stack->StackCounter > 0){
        stack->ActiveElement = stack->TopElement;
        return stack->ActiveElement->Tree;
    }

    return NULL;

}

TreeElementPtr Down(StackBasePtr stack){

    if(stack->ActiveElement != NULL){

        if(stack->ActiveElement->NextElement != NULL){
            
            stack->ActiveElement = stack->ActiveElement->NextElement;
            return stack->ActiveElement->Tree;
        }
    }

    return NULL;
}

