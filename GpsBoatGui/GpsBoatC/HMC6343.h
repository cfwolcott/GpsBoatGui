
//****************************************************************************
//
//	HMC6343.h
//
//	This file provides the definitions for the Honeywell Magnetic Compass chip.
//
//	Copyright (c) 2008-2013 N2 Imaging Systems, L.L.C.
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

#ifndef _HMC6343_H
#define _HMC6343_H

#include "includes.h"
#include "BitField.h"

//*** global defines and typedefs ********************************************

#define HMC6343__NAME															"HMC6343"

// TWI 7-bit address
#define HMC6343__ADDRESS														0x32

// supported clock rates
#define HMC6343__TWI_CLOCK__MASK												(\
																					TWI_CLOCK__100_KHZ__SUPPORTED__STATE |\
																					0L\
																				)

#define COMPASS_HEADING_INVALID													-1

//
//	HMC6343 byte stream:
//	#---#-----+-----+-----+-----+-----+-----+-----+-----#---#-----+-----+-----+-----+-----+-----+-----+-----#---#----+----+----+----+----+----+----+----#---#---#
//	# S # DA6 | DA5 | DA4 | DA3 | DA2 | DA1 | DA0 | RNW # A # RA7 | RA6 | RA5 | RA4 | RA3 | RA2 | RA1 | RA0 # A # D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 # A # P #
//	#---#-----+-----+-----+-----+-----+-----+-----+-----#---#-----+-----+-----+-----+-----+-----+-----+-----#---#----+----+----+----+----+----+----+----#---#---#
//		 +----------------------+----------------------+	 +----------------------+----------------------+	 +------------------+------------------+
//								|													|												|
//							 byte 0												 byte 1											 byte 2
//	where:
//		S		start
//		RNW		0=write, 1=read
//		DAx		device address
//		A		acknowledge
//		RAx		register address
//		Dx		data
//		P		stop
//

// byte 0
#define HMC6343__BYTE0__DA__MASK						    BIT_FIELD__MASK(0xFE)
#define HMC6343__BYTE0__WNR__MASK							BIT_FIELD__MASK(0x01)

// byte 1
#define HMC6343__BYTE1__RA__MASK							BIT_FIELD__MASK(0xFF)

// byte 2
#define HMC6343__BYTE2__D__MASK								BIT_FIELD__MASK(0xFF)

//--- EEPROM registers ---------------------------------------------------------

/*

EEPROM	Content			Description										Factory Default
Location
----	-------			-----------										---------------
0x00	Slave Address	I2C Slave Address								0x32
0x01	Reserved
0x02	S/W_Version		Software Version Number
0x03	Reserved
0x04	OP_Mode1		Operational Mode Register 1						0x11
0x05	OP_Mode2		Operational Mode Register 2						0x01
0x06	S/N LSB			Device Serial Number
0x07	S/N MSB			Device Serial Number
0x08	Date Code: YY	Package Date Code:  Last Two Digits of the Year	Year
0x09	Date Code: WW	Package Date Code: Fiscal Week					Week
0x0A	Deviation LSB	Deviation Angle ( ±1800) in tenths of a degree	0x00
0x0B	Deviation MSB	Deviation Angle ( ±1800) in tenths of a degree	0x00
0x0C	Variation LSB	Variation Angle ( ±1800) in tenths of a degree	0x00
0x0D	Variation MSB	Variation Angle ( ±1800) in tenths of a degree	0x00
0x0E	X_Offset LSB	Hard-Iron Calibration Offset for the X-axis		0x00
0x0F	X_Offset MSB	Hard-Iron Calibration Offset for the X-axis		0x00
0x10	Y_Offset LSB	Hard-Iron Calibration Offset for the Y-axis		0x00
0x11	Y_Offset MSB	Hard-Iron Calibration Offset for the Y-axis		0x00
0x12	Z_Offset LSB	Hard-Iron Calibration Offset for the Z-axis		0x00
0x13	Z Offset MSB	Hard-Iron Calibration Offset for the Z-axis		0x00
0x14	Filter LSB		Heading IIR Filter (0x00 to 0x0F typical)		0x00
0x15	Filter MSB		Heading IIR Filter (set at zero)				0x00

*/

// NOTE: The following registers accessed via the EEPROM Read/Write command (0xE1 & 0xF1)

