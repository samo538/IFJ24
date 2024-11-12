//
// Created by veve on 8.11.24.
//
#include "code_gen.h"

unsigned int ifCounter = 0;
unsigned int whileCounter = 0;
int strcpy_counter = 0;
int substring_counter = 0;

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

    gen_func_switch(main, true);

    Fn_id fnId = main->Data.TableElement->FnVar.Fn_id;

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

void gen_func_switch(TreeElementPtr func, bool isMain) {
    for(int i=0;i < func->NodeCounter;i++) {
        choose_node_type(func->Node[i], isMain);
    }
}

void choose_node_type(TreeElementPtr func, bool isMain) {
    switch(func->Data.NodeType) {
        case ASSIGN_NODE: {
            gen_assign(func);
            break;
        }
        case DEFINITION_NODE: {
            gen_definition(func);
            break;
        }
        case RETURN_NODE: {
            gen_return(func, isMain);
            break;
        }
        case WHILE_NODE: {
            gen_while(func, isMain);
            break;
        }
        case IF_NODE: {
            break;
        }
        case FUNCTION_NODE: {
            gen_func_call(func);
            break;
        }
        case IFJ_FUNCTION_NODE: {
            choose_ifj_func(func, NULL);
        }
    }
}

void gen_func(TreeElementPtr func) {
    printf("LABEL func_%s\n",func->Data.TableElement->name);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    Fn_id fnId = func->Data.TableElement->FnVar.Fn_id;

    for(int i=0;i < fnId.num_of_params;i++) {
        char* name = get_var_name_from_table(fnId.TableParams[i]);
        printf("DEFVAR %s\n",name);
        printf("POPS %s\n",name);
        free(name);
    }

    gen_func_switch(func, false);

    if(fnId.return_type.type == VOID) {
        printf("POPFRAME\n");
        printf("RETURN\n");
    }
}

void gen_while(TreeElementPtr tree, bool isMain) {
    unsigned int currentWhile = whileCounter;
    whileCounter++;

    printf("LABEL while%d\n",currentWhile);

    for(int i = 2;i < tree->NodeCounter;i++) {
        choose_node_type(tree->Node[i], isMain);
    }

    printf("JUMP  while%d\n",currentWhile);
    printf("LABEL whileend%d\n",currentWhile);
}

void gen_return(TreeElementPtr tree, bool isMain) {
    if(tree->NodeCounter == 1) {
        gen_expression(tree->Node[0]);
    }

    printf("POPFRAME\n");
    if(isMain) {
        printf("EXIT int@0\n");
    } else {
        printf("RETURN\n");
    }
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
        case ID: { //variable
            char* name = get_var_name(tree);
            printf("PUSHS %s\n",name);
            free(name);
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

    printf("CALL func_%s\n",tree->Data.TableElement->name);
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

void gen_ifj_substring(TreeElementPtr tree, TreeElementPtr var) {
    char* finalString, *origString, *i, *j;
    bool origb,ib,jb;
    finalString = get_var_name(var);
    if(tree->Node[0]->Data.Token->type == STRING) {
        char *b = "string@";
        char *a = tree->Node[0]->Data.Token->value.str;
        origString = malloc(strlen(a)+strlen(b)+1);
        strcpy(origString,b);
        strcat(origString,a);
        origb = false;
    }
    else {
        origString = get_var_name(tree->Node[0]);
        origb = true;
    }
    if(tree->Node[1]->Data.Token->type == I32_VAR) {
        char *b = "int@";
        char a[30];
        sprintf(a, "%d", tree->Node[1]->Data.Token->value.i);
        i = malloc(strlen(a)+strlen(b)+1);
        strcpy(i,b);
        strcat(i,a);
        ib = false;
    }
    else {
        i = get_var_name(tree->Node[1]);
        ib = true;
    }
    if(tree->Node[2]->Data.Token->type == I32_VAR) {
        char *b = "int@";
        char a[30];
        sprintf(a, "%d", tree->Node[2]->Data.Token->value.i);
        j = malloc(strlen(a)+strlen(b)+1);
        strcpy(j,b);
        strcat(j,a);
        jb = false;
    }
    else {
        j = get_var_name(tree->Node[2]);
        jb = true;
    }


    printf("DEFVAR GF@isnull%d\n",substring_counter);
    printf("LT GF@isnull%d %s int@0\n",substring_counter,i); // i<0
    printf("JUMPIFEQ isnulllabel%d GF@isnull%d bool@true\n",substring_counter,substring_counter);
    printf("LT GF@isnull%d %s int@0\n",substring_counter,j); // j<0
    printf("JUMPIFEQ isnulllabel%d GF@isnull%d bool@true\n",substring_counter,substring_counter);
    printf("GT GF@isnull%d %s %s\n",substring_counter,i,j); // i>j
    printf("JUMPIFEQ isnulllabel%d GF@isnull%d bool@true\n",substring_counter,substring_counter);
    printf("DEFVAR GF@slenght%d\n",substring_counter);

    printf("STRLEN GF@slenght%d,%s\n",substring_counter,origString);
    printf("LT GF@isnull%d %s int@0\n",substring_counter,i); // j>strlen
    printf("JUMPIFEQ isnulllabel%d GF@isnull%d bool@true\n",substring_counter,substring_counter);

    printf("MOVE %s string@will\\032be\\032substringed\n",finalString);

    printf("JUMP subend%d\n",substring_counter);
    printf("LABEL isnulllabel%d\n",substring_counter);
    printf("MOVE %s nil@nil\n",finalString);
    printf("LABEL subend%d\n",substring_counter++);

    if(origb) { free(origString);}
    if(ib) { free(i);}
    if(jb) { free(j);}
    //returning string
    //paramas string,i,j

    //substring
    //set cnrt to i
    //while i<j
    //getchar tempstring oldsting i
    //concat substring tempstring
    //• 𝑖 ≥ ifj.length(𝑠)
    //• 𝑗 > ifj.length(𝑠)
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

    printf("DEFVAR GF@CMPRES%d\n",strcpy_counter);
    printf("LT GF@CMPRES%d %s %s\n",strcpy_counter,str1,str2);
    printf("JUMPIFEQ s2bigger%d GF@CMPRES%d bool@true\n",strcpy_counter,strcpy_counter);
    printf("LT GF@CMPRES%d %s %s\n",strcpy_counter,str2,str1);
    printf("JUMPIFEQ s1bigger%d GF@CMPRES%d bool@true\n",strcpy_counter,strcpy_counter);
    printf("JUMP equal%d\n",strcpy_counter);

    printf("LABEL s1bigger%d\n",strcpy_counter);
    printf("MOVE %s int@1\n",dest);
    printf("JUMP strcmpend%d\n",strcpy_counter);
    printf("LABEL s2bigger%d\n",strcpy_counter);
    printf("MOVE %s int@-1\n",dest);
    printf("JUMP strcmpend%d\n",strcpy_counter);
    printf("LABEL equal%d\n",strcpy_counter);
    printf("MOVE %s int@0\n",dest);
    printf("LABEL strcmpend%d\n",strcpy_counter++);

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
    strcpy(name,"LF@var_");
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
    strcpy(name,"LF@var_");
    strcat(name,tableElement->name);
    for(int i=0;i < tableElement->stack_size;i++) {
        char layer[10];
        sprintf(layer,"%d",tableElement->level_stack[i]);
        strcat(name,layer);
    }

    return name;
}
