#include <stdbool.h>

#include "precedence.h"
#include "stack.h"
#include "../tree/tree.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"

void print_tree(TreeElementPtr element){

    for(int i = 0; i < element->NodeCounter; i++){
        print_tree(element->Node[i]);
        printf("%d -> ", element->Node[i]->Data.Token->type);
    }
}


int main(){

    TokenPtr token1 = next_token();
    TokenPtr token2 = next_token();

    PrecResultPtr result = preced_analysis(token1, token2, true, NULL, 0, NULL);
    if(result == NULL){
        fprintf(stderr, "nakokot to je\n");
        return -1;
    }

    printf("tvoj stromcek\n");

    if(result->Error == 0){
        print_tree(result->Tree);
        printf("%d\n\n", result->Tree->Data.Token->type);
        printf("result type: %d\n\n", result->Tree->Data.Type);
    }
    printf("error: %d\n\n", result->Error);
    printf("next token: %d\n", result->NextTotken->type);

    TreeNodeDelete(result->Tree);
    dealloc_token(result->NextTotken);
    free(result);
    return 0;
}