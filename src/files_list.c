#include "files_list.h"

int SendListOfFiles(int client_socket)
{
    DIR *directory;
    struct dirent *file;
    char **listOfFiles;
    int AmountOfFiles = 0;
    int *len;
    int TableLength = 0;
    int Size = 0;
    char directory_path[FOLDER_PATH_SIZE];
    if(getcwd(directory_path, sizeof(directory_path)) == NULL)
        return ERR_OPEN_DIR;
    strcat(directory_path, DOWNLOAD_FOLDER);
    directory = opendir(directory_path);
    if(directory) {
        while (( file = readdir(directory)) != NULL) {
            if (file->d_type == DT_REG)
            {
                AmountOfFiles++;
            }
        }
        rewinddir(directory);
        listOfFiles = (char **) malloc(AmountOfFiles * sizeof(char*));
        len = malloc(AmountOfFiles * sizeof(int));
        while (( file = readdir(directory)) != NULL) {
            if (file->d_type == DT_REG)
            {
                len[TableLength] = strlen(file->d_name) + 1;
                listOfFiles[TableLength] = malloc(len[TableLength] * sizeof(char));
                strcpy(listOfFiles[TableLength], file->d_name);
                Size += len[TableLength] * sizeof(char);
                TableLength++;
            }
        }

        message_label download_list_label = {
            message_type: msg_show_all_files_to_download,
            message_length: Size
        };

        write(client_socket, &download_list_label, sizeof(download_list_label));

        for(int i = 0; i < TableLength; i++) {
            write(client_socket, listOfFiles[i], (len[i] * sizeof(char)));
        }

        for(int i = 0; i < TableLength; i++)
            free(listOfFiles[i]);
        free(listOfFiles);
        free(len);
        closedir(directory);
        return SUCCESS_SEND_FILES_LIST;
    }
    return ERR_OPEN_DIR;
}
