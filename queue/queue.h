/**
 *  @file queue.h
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#ifndef QUEUE_H
#define QUEUE_H

#include "../lexer/token.h"
#include "../symtable/symtable.h"
#include "../tree/tree.h"

typedef TokenPtr *Queue_t;

// Queue struct
typedef struct {
    Queue_t Queue_act;
    int Queue_size;
}Queue;

typedef struct{
    // Token
    TokenPtr SToken;
    // Queue
    Queue *queue;
    // Symtables
    SymTable *glob_table;
    SymTable *local_table;
    SymTable *ifj_table;
    // Current function
    char *current_fn;
    bool returned;
    // Current tree node
    TreeElementPtr current_node;
    // Current stack
    int *level_stack;
    int stack_size;
    // Helper variable for level_stack
    int last_poped;
}TokenStorage, *TokenStoragePtr;

// Adds token to the queue
void queue_add_token(Queue *queue, TokenPtr token);

// Gets token from the queue
TokenPtr queue_next_token(Queue *queue);

// Frees the queue
void queue_free(Queue *queue);

// Creates the queue
Queue *queue_init();

// Function to start the first go through
void queue_fill(TokenStoragePtr stoken, TreeElement *tree_node);

// Function to fill the ifj table
void ifj_table_fill(TokenStoragePtr stoken);

#endif
