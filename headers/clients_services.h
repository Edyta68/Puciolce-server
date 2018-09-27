#pragma once

#include <pthread.h>

#include "clients_registry.h"
#include "service_ping.h"
#include "service_download.h"
#include "service_X2_handover.h"
#include "logging.h"

#define SERVICES_SLEEP_TIME 500 //ms

extern pthread_t services_thread;

void *run_services(void *unused);
void handle_client_services(int temp_c_rnti, void *Client);
