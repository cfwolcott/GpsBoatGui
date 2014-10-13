//******************************************************************************
//
//	BitField.h
//
//	Header for bit-fields.
//
//	NOTE: a bit-field of this form is defined by a mask, consisting of
//		a string of N consecutive ones, where N=1,2,3...
//		0000000011111100
//
//	NOTE: these macros are great for constant masks, but are sub-optimal for
//		non-constant masks, generating a lot of code per invocation.  you've
//		been warned.
//
/*******************************************************************************
*                                                                              *
* Copyright (c) 2007-2014 N2 Imaging Systems, LLC                              *
*                                                                              *
* N2 Imaging Systems, L.L.C. PROPRIETARY DATA                                  *
* is included in the information disclosed herein. Recipient, by accepting     *
* this document, agrees that neither this document nor any part of the         *
* information disclosed herein shall be used or disclosed to others for any    *
* purpose, except as specifically authorized in writing by N2 Imaging          *
* Systems, L.L.C.                                                              *
*                                                                              *
* WARNING: INFORMATION SUBJECT TO U.S. EXPORT CONTROLS                         *
* This document may contain Technical Data controlled under the U.S.           *
* International Traffic in Arms Regulations ("ITAR") or technology subject     *
* to the U.S. Export Administration Regulations ("EAR") and may not be         *
* exported from the United States or disclosed to any Foreign Person, by any   *
* means, except as in accordance with the applicable regulations.  Violations  *
* of U.S. export control laws and regulations are subject to severe civil and  *
* criminal penalties.  Recipient shall include this notice with any reproduced *
* portion of this document.                                                    *
*                                                                              *
*******************************************************************************/

#ifndef _BIT_FIELD_H_
#define _BIT_FIELD_H_

//*** global defines and typedefs **********************************************

// retrieves the lowest bit set in a mask
//	0000000011111100	mask
//	0000000000000100	BIT_FIELD__LSB(mask)
//
// NOTE:
//	the quantity ((mask) & -(mask)) strips all but the least
//	significant bit of the mask, but is only supported in a
//	two's complement environment.
//	((mask) & (~(mask) + 1)) should work everywhere.
#define BIT_FIELD__LSB(mask)	((mask) & (~(mask) + 1))

// retrieves the largest bit set in a mask
//		0000000011111100	mask
//		0000000010000000	BIT_FIELD__MSB(mask)
//
#define BIT_FIELD__MSB(mask)										((mask) ^ (((mask) >> 1) & (mask)))

// retrieves the LSB of the next field.
//	0000000011111100	mask
//	0000000100000000	BIT_FIELD__NEXT_FIELD_LSB(mask)
#define BIT_FIELD__NEXT_FIELD_LSB(mask)		((mask) + BIT_FIELD__LSB(mask))

typedef struct
{
	U16 u16BITS;
	U16 u16SHIFT;
	U32 u32MASK;
} BIT_FIELD;

// the mask of bits for the field
// this is only necessary because C prefers unsigned
// masks, while assembly prefers it unspecified.
#define BIT_FIELD__MASK(mask)	(mask##UL)

// determines the bit position to the lowest bit set in a mask
// mask can be up to a 32-bit value
// no bits set in the mask returns 0
#define BIT_FIELD__SHIFT(mask)		BIT_FIELD__SHIFT_16(BIT_FIELD__LSB(mask))
#define BIT_FIELD__SHIFT_16(mask)	(((((mask) & 0xFFFF0000) != 0) << 4) | BIT_FIELD__SHIFT_8(mask))
#define BIT_FIELD__SHIFT_8(mask)	(((((mask) & 0xFF00FF00) != 0) << 3) | BIT_FIELD__SHIFT_4(mask))
#define BIT_FIELD__SHIFT_4(mask)	(((((mask) & 0xF0F0F0F0) != 0) << 2) | BIT_FIELD__SHIFT_2(mask))
#define BIT_FIELD__SHIFT_2(mask)	(((((mask) & 0xCCCCCCCC) != 0) << 1) | BIT_FIELD__SHIFT_1(mask))
#define BIT_FIELD__SHIFT_1(mask)	(((((mask) & 0xAAAAAAAA) != 0) << 0))

// counts the number of bits set in a mask
// mask can be up to a 32-bit value
#define BIT_FIELD__BITS(mask)		BIT_FIELD__BITS_1(mask)
#define BIT_FIELD__BITS_1(mask)		BIT_FIELD__BITS_2((((mask) >> 1) & 0x55555555) + ((mask) & 0x55555555))
#define BIT_FIELD__BITS_2(mask)		BIT_FIELD__BITS_4((((mask) >> 2) & 0x33333333) + ((mask) & 0x33333333))
#define BIT_FIELD__BITS_4(mask)		BIT_FIELD__BITS_8((((mask) >> 4) & 0x0F0F0F0F) + ((mask) & 0x0F0F0F0F))
#define BIT_FIELD__BITS_8(mask)		BIT_FIELD__BITS_16((((mask) >> 8) & 0x00FF00FF) + ((mask) & 0x00FF00FF))
#define BIT_FIELD__BITS_16(mask)	((((mask) >> 16) & 0x0000FFFF) + ((mask) & 0x0000FFFF))

