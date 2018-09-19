#include "../headers/clients_registry.h"

Hash_Table *connected_clients = NULL;

void initialize_table() {
  connected_clients = create_Hash(MAX_CLIENTS);
}

int connected_clients_number = 0;

connected_client *get_connected_client(int temp_c_rnti) {
  return (connected_client *) lookup_Hash(connected_clients, temp_c_rnti);
}

int add_connected_client(int temp_c_rnti, Sequence sequence, RRC_Connection_Request connection_request, RRC_Connection_Setup_Complete setup_complete) {
  connected_client *client = calloc(sizeof(connected_client), 1);

  client->ping.low_battery_level = false;
  client->ping.last_request_time = (clock_t)0;
  client->ping.last_response_time = clock();
  client->temp_c_rnti = temp_c_rnti;
  client->sequence = sequence;
  client->download.in_progress = false;
  client->download.current_packet_index = 0;
  client->download.file_descriptor = -1;
  client->connection_request = connection_request;
  client->setup_complete = setup_complete;


  add_Hash(connected_clients, temp_c_rnti, client);
  connected_clients_number++;

  printf("Current connected clients number: %d\n", connected_clients_number);
  return connected_clients_number;
}

int del_connected_client(int temp_c_rnti){
  if(temp_c_rnti < 0) {
    return ERR_DEL_CC_NO_MATCH;
  }
  connected_client *client = (connected_client *)lookup_Hash(connected_clients, temp_c_rnti);
  if(client == NULL){
    return ERR_DEL_CC_NO_MATCH;
  }
  delete_value_hash(connected_clients, temp_c_rnti);
  free(client);
  connected_clients_number--;
}
