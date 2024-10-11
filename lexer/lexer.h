/**
 *  @file lexer/lexer.h
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 */
#ifndef LEXER_H
#define LEXER_H

#include<stdlib.h>
#include<stdio.h>
#include"token.h"

TokenPtr next_token();

void choose_type(TokenPtr token, char input);

void number_type(TokenPtr token, char firstChar);

void id_type(TokenPtr token, char firstChar);

void lexer_error();

#endif
