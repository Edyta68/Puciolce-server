#include "../headers/LTE.h"

int lte_random_access_procedure(int client_socket, RandomAccessPreamble *client_preamble){
  message_label preamble_label = {};
  if(read_data_from_socket(client_socket, &preamble_label, sizeof(preamble_label)) < sizeof(preamble_label)){
    return ERR_LTE_READ_TIMEOUT;
  }

  if(preamble_label.message_type != msg_random_access_preamble || preamble_label.message_length != sizeof(*client_preamble)){
    return ERR_LTE_DATA_MISMATCH;
  }

  if(read_data_from_socket(client_socket, client_preamble, sizeof(*client_preamble)) < sizeof(*client_preamble)){
    return ERR_LTE_READ_TIMEOUT;
  }

  message_label response_label = {
    message_type: msg_random_access_response,
    message_length: sizeof(RandomAccessResponse)
  };
  write(client_socket, &response_label, sizeof(response_label));

  RandomAccessResponse server_RRC_response = {};
  server_RRC_response.sequence = client_preamble->sequence;
  server_RRC_response.timing_advance_value = 5;
  server_RRC_response.uplink_resource_grant = false;
  server_RRC_response.temp_c_rnti = client_socket;
  write(client_socket, &server_RRC_response, sizeof(server_RRC_response));

  return 0;
}

int lte_rrc_connection_establishment(int client_socket, RRC_Connection_Request *connection_request){
  message_label request_label = {};
  if(read_data_from_socket(client_socket, &request_label, sizeof(request_label)) < sizeof(request_label)){
    return ERR_LTE_READ_TIMEOUT;
  }
  if(request_label.message_type != msg_rrc_connection_request || request_label.message_length != sizeof(*connection_request)){
    return ERR_LTE_DATA_MISMATCH;
  }

  if(read_data_from_socket(client_socket, connection_request, sizeof(*connection_request)) < sizeof(*connection_request)){
    return ERR_LTE_READ_TIMEOUT;
  }
  if(connection_request->c_rnti != client_socket){
    return ERR_LTE_DATA_MISMATCH;
  }

  message_label setup_label = {
    message_type: msg_rrc_connection_setup,
    message_length: sizeof(RRC_connection_Setup)
  };
  write(client_socket, &setup_label, sizeof(setup_label));

  RRC_connection_Setup connection_setup = {};
  strncpy(connection_setup.rnti_type, connection_request->rnti_type, sizeof(connection_request->rnti_type));
  connection_setup.c_rnti = connection_request->c_rnti;
  write(client_socket, &connection_setup, sizeof(connection_setup));

  return 0;
}
