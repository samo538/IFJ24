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

//stack element structure
typedef struct StackElement
{
    struct StackElement *NextElement;
    TreeElementPtr Tree;

}StackElement, *StackElementPtr;

//main point to access stack
typedef struct StackBase
{
    StackElementPtr TopElement;
    StackElementPtr BottomElement;
    StackElementPtr ActiveElement;
    int StackCounter;

}StackBase, *StackBasePtr;

//stack initialization
StackBasePtr StackInit(void);

//stack destroy
bool StackDestroy(StackBasePtr);

//read element on top of stack
TreeElementPtr Top(StackBasePtr);

//give element on top of stack
TreeElementPtr Push(StackBasePtr, TreeElementPtr);

//take first element from top of stack
TreeElementPtr Pop(StackBasePtr);

//set activity to first element
TreeElementPtr First(StackBasePtr);

//set acitivity to next element 
TreeElementPtr Down(StackBasePtr);

#endif
