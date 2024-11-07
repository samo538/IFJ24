/**
 *  @file precedence/precedence.h
 *  @author Mario Klopan (xklopam00@stud.fit.vutbr.cz)
 */

#ifndef PRECEDENCE_H
#define PRECEDENCE_H

#include <stdbool.h>

#include "../tree/tree.h"
#include "../lexer/token.h"

TreeElementPtr preced_analysis(TokenPtr first_token, TokenPtr second_token, bool);

#endif  