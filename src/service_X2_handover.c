#include "service_X2_handover.h"

X2_Server_Info other_server_info = {0};
bool other_server_connected = false;
int other_server_fd = 0;
connected_client *reconnection_clients_buffer = NULL;
int reconnection_clients_buffer_size = 0;
int reconnection_clients_buffer_index = 0;

int x2_request_server_connection(struct sockaddr_in server_address){
  other_server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if(other_server_fd == -1){
    return ERR_X2_SOCKET_ERR;
  }
  if(connect(other_server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) != -1) {
    message_label request_label = {
      message_type: msg_x2_server_connection_request,
      message_length: sizeof(other_server_info)
    };
    make_socket_non_blocking(other_server_fd);
    write(other_server_fd, &request_label, sizeof(request_label));
    write(other_server_fd, &server_info, sizeof(server_info));

    message_label response_label = {};
    int response_status = 0;
    if(read_data_from_socket(other_server_fd, &response_label, sizeof(response_label)) < sizeof(response_label)){
      return ERR_X2_READ_TIMOUT;
    }
    if(response_label.message_type != msg_x2_server_connection_response || response_label.message_length != sizeof(response_status)){
      return ERR_X2_DATA_MISMATCH;
    }
    if(read_data_from_socket(other_server_fd, &response_status, sizeof(response_status)) < sizeof(response_status)){
      return ERR_X2_READ_TIMOUT;
    }

    //register new socket to epoll
    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
    ev.data.fd = other_server_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, other_server_fd,
                &ev) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    return response_status;
  }
  else{
    return ERR_X2_SOCKET_ERR;
  }
  return 0;
}

int x2_handle_server_connection(int client_socket){
  if(read_data_from_socket(client_socket, &other_server_info, sizeof(other_server_info)) < sizeof(other_server_info)){
    return ERR_X2_READ_TIMOUT;
  }

  int x2_response = ERR_X2_SERVER_CONNECTION_ESTABLISHED;
  message_label response_label = {
    message_type: msg_x2_server_connection_response,
    message_length: sizeof(x2_response)
  };
  write(client_socket, &response_label, sizeof(response_label));

  if(other_server_connected){
    x2_response = ERR_X2_OTHER_SERVER_CONNECTED;
    write(client_socket, &x2_response, sizeof(x2_response));
    return ERR_X2_OTHER_SERVER_CONNECTED;
  }
  write(client_socket, &x2_response, sizeof(x2_response));
  other_server_fd = client_socket;
  other_server_connected = true;
  return ERR_X2_SERVER_CONNECTION_ESTABLISHED;
}

int x2_send_server_info(int client_socket) {
  message_label server_info_label = {
    message_type: msg_x2_other_server_info,
    message_length: sizeof(other_server_info)
  };
  write(client_socket, &server_info_label, sizeof(server_info_label));
  if(write(client_socket, &other_server_info, sizeof(other_server_info)) > 0) {
      return SEND_SERVER_INFO_SUCCESS;
  }
  return ERR_SEND_SERVER_INFO;
}

int x2_handle_handover(int client_socket) {

  connected_client *client = get_connected_client(client_socket);

  message_label client_info_label = {
    message_type: msg_x2_recive_client_info,
    message_length: sizeof(*client)
  };
  write(other_server_fd, &client_info_label, sizeof(client_info_label));
  int send_status = 0;
  if(write(other_server_fd, client, sizeof(*client)) > 0) {
    send_status = X2_SUCCESS;
  }
  else{
    send_status = ERR_SEND_CLIENT_INFO;
    return send_status;
  }
  bool send_message = true;
  message_label response_label = {
    message_type: msg_handover_start,
    message_length: sizeof(send_message)
  };
  write(client_socket, &response_label, sizeof(response_label));
  write(client_socket, &send_status, sizeof(send_message));
  return send_status;
}

