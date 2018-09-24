#pragma once

#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include "clients_registry.h"
#include "rrc_connection.h"
#include "server.h"

#define ADDRESS_LENGTH 4 //bytes
#define ERR_X2_SERVER_CONNECTION_ESTABLISHED 0
#define ERR_X2_OTHER_SERVER_CONNECTED -1
#define ERR_X2_SOCKET_ERR -2
#define ERR_X2_READ_TIMOUT -3
#define ERR_X2_DATA_MISMATCH -4
#define ERR_SEND_SERVER_INFO -1
#define SEND_SERVER_INFO_SUCCESS 1
#define ERR_SEND_CLIENT_INFO -1
#define X2_SUCCESS 0
#define X2_MEASURMENT_CONTROL_INTERVAL 3000.f //ms
#define X2_HANDOVER_THRESHOLD 30

typedef unsigned char byte;
typedef struct connected_client connected_client;

typedef struct X2_Server_Info {
    unsigned short eNodeB_port;
    byte address[ADDRESS_LENGTH];
}X2_Server_Info;

extern X2_Server_Info other_server_info;
extern bool other_server_connected;
extern int other_server_fd;

int x2_request_server_connection(struct sockaddr_in server_address);
int x2_handle_server_connection(int client_socket);
int x2_send_server_info(int client_socket);
int x2_handle_handover(int client_socket);
int x2_recive_client_info();
bool handle_measurment_control(connected_client *client); //server service
int handle_measurment_raport(int client_socket);
