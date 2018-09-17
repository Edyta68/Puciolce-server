#include "server.h"

void server_run(unsigned short PORT, unsigned int options)
{
  //server and client addressess
	struct sockaddr_in server_address;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  initialize_table();

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

   //starting services thread
   int thread_error = pthread_create(&services_thread, NULL,  run_services, NULL);
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

  return;
}

void server_stop(){
  int thread_error = pthread_join( services_thread, NULL);
  take_action_hash(connected_clients,close_connection);
  close(server_socket);
  printf("------------------------------------------\n");
  if(thread_error != 0){
    perror("pthread");
  }
  printf("Server down\n");
  delete_Hash(connected_clients);
  //exit(EXIT_SUCCESS);
}

void action_SIGINT(int signal){
  printf("\n");
  server_running = false;
}
