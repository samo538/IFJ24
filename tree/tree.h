#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "../lexer/token.h"
#include "../symtable/symtable.h"

enum NodeTypes{
    // Top nodes
    ROOT_NODE, //0
    TOP_FUNCTION_NODE, //1
    // Main sub nodes
    ASSIGN_NODE, //2
    DEFINITION_NODE, //3
    RETURN_NODE, //4
    WHILE_NODE, //5
    IF_NODE, //6
    ELSE_NODE, //7
    FUNCTION_NODE, //8
    IFJ_FUNCTION_NODE, //9
    // "Leaf" nodes
    EXPRESSION_NODE, //10
    ARG_NODE, // 11
    ID_NODE, // 12
};

typedef struct TreeData
{
    // Used by code gen
    bool isNullCond;
    TokenPtr Token;
    enum NodeTypes NodeType;
    Elem_id *TableElement;
    int Type;
    bool ChangeType;
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
