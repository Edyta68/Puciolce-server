#include "../headers/clients_handling.h"

int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      fprintf(server_log_file, "Error 'fcntl': %m\n");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      fprintf(server_log_file, "Error 'fcntl': %m\n");
      return -1;
    }

  return 0;
}

int read_data_from_socket(int client_socket, void *receive_buffer, int size){
  clock_t last_read_time = clock();
  clock_t current_read_time;

  int accumuleted_received_data_count = 0;
  int current_read_data_count = 0;

  while(accumuleted_received_data_count < size){
    current_read_data_count = read(client_socket, receive_buffer + accumuleted_received_data_count, size-accumuleted_received_data_count);
    if(current_read_data_count > 0){
      clock_t last_read_time = clock();
      accumuleted_received_data_count += current_read_data_count;
    }
    else{
      current_read_time = clock();
      double delta_time = (double)(current_read_time - last_read_time)/CLOCKS_PER_SEC*1000.f;
      if(delta_time > MAX_READ_TIMEOUT_MS){
        return accumuleted_received_data_count;
      }
    }
  }

  return accumuleted_received_data_count;
}

void handle_new_connection(int server_socket){
  struct sockaddr_in client_address;
  int client_address_len = sizeof(client_address);
  int client_socket = accept(server_socket,
                     (struct sockaddr *) &client_address, &client_address_len);
  if (client_socket == -1) {
      fprintf(server_log_file, "Error 'accept': %m\n");
      exit(EXIT_FAILURE);
  }
  make_socket_non_blocking(client_socket);

  fprintf(server_log_file, "------------------------------------------\n");
  fprintf(server_log_file, "OPENING NEW CONNECTION\n");
  fprintf(server_log_file, "Client fd: %d\n", client_socket);
  //return;

  message_label connection_label = {};
  if(read_data_from_socket(client_socket, &connection_label, sizeof(connection_label)) < sizeof(connection_label)){
    fprintf(server_log_file, "Error: Client not responding.\n");
    fprintf(server_log_file, "Status: Connection procedure aborted.\n");
    close_connection(client_socket);
    return;
  }

  if(connection_label.message_type == msg_random_access_preamble){
    //NEW CLIENT CONNECTING
    if(connection_label.message_length != sizeof(RandomAccessPreamble)){
      fprintf(server_log_file, "Error: Mismatch in expected and received message label.\n");
      fprintf(server_log_file, "Status: Random Access Procedure aborted.\n");
      close_connection(client_socket);
      return;
    }
    //handle LTE Random Access
    int lte_result = 0;

    RandomAccessPreamble client_preamble = {};
    lte_result = lte_random_access_procedure(client_socket, &client_preamble);
    if(lte_result == ERR_LTE_READ_TIMEOUT){
      fprintf(server_log_file, "Error: Client not responding.\n");
      fprintf(server_log_file, "Status: Random Access Procedure aborted.\n");
      close_connection(client_socket);
      return;
    }
    else if (lte_result == ERR_LTE_DATA_MISMATCH){
      fprintf(server_log_file, "Error: Mismatch in expected and received message label.\n");
      fprintf(server_log_file, "Status: Random Access Procedure aborted.\n");
      close_connection(client_socket);
      return;
    }
    fprintf(server_log_file, "Cyclic prefix: %d\n", client_preamble.cyclic_prefix);

    //handle LTE RRC Connection Establishment
    RRC_Connection_Request connection_request = {};
    RRC_Connection_Setup_Complete setup_complete = {};
    lte_result = lte_rrc_connection_establishment(client_socket, &connection_request, &setup_complete);
    if(lte_result == ERR_LTE_READ_TIMEOUT){
      fprintf(server_log_file, "Error: Client not responding.\n");
      fprintf(server_log_file, "Status: RRC Connection Establishment refused.\n");
      close_connection(client_socket);
      return;
    }
    else if(lte_result == ERR_LTE_DATA_MISMATCH){
      fprintf(server_log_file, "Error: Mismatch in expected and received c-rnti.\n");
      fprintf(server_log_file, "Status: RRC Connection Establishment refused.\n");
      close_connection(client_socket);
      return;
    }

    //read LTE DRX configuration
    DRX_Config drx_config = {};
    lte_result = lte_drx_config(client_socket, &drx_config);
    if(lte_result == ERR_LTE_READ_TIMEOUT){
      fprintf(server_log_file, "Error: Client not responding. Unable to read DRX configuration.\n");
      fprintf(server_log_file, "Status: RRC Connection Establishment refused.\n");
      close_connection(client_socket);
      return;
    }
    else if(lte_result == ERR_LTE_DATA_MISMATCH){
      fprintf(server_log_file, "Error: Missing DRX configuration.\n");
      fprintf(server_log_file, "Status: RRC Connection Establishment refused.\n");
      close_connection(client_socket);
      return;
    }

    fprintf(server_log_file, "Status: RRC Connection Establishment succeeded\n");

    add_connected_client(client_socket, client_preamble.sequence, connection_request, setup_complete);
  }
  else if(connection_label.message_type == msg_x2_server_connection_request){
    //OTHER ENODEB CONNECTING
    if(connection_label.message_length != sizeof(X2_Server_Info)){
      fprintf(server_log_file, "Error: Mismatch in expected and received message label.\n");
      fprintf(server_log_file, "Status: X2 Connection Establishment aborted.\n");
      close_connection(client_socket);
      return;
    }
    int x2_result = x2_handle_server_connection(client_socket);
    if(x2_result == ERR_X2_SERVER_CONNECTION_ESTABLISHED){
      fprintf(server_log_file, "Server address: %d.%d.%d.%d:%d\n",
      other_server_info.address[0], other_server_info.address[1],
      other_server_info.address[2], other_server_info.address[3],
      other_server_info.eNodeB_port);
      fprintf(server_log_file, "Status: X2 Connection Establishment succeeded.\n");
    }
    else if(x2_result == ERR_X2_OTHER_SERVER_CONNECTED){
      fprintf(server_log_file, "Error: Another server is allready connected.\n");
      fprintf(server_log_file, "Status: X2 Connection Establishment aborted.\n");
      return;
    }
    else if(x2_result == ERR_X2_READ_TIMOUT){
      fprintf(server_log_file, "Error: Another server not responding.\n");
      fprintf(server_log_file, "Status: X2 Connection Establishment aborted.\n");
      return;
    }
    else{
      fprintf(server_log_file, "Status: X2 Connection Establishment aborted.\n");
      return;
    }
  }
  else if(connection_label.message_type == msg_handover_client_reconnection){
    //CLIENT HANDOVER RECONNECTION
    if(connection_label.message_length != sizeof(int)){
      fprintf(server_log_file, "Error: Data mismatch.\n");
      fprintf(server_log_file, "Status: Reconnection procedure aborted.\n");
      close_connection(client_socket);
      return;
    }
    int reconnection_status = x2_handle_client_reconnection(client_socket);
    if(reconnection_status == ERR_X2_READ_TIMOUT){
      fprintf(server_log_file, "Error: Client not responding.\n");
      fprintf(server_log_file, "Status: Reconnection procedure aborted.\n");
      close_connection(client_socket);
      return;
    }
    else if(reconnection_status == ERR_RECONNECTION_CLIENT_BUFFER_CLIENT_NOT_FOUND){
      fprintf(server_log_file, "Error: Received old c-rnti not present in reconnection client buffer.\n");
      fprintf(server_log_file, "Status: Reconnection procedure aborted.\n");
      close_connection(client_socket);
      return;
    }
    fprintf(server_log_file, "Status: Reconnected client with old c-rnt '%d'.\n", reconnection_status);
  }
  else{
    fprintf(server_log_file, "Error: Invalid message label.\n");
    fprintf(server_log_file, "Status: Connection procedure aborted.\n");
    close_connection(client_socket);
    return;
  }
  //register new socket to epoll
  ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
  ev.data.fd = client_socket;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_socket,
              &ev) == -1) {
      fprintf(server_log_file,"Error 'epoll_ctl': %m\n");
      exit(EXIT_FAILURE);
  }
}

