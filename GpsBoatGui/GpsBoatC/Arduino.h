// arduino.h
// Provides an I2C interface to a connected Arduino
// Note: The arduino must be running the correct sketch and is configured as
//       a slave I2C device


#ifndef ARDUINO_h
#define ARDUINO_h

#include "includes.h"

typedef enum
{
	ARDUINO_REG_VERSION,
	ARDUINO_REG_STEERING,
	ARDUINO_REG_ESC,
	ARDUINO_REG_EXTRA_LED,

	ARDUINO_REG_MAX
} E_ARDUINO_REG;

//------------------------------------------------------------------------------
class Arduino
{
	public:
		Arduino();
		bool Init( U8 i2c_addr );
		void SetReg( E_ARDUINO_REG reg, U8 val );
		U8 GetReg( E_ARDUINO_REG reg );
	private:
		int i2c_fd;
};

#endif
