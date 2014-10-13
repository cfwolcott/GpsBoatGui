// tools.c
// Various function "tools" for projects

#ifndef TOOLS_H
#define TOOLS_H

#include "includes.h"	// for typedef's, etc.

//-------------------------------------------
// Global defines

#define LOW_PASS_FILTER_RATE (0.20f)
#define RA_MAX_SIGNED_SAMPLES 20

typedef struct
{
	bool bInitialized;
	S16 s16Average;
	S32 s32Sum;
	U8  u8SampleIndex;
	U8  u8SampleCount;
	U8  u8MaxSamples;
	S16 asSamples[RA_MAX_SIGNED_SAMPLES];
} RUNNING_SIGNED_AVERAGE_TYPE;

//-------------------------------------------
// Function prototypes

int 	TOOLS_LowPassFilter( int s16LastValue, int s16CurrentValue );
void 	TOOLS_RA_Signed_Init( U8 u8SampleCount, RUNNING_SIGNED_AVERAGE_TYPE *ptSamples );
S16 	TOOLS_RA_ComputeSingedAverage( S16 s16Sample, RUNNING_SIGNED_AVERAGE_TYPE *ptSamples );

#endif