// injection and extraction of a field
// NOTE: mask must be non-zero
#define BIT_FIELD__SET(mask, state)			(((state) * BIT_FIELD__LSB(mask)) & (mask))
#define BIT_FIELD__GET(mask, state)			(((state) & (mask)) / BIT_FIELD__LSB(mask))
#define BIT_FIELD__GET_SIGNED(mask, state)	((BIT_FIELD__GET((mask), (state)) ^ BIT_FIELD__MSB(mask)) - BIT_FIELD__MSB(mask))

// multi-field injection and extraction
// the first number indicates the total number of fields (0-based).
// the second number indicates the field to operate upon (0-based).
// higher-numbered masks are more significant; lower numbered masks are less
// significant.
#define BIT_FIELD__SET_0_0(mask, state)							BIT_FIELD__SET(mask, state)
#define BIT_FIELD__SET_1_0(mask1, mask0, state)					BIT_FIELD__SET_0_0(mask0, state)
#define BIT_FIELD__SET_1_1(mask1, mask0, state)					BIT_FIELD__SET_0_0(mask1, (state) / (1 + BIT_FIELD__GET(mask0, ~0)))
#define BIT_FIELD__SET_2_0(mask2, mask1, mask0, state)			BIT_FIELD__SET_0_0(mask0, state)
#define BIT_FIELD__SET_2_1(mask2, mask1, mask0, state)			BIT_FIELD__SET_1_1(mask1, mask0, state)
#define BIT_FIELD__SET_2_2(mask2, mask1, mask0, state)			BIT_FIELD__SET_1_1(mask2, mask1, (state) / (1 + BIT_FIELD__GET(mask0, ~0)))
#define BIT_FIELD__SET_3_0(mask3, mask2, mask1, mask0, state)	BIT_FIELD__SET_0_0(mask0, state)
#define BIT_FIELD__SET_3_1(mask3, mask2, mask1, mask0, state)	BIT_FIELD__SET_1_1(mask1, mask0, state)
#define BIT_FIELD__SET_3_2(mask3, mask2, mask1, mask0, state)	BIT_FIELD__SET_2_2(mask2, mask1, mask0, state)
#define BIT_FIELD__SET_3_3(mask3, mask2, mask1, mask0, state)	BIT_FIELD__SET_2_2(mask3, mask2, mask1, (state) / (1 + BIT_FIELD__GET(mask0, ~0)))

#define BIT_FIELD__GET_0_0(mask, state)							BIT_FIELD__GET(mask, state)
#define BIT_FIELD__GET_1_0(mask1, mask0, state)					BIT_FIELD__GET_0_0(mask0, state)
#define BIT_FIELD__GET_1_1(mask1, mask0, state)					(BIT_FIELD__GET_0_0(mask1, state) * (1 + BIT_FIELD__GET(mask0, ~0)))
#define BIT_FIELD__GET_2_0(mask2, mask1, mask0, state)			BIT_FIELD__GET_0_0(mask0, state)
#define BIT_FIELD__GET_2_1(mask2, mask1, mask0, state)			BIT_FIELD__GET_1_1(mask1, mask0, state)
#define BIT_FIELD__GET_2_2(mask2, mask1, mask0, state)			(BIT_FIELD__GET_1_1(mask2, mask1, state) * (1 + BIT_FIELD__GET(mask0, ~0)))
#define BIT_FIELD__GET_3_0(mask3, mask2, mask1, mask0, state)	BIT_FIELD__GET_0_0(mask0, state)
#define BIT_FIELD__GET_3_1(mask3, mask2, mask1, mask0, state)	BIT_FIELD__GET_1_1(mask1, mask0, state)
#define BIT_FIELD__GET_3_2(mask3, mask2, mask1, mask0, state)	BIT_FIELD__GET_2_2(mask2, mask1, mask0,state)
#define BIT_FIELD__GET_3_3(mask3, mask2, mask1, mask0, state)	(BIT_FIELD__GET_2_2(mask3, mask2, mask1, state) * (1 + BIT_FIELD__GET(mask0, ~0)))

// reverses the bits set in a mask
// mask can be up to a 32-bit value
#define BIT_FIELD__REVERSE(mask)	BIT_FIELD__REVERSE_1(mask)
#define BIT_FIELD__REVERSE_1(mask)	BIT_FIELD__REVERSE_2((((mask) >> 1) & 0x55555555) | (((mask) & 0x55555555) << 1))
#define BIT_FIELD__REVERSE_2(mask)	BIT_FIELD__REVERSE_4((((mask) >> 2) & 0x33333333) | (((mask) & 0x33333333) << 2))
#define BIT_FIELD__REVERSE_4(mask)	BIT_FIELD__REVERSE_8((((mask) >> 4) & 0x0F0F0F0F) | (((mask) & 0x0F0F0F0F) << 4))
#define BIT_FIELD__REVERSE_8(mask)	BIT_FIELD__REVERSE_16((((mask) >> 8) & 0x00FF00FF) | (((mask) & 0x00FF00FF) << 8))
#define BIT_FIELD__REVERSE_16(mask)	((((mask) >> 16) & 0x0000FFFF) | (((mask) & 0x0000FFFF) << 16))

