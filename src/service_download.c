#include "service_download.h"

bool handle_client_download(connected_client *client){
  if(!client->download.in_progress){
    return false;
  }

  message_label response_label = {
    message_type: msg_download_info,
    message_length: sizeof(Download_Info)
  };
  write(client->temp_c_rnti, &response_label, sizeof(response_label));

  Download_Packet packet = {0};
  packet.packet_number = client->download.current_packet_index;
  packet.data_size = read(client->download.file_descriptor, packet.data, DOWNLOAD_PACKET_SIZE);

  write(client->temp_c_rnti, &packet, sizeof(packet));

  if(++client->download.current_packet_index >= client->download.info.number_of_packets){
    close(client->download.file_descriptor);
    client->download.in_progress = false;
  }

  return true;
}

void start_download(connected_client *client){
  printf("------------------------------------------\n");
  printf("STARTING DOWNLOAD PROCEDURE\n");
  printf("Client fd: %d\n", client->temp_c_rnti);

  Download_Request request = {0};
  if(read_data_from_socket(client->temp_c_rnti, &request, sizeof(request)) < sizeof(request)){
    printf("Error: Failed to read download request message\n");
    printf("Status: Aborted");
    return;
  }

  message_label response_label = {
    message_type: msg_download_info,
    message_length: sizeof(Download_Info)
  };
  write(client->temp_c_rnti, &response_label, sizeof(response_label));

  if(client->download.in_progress){
    printf("Error: Downloading already in pogress\n");
    printf("Status: Sending error message\n");
    write(client->temp_c_rnti, &client->download.info, sizeof(client->download.info));
  }

  strcpy(client->download.info.filename, request.filename);
  printf("File name: %s\n", client->download.info.filename);

  if(access(request.filename, F_OK ) == -1){
    printf("Error: No such file\n");
    printf("Status: Sending error message\n");
    client->download.info.error_number = ERR_DOWNLOAD_FILE_NOT_FOUND;
    client->download.info.number_of_packets = 0;
    write(client->temp_c_rnti, &client->download.info, sizeof(client->download.info));
    return;
  }
  int file_descriptor = open(request.filename, O_RDONLY);

  struct stat file_stat;
  stat(request.filename, &file_stat);
  int file_size = file_stat.st_size;
  printf("File size: %d\n", file_size);

  int packet_number = file_size/DOWNLOAD_PACKET_SIZE;
  if(file_size%DOWNLOAD_PACKET_SIZE != 0){
    packet_number++;
  }
  client->download.info.number_of_packets = packet_number;
  client->download.info.error_number = ERR_DOWNLOAD_NO_ERRORS;
  write(client->temp_c_rnti, &client->download.info, sizeof(client->download.info));

  client->download.in_progress = true;
  client->download.current_packet_index = 0;
  client->download.file_descriptor = file_descriptor;

  printf("Status: Starting download procedure");
}
