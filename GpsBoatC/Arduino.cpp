// arduino.c
// Provides an I2C interface to a connected Arduino
// Note: The arduino must be running the correct sketch and is configured as
//       a slave I2C device

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "Arduino.h"

//------------------------------------------------------------------------------
Arduino::Arduino()
{

}

//------------------------------------------------------------------------------
bool Arduino::Init( U8 i2c_addr )
{
	bool bStatus = true;

	// We're using wiringPi here on an RPi, we need to init the library for each i2c device

	if( (i2c_fd = wiringPiI2CSetup( i2c_addr )) < 0)
	{
		fprintf (stderr, "Unable to open Arduino I2C: %s\n", strerror (errno)) ;
		bStatus = false;
	}

	return bStatus;
}

//------------------------------------------------------------------------------
void Arduino::SetReg( E_ARDUINO_REG reg, U8 val )
{
	if( wiringPiI2CWrite( i2c_fd, (U8)reg | 0x80 ) < 0 )
	{
		fprintf (stderr, "Arduino SetReg error: %s\n", strerror (errno)) ;
	}
	else
	{
		wiringPiI2CWrite( i2c_fd, val );
	}
}

//------------------------------------------------------------------------------
U8 Arduino::GetReg( E_ARDUINO_REG reg )
{
	U8 data = 0;

	if( wiringPiI2CWrite( i2c_fd, (U8)reg ) < 0 )
	{
		fprintf (stderr, "Arduino GetReg error: %s\n", strerror (errno)) ;
	}
	else
	{
		data = wiringPiI2CRead( i2c_fd );
	}
	//data = wiringPiI2CRead( i2c_fd );
//	data = wiringPiI2CReadReg8( i2c_fd, (U8)reg );

	return data;
}
