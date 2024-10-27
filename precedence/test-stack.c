#include "stack.h"
#include "../lexer/lexer.h"

int main(){

    StackBasePtr stack = StackInit();
    if(stack == NULL){
        fprintf(stderr, "alocation of stack base failed\n");
        return 1;
    }
    printf("stacke base initialized\n");

    TokenStoragePtr token[10];
    for(int i = 0; i< 10; i++){
        token[i] = malloc(sizeof(TokenStorage)*10);
    }
    if (token == NULL){
        return 99;
    }

    for(int i = 0; i < 10; i++){

        token[i]->SToken = next_token();
        token[i]->SStoredToken = NULL;

        StackElementPtr ret_element = push(stack, token[i]);
        if(ret_element == NULL){
            fprintf(stderr, "push failed\n");
            return 1;
        }
        printf("%d: token is pushed\n", i);

        TokenStoragePtr ret_token = top(stack);
        if(ret_token == NULL){
            fprintf(stderr, "token on top not found, top() failed\n");
            return 1;
        }
        if(ret_token == token[i]){
            printf("token on top is correct\n");
            printf("%s\n", token[i]->SToken->value);
        }
        else{
            fprintf(stderr, "something went wrong\n");
        }
    }

    for(int i = 0; i < 10; i++){
        TokenStoragePtr ret_token = pop(stack);
        if(ret_token == NULL){
            fprintf(stderr, "token on pop not found, pop() failed\n");
            return 1;
        }
        if(ret_token == token[9-i]){
            printf("%s: pop is correct\n", token[9-i]->SToken->value);
            dealloc_token(ret_token->SToken);
            free(ret_token);
        }
        else{
            fprintf(stderr, "something went wrong\n");
        }
    }

    StackDestroy(stack);



    return 0;
}