int x2_recive_client_info() {

    connected_client client = {0};
    if(read_data_from_socket(other_server_fd, &client, sizeof(connected_client)) < sizeof(connected_client)){
      return ERR_X2_READ_TIMOUT;
    }
    add_reconnection_client(&client);
    return X2_SUCCESS;
}

int x2_handle_client_reconnection(int client_socket){
  int old_c_rnti;
  if(read_data_from_socket(client_socket, &old_c_rnti, sizeof(old_c_rnti)) < sizeof(old_c_rnti)){
    return ERR_X2_READ_TIMOUT;
  }
  message_label info_label = {
    message_type: msg_handover_client_reconnection_info,
    message_length: sizeof(client_socket)
  };
  write(client_socket, &info_label, sizeof(info_label));

  connected_client client_data = {0};
  if(get_reconnection_client(old_c_rnti, &client_data) == ERR_RECONNECTION_CLIENT_BUFFER_CLIENT_NOT_FOUND){
    int connection_error = -1;
    write(client_socket, &connection_error, sizeof(connection_error));
    return ERR_RECONNECTION_CLIENT_BUFFER_CLIENT_NOT_FOUND;
  }
  write(client_socket, &client_socket, sizeof(client_socket));
  x2_send_server_info(client_socket);
  add_reconnected_client(client_socket, &client_data);
  return old_c_rnti;
}

bool handle_measurment_control(connected_client *client){
  time_t current_time = clock();
  if((double)(current_time - client->measurment_status.last_request_time)
  /CLOCKS_PER_SEC*1000.f >= X2_MEASURMENT_CONTROL_INTERVAL){
    unsigned char control_message = 'y';
    message_label control_label = {
      message_type: msg_handover_measurment_control,
      message_length: sizeof(control_message)
    };
    write(client->temp_c_rnti, &control_label, sizeof(control_label));
    write(client->temp_c_rnti, &control_message, sizeof(control_message));
    client->measurment_status.last_request_time = clock();
    printf("------------------------------------------\n");
    printf("SENDING MEASURMENT CONTROL\n");
    printf("Client fd: %d\n", client->temp_c_rnti);
    return true;
  }
  return false;
}

int handle_measurment_raport(int client_socket){
  int reported_signal = 0;
  if(read_data_from_socket(client_socket, &reported_signal, sizeof(reported_signal)) < sizeof(reported_signal)){
    return ERR_X2_READ_TIMOUT;
  }
  get_connected_client(client_socket)->measurment_status.reported_signal = reported_signal;
  return X2_SUCCESS;
}

int add_reconnection_client(connected_client *client){
  if(reconnection_clients_buffer == NULL){
    reconnection_clients_buffer_size = 2;
    reconnection_clients_buffer = malloc(reconnection_clients_buffer_size * sizeof(connected_client));
  }
  else if(reconnection_clients_buffer_index >= reconnection_clients_buffer_size){
    reconnection_clients_buffer_size *= 2;
    reconnection_clients_buffer = realloc(reconnection_clients_buffer, reconnection_clients_buffer_size * sizeof(connected_client));
  }
  memcpy(&(reconnection_clients_buffer[reconnection_clients_buffer_index++]), client, sizeof(*client));
}

int get_reconnection_client(int c_rnti, connected_client* target_client){
  int client_index;
  for(client_index=0; client_index<reconnection_clients_buffer_index; client_index++){
    if(reconnection_clients_buffer[client_index].temp_c_rnti == c_rnti){
      break;
    }
  }
  if(client_index >= reconnection_clients_buffer_index){
    return ERR_RECONNECTION_CLIENT_BUFFER_CLIENT_NOT_FOUND;
  }
  *target_client = reconnection_clients_buffer[client_index];
  reconnection_clients_buffer_index--;
  for(client_index; client_index<reconnection_clients_buffer_index; client_index++){
    reconnection_clients_buffer[client_index] = reconnection_clients_buffer[client_index+1];
  }
  return X2_SUCCESS;
}

void free_reconnection_client_buffer(){
  free(reconnection_clients_buffer);
}
