#pragma once
#include <stdbool.h>

typedef struct Sequence
{
	char type[8];
	int ra_rnti;
} Sequence;

typedef struct RandomAccessPreamble
{
	char cyclic_prefix;
	Sequence sequence;
} RandomAccessPreamble;

typedef struct RandomAccessResponse
{
	Sequence sequence;
	int timing_advance_value;
	bool uplink_resource_grant;
	int temp_c_rnti; // Temporary ID assigned for this session to the UE
} RandomAccessResponse;