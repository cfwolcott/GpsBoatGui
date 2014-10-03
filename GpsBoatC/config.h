#ifndef CONFIG_H
#define CONFIG_H

//-----------------------------------------------------------------------------------
// DEFINES
//#define MAG_VAR              7    // Florida
#define MAG_VAR                -13    // California

// ********************
// *** GPS Settings ***
// ********************

// Set this to 1 if you want to program a specific HOME GPS location. Otherwise, the home
// positoin will be determined where ever the GPS first gets a lock.
#define USE_HOME_POSITION    0

// The designated HOME position. i.e. a location on the beach where you know the GPS coordinates
#define WAYPOINT_HOME_LAT    0
#define WAYPOINT_HOME_LON    0

// Number of way points to program 
// Note: Waypoint 1 (0) is always the current location and will be automatically
// determined when the GPS gets a stable lock UNLESS USE_HOME_POSITION above is set to 1.

// Number of waypoints to navigate to.
// Minimum is 2 and max is 10
// Include USE_HOME_POSITION above. Example, starting location plus 4 other waypoints (A, B, C, D) would mean NUM_WAY_POINTS = 5
#define NUM_WAY_POINTS        2

// Way points to navigate to
#define WAYPOINT_A_LAT    33.714740
#define WAYPOINT_A_LON    -117.802270

#define WAYPOINT_B_LAT    33.714953
#define WAYPOINT_B_LON    -117.802114

#define WAYPOINT_C_LAT    34.3910217
#define WAYPOINT_C_LON    -119.5187683

#define WAYPOINT_D_LAT    34.3908463
#define WAYPOINT_D_LON    -119.5186005

#define WAYPOINT_E_LAT    33.71476
#define WAYPOINT_E_LON    -117.802909

#define WAYPOINT_F_LAT    33.714697
#define WAYPOINT_F_LON    -117.802753

#define WAYPOINT_G_LAT    33.71476
#define WAYPOINT_G_LON    -117.802909

#define WAYPOINT_H_LAT    33.714697
#define WAYPOINT_H_LON    -117.802753

#define WAYPOINT_I_LAT    33.71476
#define WAYPOINT_I_LON    -117.802909

#define WAYPOINT_J_LAT    33.714697
#define WAYPOINT_J_LON    -117.802753

// Initial +/- degrees to bearing tolerance
#define DEGREES_TO_BEARING_TOLERANCE    10.0

// Set this to the maximum distance to a waypoint before we switch to the next waypoint
#define SWITCH_WAYPOINT_DISTANCE        2.0

#define PRINT_MSGS            0

// COMPASS --------------------------
#define USE_COMPASS_CALIBRATION			0
#define USE_COMPASS_TILT_COMPENSATION	1

// GPS ------------------------------
#define USE_GPS_TIME_INFO     0

#define DO_GPS_TEST           0

#define USE_ULTIMATE_GPS      0
#define USE_PHAROS_GPS        1

#if USE_ULTIMATE_GPS
#define GPS_BAUD		  		9600
#else
#define GPS_BAUD		 		4800
#endif

// Arduino ---------------------------
#define USE_ARDUINO				0
#define ARDUINO_I2C_ADDR		(0x04)

// GPS Data input Pins will always be the Arduino's own Rx/Tx pins. Disconnect before programming!
// Had to do this because SoftSerial library is not compatible with Servo library!

// How many seconds to wait after GPS locks before starting navigation
#define GPS_STABALIZE_LOCK_TIME    1    // seconds

// *******************************************************************
// ESC "servo"

// These define the throttle channel settings. They are represented by a a number in the range 0 - 180 which are in degrees of shaft movement.
#define SPEED_BACKUP       180
#define SPEED_STOP         170
#define SPEED_25_PERCENT   160
#define SPEED_50_PERCENT   150
#define SPEED_100_PERCENT  140

// How fast to set the new ESC setting - allows gradual speed changes
#define SPEED_STEP_SIZE    1    // must be a common multiple of ALL servo settings!
#define SPEED_STEP_DELAY   200  // milliseconds between each new step size

// Rudder Servo
#define RUDDER_CENTER      75
#define RUDDER_FULL_LEFT   35
#define RUDDER_LEFT        RUDDER_FULL_LEFT
#define RUDDER_FULL_RIGHT  110
#define RUDDER_RIGHT       RUDDER_FULL_RIGHT


// How fast to set the new rudder setting - allows gradual rudder changes
#define RUDDER_STEP_SIZE    1    // must be a common multiple of ALL servo settings!
#define RUDDER_STEP_DELAY   20  // milliseconds between each new step size

// If rudder is going wrong way, set this to true
#define RUDDER_REVERSE      false

#endif
