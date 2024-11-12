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
        mainPosition++;
        main = tree->Node[i];
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

    gen_func_switch(main);

    printf("POPFRAME\n");
    printf("EXIT int@0\n");
}

void choose_ifj_func(TreeElementPtr tree, TreeElementPtr var) {
    if(strcmp("write", tree->Data.TableElement->name) == 0) {
        gen_ifj_write(tree);
    } else if (strcmp("length", tree->Data.TableElement->name) == 0) {
        gen_ifj_length(tree, var);
    } else if(strcmp("string", tree->Data.TableElement->name) == 0) {
        gen_ifj_string(tree,var);
    }
}

void gen_func_switch(TreeElementPtr func) {
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
                gen_func_call(func->Node[i]);
                break;
            }
            case IFJ_FUNCTION_NODE: {
                choose_ifj_func(func->Node[i], NULL);
            }
        }
    }
}

void gen_func(TreeElementPtr func) {
    printf("LABEL %s\n",func->Data.TableElement->name);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    Fn_id fnId = func->Data.TableElement->FnVar.Fn_id;

    for(int i=0;i < fnId.num_of_params;i++) {
        char* name = get_var_name_from_table(fnId.TableParams[i]);
        printf("POPS %s\n",name);
        free(name);
    }

    gen_func_switch(func);

    printf("POPFRAME\n");
    printf("RETURN\n");
}

void gen_expression(TreeElementPtr tree) {
    for(int i = 0; i < tree->NodeCounter;i++) {
        gen_expression(tree->Node[i]);
    }

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
            if(tree->Data.Type == 29) {printf("IDIVS\n");}
            else {printf("DIVS\n");}
            break;
        }
    }
}

void gen_definition(TreeElementPtr tree) {
    char* name = get_var_name(tree->Node[0]);
    printf("DEFVAR %s\n",name);
    free(name);

    gen_assign(tree);
}

void gen_assign(TreeElementPtr tree) {
    //right node
    switch (tree->Node[1]->Data.NodeType) {
        case (EXPRESSION_NODE): {
            gen_expression(tree->Node[1]);
            break;
        }
        case (FUNCTION_NODE): {
            gen_func_call(tree->Node[1]);
            break;
        }
        case (IFJ_FUNCTION_NODE): {
            choose_ifj_func(tree->Node[1], tree->Node[0]);
            return;
        }
    }

    //left node
    char* name = get_var_name(tree->Node[0]);
    printf("POPS %s\n",name);
    free(name);
}

void gen_func_call(TreeElementPtr tree) {
    for(int i = tree->NodeCounter - 1;i >= 0;i--) {
        switch(tree->Node[i]->Data.Token->type) {
            case STRING: {
                printf("PUSHS string@%s\n",tree->Node[i]->Data.Token->value.str);

                continue;
            }
            case I32_VAR: {
                printf("PUSHS int@%d\n",tree->Node[i]->Data.Token->value.i);

                continue;
            }
            case F64_VAR: {
                printf("PUSHS float@%a\n",tree->Node[i]->Data.Token->value.f64);

                continue;
            }
        }

        char* name = get_var_name(tree->Node[i]);
        printf("PUSHS %s\n",name);
        free(name);
    }

    printf("CALL %s\n",tree->Data.TableElement->name);
}

void gen_condition(TreeElementPtr tree) {

}

void gen_ifj_write(TreeElementPtr tree) {
    TreeElementPtr arg = tree->Node[0];
    if(arg->Data.NodeType == ARG_NODE) {
        switch(arg->Data.Token->type) {
            case STRING: {
                printf("WRITE string@%s\n",arg->Data.Token->value.str);

                return;
            }
            case I32_VAR: {
                printf("WRITE int@%d\n",arg->Data.Token->value.i);

                return;
            }
            case F64_VAR: {
                printf("WRITE float@%a\n",arg->Data.Token->value.f64);

                return;
            }
        }

        char* name = get_var_name(arg);
        printf("WRITE %s\n",name);
        free(name);
    }
}

void gen_ifj_length(TreeElementPtr tree, TreeElementPtr var) {
    char* length;
    char* str;
    length = get_var_name(var);
    if(tree->Node[0]->Data.Token->type == STRING) {
        str = tree->Node[0]->Data.Token->value.str;
        printf("STRLEN %s string@%s\n",length,str);
    }
    else {
        str = get_var_name(tree->Node[0]);
        printf("STRLEN %s %s\n",length,str);
        free(str);
    }
    free(length);

}

void gen_ifj_string(TreeElementPtr tree, TreeElementPtr var) {
    char* dest;
    char* str;
    str = tree->Node[0]->Data.Token->value.str;
    dest = get_var_name(var);
    printf("MOVE %s string@%s\n",dest,str);

}

char* get_var_name(TreeElementPtr tree) {
    char* name = (char*)malloc(sizeof(char)*256);
    if (name == NULL) {
        exit(99);
    }
    strcpy(name,"LF@");
    strcat(name,tree->Data.TableElement->name);
    for(int i=0;i < tree->Data.TableElement->stack_size;i++) {
        char layer[10];
        sprintf(layer,"%d",tree->Data.TableElement->level_stack[i]);
        strcat(name,layer);
    }

    return name;
}

char* get_var_name_from_table(Elem_id* tableElement) {
    char* name = (char*)malloc(sizeof(char)*256);
    if (name == NULL) {
        exit(99);
    }
    strcpy(name,"LF@");
    strcat(name,tableElement->name);
    for(int i=0;i < tableElement->stack_size;i++) {
        char layer[10];
        sprintf(layer,"%d",tableElement->level_stack[i]);
        strcat(name,layer);
    }

    return name;
}
