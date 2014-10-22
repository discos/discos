#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

# Message portions
headers = ('#', '!', '?', '@')
closers = ('\r\n', '\n\r', '\r', '\n')
socket_timeout = 3.0

# Shelve file that share the SRP position. The setpos command sets these vaules
# and the getpos command read from here.
db_name = 'positions.db'

# Available commands
commands = (
        'stow', 
        'setup', 
        'getstatus', 
        'setpos', 
        'getpos',
        'getspar',
        'getappstatus',
        'getstatus',
        'clean',
)

# Available properties
properties = (
        'actPos',
        'engTemperature',
        'counturingErr',
        'torquePerc',
        'engCurrent',
        'engVoltage',
        'driTemperature',
        'utilizationPerc',
        'dcTemperature',
        'driverStatus',
        'errorCode',
        'status'
)

filtered = ( 
        'getstatus',
        'foo'
)


# Minor servo type
app_nr = {
        0: 'PFP', # Prime Focus Positioner
        1: 'SRP', # SubReflector Positioner
        2: 'GFR', # Gregorian Feed Rotator
        3: 'M3R', # Mirror 3 Rotator
}

# Number of axis of each minor servo
number_of_axis = {
        'PFP': 3,
        'SRP': 6,
        'GFR': 1,
        'M3R': 1,
}

# Number of slaves of each minor servo
number_of_slaves = {
        'PFP': 5,
        'SRP': 7,
        'GFR': 3,
        'M3R': 3,
}

# Response to generate
response_types = (
        'expected_ack', 
        'mixed', 
)

# Available servers
servers = {
        'MSCUSimulator': ('10.0.2.15', 10000), 
        'MSCU': ('192.168.3.14', 10000)
}

time_stamp = 134770759071750080 # Completion timestamp

# Engine Temperature Indexes
ENG_TEMPERATURE_IDX = 63
ENG_TEMPERATURE_SUB = 0

# Counturing Error Indexes
COUNTURING_ERR_IDX = 1220
COUNTURING_ERR_SUB = 4

# Torque Percentage Indexes
TORQUE_PERC_IDX = 56
TORQUE_PERC_SUB = 0

# Engine Voltage Indexes
ENG_VOLTAGE_IDX = 53
ENG_VOLTAGE_SUB = 0

# Engine Current Indexes
ENG_CURRENT_IDX = 54
ENG_CURRENT_SUB = 0

# Driver Temperature Indexes
DRI_TEMPERATURE_IDX = 61
DRI_TEMPERATURE_SUB = 0

# Utilization Percentage Indexes
UTILIZATION_PERC_IDX = 64
UTILIZATION_PERC_SUB = 0

# Drive Cabinet Temperature Indexes
DC_TEMPERATURE_IDX = 3005
DC_TEMPERATURE_SUB = 0

# Driver Status Indexes
DRIVER_STATUS_IDX = 183
DRIVER_STATUS_SUB = 0

# Error Code Indexes
ERROR_CODE_IDX = 168
ERROR_CODE_SUB = 1

# Positive and Negative Limit Code Indexes
POS_LIMIT_IDX = 1223
POS_LIMIT_SUB = 0
NEG_LIMIT_IDX = 1224
NEG_LIMIT_SUB = 0

# Acceleration Code Indexes
ACCELERATION_IDX = 1250
ACCELERATION_SUB = 0

# Maximum speed Code Indexes
MAX_SPEED_IDX = 1240
MAX_SPEED_SUB = 0


app_state_max = 5
cab_state_max = 8
