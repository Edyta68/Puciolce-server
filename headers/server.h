#pragma once

#include "clients_handling.h"

#define SERVER_LOGS_PING 1

void server_run(unsigned short PORT, unsigned int options);
void server_stop();
void action_SIGINT(int signal);
