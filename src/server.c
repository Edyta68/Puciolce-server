#include "server.h"

struct epoll_event ev, events[EPOLL_MAX_EVENTS];
int nfds, epollfd;

bool server_running = false;
int server_socket = 0;
X2_Server_Info server_info = {0};

void server_run(unsigned short PORT, unsigned int options, unsigned short existing_server_port)
{
	//fill in server info
	server_info.eNodeB_port = PORT;
	server_info.address[0] = 127;
	server_info.address[1] = 0;
	server_info.address[2] = 0;
	server_info.address[3] = 1;
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

	//X2 server connection
	if(options & SERVER_ALREADY_EXISTING){
		other_server_info.eNodeB_port = existing_server_port;
		memcpy(other_server_info.address, server_info.address, ADDRESS_LENGTH);
		server_address.sin_port = htons(existing_server_port);
		int x2_status = x2_request_server_connection(server_address);
		if(x2_status == ERR_X2_SERVER_CONNECTION_ESTABLISHED){
			printf("X2 connection established\n");
			other_server_connected = true;
		}
		else if(x2_status == ERR_X2_OTHER_SERVER_CONNECTED){
			printf("Error: another server is already connected. X2 connection establishment aborted.\n");
		}
		else if(x2_status == ERR_X2_SOCKET_ERR){
			printf("Error: unable to establish socket connection. X2 connection establishment aborted.\n");
		}
		else if(x2_status == ERR_X2_READ_TIMOUT){
			printf("Error: existing server not responding. X2 connection establishment aborted.\n");
		}
		else if(x2_status == ERR_X2_DATA_MISMATCH){
			printf("Error: received unexpeted data format. X2 connection establishment aborted.\n");
		}
	}

  if(listen(server_socket, 5) == -1){
   perror("listen");
   exit(EXIT_FAILURE);
  }

  printf("Listening..\n");
	server_running = true;

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
      nfds = epoll_wait(epollfd, events, EPOLL_MAX_EVENTS, 0);
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
