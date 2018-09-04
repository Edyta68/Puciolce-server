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
#include <fcntl.h>

#include "../../LTE-Client/Headers/preambles.h"

#define MAX_MSG_LEN 256
#define MAX_EVENTS 10
#define PACKET_SIZE 256


//epoll
struct epoll_event ev, events[MAX_EVENTS];
int nfds, epollfd;

void handle_new_connection(int server_socket);

int make_socket_non_blocking (int sfd);

void handle_client(int fd);

void epoll_connection(int argc, char** argv);

#endif
