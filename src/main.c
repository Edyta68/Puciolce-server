#include "server.h"

int main(int argc, char** argv){

  int optret = 0, server_options = 0;
  opterr = 0;
  char *existing_server_port;
  while((optret = getopt(argc, argv, "lp:")) != -1){
    switch (optret){
      case 'l':
        server_options |= SERVER_LOGS_PING;
        break;
      case 'p':
        server_options |= SERVER_ALREADY_EXISTING;
        existing_server_port = optarg;
        break;
      case '?':
        if(optopt == 'p'){
          fprintf(stderr, "Option -%c requiers an argument: existing servers port\n", optopt);
        }
        else{
          fprintf(stderr, "Unknown option: -%c\n", optopt);
        }
        break;
    }
  }

  if(optind >= argc){
    printf("Invalid number of arguments.\n");
    exit(EXIT_FAILURE);
  }

  if(server_options & SERVER_ALREADY_EXISTING){
    printf("Connecting to existing server on port: %s\n", existing_server_port);
  }

  server_run(atoi(argv[argc-1]), server_options, atoi(existing_server_port));

  return 0;
}
