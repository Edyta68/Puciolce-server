#pragma once

typedef struct Download_Request
{
    char filename[50];
    int client_C_RNTI;
} Download_Request;

typedef struct Download_Info
{
    char filename[50];
    int download_id;
    int number_of_packets;
} Download_Info;

typedef struct Download_Packet
{
    int packet_number;
    char data[1024*1024];
    int data_size;
} Download_Packet;
