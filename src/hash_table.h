// Created by Jagermeister on 8/19/2018.
// https://github.com/neharkarvishal/hash_tables_in_C/blob/master/STRUCTURE.md

#ifndef HASH_TABLES_IN_C_HAST_TABLE_H
#define HASH_TABLES_IN_C_HAST_TABLE_H

#include <stdlib.h>
#include <memory.h>
#include <math.h>
//#include <xmalloc.h>

// Hash table structure:

/* Our key-value pairs (items) will each be stored in a struct ht_item */
typedef struct
{
    char* key;
    char* value;
} ht_item;

/* Our hash table stores an array of pointers to item,
 * and some details about its size and how full it is. */
typedef struct
{
    int size;
    int count;
    ht_item** items;
} ht_hash_table;

static ht_item* ht_new_item(const char* k, const char* v);
ht_hash_table* ht_new();
static ht_hash_table* ht_new_sized(const int base_size);
static void ht_del_item(ht_item* item);
void ht_del_hash_table(ht_hash_table* hashtable);
static int ht_hash(const char* s, const int a, const int m);
static int ht_get_hash(const char* s, const int num_buckets, const int attempt);

void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* h, const char* key);
static void ht_resize(ht_hash_table* ht, const int base_size);
static void ht_resize_up(ht_hash_table* ht);
static void ht_resize_down(ht_hash_table* ht);
#endif //HASH_TABLES_IN_C_HAST_TABLE_H