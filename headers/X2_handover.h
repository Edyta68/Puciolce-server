#pragma once
#include "clients_registry.h"
#include "rrc_connection.h"

#define ADDRESS_LENGTH 4 //bytes
typedef unsigned char byte;

typedef struct x2_send_to_client {
    int eNodeB_port;
    byte address[ADDRESS_LENGTH]
}x2_send_to_client;


typedef struct x2_send_to_eNodeB {
    RRC_Connection_Request client_connection_request;
    RRC_Connection_Setup_Complete client_setup_cimplete;
    connected_client client;
}x2_send_to_eNodeB;
