#pragma once

#include <stdbool.h>

#include "clients_registry.h"
#include "rrc_connection.h"

#define ADDRESS_LENGTH 4 //bytes
#define ERR_X2_SERVER_CONNECTION_ESTABLISHED 0;
#define ERR_X2_OTHER_SERVER_CONNECTED 1;

typedef unsigned char byte;

typedef struct X2_Server_Info {
    int eNodeB_port;
    byte address[ADDRESS_LENGTH];
}X2_Server_Info;


typedef struct X2_Client_Info {
    RRC_Connection_Request client_connection_request;
    RRC_Connection_Setup_Complete client_setup_complete;
    connected_client client;
}X2_send_to_eNodeB;

extern X2_Server_Info other_server_info;
extern bool other_server_connected;
extern unsigned short other_server_port;
extern int other_server_fd;