void close_connection(int client_socket){
  fprintf(server_log_file, "------------------------------------------\n");
  fprintf(server_log_file, "CLOSING CONNECTION\n");
  epoll_ctl(epollfd, EPOLL_CTL_DEL, client_socket, NULL);
  if(client_socket == other_server_fd){
    fprintf(server_log_file, "Server fd: %d\n", client_socket);
    fprintf(server_log_file, "Status: Closed connection with other server. Informing clients.\n");
    other_server_connected = false;
    other_server_info.eNodeB_port = 0;
    other_server_info.address[0] = 0;
    other_server_info.address[1] = 0;
    other_server_info.address[2] = 0;
    other_server_info.address[3] = 0;
    take_action_hash(connected_clients, (void (*)(int))x2_send_server_info);
  }
  else{
    fprintf(server_log_file, "Cient fd: %d\n", client_socket);
    del_connected_client(client_socket);
    fprintf(server_log_file, "Current connected clients number: %d\n", connected_clients_number);
  }
  close (client_socket);
}

void handle_client_input(int client_socket){
  message_label received_message_label = {};
  if(read_data_from_socket(client_socket, &received_message_label, sizeof(received_message_label)) < sizeof(received_message_label)){
    return;
  }
  fprintf(server_log_file, "------------------------------------------\n");
  fprintf(server_log_file, "RECEIVED MESSAGE\n");
  fprintf(server_log_file, "Client fd: %d\n", client_socket);
  fprintf(server_log_file, "Size: %d\n", received_message_label.message_length);
  if(received_message_label.message_type == msg_ping_request){
    fprintf(server_log_file, "Type: msg_ping_request\n");
  }
  else if(received_message_label.message_type == msg_ping_response){
    fprintf(server_log_file, "Type: msg_ping_response\n");
    char *ping_data = malloc(received_message_label.message_length);
    read(client_socket, ping_data, received_message_label.message_length);
    connected_client *client = get_connected_client(client_socket);
    client->ping.last_response_time = clock();
    free(ping_data);
  }
  else if(received_message_label.message_type == msg_battery_critcal){
    fprintf(server_log_file, "Type: msg_battery_critcal\n");
    char *battery_data = malloc(received_message_label.message_length);
    read(client_socket, battery_data, received_message_label.message_length);
    connected_client *client = get_connected_client(client_socket);
    client->ping.low_battery_level = true;
    free(battery_data);
  }
  else if(received_message_label.message_type == msg_request_download){
    fprintf(server_log_file, "Type: msg_request_download\n");
    start_download(get_connected_client(client_socket));
  }
  else if(received_message_label.message_type == msg_handover_measurment_report) {
    fprintf(server_log_file, "Type: msg_handover_measurment_report\n");
    if(received_message_label.message_length != sizeof(int)){
      fprintf(server_log_file, "Error: Invalid message length.\n");
      fprintf(server_log_file, "Status: Measurment raport not handled\n");
      return;
    }
    int report_status = handle_measurment_raport(client_socket);
    if(report_status == ERR_X2_READ_TIMOUT){
      fprintf(server_log_file, "Error: Client not responding.\n");
      fprintf(server_log_file, "Status: Measurment raport not handled\n");
      return;
    }
    int reported_signal = get_connected_client(client_socket)
    ->measurment_status.reported_signal;
    fprintf(server_log_file, "Reported signal: %d\n", reported_signal);
    if(reported_signal <= X2_HANDOVER_THRESHOLD){
      if(!other_server_connected){
        fprintf(server_log_file, "Status: Handover not possible. No other server available\n");
        return;
      }
      int handover_status = x2_handle_handover(client_socket);
      if(handover_status == X2_SUCCESS){
        fprintf(server_log_file, "Status: Starting handover procedure.\n");
        //close_connection(client_socket);
      }
      else{
        fprintf(server_log_file, "Error: Unable to send client info.\n");
        fprintf(server_log_file, "Status: Handover aborted.\n");
      }
    }
    else{
      fprintf(server_log_file, "Status: Handover not needed.\n");
    }
  }
  else if(received_message_label.message_type == msg_x2_recive_client_info) {
    fprintf(server_log_file, "Type: msg_x2_recive_client_info\n");
    connected_client client = {0};
    int receive_status = x2_recive_client_info();
    if(receive_status == X2_SUCCESS){
      fprintf(server_log_file, "Status: Receiving client info from other eNodeB succeeded\n");
    }
    else if(receive_status == ERR_X2_READ_TIMOUT){
      fprintf(server_log_file, "Error: Other server not responding\n");
      fprintf(server_log_file, "Status: Receiving aborted\n");
    }
  }
  else if(received_message_label.message_type == msg_ue_shutdown){
    fprintf(server_log_file, "Type: msg_ue_shutdown\n");
    fprintf(server_log_file, "Size: %d\n", received_message_label.message_length);
    close_connection(client_socket);
  }
  else{
    unsigned char unhandled_data;
    fprintf(server_log_file, "Error: Unhandled type - id=%d\n", received_message_label.message_type);
    fprintf(server_log_file, "Status: Reading unhandled data.\n");
    while(read(client_socket, &unhandled_data, 1)>0);
  }
}
