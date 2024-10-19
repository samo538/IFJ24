/*
* File: syntax.c
* Project: IFJ24
* Authors: Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
*/

#include <stdlib.h>
#include <stdbool.h>
#include "syntax.h"
#include "../lexer/token.h"

#include <stdio.h>

bool start(){
    if (){

    }
    else{
        return false;
    }
}

int main(){
    bool result;
    TokenStoragePtr stoken = malloc(sizeof(TokenStorage));
    if (stoken == NULL){
        return 99;
    }

    stoken->SToken = 

    result = start(stoken);

    printf("%d", result); //debug output
}

