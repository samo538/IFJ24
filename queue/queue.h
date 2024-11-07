#ifndef QUEUE_H
#define QUEUE_H

#include "../lexer/token.h"
#include "../symtable/symtable.h"

typedef TokenPtr *Queue_t;

typedef struct {
    Queue_t Queue_act;
    int Queue_size;
}Queue;

typedef struct{
    TokenPtr SToken;
    TokenPtr SStoredToken;
    Queue *queue;
    SymTable *glob_table;
    SymTable *local_table;
    char *current_fn;
    int *level_stack;
    int stack_size;
    Elem_id *tmp;
}TokenStorage, *TokenStoragePtr;

void queue_add_token(Queue *queue, TokenPtr token);

TokenPtr queue_next_token(Queue *queue);

void queue_free(Queue *queue);

Queue *queue_init();

void queue_fill(TokenStoragePtr stoken);

#endif
