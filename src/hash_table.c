// Created by Jagermeister on 8/19/2018.
/* Initialising and deleting
 * We need to define initialisation functions for ht_items.
 * This function allocates a chunk of memory the size of an ht_item,
 * and saves a copy of the strings k and v in the new chunk of memory.
 * The function is marked as static because it will only ever be called
 * by code internal to the hash table. */
#include "hash_table.h"
#include "prime.h"

int HT_INITIAL_BASE_SIZE = 151;
int HT_PRIME_1 = 151;
int HT_PRIME_2 = 163;
static ht_item HT_DELETED_ITEM = {NULL, NULL};

static ht_item* ht_new_item(const char* k, const char* v)
{
    ht_item* item  =   malloc( sizeof(ht_item) );

    item->key = strdup(k);
    item->value = strdup(v);

    return item;
}

/* ht_new initialises a new hash table.
 * size defines how many items we can store.
 * This is fixed at 53 for now.
 * We'll expand this in the section on resizing.
 * We initialise the array of items with calloc,
 * which fills the allocated memory with NULL bytes.
 * A NULL entry in the array indicates that the bucket is empty. */

ht_hash_table* ht_new()
{
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}
static ht_hash_table* ht_new_sized(const int base_size)
{
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht->size = base_size;

    ht->size = next_prime(ht->size);

    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

/*We also need functions for deleting ht_items and ht_hash_tables,
 * which free the memory we've allocated, so we don't cause memory leaks. */

static void ht_del_item(ht_item* item)
{
    free(item->key);
    free(item->value);
    free(item);
}
void ht_del_hash_table(ht_hash_table* hashtable)
{
    for (int i = 0; i < hashtable->size ; ++i)
    {
        ht_item* currentItem = hashtable->items[i];    //iterating through each item in table

        if(currentItem != NULL )
            ht_del_item(currentItem);                  //freeing currentItem if not null
    }

    free(hashtable->items);
    free(hashtable);
}
/*
Hash function
In this section, we'll write our hash function.

The hash function we choose should:

Take a string as its input and return a number between 0 and m, our desired bucket array length.
Return an even distribution of bucket indexes for an average set of inputs.
If our hash function is unevenly distributed, it will put more items in some buckets than others.
This will lead to a higher rate of [collisions](#Pathological data).
Collisions reduce the efficiency of our hash table.
Algorithm:
We'll make use of a generic string hashing function, expressed below in pseudocode.

function hash(string, a, num_buckets):
    hash = 0
    string_len = length(string)
    for i = 0, 1, ..., string_len:
        hash += (a ** (string_len - (i+1))) * char_code(string[i])
    hash = hash % num_buckets
    return hash
This hash function has two steps:

Convert the string to a large integer
Reduce the size of the integer to a fixed range by taking its remainder mod m
The variable a should be a prime number larger than the size of the alphabet.
We're hashing ASCII strings, which has an alphabet size of 128,
so we should choose a prime larger than that.

char_code is a function which returns an integer which represents the character.
We'll use ASCII character codes for this.

Let's try the hash function out:

hash("cat", 151, 53)

hash = (151**2 * 99 + 151**1 * 97 + 151**0 * 116) % 53
hash = (2257299 + 14647 + 116) % 53
hash = (2272062) % 53
hash = 5
Changing the value of a give us a different hash function.

hash("cat", 163, 53) = 3
*/
static int ht_hash(const char* s, const int a, const int m)
{
    long hash = 0;
    const int len_s = strlen(s);

    for (int i = 0 ; i < len_s ; i++)
    {
        hash += (long)pow( a, len_s - (i+1) ) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}
static int ht_get_hash(const char* s, const int num_buckets, const int attempt)
{
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}
void ht_insert(ht_hash_table* ht, const char* key, const char* value)
{
    ht_item* item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    const int load = ht->count * 100 / ht->size;
    if (load > 70)
    {
        ht_resize_up(ht);
    }
    while (cur_item != NULL)
    {
        if (cur_item != &HT_DELETED_ITEM)
        {
            if (strcmp(cur_item->key, key) == 0)
            {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }
    ht->items[index] = item;
    ht->count++;
}

char* ht_search(ht_hash_table* ht, const char* key)
{
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;

    while (item != NULL)
    {
        if (item != &HT_DELETED_ITEM)
        {
            if (strcmp(item->key, key) == 0)
            {
                return item->value;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    return NULL;
}

void ht_delete(ht_hash_table* ht, const char* key)
{
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    const int load = ht->count * 100 / ht->size;
    if (load < 10)
    {
        ht_resize_down(ht);
    }
    while (item != NULL)
    {
        if (item != &HT_DELETED_ITEM)
        {
            if (strcmp(item->key, key) == 0)
            {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    ht->count--;
}
static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->size = new_ht->size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}
static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->size * 2;
    ht_resize(ht, new_size);
}
static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->size / 2;
    ht_resize(ht, new_size);
}