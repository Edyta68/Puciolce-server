#include "../headers/LTE.h"

void lte_random_access_procedure(int client_socket){

  unsigned char receive_buffer[PACKET_SIZE];

  struct RandomAccessPreamble client_preamble = {};

  read_data_from_socket(client_socket, &client_preamble, sizeof(client_preamble));

  //struct RandomAccessPreamble client_preamble = *((struct RandomAccessPreamble *)receive_buffer);

  add_connected_client(client_socket, client_preamble.sequence);
  printf("Current connected clients number: %d\n", connected_clients_number);

  printf("Clients cyclic prefix: '%c'\n", client_preamble.cyclic_prefix);

  struct RRC_ConnectionRequest server_RRC_response = {};
  server_RRC_response.sequence = client_preamble.sequence;
  server_RRC_response.timing_advance_value = 5;
  server_RRC_response.uplink_resource_grant = false;
  server_RRC_response.temp_c_rnti = client_socket;

  write(client_socket, &server_RRC_response, sizeof(server_RRC_response));
}
