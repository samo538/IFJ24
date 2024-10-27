#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#include "../lexer/token.h"

typedef struct StackElement
{
    struct StackElement *NextElement;
    TokenPtr Token;

}StackElement, *StackElementPtr;

typedef struct StackBase
{
    StackElementPtr TopElement;
    StackElementPtr BottomElement;

}StackBase, *StackBasePtr;

StackBasePtr StackInit(void);
bool StackDestroy(StackBasePtr);

TokenPtr top(StackBasePtr);
StackElementPtr push(StackBasePtr, TokenPtr);
TokenPtr pop(StackBasePtr);

#endif
