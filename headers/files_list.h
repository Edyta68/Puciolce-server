#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define DIRECTORY_PATH "/home/przemeksnihur/test"
#define SUCCESS_SEND_FILES_LIST 1
#define ERR_OPEN_DIR -1

int SendListOfFiles(int client_socket);
