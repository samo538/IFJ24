/**
 *  @file symtable.c
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "symtable.h"
#include "../errors/error.h"

bool copy_levels(int *level1, int **level2, int level_size){
    if (level1 == NULL){
        return NULL;
    }
    *level2 = malloc(sizeof(int) * level_size);
    if (*level2 == NULL){
        return false;
    }
    for (int i = 0; i < level_size; i++){
        (*level2)[i] = level1[i];
    }
    return true;
}

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
    Elem_id **SymTable = malloc(sizeof(Elem_id *) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++){
        SymTable[i] = NULL;
    }
    return SymTable;
}

Elem_id *TableSearch(char *key, int *level_stack, int level_size, SymTable *Table){
    int index = HashFn(key, level_stack, level_size);
    int searched = 0;
    // Searches until not found, name must match and level_stack must match
    while (!(Table[index] != NULL && !strcmp(key, Table[index]->name) && level_size == Table[index]->stack_size && compare_levels(level_stack, Table[index]->level_stack, Table[index]->stack_size))){
        if (searched == TABLE_SIZE && level_size == 0){ // Table is full
            return NULL;
        }
        if (searched == TABLE_SIZE){ // If not found on this level, try one level bellow (This tries all the levels)
            return TableSearch(key, level_stack, level_size - 1, Table);
        }
        index = (index + 1) % TABLE_SIZE;
        searched++;
    }
    return Table[index];
}

Elem_id *TableAdd(char *key, int *level_stack, int stack_size, SymTable *Table){
    int index = HashFn(key, level_stack, stack_size);
    int searched = 0;
    while (Table[index] != NULL){
        if (searched == TABLE_SIZE){ // Table is full
            throw_error(99);
        }
        if(!strcmp(key, Table[index]->name) && stack_size == Table[index]->stack_size && compare_levels(level_stack, Table[index]->level_stack, stack_size)){
            // If the element is already in the table, Its always a redefinition of a variable/func
            throw_error(5);
        }
        index = (index + 1) % TABLE_SIZE;
        searched++;
    }

    Elem_id *IdAdd = malloc(sizeof(Elem_id));
    Table[index] = IdAdd;
    return Table[index]; // Returns a pointer to the new table entry, expecting the user to fill it
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
                free(Table[i]->FnVar.Fn_id.TableParams);
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






















