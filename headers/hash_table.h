#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include<stdlib.h>
#include<stdio.h>

typedef struct Hash_value
{
    int key;
    void *value;
    struct Hash_value *next;
}Hash_value;

typedef struct Hash_Table
{
    int size;
    Hash_value **Table;
}Hash_Table;

Hash_Table *create_Hash(unsigned int size); //create Hash Table
void delete_Hash(Hash_Table *table); //delete all table
int add_Hash(Hash_Table *table, int key, void *value);  //add new element to table
unsigned int compute_Hash(const Hash_Table *table, const int key);  //hash function
void iter_Hash(const Hash_Table *table, void (*func)(int key, void *value));  // run function on every value in table
void *lookup_Hash(Hash_Table *table, int key);  //Show value in table using key
void delete_value(Hash_Table *table, int key);  //delete value from table using key

#endif // HASH_TABLE_H_
