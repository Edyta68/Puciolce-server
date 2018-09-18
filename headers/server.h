#pragma once

#include "X2_handover.h"
#include "clients_handling.h"

#define SERVER_LOGS_PING 1
#define SERVER_ALREADY_EXISTING 2

#define EPOLL_MAX_EVENTS 10

//epoll
extern struct epoll_event ev, events[EPOLL_MAX_EVENTS];
extern int nfds, epollfd;

extern int server_socket;
extern bool server_running;

void server_run(unsigned short PORT, unsigned int options, unsigned short existing_server_port);
void server_stop();
void action_SIGINT(int signal);
