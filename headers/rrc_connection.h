#pragma once
#include <stdint.h>

// Request
typedef enum Establishment_Cause {
  Emergency,
  HighPriorytyAccess,
  MobileTerminatingAccess,
  MobileOrientedSignalling,
  MobileOrientedData
}Establishment_Cause;

typedef struct RRC_Connection_Request {
  char rnti_type[8];
  int c_rnti;
  char ue_identity[16]; // IMSI
  Establishment_Cause establishment_cause;
} RRC_Connection_Request;
// END:Request

// Setup
typedef enum bucket_Size_Duration {
  ms50 = 0,
  ms100,
  ms150,
  ms300,
  ms500,
  ms1000,
} bucketSizeDuration;

typedef enum liblte_rrc_periodic_prohibit_bsr_timer_text {
  sf0 = 0,
  sf10,
  sf20,
  sf50,
  sf100,
  sf200,
  sf500,
  sf1000,
} liblte_rrc_periodic_prohibit_bsr_timer_text;

typedef enum liblte_rrc_delta_mcs_enabled_text {
  en0 = 0,
  en1
} liblte_rrc_delta_mcs_enabled_text;

typedef enum RRC_POLL_PDU {
  p4 = 0,
  p8,
  p16,
  p32,
  p64,
  p128,
}RRC_POLL_PDU;

typedef enum MAX_HARQ_TX {
  n1 = 0,
  n2,
  n3,
  n4,
  n5,
  n6,
  n7,
  n8,
  n10,
  n12,
  n16,
  n20,
  n24,
  n28,
}MAX_HARQ_TX;

typedef enum MAX_RET_THRESHOLD {
  t1 = 0,
  t2,
  t3,
  t4,
  t6,
  t8,
  t16,
  t32,
}MAX_RET_THRESHOLD;

typedef struct UL_AM_RLC {
  bucketSizeDuration t_roll_retransmit;
  RRC_POLL_PDU poll_pdu;
  MAX_RET_THRESHOLD maximum_retrans_threshold;
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
  MAX_HARQ_TX max_harq_tx;
  int periodic_BSR_Timer;
  liblte_rrc_periodic_prohibit_bsr_timer_text Retransmission_BSR_Timer;
} UL_SCH_Config;

typedef struct PHR_Config {
  liblte_rrc_periodic_prohibit_bsr_timer_text periodic_PHR_Timer;
  liblte_rrc_periodic_prohibit_bsr_timer_text prohibit_PHR_Timer;
  short DL_Path_Loss_Change;
} PHR_Config;

typedef struct Uplink_Power_Control_Dedicated {
  int P0_UE_PUSCH;
  int P0_UE_PUCCH;
  int pSRS_Offset;
  liblte_rrc_delta_mcs_enabled_text deltaMCS_Enabled;
  short Filter_Coefficient;
} Uplink_Power_Control_Dedicated;

typedef struct RRC_connection_Setup {
  char rnti_type[8];
  int c_rnti;
  Radio_Resource_Config_Dedicated RRC_Dedicated;
  UL_SCH_Config ul_sch_config;
  PHR_Config phr_config;
  Uplink_Power_Control_Dedicated UPC_Dedicatede;
}RRC_connection_Setup;
// END:Setup
