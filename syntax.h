/**
 *  @file syntax.h
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */

#ifndef SYNTAX_H
#define SYNTAX_H

#include "token.h"
#include "queue.h"


void syn_error(TokenStoragePtr stoken);

bool ifj_call(TokenStoragePtr stoken);
bool fn_body(TokenStoragePtr stoken);
bool call_params(TokenStoragePtr stoken, Elem_id *fn, int pos);
bool fn_call(TokenStoragePtr stoken);

#endif
