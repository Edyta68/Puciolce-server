#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "message_label.h"
#include "service_download.h"

#define FOLDER_PATH_SIZE 256
#define SUCCESS_SEND_FILES_LIST 1
#define ERR_OPEN_DIR -1

int send_files_list(int client_socket);
