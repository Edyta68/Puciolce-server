#pragma once

typedef enum On_Duration_Timer
{
	on_duration_timer_e_psf1,
	on_duration_timer_e_psf2,
	on_duration_timer_e_psf3,
	on_duration_timer_e_psf4,
	on_duration_timer_e_psf5,
	on_duration_timer_e_psf6,
	on_duration_timer_e_psf8,
	on_duration_timer_e_psf10,
	on_duration_timer_e_psf20,
	on_duration_timer_e_psf30,
	on_duration_timer_e_psf40,
	on_duration_timer_e_psf50,
	on_duration_timer_e_psf60,
	on_duration_timer_e_psf80,
	on_duration_timer_e_psf100,
	on_duration_timer_e_psf200
} On_Duration_Timer;

typedef enum DRX_Inactivity_Timer
{
	drx_inactivity_e_psf1,
	drx_inactivity_e_psf2,
	drx_inactivity_e_psf3,
	drx_inactivity_e_psf4,
	drx_inactivity_e_psf5,
	drx_inactivity_e_psf6,
	drx_inactivity_e_psf8,
	drx_inactivity_e_psf10,
	drx_inactivity_e_psf20,
	drx_inactivity_e_psf30,
	drx_inactivity_e_psf40,
	drx_inactivity_e_psf50,
	drx_inactivity_e_psf60,
	drx_inactivity_e_psf80,
	drx_inactivity_e_psf100,
	drx_inactivity_e_psf200,
	drx_inactivity_e_psf300,
	drx_inactivity_e_psf500,
	drx_inactivity_e_psf750,
	drx_inactivity_e_psf1280,
	drx_inactivity_e_psf1920,
	drx_inactivity_e_psf2560,
	drx_inactivity_e_spare10,
	drx_inactivity_e_spare9,
	drx_inactivity_e_spare8,
	drx_inactivity_e_spare7,
	drx_inactivity_e_spare6,
	drx_inactivity_e_spare5,
	drx_inactivity_e_spare4,
	drx_inactivity_e_spare3,
	drx_inactivity_e_spare2,
	drx_inactivity_e_spare1
} DRX_Inactivity_Timer;

typedef enum DRX_Retransmission_Timer
{
	drx_retransmission_e_psf1,
	drx_retransmission_e_psf2,
	drx_retransmission_e_psf4,
	drx_retransmission_e_psf6,
	drx_retransmission_e_psf8,
	drx_retransmission_e_psf16,
	drx_retransmission_e_psf24,
	drx_retransmission_e_psf33
} DRX_Retransmission_Timer;

typedef enum Long_DRX_Cycle_Start_Offset_Label
{
	long_drx_cycle_e_sf10,
	long_drx_cycle_e_sf20,
	long_drx_cycle_e_sf32,
	long_drx_cycle_e_sf40,
	long_drx_cycle_e_sf64,
	long_drx_cycle_e_sf80,
	long_drx_cycle_e_sf128,
	long_drx_cycle_e_sf160,
	long_drx_cycle_e_sf256,
	long_drx_cycle_e_sf320,
	long_drx_cycle_e_sf512,
	long_drx_cycle_e_sf640,
	long_drx_cycle_e_sf1024,
	long_drx_cycle_e_sf1280,
	long_drx_cycle_e_sf2048,
	long_drx_cycle_e_sf2560
} Long_DRX_Cycle_Start_Offset_Label;

typedef struct Long_DRX_Cycle_Start_Offset
{
	Long_DRX_Cycle_Start_Offset_Label label;
	int value;
} Long_DRX_Cycle_Start_Offset;

typedef enum Short_DRX_Cycle
{
	short_drx_e_sf2,
	short_drx_e_sf5,
	short_drx_e_sf8,
	short_drx_e_sf10,
	short_drx_e_sf16,
	short_drx_e_sf20,
	short_drx_e_sf32,
	short_drx_e_sf40,
	short_drx_e_sf64,
	short_drx_e_sf80,
	short_drx_e_sf128,
	short_drx_e_sf160,
	short_drx_e_sf256,
	short_drx_e_sf320,
	short_drx_e_sf512,
	short_drx_e_sf640
} Short_DRX_Cycle;

typedef struct Short_DRX
{
	Short_DRX_Cycle cycle;
	int DRX_Short_Cycle_Timer;
} Short_DRX;

typedef struct DRX_Config
{
	On_Duration_Timer on_duration_timer;
	DRX_Inactivity_Timer drx_inactivity_timer;
	DRX_Retransmission_Timer drx_retransmission_timer;
	Long_DRX_Cycle_Start_Offset long_drx_cycle_start_offset;
	Short_DRX short_drx;
} DRX_Config;
