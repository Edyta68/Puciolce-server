#ifndef HANDLERS_H_
#define HANDLERS_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include "clients_services.h"
#include "preambles.h"
#include "clients_registry.h"
#include "LTE.h"
#include "message_label.h"

#define MAX_MSG_LEN 256
#define MAX_EVENTS 10
#define MAX_READ_TIMEOUT_MS 1000.f

//epoll
struct epoll_event ev, events[MAX_EVENTS];
int nfds, epollfd;

int server_socket;
extern bool server_running;

int make_socket_non_blocking (int sfd);
int read_data_from_socket(int client_socket, void *buffer, int size);
void handle_new_connection(int server_socket);
void close_connection(int client_socket);
void handle_client_input(int fd);


#endif
