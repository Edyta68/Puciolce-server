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
  msg_drx_config
} Message_Type;

typedef struct message_label
{
	Message_Type message_type;
	unsigned int message_length;
} message_label;
