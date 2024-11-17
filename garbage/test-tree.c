#include "tree.h"
#include "../lexer/lexer.h"

void print_tree(TreeElementPtr element){

    for(int i = 0; i < element->NodeCounter; i++){
        print_tree(element->Node[i]);
        printf("%s -> ", element->Node[i]->Data.Token->value.str);
    }
}


int main(){


    TokenPtr token[15];

    for(int i = 0; i < 15; i++){
        token[i] = next_token();
    }

    //A
    TreeRootPtr root = TreeInit(); 
    if(root == NULL){
        fprintf(stderr, "tree() retruned null\n");
        return -1;
    }
    root->Root->Data.Token = token[0];

    //B
    TreeElementPtr element_B = TreeElementCreate(token[1]);
    if(element_B == NULL){
        fprintf(stderr, "TreeElementConnect() returned null: elmement_B\n");
        return -1;
    }
    TreeElementPtr ret_element = TreeElementConnect(root->Root, element_B);
    if(element_B != ret_element){
        fprintf(stderr, "TreeElementConnect() returned different address than element_B\n");
        return -1;
    }

    //C
    TreeElementPtr element_C = TreeElementCreate(token[2]);
    if(element_C == NULL){
        fprintf(stderr, "TreeElementConnect() returned null: elmement_C\n");
        return -1;
    }
    ret_element = TreeElementConnect(root->Root, element_C);
    if(element_C != ret_element){
        fprintf(stderr, "TreeElementConnect() returned different address than element_C\n");
        return -1;
    }

    //D
    TreeElementPtr element_D = TreeElementCreate(token[3]);
    if(element_D == NULL){
        fprintf(stderr, "TreeElementConnect() returned null: elmement_D\n");
        return -1;
    }
    ret_element = TreeElementConnect(root->Root, element_D);
    if(element_D != ret_element){
        fprintf(stderr, "TreeElementConnect() returned different address than element_D\n");
        return -1;
    }

    //E
    TreeElementPtr element_E = TreeInsert(element_B, token[4]);
    if(element_E == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_E\n");
        return -1;
    }

    //F
    TreeElementPtr element_F = TreeInsert(element_B, token[5]);
    if(element_F == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_F\n");
        return -1;
    }

    //G
    TreeElementPtr element_G = TreeInsert(element_C, token[6]);
    if(element_G == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_G\n");
        return -1;
    }

    //H
    TreeElementPtr element_H = TreeInsert(element_C, token[7]);
    if(element_H == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_H\n");
        return -1;
    }

    //I
    TreeElementPtr element_I = TreeInsert(element_D, token[8]);
    if(element_I == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_I\n");
        return -1;
    }

    //J
    TreeElementPtr element_J = TreeInsert(element_F, token[9]);
    if(element_J == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_J\n");
        return -1;
    }

    //K
    TreeElementPtr element_K = TreeInsert(element_F, token[10]);
    if(element_K == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_K\n");
        return -1;
    }

    //L
    TreeElementPtr element_L = TreeInsert(element_H, token[11]);
    if(element_L == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_L\n");
        return -1;
    }

    //M
    TreeElementPtr element_M = TreeInsert(element_H, token[12]);
    if(element_M == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_M\n");
        return -1;
    }

    //N
    TreeElementPtr element_N = TreeInsert(element_J, token[13]);
    if(element_N == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_N\n");
        return -1;
    }

    //O
    TreeElementPtr element_O = TreeInsert(element_J, token[14]);
    if(element_O == NULL){
        fprintf(stderr, "TreeInsert() returned null: elmement_O\n");
        return -1;
    }

    print_tree(root->Root);
    printf("%s\n", root->Root->Data.Token->value.str);

    TreeDestroy(root);
}