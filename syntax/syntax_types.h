#include <stdbool.h>
#include "../queue/queue.h"
#include "../lexer/token.h"

bool h_check_ifj_fn(char *fn_name);

bool t_ifj(TokenStoragePtr stoken);
bool t_at_import(TokenStoragePtr stoken);
bool t_eq(TokenStoragePtr stoken);
bool t_comma(TokenStoragePtr stoken);
bool t_colon(TokenStoragePtr stoken);
bool t_op_bracket(TokenStoragePtr stoken);
bool t_cl_bracket(TokenStoragePtr stoken);
bool t_op_cr_bracket(TokenStoragePtr stoken);
bool t_cl_cr_bracket(TokenStoragePtr stoken);
bool t_op_sq_bracket(TokenStoragePtr stoken);
bool t_cl_sq_bracket(TokenStoragePtr stoken);
bool t_string_prolog(TokenStoragePtr stoken);
bool t_semicolon(TokenStoragePtr stoken);
bool t_pub(TokenStoragePtr stoken);
bool t_fn(TokenStoragePtr stoken);
bool t_dot(TokenStoragePtr stoken);
bool t_return(TokenStoragePtr stoken);
bool t_if(TokenStoragePtr stoken);
bool t_else(TokenStoragePtr stoken);
bool t_vertical_bar(TokenStoragePtr stoken);
bool t_while(TokenStoragePtr stoken);
bool t_eof(TokenStoragePtr stoken);
bool t_const(TokenStoragePtr stoken);
bool t_var(TokenStoragePtr stoken);
bool t_void(TokenStoragePtr stoken);
bool t_u8(TokenStoragePtr stoken);
bool t_type_keyword(TokenStoragePtr stoken);
bool t_string(TokenStoragePtr stoken);
bool t_int(TokenStoragePtr stoken);
bool t_float(TokenStoragePtr stoken);
bool t_null(TokenStoragePtr stoken);
bool t_id(TokenStoragePtr stoken);
bool t_id_ifj(TokenStoragePtr stoken);
bool t_underline(TokenStoragePtr stoken);
bool t_nullable(TokenStoragePtr stoken);
