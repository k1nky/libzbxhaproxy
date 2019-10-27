#include <stdio.h>
#include "hash_table.h"
int main()
{
    printf("lets us create and destroy one hash table!\n");

    ht_hash_table* hashTable = ht_new();
    ht_del_hash_table(hashTable);

    return 0;
}