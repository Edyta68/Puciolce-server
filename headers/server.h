#pragma once

#include <arpa/inet.h>

#include "service_X2_handover.h"
#include "clients_handling.h"
#include "server_interactive.h"

#define SERVER_MINIMAL_OUTPUT 1
#define SERVER_ALREADY_EXISTING 2
#define SERVER_LOGS_TO_FILE 2*2
#define SERVER_LOGS_APPEND 2*2*2
#define SERVER_INTERACTIVE 2*2*2*2

#define SERVER_IP_BUFFER_SIZE 20
#define SERVER_PORT_BUFFER_SIZE 10

#define EPOLL_MAX_EVENTS 10
struct X2_Server_Info;
typedef struct X2_Server_Info X2_Server_Info;

//epoll
extern struct epoll_event ev, events[EPOLL_MAX_EVENTS];
extern int nfds, epollfd;

extern int server_socket;
extern int server_options;
extern FILE *server_log_file;
extern FILE *server_log_file_read;
extern bool server_running;
extern X2_Server_Info server_info;

void server_run(char *server_address, unsigned int options, char *existing_server_address, char* log_file_name);
void server_stop();
void server_connect_to_existing(char *existing_server_address);
bool server_fill_info_from_string(X2_Server_Info *server_info, struct sockaddr_in *addr_in, char *server_address);
void action_SIGINT(int signal);
