/**
 *  @project IFJ24
 *  @file code_gen.h
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 *  @author Lukas Houzar (xhouzal00@stud.fit.vutbr.cz)
 *  Header file for code generation, for every function, the caller is the one who checks, if the arguments passed are valid
 */

#ifndef IFJ24_CODE_GEN_H
#define IFJ24_CODE_GEN_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "error.h"

/*
 * generates code in IFJcode24 from tree passed as argument
 */
void gen_code(TreeElementPtr tree);
/*
 * generates main function that's passed as argument in tree
 */
void gen_main(TreeElementPtr main);
/*
 * generates function that is not main, caller checks, if function passed as argument is main
 */
void gen_func(TreeElementPtr func);
/*
 * calls choose_node_type for every element of a function, used in gen_main and gen_func
 */
void gen_func_switch(TreeElementPtr func, bool isMain);
/*
 * generates function call from tree, push every argument to stack and call function
 */
void gen_func_call(TreeElementPtr tree);
/*
 * generates while from tree
 */
void gen_while(TreeElementPtr tree, bool isMain);
/*
 * selects the appropriate function to generate code according to the node typ
 */
void choose_node_type(TreeElementPtr func, bool isMain);
/*
 * generates return from tree, isMain is used to determine, if code should stop execution after return or not
 */
void gen_return(TreeElementPtr tree, bool isMain);
/*
 * generates definiton from tree
 */
void gen_definition(TreeElementPtr tree);
/*
 * selects the appropriate function to generate code according to the function name, caller checks, if is passing ifj function
 */
void choose_ifj_func(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates assign from tree
 */
void gen_assign(TreeElementPtr tree);
/*
 * generates expression from tree
 */
void gen_expression(TreeElementPtr tree);
/*
 * generates condition from tree
 */
void gen_condition(TreeElementPtr tree, bool isMain);
/*
 * generates ifj.write function
 */
void gen_ifj_write(TreeElementPtr tree);
/*
 * generates ifj.leght function
 */
void gen_ifj_length(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.string function
 */
void gen_ifj_string(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.concat function
 */
void gen_ifj_concat(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.readstr function
 */
void gen_ifj_readstr(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.readi32 function
 */
void gen_ifj_readi32(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.readf64 function
 */
void gen_ifj_readf64(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.f2i function
 */
void gen_ifj_f2i(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.i2f function
 */
void gen_ifj_i2f(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.chr function
 */
void gen_ifj_chr(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.ord function
 */
void gen_ifj_ord(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.strcmp function
 */
void gen_ifj_strcmp(TreeElementPtr tree, TreeElementPtr var);
/*
 * generates ifj.substring function
 */
void gen_ifj_substring(TreeElementPtr tree, TreeElementPtr var);
/*
 * returns pointer to variable name from tree
 */
char* get_var_name(TreeElementPtr tree);
/*
 * returns pointer to variable name, same function as get_var_name, but the argument is Elem_id* instead of TreeElementPtr and does not get var name for _
 */
char* get_var_name_from_table(Elem_id* TableElement);
/*
 * puts every definition (DEFVAR) before start of a while, otherwise there would be multiple DEFVAR for a single variable, argument is element of while, changes definition nodes to assign nodes
 */
void put_def_before_while(TreeElementPtr tree);

#endif //IFJ24_CODE_GEN_H
