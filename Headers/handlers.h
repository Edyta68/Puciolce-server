#ifndef HANDLERS_H_
#define HANDLERS_H_

//epoll
struct epoll_event ev, events[MAX_EVENTS];
int nfds, epollfd;

void handle_new_connection(int server_socket);

int make_socket_non_blocking (int sfd);

void handle_client(int fd);

void epoll_connection(int argc, char** argv);

#endif
