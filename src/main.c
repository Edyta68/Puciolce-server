#include "server.h"

int main(int argc, char** argv){

  if(argc < 2){
    printf("Invalid number of arguments\n");
    exit(EXIT_FAILURE);
  }

  server_run(atoi(argv[1]));

  return 0;
}
