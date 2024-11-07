#include "stack.h"
#include "../lexer/lexer.h"
#include "../tree/tree.h"

int main(){

    StackBasePtr stack = StackInit();
    if(stack == NULL){
        fprintf(stderr, "alocation of stack base failed\n");
        return 1;
    }
    printf("stacke base initialized\n");

    TreeElementPtr tree[10];

    for(int i = 0; i < 10; i++){

        tree[i] = TreeElementCreate(next_token());
        

        StackElementPtr ret_element = Push(stack, tree[i]);
        if(ret_element == NULL){
            fprintf(stderr, "push failed\n");
            return 1;
        }
        printf("%d: token is pushed\n", i);

        TreeElementPtr ret_tree = Top(stack);
        if(ret_tree == NULL){
            fprintf(stderr, "token on top not found, top() failed\n");
            return 1;
        }
        if(ret_tree == tree[i]){
            printf("token on top is correct\n");
            printf("%s\n", tree[i]->Data.Token->value.str);
        }
        else{
            fprintf(stderr, "something went wrong\n");
        }
    }

    First(stack);

    for(int i = 0; i < 10; i++){

        if(stack->ActiveElement->Tree == tree[9-i]){
            
            printf("active element is correct\n");
            Down(stack);
        }
        else{

            fprintf(stderr, "bad active element\n");
            return 1;
        }

        TreeElementPtr ret_tree = Pop(stack);
        if(ret_tree == NULL){
            fprintf(stderr, "token on pop not found, pop() failed\n");
            return 1;
        }
        if(ret_tree == tree[9-i]){
            printf("%s: pop is correct\n", tree[9-i]->Data.Token->value.str);
            
            TreeNodeDelete(ret_tree);

        }
        else{
            fprintf(stderr, "something went wrong\n");
        }
    }

    StackDestroy(stack);



    return 0;
}