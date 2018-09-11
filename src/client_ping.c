#include "client_ping.h"

void *ping_clients(void* unused)
{
  while(true){
    time_t current_time = clock();
    message_label ping_request_label = {
      message_type: msg_ping_request,
      message_length: 0
    };
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
        write(connected_clients[i].temp_c_rnti, &ping_request_label, sizeof(ping_request_label));
        printf("Pinged client with fd: %d\n", connected_clients[i].temp_c_rnti);
        connected_clients[i].ping.last_action_time = clock();
      }
    }

    usleep(PING_SELEEP_TIME*1000);//converting to microseconds
  }
}
