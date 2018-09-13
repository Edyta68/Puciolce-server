#include "hash_table.h"

unsigned int compute_Hash(const Hash_Table *table, const int key)
{
    if(table->size == 0)
    printf("ZERO\n");
    if(key < table->size)
      return key;
    else
      return key % table->size;
}

Hash_Table *create_Hash(unsigned int size)
{
    Hash_Table *h_table = calloc(sizeof(Hash_Table), 1);

    if(h_table)
    {
        h_table->Table = calloc(sizeof(Hash_value), size);

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
    unsigned int n = 0;
    Hash_value *entry = calloc(sizeof(Hash_value), 1);
    Hash_value *entries = NULL;

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
  unsigned int i = 0;
  struct Hash_value *entry = NULL;
  for( i = 0; i < table->size; i++) {
    entry = table->Table[i];
    while(entry){
      Hash_value *next_entry = entry->next;
      func(entry->key,entry->value);
      entry = next_entry;
    }
  }
}

void take_action_hash(const Hash_Table *table, void (*func)(int key))
{
  unsigned int i = 0;
  struct Hash_value *entry = NULL;
  for( i = 0; i < table->size; i++) {
    entry = table->Table[i];
    while(entry){
      Hash_value *next_entry = entry->next;
      func(entry->key);
      entry = next_entry;
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

void delete_value_hash(Hash_Table *table, int key)
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
