#ifndef __MSPARAMETERS__
#define __MSPARAMETERS__

#include <map>
#include <string>
#include <vector>
#include "utils.h"

#define NOW getNextTime()

#define NUMBER_OF_SERVOS 4
#define STATUS_WIDTH 6
#define STREAM_PRECISION 16

using namespace IRA;

struct CDBParameters {
    DWORD NUMBER_OF_AXIS;
    DWORD NUMBER_OF_SLAVES;
    double SCALE_FACTOR;
    double DELTA_MAX;
    double SCALE_OFFSET;
    IRA::CString SERVER_IP; 
    DWORD SERVER_PORT; 
    DWORD SERVER_TIMEOUT;
    DWORD SERVO_ADDRESS;
    double ZERO;
    double PARK_POSITION;
    DWORD MIN_SPEED;
    DWORD MAX_SPEED;
    DWORD VIRTUAL_RS;
    DWORD REQUIRE_CALIBRATION;
    double EXPIRE_TIME;
    double TRACKING_DELTA;
    IRA::CString LIMITS; 
};

// Time of the last values in `second.useconds`  */
struct ExpireTime {
	ACS::doubleSeq actPos[NUMBER_OF_SERVOS];
    double timeLastActPos[NUMBER_OF_SERVOS];
	ACS::doubleSeq cmdPos[NUMBER_OF_SERVOS];
    double timeLastCmdPos[NUMBER_OF_SERVOS];
	ACS::doubleSeq posDiff[NUMBER_OF_SERVOS];
    double timeLastPosDiff[NUMBER_OF_SERVOS];
	ACS::doubleSeq actElongation[NUMBER_OF_SERVOS];
    double timeLastActElongation[NUMBER_OF_SERVOS];
	ACS::doubleSeq engTemperature[NUMBER_OF_SERVOS];
    double timeLastEngTemperature[NUMBER_OF_SERVOS];
	ACS::doubleSeq counturingErr[NUMBER_OF_SERVOS];
    double timeLastCounturingErr[NUMBER_OF_SERVOS];
	ACS::doubleSeq torquePerc[NUMBER_OF_SERVOS];
    double timeLastTorquePerc[NUMBER_OF_SERVOS];
	ACS::doubleSeq engCurrent[NUMBER_OF_SERVOS];
    double timeLastEngCurrent[NUMBER_OF_SERVOS];
	ACS::doubleSeq engVoltage[NUMBER_OF_SERVOS];
    double timeLastEngVoltage[NUMBER_OF_SERVOS];
	ACS::doubleSeq driTemperature[NUMBER_OF_SERVOS];
    double timeLastDriTemperature[NUMBER_OF_SERVOS];
	ACS::doubleSeq utilizationPerc[NUMBER_OF_SERVOS];
    double timeLastUtilizationPerc[NUMBER_OF_SERVOS];
	ACS::doubleSeq dcTemperature[NUMBER_OF_SERVOS];
    double timeLastDcTemperature[NUMBER_OF_SERVOS];
	ACS::doubleSeq driverStatus[NUMBER_OF_SERVOS];
    double timeLastDriverStatus[NUMBER_OF_SERVOS];
	ACS::doubleSeq errorCode[NUMBER_OF_SERVOS];
    double timeLastErrorCode[NUMBER_OF_SERVOS];
	unsigned long status[NUMBER_OF_SERVOS];
    double timeLastStatus[NUMBER_OF_SERVOS];
	unsigned long appState[NUMBER_OF_SERVOS];
    double timeLastAppState[NUMBER_OF_SERVOS];
	unsigned long appStatus[NUMBER_OF_SERVOS];
    double timeLastAppStatus[NUMBER_OF_SERVOS];
	unsigned long cabState[NUMBER_OF_SERVOS];
    double timeLastCabState[NUMBER_OF_SERVOS];
};


struct Offsets {
    ACS::doubleSeq user;
    ACS::doubleSeq system;
};


struct PositionItem {
    ACS::Time exe_time;
    ACS::doubleSeq position;
    Offsets offsets;
};

// Axis limits  */
struct Limits {
    double min;
    double max;
};


struct StatusParameters {
    PositionItem actual_pos;
    PositionItem actual_elongation;
    unsigned short appState;
    unsigned long appStatus;
    unsigned short cabState;
};

enum {
    APP_INITIALIZATION,
    APP_STARTUP,
    APP_LOCAL,
    APP_REMOTE_MANUAL,
    APP_REMOTE_AUTO
};

