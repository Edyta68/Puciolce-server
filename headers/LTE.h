#ifndef LTE_H_
#define LTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "preambles.h"
#include "rrc_connection.h"
#include "clients_registry.h"
#include "clients_handling.h"

#define PACKET_SIZE 256

void lte_random_access_procedure(int client_socket);

bool lte_rrc_connection_establishment(int client_socket);

#endif