//
//	name:		SLAVE_ADDRESS_REG
//	offset:		0x00
//	access:		R/W-
//	default:	0011 0100											: 0x34
//
#define HMC6343__SLAVE_ADDRESS_REG												0x00

#define HMC6343__SLAVE_ADDRESS_REG__ADDR__MASK									BIT_FIELD__MASK(0xFF)

#define HMC6343__SLAVE_ADDRESS_REG__SETUP										(\
																					BIT_FIELD__SET(HMC6343__SLAVE_ADDRESS__DATA__MASK, 0x34L)\
																				)

//
//	name:		SOFTWARE_VER
//	offset:		0x02
//	access:		R/--
//
#define HMC6343__SOFTWARE_VER_REG												0x02

#define HMC6343__SOFTWARE_VER_REG__MASK											BIT_FIELD__MASK(0xFF)

//
//	name:		OP_MODE_1_REG
//	offset:		0x04
//	access:		R/W-
//	default:	0001 0001											: default
//				?--- ----	: R/-- : COMP			Calculating compass data if set . (read only)
//				-?-- ----	: R/-- : CAL			Calculating calibration offsets if set. (read only)
//				--1- ----	: R/W- : FILTER			IIR Heading Filter used if set
//				---1 ----	: R/W- : RUN			Run Mode if set
//				---- 0---	: R/W- : STDBY			Standby Mode if set
//				---- -001	: R/W- : ORIENT			Compass Orientation
//
#define HMC6343__OP_MODE_1_REG													0x04

#define HMC6343__OP_MODE_1_REG__COMP__MASK										BIT_FIELD__MASK(0x80)
#define HMC6343__OP_MODE_1_REG__CAL__MASK										BIT_FIELD__MASK(0x40)
#define HMC6343__OP_MODE_1_REG__FILTER__MASK									BIT_FIELD__MASK(0x20)
#define HMC6343__OP_MODE_1_REG__RUN__MASK										BIT_FIELD__MASK(0x10)
#define HMC6343__OP_MODE_1_REG__STDBY__MASK										BIT_FIELD__MASK(0x08)
#define HMC6343__OP_MODE_1_REG__ORIENT__MASK									BIT_FIELD__MASK(0x07)

#define HMC6343__OP_MODE_1_REG__COMP__CALCULATING__STATE						BIT_FIELD__SET(HMC6343__OP_MODE_1_REG__COMP__MASK, 0x1L)
#define HMC6343__OP_MODE_1_REG__CAL__CALCULATING__STATE							BIT_FIELD__SET(HMC6343__OP_MODE_1_REG__CAL__MASK, 0x1L)

#define HMC6343__OP_MODE_1_REG__FILTER__IIR__STATE								BIT_FIELD__SET(HMC6343__OP_MODE_1_REG__FILTER__MASK, 0x1L)
#define HMC6343__OP_MODE_1_REG__RUN__RUN__STATE									BIT_FIELD__SET(HMC6343__OP_MODE_1_REG__RUN__MASK, 0x1L)
#define HMC6343__OP_MODE_1_REG__STDBY__STDBY__STATE								BIT_FIELD__SET(HMC6343__OP_MODE_1_REG__STDBY__MASK, 0x1L)

#define HMC6343__OP_MODE_1_REG__ORIENT__FRONT__STATE							BIT_FIELD__SET(HMC6343__OP_MODE_1_REG__ORIENT__MASK, 0x4L)
#define HMC6343__OP_MODE_1_REG__ORIENT__EDGE__STATE								BIT_FIELD__SET(HMC6343__OP_MODE_1_REG__ORIENT__MASK, 0x2L)
#define HMC6343__OP_MODE_1_REG__ORIENT__LEVEL__STATE							BIT_FIELD__SET(HMC6343__OP_MODE_1_REG__ORIENT__MASK, 0x1L)

#define HMC6343__OP_MODE_1_REG__SETUP											(\
																					HMC6343__OP_MODE_1_REG__FILTER__IIR__STATE|\
																					HMC6343__OP_MODE_1_REG__RUN__RUN__STATE |\
																					HMC6343__OP_MODE_1_REG__ORIENT__LEVEL__STATE|\
																					0\
																				)
