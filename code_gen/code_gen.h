//
// Created by veve on 8.11.24.
//

#ifndef IFJ24_CODE_GEN_H
#define IFJ24_CODE_GEN_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../tree/tree.h"

void gen_code(TreeElementPtr tree);
void gen_main(TreeElementPtr main);
void gen_func(TreeElementPtr func);
void gen_definition(TreeElementPtr tree);
void choose_ifj_func(TreeElementPtr tree);
void gen_assign(TreeElementPtr tree);
void gen_expression(TreeElementPtr tree);
void gen_while(TreeElementPtr tree);
void gen_condition(TreeElementPtr tree);
void gen_ifj_write(TreeElementPtr tree);
char* get_var_name(TreeElementPtr tree);

#endif //IFJ24_CODE_GEN_H
