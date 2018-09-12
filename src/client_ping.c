#include "client_ping.h"

pthread_t ping_thread;

void *ping_clients(void* unused)
{
  //pthread_detach(pthread_self());
  time_t current_time;
  double expected_interval;
  message_label ping_request_label = {
    message_type: msg_ping_request,
    message_length: 0
  };
  char ping_data[PING_DATA_SIZE] = {0};
  while(server_running){
    current_time = clock();
    for(int i = 0; i < connected_clients_number; i++)
    {
      if(connected_clients[i].ping.low_battery_level){
        expected_interval = PING_INTERVAL_LOW_BATTERY;
      }
      else{
        expected_interval = PING_INTERVAL_HIGH_BATTERY;
      }
      if((double)(current_time - connected_clients[i].ping.last_action_time)/CLOCKS_PER_SEC*1000.f >= expected_interval){
        write(connected_clients[i].temp_c_rnti, &ping_request_label, sizeof(ping_request_label));
        write(connected_clients[i].temp_c_rnti, ping_data, PING_DATA_SIZE);
        printf("------------------------------------------\n");
        printf("SENDING PING REQUEST\n");
        printf("Client fd: %d\n", connected_clients[i].temp_c_rnti);
        printf("Ping interval: %.0fms\n", expected_interval);
        connected_clients[i].ping.last_action_time = clock();
      }
    }

    usleep(PING_SELEEP_TIME*1000);//converting to microseconds
  }
  pthread_exit(NULL);
}
