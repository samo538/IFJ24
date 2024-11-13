#include "../queue/queue.h"
#include "../lexer/token.h"

bool h_check_ifj_fn(char *fn_name);

bool t_expect(TokenStoragePtr stoken, enum TokenType type);
bool t_string_prolog(TokenStoragePtr stoken);
bool t_id_ifj(TokenStoragePtr stoken);
bool t_type_keyword(TokenStoragePtr stoken, Elem_id *new_elem);
bool t_id_var(TokenStoragePtr stoken, Elem_id *new_elem);
bool t_id_fn(TokenStoragePtr stoken);
bool t_nullable_var(TokenStoragePtr stoken, Elem_id *new_elem);
