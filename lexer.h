/**
 *  @project IFJ24
 *  @file lexer.h
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 *  @author Lukas Houzar (xhouzal00@stud.fit.vutbr.cz)
 *  Header file for lexer
 */
#ifndef LEXER_H
#define LEXER_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include"token.h"

extern const char* tokenTypeKeywords[];

/*
 * returns pointer to next token from the input
 */
TokenPtr next_token();

/*
 * creates a copy of old_token and returns pointer to copy
 */
Token *copy_token(Token *old_token);

/*
 * choose type of token
 */
void choose_type(TokenPtr token, char input);

/*
 * generate a token of type I32/F64 from input
 */
void number_type(TokenPtr token, char input);

/*
 * generate a token of type ID from input
 */
void id_type(TokenPtr token, char input);

void lexer_error();

/*
 * checks if strSize is sufficient and if it's not, allocates 20 more chars
 */
void realloc_str(char** str, size_t* strSize, size_t length);

/*
 * allocates char array of size strSize
 */
void alloc_str(char** str, size_t strSize);

void dealloc_token(TokenPtr token);

void string_type(TokenPtr token, char input);

void multi_line_string_type(TokenPtr token, char input);

#endif
