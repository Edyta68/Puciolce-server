#include "../headers/clients_handling.h"

bool server_running = true;

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
  add_connected_client(client_socket, client_preamble.sequence);

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
  message_label received_mesage_label = {};
  if(read_data_from_socket(client_socket, &received_mesage_label, sizeof(received_mesage_label)) < sizeof(received_mesage_label)){
    return;
  }
  if(received_mesage_label.message_type == msg_ping_request){
    printf("------------------------------------------\n");
    printf("RECEIVED VALID MESSAGE\n");
    printf("Client fd: %d\n", client_socket);
    printf("Type: ping_request\n");
    printf("Action: reading ping data\n");
  }
  else if(received_mesage_label.message_type == msg_ping_response){
    char ping_data[PING_DATA_SIZE];
    read(client_socket, ping_data, PING_DATA_SIZE);
    printf("------------------------------------------\n");
    printf("RECEIVED VALID MESSAGE\n");
    printf("Client fd: %d\n", client_socket);
    printf("Type: ping_response\n");
  }
}

void server_run(int argc, char** argv)
{
  //server and client addressess
  unsigned short PORT = atoi(argv[1]);
	struct sockaddr_in server_address;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);


  if(server_socket == -1){
    perror("socket");
    exit(EXIT_FAILURE);
  }

  //setting server to nonblocking
  make_socket_non_blocking(server_socket);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);

  if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
    perror("bind");
    exit(EXIT_FAILURE);
  }

  printf("Server up\n");

  if(listen(server_socket, 5) == -1){
   perror("listen");
   exit(EXIT_FAILURE);
  }

  printf("Listening..\n");

  epollfd = epoll_create1(0);
   if (epollfd == -1) {
       perror("epoll_create1");
       exit(EXIT_FAILURE);
   }
   ev.events = EPOLLIN;
   ev.data.fd = server_socket;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_socket, &ev) == -1) {
     perror("epoll_ctl");
     exit(EXIT_FAILURE);
   }

   //changing progam action for SIG_INT
   sigset_t blocking_sig_set;
   sigemptyset(&blocking_sig_set);
   struct sigaction sig_int_action;
   sig_int_action.sa_handler = action_SIGINT;
   sig_int_action.sa_mask = blocking_sig_set;
   sig_int_action.sa_flags = 0;
   sigaction(SIGINT, &sig_int_action, NULL);

   //starting pinging thread
   int thread_error = pthread_create(&ping_thread, NULL,  ping_clients, NULL);
   if(thread_error){
       fprintf(stderr,"Error - pthread_create() return code: %d\n",thread_error);
       exit(EXIT_FAILURE);
   }

   while(server_running) {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, 0);
      if (nfds == -1) {
          perror("epoll_wait");
          exit(EXIT_FAILURE);
      }

      for (int n = 0; n < nfds; ++n) {
          if (events[n].data.fd == server_socket) {
            handle_new_connection(server_socket);
          }
          else {
              if(events[n].events & (EPOLLRDHUP | EPOLLHUP)){
                close_connection(events[n].data.fd);
              }
              else if(events[n].events & EPOLLIN){
                handle_client_input(events[n].data.fd);
              }
          }
      }

      //ping_clients();
  }

  server_stop();

  pthread_join( ping_thread, NULL);
  printf("Server down\n");
  close(server_socket);
}

void server_stop(){
  int thread_error = pthread_join( ping_thread, NULL);
  for(int i = 0; i < connected_clients_number; i++)
  {
    close_connection(connected_clients[i].temp_c_rnti);
  }
  close(server_socket);
  printf("\n------------------------------------------\n");
  if(thread_error != 0){
    perror("pthread");
  }
  printf("Server down\n");
  exit(EXIT_SUCCESS);
}

void action_SIGINT(int signal){
  server_running = false;
}
