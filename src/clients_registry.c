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
  client->measurment_status.last_request_time = (clock_t)0;
  client->measurment_status.reported_signal = 100;

  add_Hash(connected_clients, temp_c_rnti, client);
  connected_clients_number++;

  printf("Current connected clients number: %d\n", connected_clients_number);
  return connected_clients_number;
}

int add_reconnected_client(int temp_c_rnti, connected_client *client_data){
  connected_client *new_client = calloc(sizeof(*client_data), 1);
  memcpy(new_client, client_data, sizeof(*client_data));
  add_Hash(connected_clients, temp_c_rnti, new_client);
  connected_clients_number++;
  new_client->temp_c_rnti = temp_c_rnti;
  new_client->ping.low_battery_level = false;
  new_client->ping.last_request_time = (clock_t)0;
  new_client->ping.last_response_time = clock();
  new_client->measurment_status.last_request_time = (clock_t)0;
  new_client->measurment_status.reported_signal = 100;
  if(new_client->download.in_progress){
    int file_descriptor = open(new_client->download.info.filename,
      O_RDONLY);
    new_client->download.file_descriptor = file_descriptor;
    lseek(file_descriptor, new_client->download.current_packet_index*DOWNLOAD_PACKET_SIZE, SEEK_SET);
  }
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