//
//	name:		OP_MODE_2_REG
//	offset:		0x05
//	access:		R/W-
//	default:	0000 0001											: default
//				0000 00--	: R/-- : RESERVED		Reserved bits
//				---- --??	: R/-- : MR				Measurement Rate
#define HMC6343__OP_MODE_2_REG													0x05

#define HMC6343__OP_MODE_2_REG__RESERVED__MASK									BIT_FIELD__MASK(0xFC)
#define HMC6343__OP_MODE_2_REG__MR__MASK										BIT_FIELD__MASK(0x03)

#define HMC6343__OP_MODE_2_REG__RESERVED__STATE									BIT_FIELD__SET(HMC6343__OP_MODE_2_REG__RESERVED__MASK, 0x00L)
#define HMC6343__OP_MODE_2_REG__MR__1HZ__STATE									BIT_FIELD__SET(HMC6343__OP_MODE_2_REG__MR__MASK, 0x0L)
#define HMC6343__OP_MODE_2_REG__MR__5HZ__STATE									BIT_FIELD__SET(HMC6343__OP_MODE_2_REG__MR__MASK, 0x1L)
#define HMC6343__OP_MODE_2_REG__MR__10HZ__STATE									BIT_FIELD__SET(HMC6343__OP_MODE_2_REG__MR__MASK, 0x2L)
#define HMC6343__OP_MODE_2_REG__MR__NA__STATE									BIT_FIELD__SET(HMC6343__OP_MODE_2_REG__MR__MASK, 0x3L)

#define HMC6343__OP_MODE_2_REG__SETUP											(\
																					HMC6343__OP_MODE_2_REG__RESERVED__STATE |\
																					HMC6343__OP_MODE_2_REG__MR__10HZ__STATE\
																				)
//
//	name:		SN_LSB_REG
//	offset:		0x06
//	access:		R/-
//				???? ????	: R/-- : DATA			Serial Number LSB
#define HMC6343__SN_LSB__REG													0x06

#define HMC6343__SN_LSB__REG__DATA__MASK										BIT_FIELD__MASK(0xFF)

//
//	name:		SN_MSB_REG
//	offset:		0x07
//	access:		R/-
//				???? ????	: R/-- : DATA			Serial Number MSB
#define HMC6343__SN_MSB__REG													0x07

#define HMC6343__SN_MSB__REG__DATA__MASK										BIT_FIELD__MASK(0xFF)

//
//	name:		DATE_CODE_YEAR_REG
//	offset:		0x08
//	access:		R/-
//				???? ????	: R/-- : DATA			Package Date Code:  Last Two Digits of the Year
#define HMC6343__DATE_CODE_YEAR_REG												0x08

#define HMC6343__DATE_CODE_YEAR_REG__DATA__MASK									BIT_FIELD__MASK(0xFF)

//
//	name:		DATE_CODE_WEEK_REG
//	offset:		0x09
//	access:		R/-
//				???? ????	: R/-- : DATA			Package Date Code: Fiscal Week
#define HMC6343__DATE_CODE_WEEK_REG												0x09

#define HMC6343__DATE_CODE_WEEK_REG__DATA__MASK									BIT_FIELD__MASK(0xFF)

//
//	name:		DEVIATION_LSB_REG
//	offset:		0x0A
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : ANGLE			Deviation Angle ( ±1800) in tenths of a degree
#define HMC6343__DEVIATION_LSB_REG												0x0A

#define HMC6343__DEVIATION_LSB_REG__ANGLE__MASK									BIT_FIELD__MASK(0xFF)

//
//	name:		DEVIATION_MSB_REG
//	offset:		0x0B
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : ANGLE			Deviation Angle ( ±1800) in tenths of a degree
#define HMC6343__DEVIATION_MSB_REG												0x0B

#define HMC6343__DEVIATION_MSB_REG__ANGLE__MASK									BIT_FIELD__MASK(0xFF)

																				//
//	name:		VARIATION_LSB_REG
//	offset:		0x0C
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : ANGLE			Variation Angle ( ±1800) in tenths of a degree
#define HMC6343__VARIATION_LSB_REG												0x0C

#define HMC6343__DEVIATION_LSB_REG__ANGLE__MASK									BIT_FIELD__MASK(0xFF)

//	name:		VARIATION_MSB_REG
//	offset:		0x0D
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : ANGLE			Variation Angle ( ±1800) in tenths of a degree
#define HMC6343__VARIATION_MSB_REG												0x0D

