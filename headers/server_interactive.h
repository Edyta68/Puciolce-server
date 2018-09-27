#pragma once
#include "server.h"
#include "print_clients.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_COMMANDS_AMOUNT 20

#define COMMAND_EXIT 0
#define COMMAND_HELP 1
#define COMMAND_SHOW_CLIENTS 2
#define COMMAND_CLOSE_ALL_CLIENTS 3

extern pthread_t interactive_thread;

void *run_server_interactive(void *unused);
void server_command_exit();
void server_command_help(char **available_commands);
void server_command_show_clients();
void server_command_close_all_clients();
