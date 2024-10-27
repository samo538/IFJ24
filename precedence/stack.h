#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#include "../syntax/syntax.h"

typedef struct StackElement
{
    struct StackElement *NextElement;
    TokenStoragePtr Token;

}StackElement, *StackElementPtr;

typedef struct StackBase
{
    StackElementPtr TopElement;
    StackElementPtr BottomElement;

}StackBase, *StackBasePtr;

StackBasePtr StackInit(void);
bool StackDestroy(StackBasePtr);

TokenStoragePtr top(StackBasePtr);
StackElementPtr push(StackBasePtr, TokenStoragePtr);
TokenStoragePtr pop(StackBasePtr);

#endif
