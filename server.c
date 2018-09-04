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

#include "../LTE-Client/Header/preambles.h"
#include "handlers.h"

#define MAX_MSG_LEN 256
#define MAX_EVENTS 10
#define PACKET_SIZE 256


int main(int argc, char** argv){

  //printf("%lu", sizeof(struct RRC_ConnectionRequest));

  if(argc < 2){
    printf("Invalid number of arguments\n");
    exit(EXIT_FAILURE);
  }
    epoll_connection(argc, argv);
  return 0;
}
