/**
 *  @file lexer/lexer.h
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 */
#ifndef LEXER_H
#define LEXER_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include"token.h"

extern const char* tokenTypeKeywords[];

TokenPtr next_token();

void choose_type(TokenPtr token, char input);

void number_type(TokenPtr token, char input);

void id_type(TokenPtr token, char input);

void lexer_error();

void realloc_str(char** str, size_t* strSize, size_t length);

void alloc_str(char** str, size_t strSize);

void dealloc_token(TokenPtr token);

void string_type(TokenPtr token, char input);

void multi_line_string_type(TokenPtr token, char input);

#endif
