#ifndef LTE_H_
#define LTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "preambles.h"
#include "rrc_connection.h"
#include "clients_registry.h"
#include "clients_handling.h"
#include "message_label.h"
#include "drx_config.h"

#define PACKET_SIZE 256

#define ERR_LTE_READ_TIMEOUT -1
#define ERR_LTE_DATA_MISMATCH -2

int lte_random_access_procedure(int client_socket, RandomAccessPreamble *client_preamble);
int lte_rrc_connection_establishment(int client_socket, RRC_Connection_Request *connection_request, RRC_Connection_Setup_Complete *setup_complete);
int lte_drx_config(int client_socket, DRX_Config *config);

#endif
