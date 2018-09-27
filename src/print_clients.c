#include "print_clients.h"

void  show_client_info(int client_c_rnti, void *value)
{
   connected_client *client = (connected_client *)value;
    printf("Client %d: %s  %d\n", client_c_rnti, client->sequence.type, client->sequence.ra_rnti);
}

void print_all_clients(const Hash_Table *table)
{
    if(connected_clients_number == 0) {
        printf("NO CLIENTS CONNECTED.\n");
    }
    else {
      printf("CONNECTED CLIENTS:\n\n");
      iter_Hash(table, show_client_info);
    }
}
