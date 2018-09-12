#pragma once
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "clients_registry.h"
#include "clients_handling.h"
#include "message_label.h"

#define PING_INTERVAL_HIGH_BATTERY 5000.f //ms
#define PING_INTERVAL_LOW_BATTERY 10000.f //ms
#define PING_SELEEP_TIME 500 //ms
#define PING_DATA_SIZE 64 //bytes

extern pthread_t ping_thread;

void *ping_clients(void *unused);
