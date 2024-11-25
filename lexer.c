/**
 *  @project IFJ24
 *  @file lexer.c
 *  @author Petr Nemec (xnemecp00@stud.fit.vutbr.cz)
 *  @author Lukas Houzar (xhouzal00@stud.fit.vutbr.cz)
 */
#include <string.h>

#include"lexer.h"
#include "token.h"
#include "error.h"

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
	"ifj",
	"u8",
};

void lexer_error() {
	throw_error(NULL,1);
}

/*
 * creates a copy of old_token and returns pointer to copy
 */
Token *copy_token(Token *old_token){
	Token *new_token;
	new_token = malloc(sizeof(Token));
	if (new_token == NULL){
		throw_error(NULL,99);
	}
	new_token->type = old_token->type;
	new_token->value = old_token->value;
	if (new_token->type == STRING || new_token->type == ID){
		new_token->value.str = strdup(old_token->value.str);
	}
	return new_token;
}

/*
 * checks if strSize is sifficient and if it's not, allocates 20 more chars
 */
void realloc_str(char** str, size_t* strSize, size_t length) {
	while(length >= *strSize) {
		*strSize += 20;
		*str = (char*)realloc(*str, sizeof(char) * (*strSize));
		if(str == NULL) {
			throw_error(NULL,99);
		}
	}
}

/*
 * allocates char array of size strSize
 */
void alloc_str(char** str, size_t strSize) {
	*str = (char*)malloc(sizeof(char) * strSize); //Alloc char array of strSize
	if(*str == NULL) {
		throw_error(NULL,99);
	}
}

void dealloc_token(TokenPtr token) {
	if(token->type == STRING || token->type == ID) {
		free(token->value.str);
	}
	free(token);
}

/*
 * returns pointer to next token from the input
 */
TokenPtr next_token() {
	TokenPtr token = (TokenPtr)malloc(sizeof(Token));
	token->type = COUNT_TOKEN_TYPE; //placeholder
	while(token->type == COUNT_TOKEN_TYPE) { //until token type is changed
		choose_type(token, getchar());
	}
	return token;
}

/*
 * choose type of token
 */