#define HMC6343__DEVIATION_MSB_REG__ANGLE__MASK									BIT_FIELD__MASK(0xFF)

//	name:		X_OFFSET_LSB_REG
//	offset:		0x0E
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : OFFSET			Hard-Iron Calibration Offset for the X-axis
#define HMC6343__X_OFFSET_LSB_REG												0x0E

#define HMC6343__X_OFFSET_LSB_REG__OFFSET__MASK									BIT_FIELD__MASK(0xFF)

//	name:		X_OFFSET_MSB_REG
//	offset:		0x0F
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : OFFSET			Hard-Iron Calibration Offset for the X-axis
#define HMC6343__X_OFFSET_MSB_REG												0x0F

#define HMC6343__X_OFFSET_MSB_REG__OFFSET__MASK									BIT_FIELD__MASK(0xFF)

//	name:		Y_OFFSET_LSB_REG
//	offset:		0x10
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : OFFSET			Hard-Iron Calibration Offset for the Y-axis
#define HMC6343__Y_OFFSET_LSB_REG												0x10

#define HMC6343__Y_OFFSET_LSB_REG__OFFSET__MASK									BIT_FIELD__MASK(0xFF)

//	name:		Y_OFFSET_MSB_REG
//	offset:		0x11
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : OFFSET			Hard-Iron Calibration Offset for the Y-axis
#define HMC6343__Y_OFFSET_MSB_REG												0x11

#define HMC6343__Y_OFFSET_MSB_REG__OFFSET__MASK									BIT_FIELD__MASK(0xFF)

//	name:		Z_OFFSET_LSB_REG
//	offset:		0x12
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : OFFSET			Hard-Iron Calibration Offset for the Z-axis
#define HMC6343__Z_OFFSET_LSB_REG												0x12

#define HMC6343__Z_OFFSET_LSB_REG__OFFSET__MASK									BIT_FIELD__MASK(0xFF)

//	name:		Z_OFFSET_MSB_REG
//	offset:		0x13
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : OFFSET			Hard-Iron Calibration Offset for the Z-axis
#define HMC6343__Z_OFFSET_MSB_REG												0x13

#define HMC6343__Z_OFFSET_MSB_REG__OFFSET__MASK									BIT_FIELD__MASK(0xFF)

//	name:		HEADING_FILTER_LSB_REG
//	offset:		0x14
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : FILTER			Heading IIR Filter (0x00 to 0x0F typical)
#define HMC6343__HEADING_FILTER_LSB_REG											0x14

#define HMC6343__HEADING_FILTER_LSB_REG__MASK									BIT_FIELD__MASK(0xFF)
#define HMC6343__HEADING_FILTER_LSB_REG__SETUP									BIT_FIELD__SET(HMC6343__HEADING_FILTER_LSB_REG__MASK, 15L)

//	name:		HEADING_FILTER_MSB_REG
//	offset:		0x15
//	access:		R/W-
//	default:	0000 0000											: default
//				0000 0000	: R/W- : FILTER			Heading IIR Filter (set at zero)
#define HMC6343__HEADING_FILTER_MSB_REG											0x15

#define HMC6343__HEADING_FILTER_MSB_REG__MASK									BIT_FIELD__MASK(0xFF)


/*
HMC6343 Interface Commands/Responses

Command		Arg1		Arg2	Response Bytes				Command Description
Byte
-------		----		----	--------------				-------------------
(0x40)							MSB/LSB Data (6 Bytes)		Post Accel Data. AxMSB, AxLSB, AyMSB, AyLSB, AzMSB, AzLSB
(0x45)							MSB/LSB Data (6 Bytes)		Post Mag Data. MxMSB, MxLSB, MyMSB, MyLSB, MzMSB, MzLSB
(0x50)							MSB/LSB Data (6 Bytes)		Post Heading Data. HeadMSB, HeadLSB, PitchMSB, PitchLSB, RollMSB, RollLSB
(0x55)							MSB/LSB Data (6 Bytes)		Post Tilt Data. PitchMSB, PitchLSB, RollMSB, RollLSB, TempMSB, TempLSB
(0x65)							Post OP Mode 1				Read the current value of OP Mode 1
(0x71)														Enter User Calibration Mode
(0x72)														Level Orientation (X=forward, +Z=up) (default)
(0x73)														Upright Sideways Orientation (X=forward, Y=up)
(0x74)														Upright Flat Front Orientation (Z=forward, -X=up)
(0x75)														Enter Run Mode (from Standby Mode)
(0x76)														Enter Standby Mode (from Run Mode)
(0x7E)														Exit User Calibration Mode
(0x82)														Reset the Processor
(0x83)														Enter Sleep Mode (from Run Mode)
(0x84)														Exit Sleep Mode (to Standby Mode)
(0xE1)		EE-ADDR				Data (1 Byte)				Read from EEPROM
(0xF1)		EE-ADDR		Data								Write to EEPROM
*/

