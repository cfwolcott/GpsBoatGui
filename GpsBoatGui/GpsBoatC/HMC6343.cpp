//****************************************************************************
//
//	HMC6343.c
//
//	This file provides the GENERIC access routines for the HMC6343 Compass.
//
//	Copyright (c) 2011-2013 N2 Imaging Systems, L.L.C.
//
//	N2 Imaging Systems, L.L.C. PROPRIETARY DATA
//	is included in the information disclosed herein. Recipient, by accepting
//	this document, agrees that neither this document nor any part of the
//	information disclosed herein shall be used or disclosed to others for any
//	purpose, except as specifically authorized in writing by N2 Imaging
//	Systems, L.L.C.
//
//	WARNING: INFORMATION SUBJECT TO U.S. EXPORT CONTROLS
//	This document may contain Technical Data controlled under the U.S.
//	International Traffic in Arms Regulations ("ITAR") or technology subject
//	to the U.S. Export Administration Regulations ("EAR") and may not be
//	exported from the United States or disclosed to any Foreign Person, by
//	any means, except as in accordance with the applicable regulations.
//	Violations of U.S. export control laws and regulations are subject to
//	severe civil and criminal penalties.  Recipient shall include this
//	notice with any reproduced portion of this document.
//
//****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "includes.h"
#include "HMC6343.h"

//*** local defines and typedefs *********************************************

#define REGISTER__SETUP(reg)				{ HMC6343__##reg##_REG, HMC6343__##reg##_REG__SETUP }

typedef struct
{
	U8	u8Register;
	U8	u8Setup;
} REGISTER_SETUP;

//*** global variable definitions ********************************************
int gfd;

//*** local function declarations ********************************************

static bool SendCommand( U8 cmd, U8 arg1, U8 arg2, U8 size);

static bool ReadResponseBytes( U8 *pBuffer, U8 size);

//*** local function definitions ********************************************
bool SendCommand( U8 cmd, U8 arg1, U8 arg2, U8 size)
{
	U8 au8CommandStream[HMC6343__MAX_CMD_SIZE];
	int i;

	switch (size)
	{
		case 3:
			au8CommandStream[2] = arg2;
			// fall through
		case 2:
			au8CommandStream[1] = arg1;
			// fall through
		case 1:
			au8CommandStream[0] = cmd;
			break;
	}

	// Start
	serialPutchar( gfd, 'S' );

	// I2C Address
	serialPutchar( gfd, (U8)HMC6343__ADDRESS );

	// Size
	serialPutchar( gfd, (U8)size + 1 );

	// Data
	for(i=0; i<size; i++ )
	{
		serialPutchar( gfd, (U8)au8CommandStream[i] );
	}

	// Stop
	serialPutchar( gfd, 'P' );

}

//*****************************************************************************
//
//	ReadResponseBytes
//
//	Reads bytes from a previously sent command via SendCommand()
//
//	Parameters:
//		buffer - pointer to U8 buffer to fill
//		size - number of expected return bytes
//
//	Returns:
//		nothing
//
//*****************************************************************************
bool ReadResponseBytes( U8 *pBuffer, U8 size)
{
	int i;
	int size_avail;
	bool bStatus = false;

	// Start
	serialPutchar( gfd, 'S' );

	// I2C Address
	serialPutchar( gfd, (U8)HMC6343__ADDRESS | 0x01 );

	// Size
	serialPutchar( gfd, (U8)size );

	// Stop
	serialPutchar( gfd, 'P' );

	delay(5);

	size_avail = serialDataAvail( gfd );

	if( size_avail == size )
	{
		// Get Data
		for(i=0; i<size_avail; i++ )
		{
			*pBuffer++ = serialGetchar ( gfd );
		}

		bStatus = true;
	}

	if( !bStatus )
	{
		printf("ReadResponseBytes failed. Bytes available: %i\n", size_avail);
		serialFlush( gfd );
	}

	return bStatus;
}

//*** global function declarations ********************************************

//*****************************************************************************
//
//	HMC6343_SendCommand
//
//	Sends the requested single byte compass command
//
//	Parameters:
//		available commands:
//			HMC6343__RESET_CPU__CMD					-	Reset the Processor
//
//			HMC6343__ENTER_CAL_MODE__CMD			-	Enter User Calibration Mode
//			HMC6343__EXIT_CAL_MODE__CMD				-	Exit User Calibration Mode
//
//			HMC6343__SET_LEVEL_ORIENT__CMD			-	Level Orientation (X=forward, +Z=up) (default)
//			HMC6343__SET_UP_SIDEWAYS_ORIENT__CMD	-	Upright Sideways Orientation (X=forward, Y=up)
//			HMC6343__SET_UP_FLAT_ORIENT__CMD		-	Upright Flat Front Orientation (Z=forward, -X=up)

