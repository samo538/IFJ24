#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <stddef.h>
#include <stdint.h>

//Functions returning int8_t return either of these values
#define H_TAB_OK 1
#define H_TAB_ERR -1

typedef struct item{
    char *key;
    uint64_t value;
    struct item *next;
}h_item_t;

typedef struct {
    size_t used; //no of entries
    size_t table_size; //no of indexes
    h_item_t **table; //pointer to the tables
}h_table_t;

// This function MUST return H_TAB_OK, else the for_each fucntion is terminated
typedef int8_t(*h_table_fn_t)(h_item_t *h_item,void *ctx);

size_t
h_tab_hash_function(const char *key);

//Creates the table and returns a pointer to it
h_table_t *
h_tab_create(size_t size);

//Returns the size of the table (no of indexes)
size_t
h_tab_size(h_table_t *h_table);

//Returns no of entries in the tableo
size_t
h_tab_used(h_table_t *h_table);

//Finds and returns a pointer to an table item
h_item_t *
h_tab_find(h_table_t *h_table ,char *key);

//Adds a new entry into the table
int8_t
h_tab_add(h_table_t *h_table, char *key, uint64_t value);

//Erases a entry in the table 
int8_t
h_tab_erase(h_table_t *h_table, char *key);

//Start a function at every entry
int8_t
h_tab_for_each(h_table_t *h_table, h_table_fn_t fn, void *ctx);

//Clears the entire table (It erases every entry)
void
h_tab_clear(h_table_t *h_table);

//Destroys the table, the pointer is invalid after this operation
void
h_tab_destroy(h_table_t *h_table);

#endif
