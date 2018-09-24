#pragma once

#define ERR_DOWNLOAD_NO_ERRORS 0
#define ERR_DOWNLOAD_FILE_NOT_FOUND -1
#define DOWNLOAD_PACKET_SIZE 16

typedef struct Download_Request
{
    char filename[50];
    int client_C_RNTI;
} Download_Request;

typedef struct Download_Info
{
    char filename[50];
    int error_number;
    int number_of_packets;
} Download_Info;

typedef struct Download_Packet
{
    int packet_number;
    char data[DOWNLOAD_PACKET_SIZE];
    int data_size;
} Download_Packet;