// injection and extraction of a reversed field
// NOTE: mask must be non-zero
#define BIT_FIELD__SET_REVERSE(mask, state)	((BIT_FIELD__REVERSE(state) / BIT_FIELD__LSB(BIT_FIELD__REVERSE(mask))) & (mask))
#define BIT_FIELD__GET_REVERSE(mask, state)	(BIT_FIELD__REVERSE((state) & (mask)) / BIT_FIELD__LSB(BIT_FIELD__REVERSE(mask)))

// multi-field injection and extraction of reversed bits
// the first number indicates the total number of fields (0-based).
// the second number indicates the field to operate upon (0-based).
// higher-numbered masks are more significant; lower numbered masks are less
// significant.
#define BIT_FIELD__SET_REVERSE_0_0(mask, state)							BIT_FIELD__SET_REVERSE(mask, state)
#define BIT_FIELD__SET_REVERSE_1_0(mask1, mask0, state)					BIT_FIELD__SET_REVERSE_0_0(mask0, state)
#define BIT_FIELD__SET_REVERSE_1_1(mask1, mask0, state)					BIT_FIELD__SET_REVERSE_0_0(mask1, (state) / (1 + BIT_FIELD__GET(mask0, ~0)))
#define BIT_FIELD__SET_REVERSE_2_0(mask2, mask1, mask0, state)			BIT_FIELD__SET_REVERSE_0_0(mask0, state)
#define BIT_FIELD__SET_REVERSE_2_1(mask2, mask1, mask0, state)			BIT_FIELD__SET_REVERSE_1_1(mask1, mask0, state)
#define BIT_FIELD__SET_REVERSE_2_2(mask2, mask1, mask0, state)			BIT_FIELD__SET_REVERSE_1_1(mask2, mask1, (state) / (1 + BIT_FIELD__GET(mask0, ~0)))
#define BIT_FIELD__SET_REVERSE_3_0(mask3, mask2, mask1, mask0, state)	BIT_FIELD__SET_REVERSE_0_0(mask0, state)
#define BIT_FIELD__SET_REVERSE_3_1(mask3, mask2, mask1, mask0, state)	BIT_FIELD__SET_REVERSE_1_1(mask1, mask0, state)
#define BIT_FIELD__SET_REVERSE_3_2(mask3, mask2, mask1, mask0, state)	BIT_FIELD__SET_REVERSE_2_2(mask2, mask1, mask0, state)
#define BIT_FIELD__SET_REVERSE_3_3(mask3, mask2, mask1, mask0, state)	BIT_FIELD__SET_REVERSE_2_2(mask3, mask2, mask1, (state) / (1 + BIT_FIELD__GET(mask0, ~0)))

#define BIT_FIELD__GET_REVERSE_0_0(mask, state)							BIT_FIELD__GET_REVERSE(mask, state)
#define BIT_FIELD__GET_REVERSE_1_0(mask1, mask0, state)					BIT_FIELD__GET_REVERSE_0_0(mask0, state)
#define BIT_FIELD__GET_REVERSE_1_1(mask1, mask0, state)					(BIT_FIELD__GET_REVERSE_0_0(mask1, state) * (1 + BIT_FIELD__GET(mask0, ~0)))
#define BIT_FIELD__GET_REVERSE_2_0(mask2, mask1, mask0, state)			BIT_FIELD__GET_REVERSE_0_0(mask0, state)
#define BIT_FIELD__GET_REVERSE_2_1(mask2, mask1, mask0, state)			BIT_FIELD__GET_REVERSE_1_1(mask1, mask0, state)
#define BIT_FIELD__GET_REVERSE_2_2(mask2, mask1, mask0, state)			(BIT_FIELD__GET_REVERSE_1_1(mask2, mask1, state) * (1 + BIT_FIELD__GET(mask0, ~0)))
#define BIT_FIELD__GET_REVERSE_3_0(mask3, mask2, mask1, mask0, state)	BIT_FIELD__GET_REVERSE_0_0(mask0, state)
#define BIT_FIELD__GET_REVERSE_3_1(mask3, mask2, mask1, mask0, state)	BIT_FIELD__GET_REVERSE_1_1(mask1, mask0, state)
#define BIT_FIELD__GET_REVERSE_3_2(mask3, mask2, mask1, mask0, state)	BIT_FIELD__GET_REVERSE_2_2(mask2, mask1, mask0, state)
#define BIT_FIELD__GET_REVERSE_3_3(mask3, mask2, mask1, mask0, state)	(BIT_FIELD__GET_REVERSE_2_2(mask3, mask2, mask1, state) * (1 + BIT_FIELD__GET(mask0, ~0)))

//*** global function prototypes ***********************************************

int BIT_FIELD_Bits(U32 u32Mask);

int BIT_FIELD_Shift(U32 u32Mask);

#endif // _BIT_FIELD_H_

