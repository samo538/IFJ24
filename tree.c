/**
 *  @project IFJ24
 *  @file syntax.c
 *  @author Mario Klopan (xklopam00@stud.fit.vutbr.cz)
 */

#include <stdlib.h>

#include "tree.h"
#include "lexer.h"

//alloc blank root element 
TreeRootPtr TreeInit(void){

    TreeRootPtr tree = malloc(sizeof(TreeRoot));
    if(tree == NULL){
        return NULL;
    }

    tree->Root = malloc(sizeof(TreeElement));
    if(tree->Root == NULL){
        free(tree);
        return NULL;
    }

    tree->Root->Node = NULL;
    tree->Root->NodeCounter = 0;
    tree->Root->Data.Token = NULL;
    tree->Root->Data.Type = 0;
    tree->Root->Data.ChangeType = false;
    tree->Root->Data.isDef = false;

    return tree;
}

//destory all elements in tree
void TreeDestroy(TreeRootPtr tree){

    TreeNodeDelete(tree->Root);
    free(tree);
}

//alloc element 
TreeElementPtr TreeElementCreate(TokenPtr token){

    TreeElementPtr element = malloc(sizeof(TreeElement));
    if(element == NULL){
        return NULL;
    }

    element->DadNode = NULL;
    element->Node = NULL;
    element->NodeCounter = 0;
    element->Data.Token = token;
    element->Data.Type = 0;
    element->Data.ChangeType = false;
    element->Data.isDef = false;

    return element;
}

//connect two elements  
TreeElementPtr TreeElementConnect(TreeElementPtr dad, TreeElementPtr son){

    if(dad->NodeCounter == 0){
    
        dad->Node = malloc(sizeof(TreeElementPtr));
        if(dad->Node == NULL){
            return NULL;
        }
    }
    else{

        dad->Node = realloc(dad->Node, sizeof(TreeElementPtr)*((dad->NodeCounter)+1));
        if(dad->Node == NULL){
            return NULL;
        }  
    }
    
    dad->Node[dad->NodeCounter] = son;
    son->DadNode = dad;
    dad->NodeCounter++;

    return dad->Node[(dad->NodeCounter)-1];
}

//alloc element and connect that element into tree
TreeElementPtr TreeInsert(TreeElementPtr dad_node, TokenPtr token){

    TreeElementPtr son_element = TreeElementCreate(token);
    if(son_element == NULL){
        return NULL;
    }

    TreeElementPtr ret_element = TreeElementConnect(dad_node, son_element);
    if(son_element != ret_element){
        return NULL;
    }
}

//recursively delete all elements to the given element of tree
void TreeNodeDelete(TreeElementPtr node){

    if(node != NULL){
        for(int i = 0; i < node->NodeCounter; i++){

            TreeNodeDelete(node->Node[i]);
        }

        if(node->NodeCounter > 0){
            free(node->Node);
        }

        free(node);
    }
}