void choose_type(TokenPtr token, char input) {
	static bool prevId = false;
	static bool inBracket = false;
	static enum TokenType prevToken;
	switch(input) {
		case '=': {
			char c = getchar();
			if(c == '=') {
				token->type = EQUAL;
				break;
			}

			ungetc(c, stdin);
			token->type = ASSIGN;
			break;
		}
		case '!': {
			char c = getchar();
			if(c == '=') {
				token->type = NOT_EQUAL;
				break;
			}

			lexer_error();
		}
		case '<': {
			char c = getchar();
			if(c == '=') {
				token->type = LESS_OR_EQUAL;
				break;
			}

			ungetc(c, stdin);
			token->type = LESS;
			break;
		}
		case '>': {
			char c = getchar();
			if(c == '=') {
				token->type = MORE_OR_EQUAL;
				break;
			}

			ungetc(c, stdin);
			token->type = MORE;
			break;
		}
		case '+': {
			token->type = PLUS;
			break;
		}
		case '-': {
			token->type = MINUS;
			break;
		}
		case '*': {
			token->type = MULTIPLY;
			break;
		}
		case '/': {
			char c = getchar();
			if(c == '/') {
				//comment, skips everything until there is a new line of EOF in input
				do {
					c = getchar();
				} while(c != '\n' && c != EOF);

				if (c == EOF) {
					token->type = END_OF_FILE;
				}

				//if last char loaded was EOF, next_token will return token, otherwise this function is called again by next_token
				return;

			}

			ungetc(c, stdin);
			token->type = DIVIDE;
			break;
		}
		case '?': {
			token->type = NULLABLE;
			break;
		}
		case '.': {
			token->type = DOT;
			break;
		}
		case '(': {
			token->type = OPENING_BRACKET;
			if(prevId) {
				inBracket = true;
			}
			break;
		}
		case ')': {
			if (prevToken == OPENING_BRACKET) {
				inBracket = false;
			}

			if(inBracket && prevToken != COMMA) { //inserts ,
				ungetc(input, stdin);
				token->type = COMMA;
				inBracket = false;
				break;
			}

			token->type = CLOSING_BRACKET;
			break;
		}
		case '{': {
			token->type = OPENING_CURLY_BRACKET;
			break;
		}
		case '}': {
			token->type = CLOSING_CURLY_BRACKET;
			break;
		}
		case '[': {
			token->type = OPENING_SQUARE_BRACKET;
			break;
		}
		case ']': {
			token->type = CLOSING_SQUARE_BRACKET;
			break;
		}
		case '|': {
			token->type = VERTICAL_BAR;
			break;
		}
		case ';': {
			token->type = SEMICOLON;
			break;
		}
		case ':': {
			token->type = COLON;
			break;
		}
		case ',': {
			token->type = COMMA;
			break;
		}
		case '_': {
			char c = getchar();
			//check if this token is id
			if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_') {
				ungetc(c, stdin);
				id_type(token, input);
				break;
			}

			ungetc(c, stdin);
			token->type = UNDERSCORE;
			break;
		}
		case EOF: {
			token->type = END_OF_FILE;
			break;
		}
		case '0': {
			char c = getchar();
			//I32 cannot start with 0, if it is not 0, F64 cannot start with 0, if it is not 0 or 0.xxx or 0exxx
			if('0' <= c && c <= '9') {

				lexer_error();
			} else if (c == 'e' || c == 'E' || c == '.') {
				ungetc(c, stdin);
				number_type(token, input);
				break;
			}

			ungetc(c, stdin);
			token->type = I32_VAR;
			token->value.i = 0;
			break;
		}
		case '@': {
			char str[7];
			scanf("%6s",str);
			if (strcmp("import", str) == 0) {
				token->type = IMPORT;
				prevId = false;
				prevToken = token->type;
				break;
			}

			lexer_error();
		}
		case '"': {
      string_type(token, input);
      break;
    }
    case '\\': {
			char c = getchar();
			if(c != '\\') {
				lexer_error();
			}

      multi_line_string_type(token,input);
      break;
    }
	}

	if('1' <= input && input <= '9') {
		number_type(token, input);
	}

	if(('a' <= input && input <= 'z') || ('A' <= input && input <= 'Z')) {
		id_type(token, input);
		if(token->type == ID) {
			prevId = true;
			prevToken = token->type;

			return;
		}
	}
	
	//check if type was changed
	if (token->type != COUNT_TOKEN_TYPE) {
		prevId = false;
		prevToken = token->type;

		return;
	}

	//checks if input is something other than whitespace
	if (input != ' ' && input != '\t' && input != '\n' && input != 13 && input != '\v' && input != 12) {//13 = carriage return, 12 = form feed, \v = vertical tab
		lexer_error();
	}
}

/*
 * generate a token of type I32/F64 from input
 */
void number_type(TokenPtr token, char input) {
	bool isI32 = true;
	bool isEXP = true;
	bool isSecondPartEmpty = true;
	size_t strSize = 20;
	alloc_str(&token->value.str, strSize);
	token->value.str[0] = input;
	size_t length = 1;
	input = getchar();
	while(('0' <= input && input <= '9') || ((input == 'e' || input == 'E') && isEXP) || (input == '.' && isI32)) { //if I32 = false, e is already in token->value.str
		if( ! isI32) {
			isSecondPartEmpty = false;
		}

		token->value.str[length] = input;
		length++;
		realloc_str(&token->value.str, &strSize, length);

		if(input == '.') {
			isI32 = false;
		} else if (input == 'e' || input == 'E') {
			isI32 = false;
			isEXP = false;
			input = getchar();
			if(input == '+' || input == '-') {
				token->value.str[length] = input;
				length++;
				realloc_str(&token->value.str, &strSize, length);
			} else { //if the condition is not met, the loop must start again, otherwise 1 character would not be loaded
				continue;
			}
		}

		input = getchar();
	}
	token->value.str[length] = '\0'; //end of string
	ungetc(input, stdin);

	if(isI32) {
		int temp = atoi(token->value.str);
		free(token->value.str);
		token->value.i = temp;
		token->type = I32_VAR;

		return;
	}

	if(isSecondPartEmpty) {//number is float
		lexer_error();
	}

	double temp = atof(token->value.str);
	free(token->value.str);
	token->value.f64 = temp;
	token->type = F64_VAR;
}

