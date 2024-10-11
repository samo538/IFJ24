#include"lexer.c"
#include<stdbool.h>

int main() {
	TokenPtr token = next_token();
	while(true) {
		if(token->type < ID) {
			printf("keyword: %s\n", tokenTypeKeywords[token->type]);
		} else if (token->type == ID) {
			printf("id: %s\n", token->value.str);
			free(token->value.str);
		} else if (token->type == I32_VAR) {
			printf("int: %d\n", token->value.i);
		} else if (token->type == F64_VAR) {
			printf("float: %f\n", token->value.f);
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
