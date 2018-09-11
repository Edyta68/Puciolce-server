#include "hash_table.h"

#define NEW(T) (T*) malloc(sizeof(T))
#define NEWARRAY(T, N) (T*) calloc((N), sizeof(T))

unsigned int compute_Hash(const Hash_Table *table, const int key)
{
    return (key < table->size) ? key : (key % table->size);
}

Hash_Table *create_Hash(unsigned int size)
{
    Hash_Table *h_table = NEW(struct Hash_Table);

    if(h_table)
    {
        h_table->Table = NEWARRAY(struct Hash_value, size);

        if(!h_table->Table)
        {
            free(h_table);
            return NULL;
        }
        h_table->size = size;
    }
    return h_table;
}

void delete_Hash(Hash_Table *table)
{
    unsigned int i;
    Hash_value *entry;
    Hash_value *next;

    for(i = 0; i < table->size; i++)
    {
        entry = table->Table[i];
        while(entry)
        {
            next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(table->Table);
    free(table);
}

int add_Hash(Hash_Table *table, int key, void *value)
{
    unsigned int n;
    Hash_value *entry = malloc(sizeof(Hash_value));
    Hash_value *entries;

    if(entry)
    {
        entry->key = key;
        entry->value = value;
        entry->next = NULL;
        n = compute_Hash(table, key);
        entries = table->Table[n];
        if(!entries)
            table->Table[n] = entry;
        else
        {
            while(entries->next)
                entries=entries->next;
            entries->next = entry;
        }
        return 1;
    }
    return 0;
}

void iter_Hash(const Hash_Table *table, void (*func)(int key, void *value))
{
    unsigned int i;
    struct Hash_value *entry;

    for( i = 0; i < table->size; i++) {
        for(entry = table->Table[i]; entry; entry = entry->next) {
            func(entry->key,entry->value);
        }
    }
}

void *lookup_Hash(Hash_Table *table, int key)
{
    unsigned int n = compute_Hash(table, key);
    struct Hash_value *entry = table->Table[n];

    while(entry) {
        if(key == entry->key)
            break;
        entry = entry->next;
    }

    return entry ? entry->value : NULL;
}

void delete_value(Hash_Table *table, int key)
{
    unsigned int n = compute_Hash(table, key);
    struct Hash_value *entry = table->Table[n];

    if(key == entry->key)
    {
        struct Hash_value *to_delete = entry;
        table->Table[n] = entry->next;
        free(to_delete);
        return;
    }
    while(entry) {
        if(key == entry->next->key) {
            struct Hash_value *to_delete = entry->next;
            entry->next = to_delete->next;
            free(to_delete);
            break;
        }
        entry = entry->next;
    }
}

void print_node(Hash_Table *table, int key)
{
    unsigned int n = compute_Hash(table, key);
    struct Hash_value *entry = table->Table[n];

    while(entry) {
        printf("%d -> %s\n", entry->key, entry->value);
        entry = entry->next;
    }
}
