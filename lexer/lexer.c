/**
 *  @file lexer/lexer.c
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 */

#include"lexer.h"

void lexer_error() { //TODO: vyměnit za real error
	printf("ERROR: lexer\n");
}

void realloc_str(char** str, size_t* strSize, size_t length) {
	if(length == *strSize) {
		*strSize += 20;
		*str = (char*)realloc(*str, sizeof(char) * (*strSize));
		if(str == NULL) {//TODO: pořešit alloc error
			printf("chyba při realokaci paměti");
		}
	}
}

void alloc_str(char** str, size_t strSize) {
	*str = (char*)malloc(sizeof(char) * strSize); //string o velikosti strSize znaků
	if(*str == NULL) {//TODO: pořešit alloc error
		printf("chyba při alokaci paměti");

		return;
	}
}

TokenPtr next_token() {
	TokenPtr token = (TokenPtr)malloc(sizeof(Token));
	token->type = COUNT_TOKEN_TYPE; //placeholder, jediný token type, jenž nemůže být na vstupu
	while(token->type == COUNT_TOKEN_TYPE) { //dokud se nevybere jiný token type
		choose_type(token, getchar());
	}
	return token;
}

void choose_type(TokenPtr token, char input) {
	switch(input) { //TODO: string, []u8
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
		case '[': {
			token->type = OPENING_SQUARE_BRACKET;
			return;
		}
		case ']': {
			token->type = CLOSING_SQUARE_BRACKET;
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
			if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')) {
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
		case '0': {
			char c = getchar();
			if('0' <= c && c <= '9') {
				lexer_error();

				return;
			} else if (c == 'e' || c == 'E' || c == '.') {
				ungetc(c, stdin);
				number_type(token, input);
				return;
			}

			ungetc(c, stdin);
			token->type = I32_VAR;
			token->value.i = 0;
			return;
		}
	}

	if('1' <= input && input <= '9') {
		number_type(token, input);
	}

	if(('a' <= input && input <= 'z') || ('A' <= input && input <= 'Z')) {
		id_type(token, input);
	}
}

void number_type(TokenPtr token, char input) {
	bool isI32 = true;
	size_t strSize = 20;
	alloc_str(&token->value.str, strSize);
	token->value.str[0] = input;
	size_t length = 1;
	input = getchar();
	while(('0' <= input && input <= '9') || ((input == 'e' || input == 'E' || input == '.') && isI32)) { //pokud I32 = false, už bylo e načteno
		token->value.str[length] = input;
		length++;
		realloc_str(&token->value.str, &strSize, length);

		if(input == '.') {
			isI32 = false;
		} else if (input == 'e' || input == 'E') {
			isI32 = false;
			input = getchar();
			if(input == '+' || input == '-') {
				token->value.str[length] = input;
				length++;
				realloc_str(&token->value.str, &strSize, length);
			} else { //pokud se neprovede podmínka, musí cyklus začít znovu, jinak by se vynechal 1 znak
				continue;
			}
		}

		input = getchar();
	}
	token->value.str[length] = '\0'; //ukončení str
	ungetc(input, stdin);

	if(isI32) {
		int temp = atoi(token->value.str);
		free(token->value.str);
		token->value.i = temp;
		token->type = I32_VAR;

		return;
	}

	double temp = atof(token->value.str);
	free(token->value.str);
	token->value.f64 = temp;
	token->type = F64_VAR;
}

void id_type(TokenPtr token, char input) {
	size_t strSize = 20;
	alloc_str(&token->value.str, strSize);
	token->value.str[0] = input;
	size_t idLength = 1;
	input = getchar();
	while(('a' <= input && input <= 'z') || ('A' <= input && input <= 'Z') || ('0' <= input && input <= '9')) {
		token->value.str[idLength] = input;
		idLength++;
		realloc_str(&token->value.str, &strSize, idLength);
		
		input = getchar();
	}
	token->value.str[idLength] = '\0'; //ukončení str
	ungetc(input, stdin);

	for(int i=0;i < ID;i++) { //tokenType ID je hned po posledním keywordu, proto by pole keywords mělo mít velikost ID
		if(strcmp(tokenTypeKeywords[i], token->value.str) == 0) {
			token->type = i;
			free(token->value.str);

			return;
		}
	}

	token->type = ID;
}
