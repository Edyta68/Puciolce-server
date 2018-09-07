#ifndef SERVER_DATA_H_
#define SERVER_DATA_H_

#include <stdio.h>

#include "preambles.h"
#include <time.h>

#define MAX_CLIENTS 256

//errors
#define ERR_ADD_CC_OVERFLOW -1
#define ERR_ADD_CC_ALREADY_CONNECTED -2
#define ERR_GET_CC_NO_MATCH -1
#define ERR_DEL_CC_NO_MATCH -1

typedef struct ping_info {
  bool low_battery_level;
  clock_t last_time_action;
} ping_info;

typedef struct connected_client{
  int temp_c_rnti;
  Sequence sequence;
  ping_info ping;
}connected_client;

extern connected_client connected_clients[MAX_CLIENTS];
extern int connected_clients_number;

int get_connected_client(int temp_c_rnti);
int add_connected_client(int temp_c_rnti, Sequence sequence);
int del_connected_client(int temp_c_rnti);

#endif
