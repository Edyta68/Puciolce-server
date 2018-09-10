#pragma once
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "clients_registry.h"
#include "message_label.h"

#define PING_INTERVAL_HIGH_BATTERY 5000.f //ms
#define PING_INTERVAL_LOW_BATTERY 10000.f //ms

#define PING_MESSAGE "Ping"

void ping_clients();
