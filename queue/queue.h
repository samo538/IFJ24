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
    // Tokens
    TokenPtr SToken;
    TokenPtr SStoredToken;
    // Queue
    Queue *queue;
    // Symtables
    SymTable *glob_table;
    SymTable *local_table;
    SymTable *ifj_table;
    // Current function
    char *current_fn;
    bool returned;
    // Current stack
    int *level_stack;
    int stack_size;
    // Helper variable for level_stack
    int last_poped;
}TokenStorage, *TokenStoragePtr;

void queue_add_token(Queue *queue, TokenPtr token);

TokenPtr queue_next_token(Queue *queue);

void queue_free(Queue *queue);

Queue *queue_init();

void queue_fill(TokenStoragePtr stoken);

void ifj_table_fill(TokenStoragePtr stoken);

#endif