#define HMC6343__MAX_CMD_SIZE								(3)

#define HMC6343__GET_ACCEL_DATA__CMD						(0x40)
#define HMC6343__GET_ACCEL_DATA__CMD_SIZE					(1)
#define HMC6343__GET_ACCEL_DATA__DATA_SIZE					(6)

#define HMC6343__GET_MAG_DATA__CMD							(0x45)
#define HMC6343__GET_MAG_DATA__CMD_SIZE						(1)
#define HMC6343__GET_MAG_DATA__DATA_SIZE					(6)

#define HMC6343__GET_HEADING_DATA__CMD						(0x50)
#define HMC6343__GET_HEADING_DATA__CMD_SIZE					(1)
#define HMC6343__GET_HEADING_DATA__DATA_SIZE				(6)

#define HMC6343__GET_TILT_DATA__CMD							(0x55)
#define HMC6343__GET_TILT_DATA__CMD_SIZE					(1)
#define HMC6343__GET_TILT_DATA__DATA_SIZE					(6)

#define HMC6343__GET_OP_MODE1_REG_DATA__CMD					(0x65)
#define HMC6343__GET_OP_MODE1_REG_DATA__CMD_SIZE			(1)
#define HMC6343__GET_OP_MODE1_REG_DATA__DATA_SIZE			(1)

#define HMC6343__ENTER_CAL_MODE__CMD						(0x71)
#define HMC6343__ENTER_CAL_MODE__CMD_SIZE					(1)

#define HMC6343__SET_LEVEL_ORIENT__CMD						(0x72)
#define HMC6343__SET_LEVEL_ORIENT__CMD_SIZE					(1)

#define HMC6343__SET_UP_SIDEWAYS_ORIENT__CMD				(0x73)
#define HMC6343__SET_UP_SIDEWAYS_ORIENT__CMD_SIZE			(1)

#define HMC6343__SET_UP_FLAT_ORIENT__CMD					(0x74)
#define HMC6343__SET_UP_FLAT_ORIENT__CMD_SIZE				(1)

#define HMC6343__ENTER_RUN_MODE__CMD						(0x75)
#define HMC6343__ENTER_RUN_MODE__CMD_SIZE					(1)

#define HMC6343__ENTER_STANDBY_MODE__CMD					(0x76)

#define HMC6343__ENTER_STANDBY_MODE__CMD_SIZE				(1)

#define HMC6343__EXIT_CAL_MODE__CMD							(0x7E)
#define HMC6343__EXIT_CAL_MODE__CMD_SIZE					(1)

#define HMC6343__RESET_CPU__CMD								(0x82)
#define HMC6343__RESET_CPU__CMD_SIZE						(1)

#define HMC6343__ENTER_SLEEP_MODE__CMD						(0x83)
#define HMC6343__ENTER_SLEEP_MODE__CMD_SIZE					(1)

#define HMC6343__EXIT_SLEEP_MODE__CMD						(0x84)
#define HMC6343__EXIT_SLEEP_MODE__CMD_SIZE					(1)

#define HMC6343__READ_EEPROM__CMD							(0xE1)
#define HMC6343__READ_EEPROM__CMD_SIZE						(2)
#define HMC6343__READ_EEPROM__DATA_SIZE						(1)

#define HMC6343__WRITE_EEPROM__CMD							(0xF1)
#define HMC6343__WRITE_EEPROM__CMD_SIZE						(HMC6343__MAX_CMD_SIZE)


//*** global function prototypes *********************************************

void	HMC6343_Setup( void );
void	HMC6343_Shutdown( void );
void	HMC6343_SendCommand( U8 cmd );
S16		HMC6343_GetHeading( void );

#endif // _HMC6343_H
