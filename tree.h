/**
 *  @project IFJ24
 *  @file syntax.c
 *  @author Mario Klopan (xklopam00@stud.fit.vutbr.cz)
 */

#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "token.h"
#include "symtable.h"

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
    bool isDef;
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

//alloc blank root element 
TreeRootPtr TreeInit(void);

//tree destroy
void TreeDestroy(TreeRootPtr);

//create element and insert it into tree
TreeElementPtr TreeInsert(TreeElementPtr, TokenPtr);

//recursively delete all elements to the given element of tree
void TreeNodeDelete(TreeElementPtr);

//alloc element
TreeElementPtr TreeElementCreate(TokenPtr);

//connect created element
TreeElementPtr TreeElementConnect(TreeElementPtr dad, TreeElementPtr son);




#endif
