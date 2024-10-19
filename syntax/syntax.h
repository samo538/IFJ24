#ifndef SYNTAX_H
#define SYNTAX_H

#include "../lexer/token.h"

typedef struct{
    TokenPtr SToken;
    TokenPtr SStoredToken;
}TokenStorage, *TokenStoragePtr;

#endif
