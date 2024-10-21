/**
 *  @file lexer/token.h
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 *  @brief definice struktury tokenu
 */
#ifndef TOKEN_H
#define TOKEN_H

#include<stdio.h>

enum TokenType {
	CONST,
	IF,
	ELSE,
	FN,
	I32,
	F64,
	NULL_VALUE,
	PUB,
	RETURN,
	VAR,
	VOID,
	WHILE,
	IFJ,
	U8,
	ID,
	EQUAL, //==
	NOT_EQUAL, //!=
	ASSIGN, //=
	LESS, //<
	LESS_OR_EQUAL, //<=
	MORE, //>
	MORE_OR_EQUAL, //>=
	PLUS, //+
	MINUS, //-
	MULTIPLY, //*
	DIVIDE, ///
	NULLABLE, //?
	I32_VAR,
	F64_VAR,
	DOT, //.
	OPENING_BRACKET, //(
	CLOSING_BRACKET, //)
	OPENING_CURLY_BRACKET, //{
	CLOSING_CURLY_BRACKET, //}
	OPENING_SQUARE_BRACKET, //[
	CLOSING_SQUARE_BRACKET, //]
	VERTICAL_BAR, //|
	SEMICOLON, //;
	COLON, //:
	COMMA, //,
	IMPORT, //@import
	STRING,
	UNDERSCORE, //_
	END_OF_FILE,
	COUNT_TOKEN_TYPE,
};

union TokenValue{
	int i;
	double f64; //klasicky float je f32, double f64
	char* str;
};

typedef struct {
	enum TokenType type;
	union TokenValue value;
} Token, *TokenPtr;

#endif
