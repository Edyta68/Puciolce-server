#pragma once

#include "clients_handling.h"

#define SERVER_LOGS_PING 1
#define SERVER_ALREADY_EXISTING 2

void server_run(unsigned short PORT, unsigned int options);
void server_stop();
void action_SIGINT(int signal);
