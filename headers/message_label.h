#pragma once

typedef enum Message_Type
{
	msg_random_access_preamble = 0,
	msg_random_access_response,
	msg_rrc_connection_request,
	msg_rrc_connection_setup,
	msg_rrc_connection_setup_complete,
	msg_ping_request,
	msg_ping_response,
	msg_ue_shutdown,
	msg_drx_config,
	msg_battery_critcal,
	msg_request_download,
	msg_download_info,
	msg_download_packet,
	msg_x2_server_connection_request,
	msg_x2_server_connection_response
} Message_Type;

typedef struct message_label
{
	Message_Type message_type;
	unsigned int message_length;
} message_label;
