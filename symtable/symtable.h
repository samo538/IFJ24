#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include "../lexer/token.h"

#define TABLE_SIZE 1001

struct elem_id;

typedef struct{
    enum TokenType type;
    bool nullable;
} Id_type;

typedef enum type{
    FUNCTION,
    VARIABLE
}Type;

typedef struct {
    Id_type type;
    bool const_t;
}Var_id;

typedef struct fn_id{
    Id_type return_type;
    int num_of_params;
    Id_type *type_of_params;
    struct elem_id **LocalSymTable;
}Fn_id;

union FnVar{
    Fn_id Fn_id;
    Var_id Var_id;
};

typedef struct elem_id{
    char *name;
    int *level_stack;
    int stack_size;
    Type Type;
    union FnVar FnVar;
}Elem_id;

typedef Elem_id *SymTable;

bool copy_levels(int *level1, int **level2, int level_size);
SymTable *TableInit();
Elem_id *TableSearch(char *key, int *level, int level_size, SymTable *Table);
bool TableAdd(Elem_id Elem,SymTable *Table);
void TableClear(SymTable *Table, Type VarFn);

#endif
