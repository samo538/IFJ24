#ifndef __syntax__
#define __syntax__

#include "../lexer/token.h"

typedef struct{
    TokenPtr SToken;
    TokenPtr SStoredToken;
}TokenStorage, *TokenStoragePtr;

#endif
