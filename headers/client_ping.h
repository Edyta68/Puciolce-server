#pragma once
#include "clients_registry.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct not_active_clients {
  int size;
  int *client_list;
} not_active_clients;

not_active_clients get_all_not_active_clients(connected_client *connected_clients);
void ping_clients(not_active_clients *unactive_clients);
