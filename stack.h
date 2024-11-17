/**
 *  @project IFJ24
 *  @file stack.h
 *  @author Mario Klopan (xklopam00@stud.fit.vutbr.cz)
 */

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#include "tree.h"
#include "token.h"

typedef struct StackElement
{
    struct StackElement *NextElement;
    TreeElementPtr Tree;

}StackElement, *StackElementPtr;

typedef struct StackBase
{
    StackElementPtr TopElement;
    StackElementPtr BottomElement;
    StackElementPtr ActiveElement;
    int StackCounter;

}StackBase, *StackBasePtr;

StackBasePtr StackInit(void);
bool StackDestroy(StackBasePtr);

TreeElementPtr Top(StackBasePtr);
TreeElementPtr Push(StackBasePtr, TreeElementPtr);
TreeElementPtr Pop(StackBasePtr);

TreeElementPtr First(StackBasePtr);
TreeElementPtr Down(StackBasePtr);

#endif
