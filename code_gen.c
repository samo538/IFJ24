/**
 *  @project IFJ24
 *  @file code_gen.c
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 *  @author Lukas Houzar (xhouzal00@stud.fit.vutbr.cz)
 */

#include "code_gen.h"

int strcpy_counter = 0;
int substring_counter = 0;

/*
 * generates code in IFJcode24 from tree passed as argument
 */
void gen_code(TreeElementPtr tree) {
    printf(".IFJcode24\n");
    printf("DEFVAR GF@_\n");

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

/*
 * generates main function that's passed as argument in tree
 */
void gen_main(TreeElementPtr main) {

    //Local Frame
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    gen_func_switch(main, true);

    Fn_id fnId = main->Data.TableElement->FnVar.Fn_id;

    printf("POPFRAME\n");
    printf("EXIT int@0\n");
}

/*
 * selects the appropriate function to generate code according to the function name, caller checks, if is passing ifj function
 */
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
    } else if(strcmp("substring", tree->Data.TableElement->name) == 0) {
        gen_ifj_substring(tree,var);
    }

}

/*
 * calls choose_node_type for every element of a function, used in gen_main and gen_func
 */
void gen_func_switch(TreeElementPtr func, bool isMain) {
    for(int i=0;i < func->NodeCounter;i++) {
        choose_node_type(func->Node[i], isMain);
    }
}

/*
 * selects the appropriate function to generate code according to the node typ
 */
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
            gen_condition(func, isMain);
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

/*
 * generates function that is not main, caller checks, if function passed as argument is main
 */
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

/*
 * puts every definition (DEFVAR) before start of a while, otherwise there would be multiple DEFVAR for a single variable, argument is element of while, changes definition nodes to assign nodes
 */
void put_def_before_while(TreeElementPtr tree) {
    if(tree->Data.NodeType == DEFINITION_NODE) {
        char* name = get_var_name(tree->Node[0]);
        printf("DEFVAR %s\n",name);
        free(name);

        tree->Data.NodeType = ASSIGN_NODE;

    } else if (tree->Data.NodeType == WHILE_NODE || tree->Data.NodeType == IF_NODE) {
        //puts |var| definition before first while
        if(tree->Data.isNullCond && ! tree->Data.isDef) {
            char* newVar = get_var_name(tree->Node[0]);
            printf("DEFVAR %s\n",newVar);
            free(newVar);
            tree->Data.isDef = true;
        }

        for(int i=0;i < tree->NodeCounter;i++) {
            put_def_before_while(tree->Node[i]);
        }
    }
}

/*
 * generates while from tree
 */
void gen_while(TreeElementPtr tree, bool isMain) {
    static unsigned int whileCounter = 0;
    unsigned int currentWhile = whileCounter;
    whileCounter++;

    if(tree->Data.isNullCond && ! tree->Data.isDef) {
        char* newVar = get_var_name(tree->Node[0]);
        printf("DEFVAR %s\n",newVar);
        free(newVar);
        tree->Data.isDef = true;
    }

    //find DEFINITION NODES and change to assign
    for(int i = 2;i < tree->NodeCounter;i++) {
        put_def_before_while(tree->Node[i]);
    }

    printf("LABEL while%d\n",currentWhile);

    if(tree->Data.isNullCond) {
        char* newVar = get_var_name(tree->Node[0]);
        char* oldVar = get_var_name(tree->Node[1]);
        printf("JUMPIFEQ whileend%d nil@nil %s\n",currentWhile,oldVar);
        printf("MOVE %s %s\n",newVar, oldVar);
        free(newVar);
        free(oldVar);
    } else {
        gen_expression(tree->Node[1]->Node[0]);//left
        gen_expression(tree->Node[1]->Node[1]);//right

        switch (tree->Node[1]->Data.Token->type) {
            case LESS: {
                printf("LTS\n");
                printf("PUSHS bool@true\n");
                printf("JUMPIFNEQS whileend%d\n",currentWhile);
                break;
            }
            case MORE: {
                printf("GTS\n");
                printf("PUSHS bool@true\n");
                printf("JUMPIFNEQS whileend%d\n",currentWhile);
                break;
            }
            case LESS_OR_EQUAL: {
                printf("GTS\n"); //neg
                printf("PUSHS bool@true\n");
                printf("JUMPIFEQS whileend%d\n",currentWhile);
                break;
            }
            case MORE_OR_EQUAL: {
                printf("LTS\n"); //neg
                printf("PUSHS bool@true\n");
                printf("JUMPIFEQS whileend%d\n",currentWhile);
                break;
            }
            case EQUAL: {
                printf("JUMPIFNEQS whileend%d\n",currentWhile);
                break;
            }
            case NOT_EQUAL: {
                printf("JUMPIFEQS whileend%d\n",currentWhile);
                break;
            }
        }
    }

    for(int i = 2;i < tree->NodeCounter;i++) {
        choose_node_type(tree->Node[i], isMain);
    }

    printf("JUMP  while%d\n",currentWhile);
    printf("LABEL whileend%d\n",currentWhile);
}