//			HMC6343__ENTER_RUN_MODE__CMD			-	Enter Run Mode (from Standby Mode)
//			HMC6343__ENTER_STANDBY_MODE__CMD		-	Enter Standby Mode (from Run Mode)
//			HMC6343__ENTER_SLEEP_MODE__CMD			-	Enter Sleep Mode (from Run Mode)
//			HMC6343__EXIT_SLEEP_MODE__CMD			-	Exit Sleep Mode (to Standby Mode)
//
//	Returns:
//		nothing
//
//*****************************************************************************
void HMC6343_SendCommand( U8 cmd )
{
	SendCommand(cmd, 0, 0, 1);
}

//*****************************************************************************
//
//	HMC6343_Setup
//
//	Initializes the default state of the compass.
//
//	Parameters:
//		none
//
//	Returns:
//		nothing
//
//*****************************************************************************
void HMC6343_Setup( void )
{
	static const REGISTER_SETUP atRegisterSetup[] =
	{
		REGISTER__SETUP(OP_MODE_1),
		REGISTER__SETUP(OP_MODE_2),
		REGISTER__SETUP(HEADING_FILTER_LSB)
	};
	REGISTER_SETUP const* ptRegisterSetup;
	S16 s16Size;
	U8 u8RegData;

	// Open the WiringPi serial port to SC18IM700 (Master I2C controller with uart interface)
	printf("Opening serial port ... ");
	gfd = serialOpen( "/dev/ttyUSB0" , 9600 );

	if( !gfd )
	{
		fprintf (stderr, "Unable to open serial port: %s\n", strerror (errno)) ;
		return;
	}
	else
	{
		printf("Serial Port opened\n");
		serialFlush( gfd );
	}

	// reset the compass
	HMC6343_SendCommand( HMC6343__RESET_CPU__CMD );

	// per chip spec, wait 500ms after reset
	delay( 500 );

	// Verify operational mode registers are set correctly
	for (
		s16Size = sizeof(atRegisterSetup) / sizeof(atRegisterSetup[0]),
		ptRegisterSetup = atRegisterSetup;
		s16Size--;
		ptRegisterSetup++
	)
	{
		// Read
		SendCommand(
			HMC6343__READ_EEPROM__CMD,
			ptRegisterSetup->u8Register, 0,
			HMC6343__READ_EEPROM__CMD_SIZE );

		// EEPROM read/writes need 10ms delay per spec
		delay( 10 );

		ReadResponseBytes(
			&u8RegData,
			HMC6343__READ_EEPROM__DATA_SIZE );

		// Verify
		if( ptRegisterSetup->u8Setup != u8RegData )
		{
			// Update
			SendCommand(
				HMC6343__WRITE_EEPROM__CMD,
				ptRegisterSetup->u8Register,
				ptRegisterSetup->u8Setup,
				HMC6343__WRITE_EEPROM__CMD_SIZE
			);

			delay(10);
		}
	}

	SendCommand(
			HMC6343__SET_UP_SIDEWAYS_ORIENT__CMD, 0, 0,
			HMC6343__SET_UP_SIDEWAYS_ORIENT__CMD_SIZE
	);
}

//*****************************************************************************
//
//	HMC6343_Shutdown
//
//	Shuts down the compass chip
//
//	Parameters:
//		none
//
//	Returns:
//		none
//
//*****************************************************************************
void HMC6343_Shutdown( void )
{
	HMC6343_SendCommand( HMC6343__ENTER_SLEEP_MODE__CMD );
}

//*****************************************************************************
//
//	HMC6343_GetHeading
//
//	Gets current compass heading in tenths degrees
//
//	Parameters:
//		none
//
//	Returns:
//		U16 - Heading
//
//*****************************************************************************
S16 HMC6343_GetHeading( void )
{
	U8 u8HeadPitchRoll[HMC6343__GET_HEADING_DATA__DATA_SIZE];
	S16 s16Heading = COMPASS_HEADING_INVALID;

	if (SendCommand(
			HMC6343__GET_HEADING_DATA__CMD, 0, 0,
			HMC6343__GET_HEADING_DATA__CMD_SIZE
		)
	)
	{
		delay(1);

		if( ReadResponseBytes( u8HeadPitchRoll, HMC6343__GET_HEADING_DATA__DATA_SIZE) )
		{
			s16Heading = (U16)(u8HeadPitchRoll[0]<<8 | u8HeadPitchRoll[1]);
		}
	}

	return s16Heading;
}
