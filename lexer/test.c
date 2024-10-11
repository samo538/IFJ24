#include"lexer.c"
#include<stdbool.h>

int main() {
	TokenPtr token = next_token();
	while(true) {
		if(token->type < ID) {
			printf("%s\n", tokenTypeKeywords[token->type]);
		} else if (token->type == I32_VAR) {
			printf("int: %d\n", (long)token->value);
		} else {
			printf("%d\n", token->type);
		}

		if(token->type == END_OF_FILE){break;}
		free(token);
		token = next_token();
	}
	free(token);
	return 0;
}
