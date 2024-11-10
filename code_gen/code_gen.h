//
// Created by veve on 8.11.24.
//

#ifndef IFJ24_CODE_GEN_H
#define IFJ24_CODE_GEN_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../tree/tree.h"

void generate_code(TreeElementPtr tree);
void generate_function(TreeElementPtr tree);
void generate_expression(TreeElementPtr tree);

#endif //IFJ24_CODE_GEN_H
