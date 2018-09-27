#include "server_interactive.h"

pthread_t interactive_thread;


void *run_server_interactive(void *unused){
  char input_buffer[MAX_COMMAND_LENGTH];
  char cleanup_buffer;
  char **available_commands = malloc(sizeof(char*)*MAX_COMMANDS_AMOUNT);
  available_commands[0] = "exit";
  available_commands[1] = "help";
  available_commands[2] = "show clients";
  available_commands[3] = "close all clients";
  available_commands[4] = NULL;
  while(server_running){
    printf("Write your command: ");
    scanf("%[^\n]s", input_buffer);
    int command_index;
    for(command_index=0; command_index<MAX_COMMANDS_AMOUNT; command_index++){
      if(!available_commands[command_index]){
        break;
      }
      if(strcmp(available_commands[command_index], input_buffer) == 0){
        break;
      }
    }
    if(available_commands[command_index]){
      switch(command_index){
        case COMMAND_EXIT:
          server_command_exit();
          break;
        case COMMAND_HELP:
          server_command_help(available_commands);
          break;
        case COMMAND_SHOW_CLIENTS:
          server_command_show_clients();
          break;
        case COMMAND_CLOSE_ALL_CLIENTS:
          server_command_close_all_clients();
          break;
      }
    }
    else{
      printf("Unrecognized command '%s'\n", input_buffer);
    }
    while ((cleanup_buffer = getchar()) != '\n' && cleanup_buffer != EOF);
  }
  free(available_commands);
  return NULL;
}

void server_command_exit(){
  server_running = false;
  printf("SERVER SHUTDOWN\n");
}

void server_command_help(char **available_commands){
  printf("Available commands: \n");
  for(int i=0; i<MAX_COMMANDS_AMOUNT; i++){
    if(!available_commands[i]){
      break;
    }
    printf("- %s\n", available_commands[i]);
  }
}

void server_command_show_clients(){
  printf("\n");
  print_all_clients(connected_clients);
  printf("\n");
}

void server_command_close_all_clients(){
  take_action_hash(connected_clients,close_connection);
  printf("Closed connection with all clients.\n");
}
