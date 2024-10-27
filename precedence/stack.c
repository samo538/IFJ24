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

    return stack_base;
}

bool StackDestroy(StackBasePtr stack){

    TokenStoragePtr ret = NULL;

    do
    {
        ret = pop(stack);
    } while (ret != NULL);

    free(stack);

    return true;
}

TokenStoragePtr top(StackBasePtr stack){

    return stack->TopElement->Token;
}

StackElementPtr push(StackBasePtr stack, TokenStoragePtr token){

    StackElementPtr new_element = (StackElementPtr) malloc(sizeof(StackElement));
    if(new_element == NULL){
        return NULL;
    }

    new_element->Token = token;
    new_element->NextElement = stack->TopElement;
    stack->TopElement = new_element;

    if(stack->BottomElement == NULL){
        stack->BottomElement = new_element;
    }

    return new_element;
}

TokenStoragePtr pop(StackBasePtr stack){

    if(stack->BottomElement != NULL){
        StackElementPtr element_to_pop = stack->TopElement;
        
        if(element_to_pop->NextElement == NULL){

            stack->TopElement = NULL;
            stack->BottomElement = NULL;
        }
        else{

            stack->TopElement = element_to_pop->NextElement;
        }

        TokenStoragePtr token = element_to_pop->Token;
        
        free(element_to_pop);

        return token;
    }
    return NULL;
}
