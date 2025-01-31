/**
 *  @project IFJ24
 *  @file token.h
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 *  Definition of token
 */
#ifndef TOKEN_H
#define TOKEN_H

#include<stdio.h>

enum TokenType {
	PLUS, //+
	MINUS, //-
	MULTIPLY, //*
	DIVIDE, ///
	OPENING_BRACKET, //(
	CLOSING_BRACKET, //)
	ID,
	EQUAL, //==
	NOT_EQUAL, //!=
	LESS, //<
	MORE, //>
	LESS_OR_EQUAL, //<=
	MORE_OR_EQUAL, //>=
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
	ASSIGN, //=
	NULLABLE, //?
	I32_VAR,
	F64_VAR,
	DOT, //.
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
	double f64; //float is f32, double is f64
	char* str;
};

typedef struct {
	enum TokenType type; //type of token
	union TokenValue value; //value of token, if token has any value
} Token, *TokenPtr;

#endif
