#include "../headers/clients_registry.h"

connected_client connected_clients[MAX_CLIENTS];
int connected_clients_number = 0;

int get_connected_client(int temp_c_rnti){
  for(int i=0; i<connected_clients_number; i++){
    if(connected_clients[i].temp_c_rnti == temp_c_rnti){
      return i;
    }
  }
  return ERR_GET_CC_NO_MATCH;
}

int add_connected_client(int temp_c_rnti, Sequence sequence){
  if(connected_clients_number == MAX_CLIENTS-1){
    return ERR_ADD_CC_OVERFLOW;
  }
  for(int i=0; i<connected_clients_number; i++){
    if(connected_clients[i].temp_c_rnti == temp_c_rnti){
      return ERR_ADD_CC_ALREADY_CONNECTED;
    }
  }
  connected_clients[connected_clients_number].temp_c_rnti = temp_c_rnti;
  connected_clients[connected_clients_number++].sequence = sequence;
  printf("Current connected clients number: %d\n", connected_clients_number);
  return connected_clients_number;
}

int del_connected_client(int temp_c_rnti){
  int client_index = get_connected_client(temp_c_rnti);
  if(client_index < 0){
    return ERR_DEL_CC_NO_MATCH;
  }

  connected_clients_number--;
  for(int i=client_index; i<connected_clients_number; i++){
    connected_clients[i] = connected_clients[i+1];
  }
}