void multi_line_string_type(TokenPtr token, char input) {
    size_t strSize = 20;
    alloc_str(&token->value.str, strSize);
    input = getchar();
    token->value.str[0] = input;
    size_t length = 1;

    do {
        while ('\n' != input) {
            input = getchar();
            if ('\n' == input) { break; }
        	if(0 < (int)input && (int)input <= 9) {
    				length+=4;
    				realloc_str(&token->value.str, &strSize, length);
        		token->value.str[length-4] = '\\';
						token->value.str[length-3] = '0';
						token->value.str[length-2] = '0';
						token->value.str[length-1] = (int)input;
        	}
        	else if((9 < (int)input && (int)input <= 32) || (int)input == 92 || (int)input == 35) {
    				length+=4;
    				realloc_str(&token->value.str, &strSize, length);
        		token->value.str[length-4] = '\\';
						token->value.str[length-3] = '0';
						token->value.str[length-2] = ((int)input /10)+48;
						token->value.str[length-1] = ((int)input %10)+48;
        	}
        	else {
        		token->value.str[length] = input;
        		length++;
        		realloc_str(&token->value.str, &strSize, length);
        	}
        }

        do {
            input = getchar();
        } while (input == ' ' || input == '\t');

        if('\\' == input) {
            token->value.str[length] = '\\';
            length++;
			realloc_str(&token->value.str, &strSize, length);
			token->value.str[length] = '0';
            length++;
			realloc_str(&token->value.str, &strSize, length);
			token->value.str[length] = '1';
            length++;
			realloc_str(&token->value.str, &strSize, length);
			token->value.str[length] = '0';
            length++;
			realloc_str(&token->value.str, &strSize, length);
            input = getchar();
        }
        else {ungetc(input, stdin);}
    }
    while ('\\' == input);
    token->value.str[length] = '\0';
    token->type = STRING;
}


