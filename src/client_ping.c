#include "client_ping.h"

pthread_t ping_thread;

void *ping_clients(void* unused)
{
  //pthread_detach(pthread_self());
  while(server_running){
    iter_Hash(connected_clients, ping_client);
    usleep(PING_SELEEP_TIME*1000);//converting to microseconds
  }
  pthread_exit(NULL);
}

void ping_client(int temp_c_rnti, void *Client) {

  time_t current_time;
  double expected_interval;
  connected_client *client = (connected_client *)Client;

  if(client->ping.low_battery_level){
    expected_interval = PING_INTERVAL_LOW_BATTERY;
  }
  else{
    expected_interval = PING_INTERVAL_HIGH_BATTERY;
  }

  current_time = clock();
  if((double)(current_time - client->ping.last_action_time)/CLOCKS_PER_SEC*1000.f >= expected_interval){
    message_label ping_request_label = {
      message_type: msg_ping_request,
      message_length: 0
    };
    char ping_data[PING_DATA_SIZE] = {0};


    write(client->temp_c_rnti, &ping_request_label, sizeof(ping_request_label));
    write(client->temp_c_rnti, ping_data, PING_DATA_SIZE);
    printf("------------------------------------------\n");
    printf("SENDING PING REQUEST\n");
    printf("Client fd: %d\n", client->temp_c_rnti);
    printf("Ping interval: %.0fms\n", expected_interval);
    client->ping.last_action_time = clock();
  }
}
