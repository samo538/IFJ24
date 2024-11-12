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
    } else if(strcmp("concat", tree->Data.TableElement->name) == 0) {
        gen_ifj_concat(tree,var);
    } else if(strcmp("readstr", tree->Data.TableElement->name) == 0) {
        gen_ifj_readstr(tree,var);
    } else if(strcmp("readi32", tree->Data.TableElement->name) == 0) {
        gen_ifj_readi32(tree,var);
    } else if(strcmp("readf64", tree->Data.TableElement->name) == 0) {
        gen_ifj_readf64(tree,var);
    } else if(strcmp("i2f", tree->Data.TableElement->name) == 0) {
        gen_ifj_i2f(tree,var);
    } else if(strcmp("f2i", tree->Data.TableElement->name) == 0) {
        gen_ifj_f2i(tree,var);
    } else if(strcmp("chr", tree->Data.TableElement->name) == 0) {
        gen_ifj_chr(tree,var);
    } else if(strcmp("ord", tree->Data.TableElement->name) == 0) {
        gen_ifj_ord(tree,var);
    } else if(strcmp("strcmp", tree->Data.TableElement->name) == 0) {
        gen_ifj_strcmp(tree,var);
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
    free(dest);
}


void gen_ifj_concat(TreeElementPtr tree, TreeElementPtr var) {
    char* dest;
    char *str1,*str2;
    bool st1, st2;
    if(tree->Node[0]->Data.Token->type == STRING) {
        char *b = "string@";
        char *a = tree->Node[0]->Data.Token->value.str;
        str1 = malloc(strlen(a)+strlen(b)+1);
        strcpy(str1,b);
        strcat(str1,a);
        st1 = false;
    }
    else {
        str1 = get_var_name(tree->Node[0]);
        st1 = true;
    }
    if(tree->Node[1]->Data.Token->type == STRING) {
        char *b = "string@";
        char *a = tree->Node[1]->Data.Token->value.str;
        str2 = malloc(strlen(a)+strlen(b)+1);
        strcpy(str2,b);
        strcat(str2,a);
        st2 = false;
    }
    else {
        str2 = get_var_name(tree->Node[1]);
        st2 = true;
    }

    dest = get_var_name(var);
    printf("CONCAT %s %s %s\n",dest,str1,str2);
    if(st1) { free(str1);}
    if(st2) { free(str2);}
}

void gen_ifj_readstr(TreeElementPtr tree, TreeElementPtr var) {
    char* dest;
    dest = get_var_name(var);
    printf("READ %s string\n",dest);
    free(dest);
}

void gen_ifj_readi32(TreeElementPtr tree, TreeElementPtr var) {
    char* dest;
    dest = get_var_name(var);
    printf("READ %s int\n",dest);
    free(dest);
}

void gen_ifj_readf64(TreeElementPtr tree, TreeElementPtr var) {
    char* dest;
    dest = get_var_name(var);
    printf("READ %s float\n",dest);
    free(dest);
}

void gen_ifj_i2f(TreeElementPtr tree, TreeElementPtr var) {
    char* converted;
    converted = get_var_name(var);
    if(tree->Node[0]->Data.Token->type == I32_VAR) {
        int i;
        i = tree->Node[0]->Data.Token->value.i;
        printf("INT2FLOAT %s int@%d\n",converted,i);
    }
    else {
        char* str;
        str = get_var_name(tree->Node[0]);
        printf("INT2FLOAT %s %s\n",converted,str);
        free(str);
    }
    free(converted);
}

void gen_ifj_f2i(TreeElementPtr tree, TreeElementPtr var) {
    char* converted;
    converted = get_var_name(var);
    if(tree->Node[0]->Data.Token->type == F64_VAR) {
        double i;
        i = tree->Node[0]->Data.Token->value.f64;
        printf("FLOAT2INT %s float@%a\n",converted,i);
    }
    else {
        char* str;
        str = get_var_name(tree->Node[0]);
        printf("FLOAT2INT %s %s\n",converted,str);
        free(str);
    }
    free(converted);
}

void gen_ifj_ord(TreeElementPtr tree, TreeElementPtr var) {
    char* ascii;
    char* str;
    char* pos;
    bool strb;
    ascii = get_var_name(var);
    if(tree->Node[0]->Data.Token->type == STRING) {
        char *b = "string@";
        char *a = tree->Node[0]->Data.Token->value.str;
        str = malloc(strlen(a)+strlen(b)+1);
        strcpy(str,b);
        strcat(str,a);
        strb = false;
    }
    else {
        str = get_var_name(tree->Node[0]);
        strb = true;
    }
    if(tree->Node[1]->Data.Token->type == I32_VAR) {
        int i;
        i = tree->Node[1]->Data.Token->value.i;
        printf("STRI2INT %s %s int@%d\n",ascii,str,i);
    }
    else {
        char* numb;
        numb = get_var_name(tree->Node[1]);
        printf("STRI2INT %s %s %s\n",ascii,str,numb);
        free(numb);
    }
    free(ascii);
    if(strb) { free(str);}

}

void gen_ifj_strcmp(TreeElementPtr tree, TreeElementPtr var) {
    char *dest;
    char *str1,*str2;
    bool st1, st2;
    if(tree->Node[0]->Data.Token->type == STRING) {
        char *b = "string@";
        char *a = tree->Node[0]->Data.Token->value.str;
        str1 = malloc(strlen(a)+strlen(b)+1);
        strcpy(str1,b);
        strcat(str1,a);
        st1 = false;
    }
    else {
        str1 = get_var_name(tree->Node[0]);
        st1 = true;
    }
    if(tree->Node[1]->Data.Token->type == STRING) {
        char *b = "string@";
        char *a = tree->Node[1]->Data.Token->value.str;
        str2 = malloc(strlen(a)+strlen(b)+1);
        strcpy(str2,b);
        strcat(str2,a);
        st2 = false;
    }
    else {
        str2 = get_var_name(tree->Node[1]);
        st2 = true;
    }
    dest = get_var_name(var);

    printf("DEFVAR GF@CMPRES\n");
    printf("DEFVAR GF@CMPRES\n");
    printf("LT TF@CMPRES %s %s\n",str1,str2);
    printf("JUMPIFEQ s2bigger GF@CMPRES bool@true\n");
    printf("LT TF@CMPRES %s %s\n",str2,str1);
    printf("JUMPIFEQ s1bigger GF@CMPRES bool@true\n");
    printf("JUMP equal\n");

    printf("LABEL s1bigger\n");
    printf("MOVE %s int@1\n",dest);
    printf("JUMP end\n");
    printf("LABEL s2bigger\n");
    printf("MOVE %s int@-1\n",dest);
    printf("JUMP end\n");
    printf("LABEL equal\n");
    printf("MOVE %s int@0\n",dest);
    printf("LABEL end\n");

    if(st1) { free(str1);}
    if(st2) { free(str2);}

    free(dest);
}

void gen_ifj_chr(TreeElementPtr tree, TreeElementPtr var) {
    char* ascii;
    char* str;
    ascii = get_var_name(var);
    if(tree->Node[0]->Data.Token->type == I32_VAR) {
        int i;
        i = tree->Node[0]->Data.Token->value.i;
        printf("INT2CHAR %s int@%d\n",ascii,i);
    }
    else {
        str = get_var_name(tree->Node[0]);
        printf("INT2CHAR %s %s\n",ascii,str);
        free(str);
    }
    free(ascii);

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
