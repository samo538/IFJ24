#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "../lexer/token.h"

enum NodeTypes{
    // Top nodes
    ROOT_NODE,
    TOP_FUNCTION_NODE,
    // Main sub nodes
    ASSIGN_NODE,
    DEFINITION_NODE,
    RETURN_NODE,
    WHILE_NODE,
    IF_NODE,
    FUNCTION_NODE,
    IFJ_FUNCTION_NODE,
    // "Leaf" nodes
    EXPRESSION_NODE,
    ID_NODE,
};

typedef struct TreeData
{
    TokenPtr Token;
    enum NodeTypes NodeType;

    // Used by precedence
    int Type;
    bool ChangeType;
    // Used by code gen
    bool isNullCond;
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
