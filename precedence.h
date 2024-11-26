/**
 *  @project IFJ24
 *  @file precedence.h
 *  @author Mario Klopan (xklopam00@stud.fit.vutbr.cz)
 */

#ifndef PRECEDENCE_H
#define PRECEDENCE_H

#include <stdbool.h>

#include "tree.h"
#include "token.h"
#include "symtable.h"
#include "queue.h"

typedef struct PrecResult
{
    TreeElementPtr Tree;
    TokenPtr NextTotken;
    int Error;
}PrecResult, *PrecResultPtr;

//main function for expression parser
PrecResultPtr preced_analysis(TokenPtr first_token, bool rel_op, int *level, int level_size, SymTable *Table, Queue *queue);

#endif  
