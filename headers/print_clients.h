#ifndef PRINT_CLIENTS_H_
#define PRINT_CLIENTS_H_
#include "hash_table.h"
#include "clients_registry.h"
#include <stdio.h>

void  show_client_info(int client_c_rnti, void *value);

void print_all_clients(const Hash_Table *table);

#endif // PRINT_CLIENTS_H_
