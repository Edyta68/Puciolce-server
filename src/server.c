#include "server.h"

struct epoll_event ev, events[EPOLL_MAX_EVENTS];
int nfds, epollfd;

bool server_running = false;
int server_socket = 0;
int server_options = 0;
FILE *server_log_file = NULL;
FILE *server_log_file_read = NULL;
X2_Server_Info server_info = {0};

void server_run(char *server_address, unsigned int options, char *existing_server_address, char* log_file_name)
{
	server_options = options;
	//initialize server_log_file to stdout
	server_log_file = stdout;
	if(server_options & SERVER_LOGS_TO_FILE){
		FILE *logs_file = NULL;
		if(server_options & SERVER_LOGS_APPEND){
			logs_file = fopen(log_file_name, "a");
		}else{
			logs_file = fopen(log_file_name, "w");
		}
		if(logs_file == NULL){
			fprintf(server_log_file, "Error 'fopen': %m\n");
			exit(EXIT_FAILURE);
		}
		else{
			server_log_file_read = fopen(log_file_name, "r");
			server_log_file = logs_file;
			printf("Savings logs to file -> %s\n", log_file_name);
		}
	}

	//server and client addressess
	struct sockaddr_in addr_in;
	fprintf(server_log_file, "\n");
	//fill in server info
	if(!server_fill_info_from_string(&server_info, &addr_in, server_address)){
		fprintf(server_log_file, "Creating server failed.\n");
		exit(EXIT_FAILURE);
	}
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
  initialize_table();
  if(server_socket == -1){
		fprintf(server_log_file, "Error 'socket': %m\n");
		server_stop();
    exit(EXIT_FAILURE);
  }
  //setting server to nonblocking
  make_socket_non_blocking(server_socket);

  if(bind(server_socket, (struct sockaddr*)&addr_in, sizeof(addr_in)) < 0){
		fprintf(server_log_file, "Error 'bind': %m\n");
		server_stop();
    exit(EXIT_FAILURE);
  }

  fprintf(server_log_file, "Server up\n");

  if(listen(server_socket, 5) == -1){
	 fprintf(server_log_file, "Error 'listen': %m\n");
	 server_stop();
   exit(EXIT_FAILURE);
  }

  fprintf(server_log_file, "Listening..\n");
	server_running = true;

  epollfd = epoll_create1(0);
   if (epollfd == -1) {
			 fprintf(server_log_file, "Error 'epoll_create1': %m\n");
			 server_stop();
       exit(EXIT_FAILURE);
   }
   ev.events = EPOLLIN;
   ev.data.fd = server_socket;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_socket, &ev) == -1) {
		 fprintf(server_log_file, "Error 'epoll_ctl': %m\n");
		 server_stop();
     exit(EXIT_FAILURE);
   }

	 //X2 server connection
	 if(options & SERVER_ALREADY_EXISTING && existing_server_address){
		fprintf(server_log_file, "Establishing X2 server connection.\n");
		if(server_fill_info_from_string(&other_server_info, &addr_in, existing_server_address)){
		 	int x2_status = x2_request_server_connection(addr_in);
		 	if(x2_status == ERR_X2_SERVER_CONNECTION_ESTABLISHED){
		 		fprintf(server_log_file, "X2 connection established.\n");
		 		other_server_connected = true;
		 	}
		 	else if(x2_status == ERR_X2_OTHER_SERVER_CONNECTED){
		 		fprintf(server_log_file, "Error: another server is already connected. X2 connection establishment aborted.\n");
		 	}
		 	else if(x2_status == ERR_X2_SOCKET_ERR){
		 		fprintf(server_log_file, "Error: unable to establish socket connection. X2 connection establishment aborted.\n");
		 	}
		 	else if(x2_status == ERR_X2_READ_TIMOUT){
		 		fprintf(server_log_file, "Error: existing server not responding. X2 connection establishment aborted.\n");
		 	}
		 	else if(x2_status == ERR_X2_DATA_MISMATCH){
		 		fprintf(server_log_file, "Error: received unexpeted data format. X2 connection establishment aborted.\n");
		 	}
		}
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
       fprintf(server_log_file, "Error - pthread_create() return code: %d\n",thread_error);
			 server_stop();
			 exit(EXIT_FAILURE);
   }

   while(server_running) {
      nfds = epoll_wait(epollfd, events, EPOLL_MAX_EVENTS, 0);
      if (nfds == -1) {
					fprintf(server_log_file, "Error 'epoll_wait': %m\n");
					server_stop();
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
  fprintf(server_log_file, "------------------------------------------\n");
  fprintf(server_log_file, "Server down\n");
	fprintf(server_log_file, "------------------------------------------\n\n");
  delete_Hash(connected_clients);
	free_reconnection_client_buffer();
	if(server_log_file != stdout){
		fclose(server_log_file);
	}
}
bool server_fill_info_from_string(X2_Server_Info *server_info, struct sockaddr_in *addr_in, char *server_address){
	char server_ip[SERVER_IP_BUFFER_SIZE];
	char server_port[SERVER_PORT_BUFFER_SIZE];
	char *token = strtok(server_address, ":");
	addr_in->sin_family = AF_INET;
	if(!token){
		fprintf(server_log_file, "Error: Invalid address format. Valid format is: 'ip:port'.\n");
		return false;
	}
	memcpy(server_ip, token, strlen(token)+1);
	addr_in->sin_addr.s_addr = inet_addr(server_ip);
	token = strtok(NULL, ":");
	if(!token){
		fprintf(server_log_file, "Error: Invalid address format. Valid format is: 'ip:port'.\n");
		return false;
	}
	memcpy(server_port, token, strlen(token)+1);
	server_info->eNodeB_port = atoi(server_port);
	token = strtok(server_ip, ".");
	if(!token){
		fprintf(server_log_file, "Error: Invalid address format. Valid format is: 'ip:port'.\n");
		return false;
	}
	server_info->address[0] = atoi(token);
	token = strtok(NULL, ".");
	if(!token){
		fprintf(server_log_file, "Error: Invalid address format. Valid format is: 'ip:port'.\n");
		return false;
	}
	server_info->address[1] = atoi(token);
	token = strtok(NULL, ".");
	if(!token){
		fprintf(server_log_file, "Error: Invalid address format. Valid format is: 'ip:port'.\n");
		return false;
	}
	server_info->address[2] = atoi(token);
	token = strtok(NULL, ".");
	if(!token){
		fprintf(server_log_file, "Error: Invalid address format. Valid format is: 'ip:port'.\n");
		return false;
	}
	server_info->address[3] = atoi(token);

	addr_in->sin_family = AF_INET;
	addr_in->sin_port = htons(atoi(server_port));
	return true;
}

void action_SIGINT(int signal){
  printf("\n");
  server_running = false;
}
