#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "clients_registry.h"
#include "clients_handling.h"
#include "message_label.h"
#include "download.h"

//returns true if clients downloading is currently in progress
bool handle_client_download(connected_client *client);
void start_download(connected_client *client);
