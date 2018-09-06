#include "../headers/clients_handling.h"

int main(int argc, char** argv){

  if(argc < 2){
    printf("Invalid number of arguments\n");
    exit(EXIT_FAILURE);
  }

  server_run(argc, argv);

  return 0;
}
