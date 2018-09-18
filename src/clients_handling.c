#include "../headers/clients_handling.h"

int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      perror ("fcntl");
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
      perror("accept");
      exit(EXIT_FAILURE);
  }
  make_socket_non_blocking(client_socket);

  printf("------------------------------------------\n");
  printf("OPENING NEW CONNECTION\n");
  printf("Client fd: %d\n", client_socket);
  //return;
  //handle LTE Random Access
  int lte_result = 0;

  RandomAccessPreamble client_preamble = {};
  lte_result = lte_random_access_procedure(client_socket, &client_preamble);
  if(lte_result == ERR_LTE_READ_TIMEOUT){
    printf("Error: Client not responding.\n");
    printf("Status: Random Access Procedure aborted.\n");
    close_connection(client_socket);
    return;
  }
  else if (lte_result == ERR_LTE_DATA_MISMATCH){
    printf("Error: Mismatch in expected and received message label.\n");
    printf("Status: Random Access Procedure aborted.\n");
    close_connection(client_socket);
    return;
  }
  printf("Cyclic prefix: %d\n", client_preamble.cyclic_prefix);

  //handle LTE RRC Connection Establishment
  RRC_Connection_Request connection_request = {};
  lte_result = lte_rrc_connection_establishment(client_socket, &connection_request);
  if(lte_result == ERR_LTE_READ_TIMEOUT){
    printf("Error: Client not responding.\n");
    printf("Status: RRC Connection Establishment refused.\n");
    close_connection(client_socket);
    return;
  }
  else if(lte_result == ERR_LTE_DATA_MISMATCH){
    printf("Error: Mismatch in expected and received c-rnti.\n");
    printf("Status: RRC Connection Establishment refused.\n");
    close_connection(client_socket);
    return;
  }

  //read LTE DRX configuration
  DRX_Config drx_config = {};
  lte_result = lte_drx_config(client_socket, &drx_config);
  if(lte_result == ERR_LTE_READ_TIMEOUT){
    printf("Error: Client not responding. Unable to read DRX configuration.\n");
    printf("Status: RRC Connection Establishment refused.\n");
    close_connection(client_socket);
    return;
  }
  else if(lte_result == ERR_LTE_DATA_MISMATCH){
    printf("Error: Missing DRX configuration.\n");
    printf("Status: RRC Connection Establishment refused.\n");
    close_connection(client_socket);
    return;
  }

  printf("Status: RRC Connection Establishment succeeded\n");

  add_connected_client(client_socket, client_preamble.sequence);

  //register new socket to epoll
  ev.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
  ev.data.fd = client_socket;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_socket,
              &ev) == -1) {
      perror("epoll_ctl");
      exit(EXIT_FAILURE);
  }
}

void close_connection(int client_socket){
  printf("------------------------------------------\n");
  printf("CLOSING CONNECTION\n");
  epoll_ctl(epollfd, EPOLL_CTL_DEL, client_socket, NULL);
  printf("Cient fd: %d\n", client_socket);
  del_connected_client(client_socket);
  printf("Current connected clients number: %d\n", connected_clients_number);
  close (client_socket);
}

void handle_client_input(int client_socket){
  message_label received_message_label = {};
  if(read_data_from_socket(client_socket, &received_message_label, sizeof(received_message_label)) < sizeof(received_message_label)){
    return;
  }
  printf("------------------------------------------\n");
  printf("RECEIVED VALID MESSAGE\n");
  printf("Client fd: %d\n", client_socket);
  if(received_message_label.message_type == msg_ping_request){
    printf("Type: msg_ping_request\n");
  }
  else if(received_message_label.message_type == msg_ping_response){
    printf("Type: msg_ping_response\n");
      printf("Size: %d\n", received_message_label.message_length);
    char *ping_data = malloc(received_message_label.message_length);
    read(client_socket, ping_data, received_message_label.message_length);
    connected_client *client = get_connected_client(client_socket);
    client->ping.last_response_time = clock();
    free(ping_data);
  }
  else if(received_message_label.message_type == msg_battery_critcal){
    printf("Type: msg_battery_critcal\n");
    printf("Size: %d\n", received_message_label.message_length);
    char *battery_data = malloc(received_message_label.message_length);
    read(client_socket, battery_data, received_message_label.message_length);
    connected_client *client = get_connected_client(client_socket);
    client->ping.low_battery_level = true;
    free(battery_data);
  }
  else if(received_message_label.message_type == msg_request_download){
    printf("Type: msg_request_download\n");
    printf("Size: %d\n", received_message_label.message_length);
    start_download(get_connected_client(client_socket));
  }
  else if(received_message_label.message_type == msg_ue_shutdown){
    printf("Type: msg_ue_shutdown\n");
    printf("Size: %d\n", received_message_label.message_length);
    close_connection(client_socket);
  }
  else{
    printf("Error: unhandled type - id=%d\n", received_message_label.message_type);
  }
}
