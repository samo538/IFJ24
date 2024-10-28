#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>

#define TABLE_SIZE 5

struct elem_id;

typedef struct {
    int data;
}Var_id;

typedef enum type{
    FUNCTION,
    VARIABLE
}Type;

typedef struct fn_id{
    int data;
    struct elem_id **LocalSymTable;
}Fn_id;

union FnVar{
    Fn_id Fn_id;
    Var_id Var_id;
};

typedef struct elem_id{
    char *name;
    int level;
    Type Type;
    union FnVar FnVar;
}Elem_id;



Elem_id **TableInit();
Elem_id *TableSearch(char *key, int level, Elem_id **Table);
bool TableAdd(Elem_id Elem,Elem_id **Table);
void TableClear(Elem_id **Table, Type VarFn);

#endif
