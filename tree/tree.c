#include <stdlib.h>

#include "tree.h"
#include "../lexer/lexer.h"

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

void TreeDestroy(TreeRootPtr tree){

    TreeNodeDelete(tree->Root);
    free(tree);
}

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

void TreeNodeDelete(TreeElementPtr node){

    if(node != NULL){
        for(int i = 0; i < node->NodeCounter; i++){

            TreeNodeDelete(node->Node[i]);
        }

        dealloc_token(node->Data.Token);
        
        if(node->NodeCounter > 0){
            free(node->Node);
        }

        free(node);
    }
}
