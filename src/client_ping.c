#include "client_ping.h"

not_active_clients get_all_not_active_clients(connected_client *connected_clients)
{
  int i;
  int amount = 0;
  clock_t current = clock();
  int size = connected_clients_number;
  int *not_active = malloc(sizeof(connected_client) * size);
  for(i = 0; i < size; i++)
  {
    if(connected_clients[i].ping.low_battery_level){
      if(connected_clients[i].ping.last_time_action > 60.0 * CLOCKS_PER_SEC)
      {
        not_active[amount] = connected_clients[i].temp_c_rnti;
        amount ++;
      }
    }
    else
      if((current - connected_clients[i].ping.last_time_action) > 30.0 * CLOCKS_PER_SEC) {
        not_active[amount] = connected_clients[i].temp_c_rnti;
        amount ++;
      }

  }
  not_active = realloc(not_active, amount * sizeof(int));

  not_active_clients clients;
  clients.client_list = not_active;
  clients.size = amount;

  return clients;
}

void ping_clients(not_active_clients *unactive_clients)
{
  int client_c_rnti;
  int client;
  int i;
  for(i = 0; i < unactive_clients->size; i++)
  {
    write(unactive_clients->client_list[i], "Ping", 5);
    read(unactive_clients->client_list[i], &client_c_rnti, sizeof(int));
    printf("Client: %d pinged.\n", client_c_rnti);
    client = get_connected_client(unactive_clients->client_list[i]);
    connected_clients[client].ping.last_time_action = clock();
  }
  free(unactive_clients->client_list);
}
