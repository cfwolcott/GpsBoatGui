#include <stdio.h>      /* printf */
#include <stdlib.h>
#include <string.h>
#include <math.h>       /* sin */

#include <wiringPi.h>
#include <wiringSerial.h>

#include "includes.h"
#include "HMC6343.h"

float GetCompassHeading( float declination );

//------------------------------------------------------------------------------
int main ()
{
	char id_str[3];
	unsigned int counter = 0;

	printf("sizeof float: %i\n", sizeof(float) );
	printf("sizeof int: %i\n", sizeof(int) );

	//-----------------------
	printf("WireingPi ... ");
	wiringPiSetup();
	printf("OK\n");

    // Init compass
	printf("Compass ... ");
	HMC6343_Setup();
	printf("OK\n");

	while(1)
	{
		system("clear");
		printf("Counter: %i\n", counter++);
		printf("True Heading (with deviation): %.1f\n", GetCompassHeading( -13.0 ) );
//		printf("Heading (without deviation): %.1f\n", GetCompassHeading( 0.0 ) );

		delay( 250 );
	}

	return 0;
}

//------------------------------------------------------------------------------
float GetCompassHeading( float declination )
{
	float heading = (float)(HMC6343_GetHeading()) / 10.0;

    // If you have an EAST declination, use + declinationAngle, if you
    // have a WEST declination, use - declinationAngle 

	heading -= declination;

    // Correct for when signs are reversed. 
    if( heading < 0.0 )
    {
		heading += 360.0;
    }

    // Check for wrap due to addition of declination. 
    if( heading > 360.0 )
    {
    	heading -= 360.0;
    }

	return heading;
}
