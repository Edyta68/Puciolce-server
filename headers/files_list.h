#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "message_label.h"

#define DOWNLOAD_FOLDER "/Download/"
#define FOLDER_PATH_SIZE 256
#define SUCCESS_SEND_FILES_LIST 1
#define ERR_OPEN_DIR -1

int SendListOfFiles(int client_socket);
