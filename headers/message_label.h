

typedef enum Message_Type{
  msg_random_access_preamble = 0,
  msg_rrc_connection_request,
  msq_ping_request
}Message_Type;

typedef struct message_label{
  Message_Type message_type;
  unsigned int message_length;
}message_label;
