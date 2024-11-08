/**
 *  @file precedence/precedence.h
 *  @author Mario Klopan (xklopam00@stud.fit.vutbr.cz)
 */

#ifndef PRECEDENCE_H
#define PRECEDENCE_H

#include <stdbool.h>

#include "../tree/tree.h"
#include "../lexer/token.h"
#include "../symtable/symtable.h"
#include "../queue/queue.h"

typedef struct PrecResult
{
    TreeElementPtr Tree;
    TokenPtr NextTotken;
    int Error;
}PrecResult, *PrecResultPtr;


PrecResultPtr preced_analysis(TokenPtr first_token, bool rel_op, int *level, int level_size, SymTable *Table, Queue *queue);

#endif  