#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

static h_item_t *
create_item(char *key, uint64_t value);
static int8_t
add_item(h_item_t **item, char *key, uint64_t value);
static int8_t
remove_item(h_item_t **previous_item, h_item_t *item);

size_t
h_tab_hash_function(const char *key){
    uint32_t h=0;     // musí mít 32 bitů
    const unsigned char *p;
    for(p=(const unsigned char*)key; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}


h_table_t *
h_tab_create(size_t size){
    h_table_t *new_table = malloc(sizeof(h_table_t));

    if (new_table == NULL){
        return NULL;
    }
    new_table->table_size = size;
    new_table->used = 0;
    new_table->table = malloc(sizeof(h_item_t *) * size);
    if (new_table == NULL){
        return NULL;
    }

    for(int i = 0; i < size; i++){ //TODO try to substitute for some memxxx command
        new_table->table[i] = NULL;
    }

    return new_table;
}

size_t
h_tab_size(h_table_t *h_table){
    return h_table->table_size;
}

size_t
h_tab_used(h_table_t *h_table){
    return h_table->used;
}

h_item_t *
h_tab_find(h_table_t *h_table, char *key){
    size_t index = h_tab_hash_function(key) % h_table->table_size; 
    h_item_t *ptr = h_table->table[index];

    int cmp;

    while(ptr != NULL){ 
        cmp = strncmp(ptr->key, key, strlen(key));
        if (cmp == 0){
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

int8_t
h_tab_add(h_table_t *h_table, char *key, uint64_t value){

    size_t index = h_tab_hash_function(key) % h_table->table_size; 
    h_item_t *ptr = h_table->table[index];
    h_item_t **saved_ptr = &h_table->table[index];

    int cmp;
    int8_t ret;

    while(ptr != NULL){ 
        cmp = strncmp(ptr->key, key, strlen(key));
        if (cmp == 0){
            ptr->value = value;
            return H_TAB_OK;
        }
        saved_ptr = &ptr->next;
        ptr = ptr->next;
    }

    ret = add_item(saved_ptr,key, value);
    h_table->used++;
    return ret;
}

int8_t
h_tab_erase(h_table_t *h_table, char *key){
    size_t index = h_tab_hash_function(key) % h_table->table_size; 
    h_item_t *ptr = h_table->table[index];
    h_item_t **saved_ptr = &h_table->table[index];

    int cmp;
    int8_t ret;

    while(ptr != NULL){ 
        cmp = strncmp(ptr->key, key, strlen(key));
        if (cmp == 0){
            ret = remove_item(saved_ptr, ptr);
            h_table->used--;
            return H_TAB_OK;
        }
        saved_ptr = &ptr->next;
        ptr = ptr->next;
    }

    return H_TAB_ERR;
}

int8_t
h_tab_for_each(h_table_t *h_table, h_table_fn_t fn, void *ctx){
    int8_t fn_ret;
    h_item_t *item;

    for (int i = 0; i < h_table->table_size; i++){
        item = h_table->table[i];
        while(item != NULL){
            fn_ret = fn(item, ctx);
            if (fn_ret != H_TAB_OK){
                return H_TAB_ERR;
            }
            item = item->next;
        }
    }

    return H_TAB_OK;
}

void
h_tab_clear(h_table_t *h_table){
    h_item_t *item;
    h_item_t *item_free;

    for (int i = 0; i < h_table->table_size; i++){
        item = h_table->table[i];
        h_table->table[i] = NULL;
        while(item != NULL){
            item_free = item;
            item = item->next;
            free(item_free->key);
            free(item_free);
        }
    }
}

void
h_tab_destroy(h_table_t *h_table){
    h_item_t * item;
    h_item_t * item_free;
    h_table->used = 0;

    for (int i = 0; i < h_table->table_size; i++){
        item = h_table->table[i];
        while(item != NULL){
            item_free = item;
            item = item->next;
            free(item_free->key);
            free(item_free);
        }
    }
    free(h_table->table);
    free(h_table);
}

static h_item_t *
create_item(char *key, uint64_t value){
    h_item_t *ptr;
    ptr = malloc(sizeof(h_item_t));
    if (ptr == NULL){
        return NULL;
    }
    ptr->key = strdup(key);

    ptr->value = value;
    ptr->next = NULL;
    return ptr;
}

static int8_t
add_item(h_item_t **item, char *key, uint64_t value){
    h_item_t *new_item = create_item(key, value);
    if (new_item == NULL){
        return H_TAB_ERR;
    }

    *item = new_item; 
    return H_TAB_OK; 
}

static int8_t
remove_item(h_item_t **previous_item, h_item_t *item){
    *previous_item = item->next;
    free(item->key);
    free(item);
    return H_TAB_OK;
}
