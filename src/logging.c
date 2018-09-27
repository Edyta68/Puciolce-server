#include "logging.h"

void print_logs_to_console(){
  if(server_options&SERVER_INTERACTIVE){
    fflush(server_log_file);
  }
  else if(server_options&SERVER_LOGS_TO_FILE){
    fflush(server_log_file);
    char buffer = 0;
    int read_size = 0;
    while((read_size = fread(&buffer, sizeof(char), 1, server_log_file_read)) == 1){
      printf("%c", buffer);
    }
    if(read_size > 0){
      printf("\n");
    }
  }
}
