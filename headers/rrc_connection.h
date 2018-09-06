#pragma once
#include <stdint.h>

// Request
enum Establishment_Cause {
  Emergency,
  HighPriorytyAccess,
  MobileTerminatingAccess,
  MobileOrientedSignalling,
  MobileOrientedData
};

typedef struct RRC_Connection_Request {
  char c_rnti_type[8];
  int c_rnti;
  char ue_identity[16]; // IMSI
  Establishment_Cause establishment_cause;
} RRC_Connection_Request;
// END:Request

// Setup
typedef enum bucket_Size_Duration {
  ms50,
  ms100,
  ms150,
  ms300,
  ms500,
  ms1000,
  spare2,
  spare1
} bucketSizeDuration;

typedef enum liblte_rrc_periodic_bsr_timer_text {
  sf5,
  sf10,
  sf16,
  sf20,
  sf32,
  sf40,
  sf64,
  sf80,
  sf128,
  sf160,
  sf320,
  sf640,
  sf1280,
  sf2560,
  INFINITY,
  SPARE
} liblte_rrc_periodic_bsr_timer_text;

typedef enum liblte_rrc_delta_mcs_enabled_text {
  en0,
  en1
} liblte_rrc_delta_mcs_enabled_text;

typedef struct UL_AM_RLC {
  bucketSizeDuration t_roll_retransmit;
  char poll_pdu;
  char maximum_retrans_threshold;
} UL_AM_RLC;

typedef struct DL_AM_RLC {
  bucketSizeDuration t_reordering;
} DL_AM_RLC;

typedef struct SRB_To_ADD_Modify {
  UL_AM_RLC ul_am_rlc;
  DL_AM_RLC dl_am_rlc;
  int SRB_Identity;
} SRB_To_ADD_Modify;

typedef struct Radio_Resource_Config_Dedicated {
  SRB_To_ADD_Modify srb_to_add_modify;
} Radio_Resource_Config_Dedicated;

typedef struct UL_SCH_Config {
  short max_HARQ_Tx;
  int periodic_BSR_Timer;
  liblte_rrc_periodic_bsr_timer_text Retransmission_BSR_Timer;
} UL_SCH_Config;

typedef struct PHR_Config {
  liblte_rrc_periodic_bsr_timer_text periodic_PHR_Timer;
  liblte_rrc_periodic_bsr_timer_text prohibit_PHR_Timer;
  short DL_Path_Loss_Change;
} PHR_Config;

typedef struct Uplink_Power_Control_Dedicated {
  int P0_UE_PUSCH;
  int P0_UE_PUCCH;
  int pSRS_Offset;
  liblte_rrc_delta_mcs_enabled_text deltaMCS_Enabled;
  short Filter_Coefficient;
} Uplink_Power_Control_Dedicated;
// END:Setup