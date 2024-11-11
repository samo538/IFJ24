//
// Created by veve on 8.11.24.
//
#include "code_gen.h"

void gen_code(TreeElementPtr tree) {
    printf(".IFJcode24\n");

    //find main
    TreeElementPtr main = tree->Node[0];
    int mainPosition = 0;
    for(int i=1;i < tree->NodeCounter;i++) {
        if(strcmp("main", main->Data.TableElement->name) == 0) {//check if fn name == main
            break;
        }
        main = tree->Node[i];
        mainPosition++;
    }

    gen_main(main);

    for(int i=0;i < tree->NodeCounter;i++) {
        if(i != mainPosition) {
            gen_func(tree->Node[i]);
        }
    }
}

void gen_main(TreeElementPtr main) {
    for(int i=0;i < main->NodeCounter;i++) {
        switch(main->Node[i]->Data.NodeType) {
            case ASSIGN_NODE: {
                break;
            }
            case DEFINITION_NODE: {
                break;
            }
            case RETURN_NODE: {
                break;
            }
            case WHILE_NODE: {
                break;
            }
            case IF_NODE: {
                break;
            }
            case FUNCTION_NODE: {
                break;
            }
            case IFJ_FUNCTION_NODE: {
                choose_ifj_func(main->Node[i]);
            }
        }
    }
}

void choose_ifj_func(TreeElementPtr tree) {
    if(strcmp("write", tree->Data.TableElement->name) == 0) {
        gen_ifj_write(tree);
    }
}

void gen_func(TreeElementPtr func) {
    printf("%s\n",func->Data.TableElement->name);
}

void gen_expression(TreeElementPtr tree) {

}

void gen_func_call(TreeElementPtr tree) {

}

void gen_condition(TreeElementPtr tree) {

}

void gen_ifj_write(TreeElementPtr tree) {
    TreeElementPtr arg = tree->Node[0];
    if(arg->Data.NodeType == ARG_NODE) {
        printf("%i %i\n",arg->Data.Token->type,COMMA);
        printf("%s\n",arg->Data.Token->value.str);
        switch(arg->Data.Token->type) {
            case STRING: {
                printf("WRITE string@%s",arg->Data.Token->value.str);
                break;
            }
            case I32: {
                printf("WRITE int@%s",arg->Data.Token->value.i);
                break;
            }
            case F64: {
                printf("WRITE float@%s",arg->Data.Token->value.f64);
            }
        }
    }
}
