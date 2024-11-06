/*
*   file: Symtable
*   Author: Samuel Luptak
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "symtable.h"

bool compare_levels(int *level1, int *level2, int level_size){
    for (int i = 0; i < level_size; i++){
        if(level1[i] != level2[i]){
            return false;
        }
    }
    return true;
}

int HashFn(char *key, int* level,int level_size){
    int len = strlen(key);
    int hash = 0;
    for (int i = 0; i < len; i++){
        hash += key[i];
    }
    for (int i = 0; i < level_size; i++){
        hash += level[i];
    }
    return hash % TABLE_SIZE;
}

SymTable *TableInit(){
    Elem_id **SymTable = malloc(sizeof(Elem_id) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++){
        SymTable[i] = NULL;
    }
    return SymTable;
}

Elem_id *TableSearch(char *key, int *level, int level_size, SymTable *Table){
    int index = HashFn(key, level, level_size);
    int searched = 0;
    while (Table[index] != NULL && (strcmp(key, Table[index]->name) || level_size == Table[index]->stack_size && !compare_levels(level, Table[index]->level_stack, level_size))){
        if (searched == TABLE_SIZE){ // Table is full
            return NULL;
        }
        index = (index + 1) % TABLE_SIZE;
        searched++;
    }
    return Table[index];
}

bool TableAdd(Elem_id Elem,SymTable *Table){
    int index = HashFn(Elem.name, Elem.level_stack, Elem.stack_size);
    int searched = 0;
    while (Table[index] != NULL){
        if (searched == TABLE_SIZE){ // Table is full
            return false;
        }
        if(!strcmp(Elem.name, Table[index]->name) && Elem.stack_size == Table[index]->stack_size && compare_levels(Elem.level_stack, Table[index]->level_stack, Elem.stack_size)){ // Elem alredy in the table
            Table[index]->Type = Elem.Type;
            if (Table[index]->Type == VARIABLE){
                Table[index]->FnVar = Elem.FnVar;
            }
            else {
                Elem_id **tmp = Table[index]->FnVar.Fn_id.LocalSymTable;
                Table[index]->FnVar = Elem.FnVar;
                if (Elem.FnVar.Fn_id.LocalSymTable == NULL){ // If LocalTable is set to NULL, save the initialized Table
                    Table[index]->FnVar.Fn_id.LocalSymTable = tmp;
                }
            }
            return true;
        }

        index = (index + 1) % TABLE_SIZE;
        searched++;

    }
    Elem_id *IdAdd = malloc(sizeof(Elem_id));
    IdAdd->name = strdup(Elem.name);
    IdAdd->level_stack = Elem.level_stack;
    IdAdd->stack_size = Elem.stack_size;
    IdAdd->Type = Elem.Type;
    IdAdd->FnVar = Elem.FnVar;
    Table[index] = IdAdd;
    return true;
}

void TableClear(SymTable *Table, Type VarFn){
    if (Table == NULL){
        return;
    }
    if (VarFn == FUNCTION){
        for (int i = 0; i < TABLE_SIZE; i++){
            if(Table[i] != NULL){
                free(Table[i]->name);
                free(Table[i]->FnVar.Fn_id.type_of_params);
                TableClear(Table[i]->FnVar.Fn_id.LocalSymTable, VARIABLE);
                free(Table[i]);
            }
        }
        free(Table);
    }
    if (VarFn == VARIABLE){
        for (int i = 0; i < TABLE_SIZE; i++){
            if(Table[i] != NULL){
                free(Table[i]->name);
                free(Table[i]->level_stack);
                free(Table[i]);
            }
        }
        free(Table);
    }
}






