enum {
    ASTATUS_LINK,
    ASTATUS_INIT,
    ASTATUS_REMOTE,
    ASTATUS_CALIBRATED,
    ASTATUS_READY,
    ASTATUS_AUTOMATIC,
    ASTATUS_NC1,
    ASTATUS_NC2,
    ASTATUS_POWER,
    ASTATUS_ENABLE,
    ASTATUS_FAULT,
    ASTATUS_MESSAGE,
    ASTATUS_TRIP,
    ASTATUS_WARNING,
    ASTATUS_QSTOP,
    ASTATUS_ONTARGET
};

enum {
    CAB_OK,
    CAB_STARTUP,
    CAB_BLOCK_REMOVED,
    CAB_DISABLED_FROM_AIF_IN,
    CAB_SUPPLY_FAIL,
    CAB_EMERGENCY_REMOVED,
    CAB_BLOCK_ACTIVE
};

enum {
    STATUS_READY,
    STATUS_WARNING,
    STATUS_FAILURE,
    STATUS_SETUP,
    STATUS_PARKED,
    STATUS_TRACKING
};


// Number of bytes of answer buffer
#define BUFF_LIMIT 1024

// Maximum number of request attempts
#define MAX_ATTEMPTS 6

// Engine Temperature Indexes
#define ENG_TEMPERATURE_IDX 63
#define ENG_TEMPERATURE_SUB 0

// Counturing Error Indexes
#define COUNTURING_ERR_IDX 1220
#define COUNTURING_ERR_SUB 4

// Torque Percentage Indexes
#define TORQUE_PERC_IDX 56
#define TORQUE_PERC_SUB 0

// Engine Voltage Indexes
#define ENG_VOLTAGE_IDX 53
#define ENG_VOLTAGE_SUB 0

// Engine Current Indexes
#define ENG_CURRENT_IDX 54
#define ENG_CURRENT_SUB 0

// Engine Current Indexes
#define DRI_TEMPERATURE_IDX 61
#define DRI_TEMPERATURE_SUB 0

// Utilization Percentage Indexes
#define UTILIZATION_PERC_IDX 64
#define UTILIZATION_PERC_SUB 0

// Drive Cabinet Temperature Indexes
#define DC_TEMPERATURE_IDX 3005
#define DC_TEMPERATURE_SUB 0

// Driver Status Indexes (See manual page 462, section 7.8)
#define DRIVER_STATUS_IDX 183
#define DRIVER_STATUS_SUB 0

// Error Code Indexes (See manual page 553, section 8.3)
#define ERROR_CODE_IDX 168
#define ERROR_CODE_SUB 1

// Positive and Negative Limit Code Indexes
#define POS_LIMIT_IDX 1223
#define POS_LIMIT_SUB 0
#define NEG_LIMIT_IDX 1224
#define NEG_LIMIT_SUB 0
// Acceleration Code Indexes
#define ACCELERATION_IDX 1250
#define ACCELERATION_SUB 0
// Maximum speed Code Indexes
#define MAX_SPEED_IDX 1240
#define MAX_SPEED_SUB 0


// Status bit indexes to flip. In doing so the minor servo is all right when its status bits are turned all down.
//   LINK_IDX: 0
//   REMOTE_IDX: 2
//   CALIBRATED_IDX: 3
//   POWER_IDX: 8
//   ENABLE_IDX: 9
//   ONTARGET_IDX: 15
const int status_flipped_bits[] = {0, 2, 3, 8, 9, 15};
const string status_messages[16] = {
    "Link", 
    "Init", 
    "Remote", 
    "Calibration", 
    "Dummy", 
    "Dummy", 
    "Dummy", 
    "Dummy",
    "Power", 
    "Enable", 
    "Fault", 
    "Message", 
    "Trip", 
    "Warning", 
    "QStop", 
    "Moving"
};


// Set the sleep_time in useconds: for instance 2 * 10 means 2 useconds
const unsigned long scheduler_sleep_time = 20 * 10;
const unsigned long listener_sleep_time = 2 * 10;
const unsigned long status_sleep_time = 100000 * 10; // 100 ms
const unsigned long status_response_time = status_sleep_time;

// WPServoTalker parameters
#define USLEEP_TIME 5 // 5us
#define STATUS_USLEEP_TIME 500000 // 500000us = 0.5 sec
#define GUARD_TIME 0.0005 // In seconds
#define TIME_SF 1000000.0 // Time Scale Factor

#endif
