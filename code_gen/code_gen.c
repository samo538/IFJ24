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

    //Local Frame
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    for(int i=0;i < main->NodeCounter;i++) {
        switch(main->Node[i]->Data.NodeType) {
            case ASSIGN_NODE: {
                gen_assign(main->Node[i]);
                break;
            }
            case DEFINITION_NODE: {
                gen_definition(main->Node[i]);
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

    printf("POPFRAME\n");
}

void choose_ifj_func(TreeElementPtr tree) {
    if(strcmp("write", tree->Data.TableElement->name) == 0) {
        gen_ifj_write(tree);
    }
}

void gen_func(TreeElementPtr func) {
    printf("%s\n",func->Data.TableElement->name);
    for(int i=0;i < func->NodeCounter;i++) {
        switch(func->Node[i]->Data.NodeType) {
            case ASSIGN_NODE: {
                gen_assign(func->Node[i]);
                break;
            }
            case DEFINITION_NODE: {
                gen_definition(func->Node[i]);
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
                choose_ifj_func(func->Node[i]);
            }
        }
    }
}

void gen_expression(TreeElementPtr tree) {
    for(int i = 0; i < tree->NodeCounter;i++) {
        gen_expression(tree->Node[i]);
    }
    //printf("Kokot:%d\n",tree->Data.Token->type);
    switch (tree->Data.Token->type) {
        case I32_VAR: { //konst
            //add redef
            printf("PUSHS int@%d\n", tree->Data.Token->value.i);
            if(tree->Data.ChangeType) {
                printf("INT2FLOATS\n");
            }
            break;
        }
        case F64_VAR: { //konst
            //add redef
            printf("PUSHS float@%a\n",tree->Data.Token->value.f64);
            if(tree->Data.ChangeType) {
                printf("FLOAT2INTS\n");
            }
            break;
        }
        case ID: { //variable //zatím nejde
            //printf("PUSH LF@%d\n",tree->Data);
            break;
        }
        case PLUS:{
            printf("ADDS\n");
            break;}
        case MINUS:{
            printf("SUBS\n");
            break;}
        case MULTIPLY:{
            printf("MULS\n");
            break;}
        case DIVIDE:{
            printf("IDIFS\n");
            break;}
    }
    //tree->Data.Type
    //if var/cost push
    //if oper use stackInstrucion
    //check for předefinování


}

void gen_definition(TreeElementPtr tree) {
    //printf("%s\n",get_var_name(tree->Node[0]));
    printf("def:\n");
    gen_assign(tree);
}

void gen_assign(TreeElementPtr tree) {
    printf("kokot:%d\n",tree->Node[1]->Data.NodeType);
    if(tree->Node[1]->Data.NodeType == EXPRESSION_NODE)
    {gen_expression(tree->Node[1]);}
}

void gen_func_call(TreeElementPtr tree) {

}

void gen_condition(TreeElementPtr tree) {

}

void gen_ifj_write(TreeElementPtr tree) {
    TreeElementPtr arg = tree->Node[0];
    if(arg->Data.NodeType == ARG_NODE) {
        switch(arg->Data.Token->type) {
            case STRING: {
                printf("WRITE string@%s\n",arg->Data.Token->value.str);
                break;
            }
            case I32_VAR: {
                printf("WRITE int@%d\n",arg->Data.Token->value.i);
                break;
            }
            case F64_VAR: {
                printf("WRITE float@%a\n",arg->Data.Token->value.f64);
            }
        }
    }
}

char* get_var_name(TreeElementPtr tree) {
    
    char* name = "LF@";
    strcat(name,tree->Data.TableElement->name);
    /*for(int i=0;i < tree->Data.TableElement->stack_size;i++) {
        strcat(name,itoa(tree->Data.TableElement->level_stack[i]));
    }*/
    return name;
}
