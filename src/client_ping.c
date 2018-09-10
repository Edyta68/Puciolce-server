#include "client_ping.h"

void ping_clients()
{
  time_t current_time = clock();
  for(int i = 0; i < connected_clients_number; i++)
  {
    double expected_interval;
    if(connected_clients[i].ping.low_battery_level){
      expected_interval = PING_INTERVAL_LOW_BATTERY;
    }
    else{
      expected_interval = PING_INTERVAL_HIGH_BATTERY;
    }
    if((double)(current_time - connected_clients[i].ping.last_action_time)/CLOCKS_PER_SEC*1000.f >= expected_interval){
      write(connected_clients[i].temp_c_rnti, PING_MESSAGE, sizeof(PING_MESSAGE));
      printf("Pinged client with fd: %d\n", connected_clients[i].temp_c_rnti);
      connected_clients[i].ping.last_action_time = clock();
    }
    //write(unactive_clients->client_list[i], "Ping", 5);
    //read(unactive_clients->client_list[i], &client_c_rnti, sizeof(int));
    //printf("Client: %d pinged.\n", client_c_rnti);
    //connected_clients[client].ping.last_time_action = clock();
  }
}
