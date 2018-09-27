#include "service_download.h"

bool handle_client_download(connected_client *client){
  if(!client->download.in_progress){
    return false;
  }

  fprintf(server_log_file, "------------------------------------------\n");
  fprintf(server_log_file, "SENDING DOWNLOAD PACKET TO CLIENT\n");
  fprintf(server_log_file, "Client fd: %d\n", client->temp_c_rnti);
  fprintf(server_log_file, "File name: %s\n", client->download.info.filename);

  message_label response_label = {
    message_type: msg_download_packet,
    message_length: sizeof(Download_Packet)
  };
  write(client->temp_c_rnti, &response_label, sizeof(response_label));
  Download_Packet packet = {0};
  packet.packet_number = client->download.current_packet_index;
  packet.data_size = fread(packet.data, 1, DOWNLOAD_PACKET_SIZE, client->download.file_descriptor);
  fprintf(server_log_file, "Packet number: %d/%d\n", packet.packet_number+1,client->download.info.number_of_packets);
  char* data = malloc(DOWNLOAD_PACKET_SIZE+1);
  memcpy(data, packet.data, DOWNLOAD_PACKET_SIZE);
  data[DOWNLOAD_PACKET_SIZE] = '\0';
  fprintf(server_log_file, "Packet data: '%s'\n", data);
  write(client->temp_c_rnti, &packet, sizeof(packet));
  free(data);

  if(++client->download.current_packet_index >= client->download.info.number_of_packets){
    fclose(client->download.file_descriptor);
    client->download.in_progress = false;
    fprintf(server_log_file, "Status: Closing download procedure\n");
  }
  else{
    fprintf(server_log_file, "Status: Download procedure in progress\n");
  }

  return true;
}

void start_download(connected_client *client){
  fprintf(server_log_file, "------------------------------------------\n");
  fprintf(server_log_file, "STARTING DOWNLOAD PROCEDURE\n");
  fprintf(server_log_file, "Client fd: %d\n", client->temp_c_rnti);

  Download_Request request = {0};
  if(read_data_from_socket(client->temp_c_rnti, &request, sizeof(request)) < sizeof(request)){
    fprintf(server_log_file, "Error: Failed to read download request message\n");
    fprintf(server_log_file, "Status: Aborted");
    return;
  }

  message_label response_label = {
    message_type: msg_download_info,
    message_length: sizeof(Download_Info)
  };
  write(client->temp_c_rnti, &response_label, sizeof(response_label));

  if(client->download.in_progress){
    fprintf(server_log_file, "Error: Downloading already in pogress\n");
    fprintf(server_log_file, "Status: Sending error message\n");
    write(client->temp_c_rnti, &client->download.info, sizeof(client->download.info));
    return;
  }

  strcpy(client->download.info.filename, request.filename);
  fprintf(server_log_file, "File name: '%s'\n", client->download.info.filename);

  int length = 0;
  length += strlen(client->download.info.filename);
  length += strlen(DOWNLOAD_FOLDER) + 1;
  char *file_path = malloc(length * sizeof(char));
  strcpy(file_path, DOWNLOAD_FOLDER);
  strcat(file_path, client->download.info.filename);

  if(access(file_path, F_OK ) == -1){
    fprintf(server_log_file, "Error: No such file\n");
    fprintf(server_log_file, "Status: Sending error message\n");
    client->download.info.error_number = ERR_DOWNLOAD_FILE_NOT_FOUND;
    client->download.info.number_of_packets = 0;
    write(client->temp_c_rnti, &client->download.info, sizeof(client->download.info));
    free(file_path);
    return;
  }
  FILE *file_descriptor = fopen(file_path, "rb");

  struct stat file_stat;
  stat(file_path, &file_stat);
  int file_size = file_stat.st_size;
  fprintf(server_log_file, "File size: %d\n", file_size);

  int packet_number = file_size/DOWNLOAD_PACKET_SIZE;
  if(file_size%DOWNLOAD_PACKET_SIZE != 0){
    packet_number++;
  }
  fprintf(server_log_file, "Packet number: %d\n", packet_number);
  client->download.info.number_of_packets = packet_number;
  client->download.info.error_number = ERR_DOWNLOAD_NO_ERRORS;
  write(client->temp_c_rnti, &client->download.info, sizeof(client->download.info));

  client->download.in_progress = true;
  client->download.current_packet_index = 0;
  client->download.file_descriptor = file_descriptor;

  fprintf(server_log_file, "Status: Starting download procedure\n");

  free(file_path);
}