/*
 * generates return from tree, isMain is used to determine, if code should stop execution after return or not
 */
void gen_return(TreeElementPtr tree, bool isMain) {
    if(tree->NodeCounter == 1) {
        gen_expression(tree->Node[0]);
    }

    printf("POPFRAME\n");
    if(isMain) {//if RETURN was used in main -> ERROR
        printf("EXIT int@0\n");
    } else {//if EXIT was used in func -> end of program
        printf("RETURN\n");
    }
}

/*
 * generates expression from tree
 */
void gen_expression(TreeElementPtr tree) {
    for(int i = 0; i < tree->NodeCounter;i++) {
        gen_expression(tree->Node[i]);
    }

    switch (tree->Data.Token->type) {
        case NULL_VALUE: {
            printf("PUSHS nil@nil\n");
            break;
        }
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

/*
 * generates definiton from tree
 */
void gen_definition(TreeElementPtr tree) {
    char* name = get_var_name(tree->Node[0]);
    printf("DEFVAR %s\n",name);
    free(name);

    gen_assign(tree);
}

/*
 * generates assign from tree
 */
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

/*
 * generates function call from tree, push every argument to stack and call function
 */
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

/*
 * generates condition from tree
 */
void gen_condition(TreeElementPtr tree, bool isMain) {
    static unsigned int ifCounter = 0;
    unsigned int currentIf = ifCounter;
    ifCounter++;

    if(tree->Data.isNullCond) {
        char* newVar = get_var_name(tree->Node[0]);
        char* oldVar = get_var_name(tree->Node[1]);
        if( ! tree->Data.isDef) { //if isDef = true, newVar is already defined before, because the if is in while
            printf("DEFVAR %s\n",newVar);
            tree->Data.isDef = false;
        }
        printf("MOVE %s %s\n",newVar, oldVar);
        free(newVar);
        free(oldVar);
    }

    printf("LABEL if%d\n",currentIf);

    if(tree->Data.isNullCond) {
        char* oldVar = get_var_name(tree->Node[1]);
        printf("JUMPIFEQ else%d nil@nil %s\n",currentIf,oldVar);
        free(oldVar);
    } else {
        gen_expression(tree->Node[1]->Node[0]);//left
        gen_expression(tree->Node[1]->Node[1]);//right

        switch (tree->Node[1]->Data.Token->type) {
            case LESS: {
                printf("LTS\n");
                printf("PUSHS bool@true\n");
                printf("JUMPIFNEQS else%d\n",currentIf);
                break;
            }
            case MORE: {
                printf("GTS\n");
                printf("PUSHS bool@true\n");
                printf("JUMPIFNEQS else%d\n",currentIf);
                break;
            }
            case LESS_OR_EQUAL: {
                printf("GTS\n"); //neg
                printf("PUSHS bool@true\n");
                printf("JUMPIFEQS else%d\n",currentIf);
                break;
            }
            case MORE_OR_EQUAL: {
                printf("LTS\n"); //neg
                printf("PUSHS bool@true\n");
                printf("JUMPIFEQS else%d\n",currentIf);
                break;
            }
            case EQUAL: {
                printf("JUMPIFNEQS else%d\n",currentIf);
                break;
            }
            case NOT_EQUAL: {
                printf("JUMPIFEQS else%d\n",currentIf);
                break;
            }
        }
    }

    for(int i = 2;i < tree->NodeCounter;i++) {
        if(tree->Node[i]->Data.NodeType != ELSE_NODE) {
            choose_node_type(tree->Node[i], isMain);

            continue;
        }

        printf("JUMP ifend%d\n",currentIf);
        printf("LABEL else%d\n",currentIf);
    }

    printf("LABEL ifend%d\n",currentIf);

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

    printf("CREATEFRAME\n");
    printf("DEFVAR TF@isnull%d\n",substring_counter);
    printf("LT TF@isnull%d %s int@0\n",substring_counter,i); // i<0
    printf("JUMPIFEQ isnulllabel%d TF@isnull%d bool@true\n",substring_counter,substring_counter);
    printf("LT TF@isnull%d %s int@0\n",substring_counter,j); // j<0
    printf("JUMPIFEQ isnulllabel%d TF@isnull%d bool@true\n",substring_counter,substring_counter);
    printf("GT TF@isnull%d %s %s\n",substring_counter,i,j); // i>j
    printf("JUMPIFEQ isnulllabel%d TF@isnull%d bool@true\n",substring_counter,substring_counter);
    printf("DEFVAR TF@slenght%d\n",substring_counter);

    printf("STRLEN TF@slenght%d %s\n",substring_counter,origString);


    printf("GT TF@isnull%d %s TF@slenght%d\n",substring_counter,j,substring_counter); // j>strlen
    printf("JUMPIFEQ isnulllabel%d TF@isnull%d bool@true\n",substring_counter,substring_counter);
    printf("LT TF@isnull%d %s TF@slenght%d\n",substring_counter,i,substring_counter); // i>=strlen jumpni    i<strlen nejumpuj
    printf("JUMPIFEQ isnulllabel%d TF@isnull%d bool@false\n",substring_counter,substring_counter);


    printf("DEFVAR TF@substringI%d\n",substring_counter);
    printf("DEFVAR TF@substringJ%d\n",substring_counter);
    printf("MOVE TF@substringI%d %s\n",substring_counter,i);
    printf("MOVE TF@substringJ%d %s\n",substring_counter,j);
    printf("ADD TF@substringJ%d TF@substringJ%d int@1\n",substring_counter,substring_counter);


    printf("DEFVAR TF@tempstring%d\n",substring_counter);
    printf("DEFVAR TF@char%d\n",substring_counter);
    printf("MOVE TF@tempstring%d string@\n",substring_counter);

    printf("LABEL substringlabel%d\n",substring_counter);
    printf("GETCHAR TF@char%d %s TF@substringI%d\n",substring_counter,origString,substring_counter);
    printf("CONCAT TF@tempstring%d TF@tempstring%d TF@char%d \n",substring_counter,substring_counter,substring_counter);
    printf("ADD TF@substringI%d TF@substringI%d int@1\n",substring_counter,substring_counter);
    printf("JUMPIFNEQ substringlabel%d TF@substringI%d TF@substringJ%d\n",substring_counter,substring_counter,substring_counter);

    printf("MOVE %s TF@tempstring%d\n",finalString,substring_counter);


    printf("JUMP subend%d\n",substring_counter);
    printf("LABEL isnulllabel%d\n",substring_counter);
    printf("MOVE %s nil@nil\n",finalString);
    printf("LABEL subend%d\n",substring_counter++);

    if(origb) { free(origString);}
    if(ib) { free(i);}
    if(jb) { free(j);}

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

    printf("CREATEFRAME\n");
    printf("DEFVAR TF@CMPRES%d\n",strcpy_counter);
    printf("LT TF@CMPRES%d %s %s\n",strcpy_counter,str1,str2);
    printf("JUMPIFEQ s2bigger%d TF@CMPRES%d bool@true\n",strcpy_counter,strcpy_counter);
    printf("LT TF@CMPRES%d %s %s\n",strcpy_counter,str2,str1);
    printf("JUMPIFEQ s1bigger%d TF@CMPRES%d bool@true\n",strcpy_counter,strcpy_counter);
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

/*
 * returns pointer to variable name from tree
 */
char* get_var_name(TreeElementPtr tree) {
    char* name = (char*)malloc(sizeof(char)*256);
    if (name == NULL) {
		    throw_error(NULL,99);
    }

    if(tree->Data.TableElement == NULL && tree->Data.Token->type == UNDERSCORE) { //checks for _ var
        strcpy(name,"GF@_");

        return name;
    }

    strcpy(name,"LF@var");
    //concats all layers into variable name
    for(int i=0;i < tree->Data.TableElement->stack_size;i++) {
        char layer[10];
        sprintf(layer,"%d",tree->Data.TableElement->level_stack[i]);
        strcat(name,layer);
    }
    strcat(name,"_");
    strcat(name,tree->Data.TableElement->name);

    return name;
}

/*
 * returns pointer to variable name, same function as get_var_name, but the argument is Elem_id* instead of TreeElementPtr and does not get var name for _
 */
char* get_var_name_from_table(Elem_id* tableElement) { //used only when generation func parametrs -> no need for _
    char* name = (char*)malloc(sizeof(char)*256);
    if (name == NULL) {
		    throw_error(NULL,99);
    }
    strcpy(name,"LF@var");
    for(int i=0;i < tableElement->stack_size;i++) {
        char layer[10];
        sprintf(layer,"%d",tableElement->level_stack[i]);
        strcat(name,layer);
    }
    strcat(name,"_");
    strcat(name,tableElement->name);

    return name;
}
