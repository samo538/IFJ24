#ifndef SYNTAX_H
#define SYNTAX_H

#include "../lexer/token.h"
#include "../queue/queue.h"


void syn_error(TokenStoragePtr stoken);

bool ifj_call(TokenStoragePtr stoken);
bool fn_body(TokenStoragePtr stoken);

#endif
