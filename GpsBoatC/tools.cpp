// tools.c
// Various function "tools" for projects

#include <string.h>
#include "tools.h"

//-----------------------------------------------------------------------------
//
// LowPassFilter
//
// time smoothing constant for low-pass filter
// 0 ? alpha ? 1 ; a smaller value basically means more smoothing
int TOOLS_LowPassFilter( int s16LastValue, int s16CurrentValue )
{
  return ( s16LastValue + LOW_PASS_FILTER_RATE * (s16CurrentValue - s16LastValue) );
} 

//-----------------------------------------------------------------------------
void TOOLS_RA_Signed_Init( U8 u8SampleCount, RUNNING_SIGNED_AVERAGE_TYPE *ptSamples )
{
	// limit number of samples based on RA_MAX_SAMPLES define
	if( ptSamples->u8SampleCount >= RA_MAX_SIGNED_SAMPLES )
	{
		u8SampleCount = RA_MAX_SIGNED_SAMPLES;
	}
	
	// Initialize data structure
	ptSamples->s16Average = 0;
	ptSamples->s32Sum = 0;
	ptSamples->u8SampleIndex = 0;
	ptSamples->u8SampleCount = 0;
	ptSamples->u8MaxSamples = u8SampleCount;
	
	memset( (void *)ptSamples->asSamples, 0, sizeof(ptSamples->asSamples) );
	
	ptSamples->bInitialized = true;
}

//-----------------------------------------------------------------------------
S16 TOOLS_RA_ComputeSingedAverage( S16 s16Sample, RUNNING_SIGNED_AVERAGE_TYPE *ptSamples )
{
	// Handle sample counter
	if( ptSamples->u8SampleCount < ptSamples->u8MaxSamples )
	{
		ptSamples->u8SampleCount++;
	}
	else
	{
		// We have a full set of samples. Start replacing old ones
		ptSamples->s32Sum -= ptSamples->asSamples[ ptSamples->u8SampleIndex ];
	}
	
	// Store the sample
	ptSamples->asSamples[ ptSamples->u8SampleIndex ] = s16Sample;
	
	// Compute new sum
	ptSamples->s32Sum += s16Sample;
	
	// Wrap the sample index counter
	ptSamples->u8SampleIndex++;
	ptSamples->u8SampleIndex %= ptSamples->u8MaxSamples;

	// Calculate the average based on the number of samples acquired so far
	ptSamples->s16Average = ptSamples->s32Sum / ptSamples->u8SampleCount;
		
	return ptSamples->s16Average;
}
