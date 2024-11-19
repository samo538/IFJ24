/**
 *  @project IFJ24
 *  @file code_gen.h
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 *  @author Lukas Houzar (xhouzal00@stud.fit.vutbr.cz)
 */

#ifndef IFJ24_CODE_GEN_H
#define IFJ24_CODE_GEN_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"

void gen_code(TreeElementPtr tree);
void gen_main(TreeElementPtr main);
void gen_func(TreeElementPtr func);
void gen_func_switch(TreeElementPtr func, bool isMain);
void gen_func_call(TreeElementPtr tree);
void gen_while(TreeElementPtr tree, bool isMain);
void choose_node_type(TreeElementPtr func, bool isMain);
void gen_return(TreeElementPtr tree, bool isMain);
void gen_definition(TreeElementPtr tree);
void choose_ifj_func(TreeElementPtr tree, TreeElementPtr var);
void gen_assign(TreeElementPtr tree);
void gen_expression(TreeElementPtr tree);
void gen_condition(TreeElementPtr tree, bool isMain);
void gen_ifj_write(TreeElementPtr tree);
void gen_ifj_length(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_string(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_concat(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_readstr(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_readi32(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_readf64(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_f2i(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_i2f(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_chr(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_ord(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_strcmp(TreeElementPtr tree, TreeElementPtr var);
void gen_ifj_substring(TreeElementPtr tree, TreeElementPtr var);
char* get_var_name(TreeElementPtr tree);
char* get_var_name_from_table(Elem_id* TableElement);
void put_def_before_while(TreeElementPtr tree);

#endif //IFJ24_CODE_GEN_H
