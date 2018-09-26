#include "server.h"

int main(int argc, char** argv){

  int optret = 0, program_options = 0;
  opterr = 0;
  char *existing_server_port = "0";
  char *log_file_name = "logs";
  while((optret = getopt(argc, argv, "alp:f:")) != -1){
    switch (optret){
      case 'l':
        program_options |= SERVER_LOGS_PING;
        break;
      case 'p':
        program_options |= SERVER_ALREADY_EXISTING;
        existing_server_port = optarg;
        break;
      case 'f':
        program_options |= SERVER_LOGS_TO_FILE;
        log_file_name = optarg;
        break;
      case 'a':
        program_options |= SERVER_LOGS_APPEND;
        break;
      case '?':
        if(optopt == 'p' || optopt == 'f'){
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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

  server_run(argv[optind], program_options, existing_server_port, log_file_name);

  return 0;
}
