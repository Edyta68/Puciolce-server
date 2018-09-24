#include "service_ping.h"

bool ping_client(connected_client *client) {

  time_t current_time;
  double expected_interval;

  if(client->ping.low_battery_level){
    expected_interval = PING_INTERVAL_LOW_BATTERY;
  }
  else{
    expected_interval = PING_INTERVAL_HIGH_BATTERY;
  }

  current_time = clock();
  if((double)(current_time - client->ping.last_request_time)/CLOCKS_PER_SEC*1000.f >= expected_interval){
    message_label ping_request_label = {
      message_type: msg_ping_request,
      message_length: 64
    };
    char ping_data[PING_DATA_SIZE] = {0};

    write(client->temp_c_rnti, &ping_request_label, sizeof(ping_request_label));
    write(client->temp_c_rnti, ping_data, PING_DATA_SIZE);
    printf("------------------------------------------\n");
    printf("SENDING PING REQUEST\n");
    printf("Client fd: %d\n", client->temp_c_rnti);
    printf("Ping interval: %.0fms\n", expected_interval);
    client->ping.last_request_time = clock();
  }

  if((double)(client->ping.last_request_time - client->ping.last_response_time)/CLOCKS_PER_SEC*1000.f >= PING_MAX_RESPONSE_TIME){
    printf("------------------------------------------\n");
    printf("CLIENT NOT RESPONDING TO PINGS\n");
    printf("Client fd: %d\n", client->temp_c_rnti);
    close_connection(client->temp_c_rnti);
    return true;
  }

  return false;
}
