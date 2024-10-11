/**
 *  @file lexer/lexer.c
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 */

#include"lexer.h"

void lexer_error() { //TODO: vyměnit za real error
	printf("ERROR: lexer");
}

TokenPtr next_token() {
	TokenPtr token = (TokenPtr)malloc(sizeof(Token));
	token->type = COUNT_TOKEN_TYPE; //placeholder, jediný token type, než nemůže být na vstupu
	while(token->type == COUNT_TOKEN_TYPE) { //dokud se nebybere jiný token type
		choose_type(token, getchar());
	}
	return token;
}

void choose_type(TokenPtr token, char input) {
	switch(input) { //TODO: komplikovanější typy
		case '=': {
			char c = getchar();
			if(c == '=') {
				token->type = EQUAL;
				return;
			}

			ungetc(c, stdin);
			token->type = ASSIGN;
			return;
		}
		case '<': {
			char c = getchar();
			if(c == '=') {
				token->type = LESS_OR_EQUAL;
				return;
			}

			ungetc(c, stdin);
			token->type = LESS;
			return;
		}
		case '>': {
			char c = getchar();
			if(c == '=') {
				token->type = MORE_OR_EQUAL;
				return;
			}

			ungetc(c, stdin);
			token->type = MORE;
			return;
		}
		case '+': {
			token->type = PLUS;
			return;
		}
		case '-': {
			token->type = MINUS;
			return;
		}
		case '*': {
			token->type = MULTIPLY;
			return;
		}
		case '/': {
			token->type = DIVIDE;
			return;
		}
		case '?': {
			token->type = NULLABLE;
			return;
		}
		case '.': {
			token->type = DOT;
			return;
		}
		case '(': {
			token->type = OPENING_BRACKET;
			return;
		}
		case ')': {
			token->type = CLOSING_BRACKET;
			return;
		}
		case '{': {
			token->type = OPENING_CURLY_BRACKET;
			return;
		}
		case '}': {
			token->type = CLOSING_CURLY_BRACKET;
			return;
		}
		case '|': {
			token->type = VERTICAL_BAR;
			return;
		}
		case ';': {
			token->type = SEMICOLON;
			return;
		}
		case ':': {
			token->type = COLON;
			return;
		}
		case ',': {
			token->type = COMMA;
			return;
		}
		case '_': {
			char c = getchar();
			if(('a' <= input && input <= 'z') || ('A' <= input && input <= 'Z') || ('0' <= input && input <= '9')) {
				ungetc(c, stdin);
				id_type(token, input);
				return;
			}

			ungetc(c, stdin);
			token->type = UNDERSCORE;
			return;
		}
		case EOF: {
			token->type = END_OF_FILE;
			return;
		}
	}

	if('0' <= input && input <= '9') {
		number_type(token, input);
	}

	if(('a' <= input && input <= 'z') || ('A' <= input && input <= 'Z')) {
		id_type(token, input);
	}
}

void number_type(TokenPtr token, char firstChar) {//TODO
	token->type = I32_VAR;
	token->value = (void*)9;
}

void id_type(TokenPtr token, char firstChar) {//TODO
	token->type = ID;
}
