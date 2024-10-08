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
	VARIABLE,
	VOID,
	WHILE,
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
	DEVIDE, ///
	NULLABLE, //?
	I32_VAR,
	F64_VAR,
	DOT, //.
	OPENING_BRACKET, //(
	CLOSING_BRACKET, //)
	OPENING_CURLY_BRACKET, //{
	CLOSING_CURLY_BRACKET, //}
	VERTICAL_BAR, //|
	SEMICOLON, //;
	COLON, //:
	COMMA, //,
	IMPORT, //@import
	STRING, //[]u8
	END_OF_FILE,
	COUNT_TOKEN_TYPE,
};

typedef struct {
	enum TokenType type;
	void* value;
} Token, *TokenPtr;

const char* tokenTypeKeywords[]= {
	"const",
	"if",
	"else",
	"fn",
	"i32",
	"f64",
	"null",
	"pub",
	"return",
	"var",
	"void",
	"while",
};

#endif
