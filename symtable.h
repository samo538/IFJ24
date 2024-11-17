/**
 *  @file symtable.h
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include "token.h"

// Static size, because the assingment said so
#define TABLE_SIZE 1001

struct elem_id;

typedef struct{ // Helper struct
    enum TokenType type;
    bool nullable;
} Id_type;

typedef enum type{
    FUNCTION,
    VARIABLE
}Type;

typedef struct { // Data for variables
    Id_type type;
    bool const_t;
    bool used;
}Var_id;

typedef struct fn_id{ // Data for funtions
    Id_type return_type; // Return type
    Id_type *type_of_params; // Type of the parameters
    int num_of_params; // Number of parameters
    struct elem_id **TableParams; // Pointer to local variables that repressent the arguments
    struct elem_id **LocalSymTable; // Pointer to the whole table
}Fn_id;

union FnVar{
    Fn_id Fn_id;
    Var_id Var_id;
};

/*
 * Universal element in the table
 * Double name/level_stack are used as a key in the hash table
 */
typedef struct elem_id{
    char *name; // Name of the var/fn
    int *level_stack; // Stack representing the position in code
    int stack_size; // Size of the stack
    Type Type; // Specifing the type (var/fn)
    union FnVar FnVar; // Data specific to fn or var
}Elem_id;

typedef Elem_id *SymTable;

// Copies level1 into level2 of size level_size
bool copy_levels(int *level1, int **level2, int level_size);

// Creates a table and returnes it
SymTable *TableInit();

// Searches the table for a specific entry ! Searches every stack level !
Elem_id *TableSearch(char *key, int *level_stack, int level_size, SymTable *Table);

// Adds a new table entry, the user is expected to fill it after adding
Elem_id *TableAdd(char *key, int *level_stack, int level_size, SymTable *Table);

// If VarFn is VARIABLE -> local table is freed
// If VarFn is FUNCTION -> Global table with all local tables is freed
void TableClear(SymTable *Table, Type VarFn);

#endif
