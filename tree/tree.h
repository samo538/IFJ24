#ifndef TREE_H
#define TREE_H

#include "../lexer/token.h"

typedef struct TreeData
{
    TokenPtr Token;

}TreeData;

typedef struct TreeElement
{
    struct TreeElement **Node;
    struct TreeElement *DadNode;
    int NodeCounter;
    TreeData Data;
    
}TreeElement, *TreeElementPtr;

typedef struct TreeRoot
{
    TreeElementPtr Root;

}TreeRoot, *TreeRootPtr;

TreeRootPtr TreeInit(void);
void TreeDestroy(TreeRootPtr);

TreeElementPtr TreeInsert(TreeElementPtr, TokenPtr);
void TreeNodeDelete(TreeElementPtr);

TreeElementPtr TreeElementCreate(TokenPtr);
TreeElementPtr TreeElementConnect(TreeElementPtr dad, TreeElementPtr son);




#endif