void string_type(TokenPtr token, char input) {
    size_t strSize = 20;
    alloc_str(&token->value.str, strSize);
    input = getchar();
    size_t length = 0;
    if(input == '"') {
       token->value.str[length] = '\0';
       token->type = STRING;
       return;
    }
    else {
        ungetc(input,stdin);
    }

    while(true) {
        input = getchar();
        if('"' == input) { break;}
        if('\n' == input) {lexer_error();}
        if('\\' == input) {
       	char c = getchar();
        switch(c) {
          	case 'n': {
          			length+=4;
          			realloc_str(&token->value.str, &strSize, length);
          			token->value.str[length-4] = '\\';
                token->value.str[length-3] = '0';
                token->value.str[length-2] = '1';
                token->value.str[length-1] = '0';
								continue;
                  break;}
        	case 'r': {
          			length+=4;
          			realloc_str(&token->value.str, &strSize, length);
                  	token->value.str[length-4] = '\\';
                    token->value.str[length-3] = '0';
                    token->value.str[length-2] = '1';
                    token->value.str[length-1] = '3';
					continue;
                  break;}
        	case 't': {
          			length+=4;
          			realloc_str(&token->value.str, &strSize, length);
                  	token->value.str[length-4] = '\\';
                    token->value.str[length-3] = '0';
                    token->value.str[length-2] = '0';
                    token->value.str[length-1] = '9';
					continue;
                  break;}
        	case '\\': {
          			length+=4;
          			realloc_str(&token->value.str, &strSize, length);
                  	token->value.str[length-4] = '\\';
                    token->value.str[length-3] = '0';
                    token->value.str[length-2] = '9';
                    token->value.str[length-1] = '2';
					continue;
                  break;}
        	case '\'': {
                  	token->value.str[length] = '\'';
          			length+=1;
          			realloc_str(&token->value.str, &strSize, length);
					continue;
                  break;}
        	case '\"': {
                  	token->value.str[length] = '\"';
          			length+=1;
          			realloc_str(&token->value.str, &strSize, length);
					continue;
                  break;}
        	case 'x': {
                  	char c1 = getchar();
                    char c2 = getchar();

                    char highHex = 0;
    				char lowHex = 0;


    				if (c1 >= '0' && c1 <= '9') {
       					highHex = c1 - '0';
    				} else if (c1 >= 'A' && c1 <= 'F') {
        				highHex = c1 - 'A' + 10;
    				} else if (c1 >= 'a' && c1 <= 'f') {
        				highHex = c1 - 'a' + 10;
    				} else {
       				 lexer_error();
    				}


    				if (c2 >= '0' && c2 <= '9') {
        				lowHex = c2 - '0'; // Convert '0'-'9' to 0-9
    				} else if (c2 >= 'A' && c2 <= 'F') {
        				lowHex = c2 - 'A' + 10; // Convert 'A'-'F' to 10-15
    				} else if (c2 >= 'a' && c2 <= 'f') {
        				lowHex = c2 - 'a' + 10; // Convert 'a'-'f' to 10-15
    				} else {
         				lexer_error();
   					}

					char r = (highHex << 4) | lowHex;
                  	token->value.str[length] = r;
          			length+=1;
          			realloc_str(&token->value.str, &strSize, length);
					continue;
                  break;}
            default: {ungetc(c, stdin); lexer_error();}
        	}
		}

    	if(0 < (int)input && (int)input <= 9) {
    		length+=4;
    		realloc_str(&token->value.str, &strSize, length);
    		token->value.str[length-4] = '\\';
			token->value.str[length-3] = '0';
			token->value.str[length-2] = '0';
			token->value.str[length-1] = (int)input;
    	}
    	else if((9 < (int)input && (int)input <= 32) || (int)input == 92 || (int)input == 35) {
    		length+=4;
    		realloc_str(&token->value.str, &strSize, length);
			token->value.str[length-4] = '\\';
			token->value.str[length-3] = '0';
			token->value.str[length-2] = ((int)input /10)+48;
			token->value.str[length-1] = ((int)input %10)+48;
    	}
        else {
        	token->value.str[length] = input;
        	length++;
        	realloc_str(&token->value.str, &strSize, length);
        }

    }
    token->value.str[length] = '\0';
    token->type = STRING;
}


/*
 * generate a token of type ID from input
 */
void id_type(TokenPtr token, char input) {
	size_t strSize = 20;
	alloc_str(&token->value.str, strSize);
	token->value.str[0] = input;
	size_t idLength = 1;
	input = getchar();
	//until there is a char, than cannot be in id name
	while(('a' <= input && input <= 'z') || ('A' <= input && input <= 'Z') || ('0' <= input && input <= '9') || input == '_') {
		token->value.str[idLength] = input;
		idLength++;
		realloc_str(&token->value.str, &strSize, idLength);
		
		input = getchar();
	}
	token->value.str[idLength] = '\0'; //end of string
	ungetc(input, stdin);

	for(int i=CONST;i <= U8;i++) { //tokenType U8 is last keyword, that is why array keywords should be of size U8 - CONST
		if(strcmp(tokenTypeKeywords[i - CONST], token->value.str) == 0) {
			token->type = i;
			free(token->value.str);

			return;
		}
	}

	token->type = ID;
}
