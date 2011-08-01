#include "ReceiverControl.h"

using IRA::ReceiverControl; 
using IRA::ReceiverControlEx;
using IRA::FetValues;
using IRA::FetValue;
using IRA::StageValues;


ReceiverControl::ReceiverControl(
        const std::string dewar_ip,
        const unsigned short dewar_port, 
        const std::string lna_ip, 
        const unsigned short lna_port, 
        const unsigned short number_of_feeds,
        const BYTE dewar_madd,
        const BYTE dewar_sadd,
        const BYTE lna_madd,
        const BYTE lna_sadd,
        bool reliable_comm,
        const unsigned int guard_time
) throw (ReceiverControlEx) : 
    m_dewar_ip(dewar_ip),
    m_dewar_port(dewar_port),
    m_lna_ip(lna_ip),
    m_lna_port(lna_port),
    m_number_of_feeds(number_of_feeds),
    m_dewar_madd(dewar_madd),
    m_dewar_sadd(dewar_sadd),
    m_lna_madd(lna_madd),
    m_lna_sadd(lna_sadd),
    m_reliable_comm(reliable_comm), 
    m_dewar_board_ptr(NULL),
    m_lna_board_ptr(NULL),
    m_guard_time(guard_time)
{
    openConnection();
}


ReceiverControl::~ReceiverControl() 
{
    if(m_dewar_board_ptr != NULL)
        delete m_dewar_board_ptr;

    if(m_lna_board_ptr != NULL)
        delete m_lna_board_ptr;
}


void ReceiverControl::setCalibrationOn() throw (ReceiverControlEx)
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_11,    // Port Number 11
                0x01  // The value to set: 0x01 to set the mark generator to ON
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCalibrationOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setCalibrationOff() throw (ReceiverControlEx)
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_11,    // Port Number 11
                0x00  // The value to set: 0x00 to set the mark generator to OFF
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCalibrationOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isCalibrationOn() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_11     // Port Number 11
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isCalibrationOn(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCalibrationOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


double ReceiverControl::vacuum(double (*converter)(double voltage)) throw (ReceiverControlEx) 
{
    try {

        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,      // Pointer to the dewar board
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                MCB_CMD_DATA_TYPE_F32,  // Data type: 32 bit floating point
                MCB_PORT_TYPE_AD24,     // Port type: AD24
                MCB_PORT_NUMBER_00_07   // Port Number from 08 to 15
        );

        const size_t VCM_SENSOR_RAW_IDX = 2; // Index of the vacuum in the AD24 port
        // Return the vacuum value in mbar for a given voltage VALUE
        return converter != NULL ? converter(get_value(parameters, VCM_SENSOR_RAW_IDX)) : \
                            get_value(parameters, VCM_SENSOR_RAW_IDX);
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error getting the vacuum.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


double ReceiverControl::lowTemperature(double (*converter)(double voltage)) throw (ReceiverControlEx) 
{
    try {

        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,      // Pointer to the dewar board
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                MCB_CMD_DATA_TYPE_F32,  // Data type: 32 bit floating point
                MCB_PORT_TYPE_AD24,     // Port type: AD24
                MCB_PORT_NUMBER_00_07   // Port Number from 08 to 15
        );

        const size_t TEMP_CRYO_1_IDX = 0; // Index of the low cryogenic temperature in the AD24 port
        // Return the temperature value in Kelvin for a given voltage VALUE if converter != NULL, the
        // value in voltage (before the conversion) otherwise.
        return converter != NULL ? converter(get_value(parameters, TEMP_CRYO_1_IDX)) : \
                            get_value(parameters, TEMP_CRYO_1_IDX);
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error getting the low cryogenic temperature.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


double ReceiverControl::highTemperature(double (*converter)(double voltage)) throw (ReceiverControlEx) 
{
    try {

        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,      // Pointer to the dewar board
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                MCB_CMD_DATA_TYPE_F32,  // Data type: 32 bit floating point
                MCB_PORT_TYPE_AD24,     // Port type: AD24
                MCB_PORT_NUMBER_00_07   // Port Number from 08 to 15
        );

        const size_t TEMP_CRYO_2_IDX = 1; // Index of the high cryogenic temperature in the AD24 port
        // Return the temperature value in Kelvin for a given voltage VALUE if converter != NULL, the
        // value in voltage (before the conversion) otherwise.
        return converter != NULL ? converter(get_value(parameters, TEMP_CRYO_2_IDX)) : \
                            get_value(parameters, TEMP_CRYO_2_IDX);
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error getting the low cryogenic temperature.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setCoolHeadOn() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_08,    // Port Number 08
                0x01  // The value to set: 0x01 to set the cool head to ON
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCoolHeadOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setCoolHeadOff() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_08,    // Port Number 08
                0x00  // The value to set: 0x00 to set the cool head to OFF
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCoolHeadOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isCoolHeadOn() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_08     // Port Number 08
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isCoolHeadOn(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isCoolHeadOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumSensorOn() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_04,    // Port Number 04
                0x01  // The value to set: 0x01 to set the vacuum sensor to ON
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumSensorOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumSensorOff() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_04,    // Port Number 04
                0x00  // The value to set: 0x00 to set the vacuum sensor to OFF
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumSensorOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isVacuumSensorOn() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_04     // Port Number 04
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isVacuumSensorOn(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isVacuumSensorOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumPumpOn() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_05,    // Port Number 05
                0x01  // The value to set: 0x01 to set the vacuum pump to ON
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumPumpOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumPumpOff() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_05,    // Port Number 05
                0x00  // The value to set: 0x00 to set the vacuum pump to OFF
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumPumpOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isVacuumPumpOn() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_05     // Port Number 05
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isVacuumPumpOn(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isVacuumPumpOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumValveOn() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_07,    // Port Number 07
                0x01  // The value to set: 0x01 opens the valve immediately
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumValveOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumValveOff() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_07,    // Port Number 07
                0x00  // The value to set: 0x00 closes the valve
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumValveOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isVacuumValveOn() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_07     // Port Number 07
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isVacuumValveOn(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isVacuumValveOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isRemoteOn() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_26     // Port Number 26
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isRemoteOn(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isRemoteOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::selectLO1() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_00,    // Port Number 00
                0x00  // The value to set: 0x00 selects the LO1
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing selectLO1().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isLO1Selected() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_16     // Port Number 16
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isLO1Selected(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isLO1Selected().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::selectLO2() throw (ReceiverControlEx) 
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_00,    // Port Number 00
                0x01  // The value to set: 0x01 selects the LO2
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing selectLO2().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isLO2Selected() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_17     // Port Number 17
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isLO2Selected(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isLO2Selected().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isLO2Locked() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_18     // Port Number 18
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isLO2Locked(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isLO2Locked().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setSingleDishMode() throw (ReceiverControlEx) 
{
    try {
        // Turn OFF the VLBI mode on port number 14
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_14,    // Port Number 14
                0x01  // The value to set: 0x01 to set the VLBI mode to OFF
        );
        // Turn ON the single dish mode on port number 13
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_13,    // Port Number 13
                0x00  // The value to set: 0x00 to set the single dish mode to ON
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setSigleDishOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isSingleDishModeOn() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_29     // Port Number 29 (IN13)
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isSingleDishModeOn(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isSingleDishModeOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVLBIMode() throw (ReceiverControlEx) 
{
    try {
        // Turn OFF the single dish mode on port number 13
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_14,    // Port Number 14
                0x01  // The value to set: 0x01 to set the single dish mode to OFF
        );
        // Turn ON the VLBI mode on port number 14
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_14,    // Port Number 14
                0x00  // The value to set: 0x00 to set VLBI mode to ON
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVLBIMode().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isVLBIModeOn() throw (ReceiverControlEx)
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_30     // Port Number 30 (IN14)
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isVLBIModeOn(): wrong number of parameters.");

        return parameters[0] == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isVLBIModeOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::openConnection(void) throw (ReceiverControlEx)
{
    try {
        m_dewar_board_ptr = new MicroControllerBoard(m_dewar_ip, m_dewar_port, m_dewar_madd, m_dewar_sadd);
        m_dewar_board_ptr->openConnection();
    }
    catch(MicroControllerBoardEx& ex) {
        throw ReceiverControlEx("Dewar MicroControllerBoard error: " + ex.what());
    }

    try {
        m_lna_board_ptr = new MicroControllerBoard(m_lna_ip, m_lna_port, m_lna_madd, m_lna_sadd);
        m_lna_board_ptr->openConnection();
    }
    catch(MicroControllerBoardEx& ex) {
        throw ReceiverControlEx("LNA MicroControllerBoard error: " + ex.what());
    }
}


void ReceiverControl::closeConnection(void)
{
        m_dewar_board_ptr->closeConnection();
        m_lna_board_ptr->closeConnection();
}


bool ReceiverControl::isLNABoardConnectionOK(void)
{
    return (m_lna_board_ptr->getConnectionStatus() == CSocket::NOTCREATED) ? false : true;
}


bool ReceiverControl::isDewarBoardConnectionOK(void)
{
    return (m_dewar_board_ptr->getConnectionStatus() == CSocket::NOTCREATED) ? false : true;
}


FetValues ReceiverControl::fetValues(
        unsigned short feed_number, 
        unsigned short stage_number,
        double (*currentConverter)(double voltage),
        double (*voltageConverter)(double voltage)
        ) throw (ReceiverControlEx)
{
    std::string colon_selector;            // EN03: the signal that addresses the colon multiplexing of AD24
    std::string vd_selector;               // A03: it allows to select the value requested for a given stadium
    std::string id_selector;               // A03: it allows to select the value requested for a given stadium
    std::string vg_selector;               // A03: it allows to select the value requested for a given stadium
    size_t FEED_LIDX, FEED_RIDX;           // Indexes of the feed channels in the AD24 port, for a given colon
    
    FetValues values;

    if(feed_number >= m_number_of_feeds)
        throw ReceiverControlEx("ReceiverControl error: invalid feed number.");

    // Select the colon in which there is the given feed
    switch(feed_number) {
        case 0:
        case 2:
        case 4:
        case 6:
            colon_selector = "0001";
            break;
        case 1:
        case 3:
        case 5:
        case 7:
            colon_selector = "0010";
            break;
        case 8:
        case 10:
        case 12:
        case 14:
            colon_selector = "0011";
            break;
        case 9:
        case 11:
        case 13:
        case 15:
            colon_selector = "0100";
            break;
        default:
            throw ReceiverControlEx("ReceiverControl error: invalid feed number.");
    }

    // Set the index of the feed channel in the AD24
    switch(feed_number) {
        case 0:
        case 1:
        case 8:
        case 9:
            FEED_LIDX = 0;
            FEED_RIDX = 1;
            break;
        case 2:
        case 3:
        case 10:
        case 11:
            FEED_LIDX = 2;
            FEED_RIDX = 3;
            break;
        case 4:
        case 5:
        case 12:
        case 13:
            FEED_LIDX = 4;
            FEED_RIDX = 5;
            break;
        case 6:
        case 7:
        case 14:
        case 15:
            FEED_LIDX = 6;
            FEED_RIDX = 7;
            break;
        default:
            FEED_LIDX = 0;
            FEED_RIDX = 0;
            throw ReceiverControlEx("ReceiverControl error: invalid feed number.");
    }

    switch(stage_number) {
        case 1:
            vd_selector = "0000"; 
            id_selector = "0001";
            vg_selector = "0010";
            break;
        case 2:
            vd_selector = "0011";
            id_selector = "0100";
            vg_selector = "0101";
            break;
        case 3:
            vd_selector = "0110";
            id_selector = "0111";
            vg_selector = "1000";
            break;
        case 4:
            vd_selector = "1001";
            id_selector = "1010";
            vg_selector = "1011";
            break;
        case 5:
            vd_selector = "1100";
            id_selector = "1101";
            vg_selector = "1110";
            break;
        default:
            throw ReceiverControlEx("ReceiverControl error: invalid stage number.");
    }

    std::bitset<8> vd_request(colon_selector + vd_selector);
    std::bitset<8> id_request(colon_selector + id_selector);
    std::bitset<8> vg_request(colon_selector + vg_selector);

    try {

        makeRequest(
                m_lna_board_ptr,                           // Pointer to the LNA board socket
                MCB_CMD_SET_DATA,                          // Command to send
                4,                                         // Number of parameters
                MCB_CMD_DATA_TYPE_U08,                     // Data type: unsigned 08 bit
                MCB_PORT_TYPE_DIO,                         // Port type: Digital IO
                MCB_PORT_NUMBER_00_07,                     // Port Number from 00 to 07
                static_cast<BYTE>(vd_request.to_ulong())   // Value to set                   
        );

        usleep(m_guard_time);

        std::vector<BYTE> parameters = makeRequest(
                m_lna_board_ptr,        // Pointer to the LNA board
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                MCB_CMD_DATA_TYPE_F32,  // Data type: 32 bit floating point
                MCB_PORT_TYPE_AD24,     // Port type: AD24
                MCB_PORT_NUMBER_00_07   // Port Number from 08 to 15
        );

        try {
            values.VDL = voltageConverter != NULL ? voltageConverter(get_value(parameters, FEED_LIDX)) : \
                                get_value(parameters, FEED_LIDX);
            values.VDR = voltageConverter != NULL ? voltageConverter(get_value(parameters, FEED_RIDX)) : \
                                get_value(parameters, FEED_RIDX);
        }
        catch(...) {
            throw ReceiverControlEx("ReceiverControl error: unexpected exception occurs performing the VD conversion.");
        }

        makeRequest(
                m_lna_board_ptr,                           // Pointer to the LNA board
                MCB_CMD_SET_DATA,                          // Command to send
                4,                                         // Number of parameters
                MCB_CMD_DATA_TYPE_U08,                     // Data type: unsigned 08 bit
                MCB_PORT_TYPE_DIO,                         // Port type: Digital IO
                MCB_PORT_NUMBER_00_07,                     // Port Number from 00 to 07
                static_cast<BYTE>(id_request.to_ulong())   // Value to set                   
        );

        usleep(m_guard_time);

        parameters = makeRequest(
                m_lna_board_ptr,        // Pointer to the LNA
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                MCB_CMD_DATA_TYPE_F32,  // Data type: 32 bit floating point
                MCB_PORT_TYPE_AD24,     // Port type: AD24
                MCB_PORT_NUMBER_00_07   // Port Number from 08 to 15
        );

        try {
            values.IDL = currentConverter != NULL ? currentConverter(get_value(parameters, FEED_LIDX)) : \
                                get_value(parameters, FEED_LIDX);
            values.IDR = currentConverter != NULL ? currentConverter(get_value(parameters, FEED_RIDX)) : \
                                get_value(parameters, FEED_RIDX);
        }
        catch(...) {
            throw ReceiverControlEx("ReceiverControl error: unexpected exception occurs performing the ID conversion.");
        }

        makeRequest(
                m_lna_board_ptr,                           // Pointer to the LNA board
                MCB_CMD_SET_DATA,                          // Command to send
                4,                                         // Number of parameters
                MCB_CMD_DATA_TYPE_U08,                     // Data type: unsigned 08 bit
                MCB_PORT_TYPE_DIO,                         // Port type: Digital IO
                MCB_PORT_NUMBER_00_07,                     // Port Number from 00 to 07
                static_cast<BYTE>(vg_request.to_ulong())   // Value to set                   
        );

        usleep(m_guard_time);

        parameters = makeRequest(
                m_lna_board_ptr,        // Pointer to the LNA board
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                MCB_CMD_DATA_TYPE_F32,  // Data type: 32 bit floating point
                MCB_PORT_TYPE_AD24,     // Port type: AD24
                MCB_PORT_NUMBER_00_07   // Port Number from 08 to 15
        );

        try {
            values.VGL = voltageConverter != NULL ? voltageConverter(get_value(parameters, FEED_LIDX)) : \
                                get_value(parameters, FEED_LIDX);
            values.VGR = voltageConverter != NULL ? voltageConverter(get_value(parameters, FEED_RIDX)) : \
                                get_value(parameters, FEED_RIDX);
        }
        catch(...) {
            throw ReceiverControlEx("ReceiverControl error: unexpected exception occurs performing the VG conversion.");
        }

        return values;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error getting LNA values.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


StageValues ReceiverControl::stageValues(
        FetValue quantity, 
        unsigned short stage_number,
        double (*converter)(double voltage)
        ) throw (ReceiverControlEx)
{

    // Each item is a EN03 value: the signal that addresses the colon multiplexing of AD24
    std::vector<std::string> colon_selectors;  
    std::string vd_selector;               // A03: it allows to select the value requested for a given stadium
    std::string id_selector;               // A03: it allows to select the value requested for a given stadium
    std::string vg_selector;               // A03: it allows to select the value requested for a given stadium
    
    if(m_number_of_feeds == 1)
        colon_selectors.push_back("0001");
    else if(m_number_of_feeds <= 8)  {
        colon_selectors.push_back("0001");
        colon_selectors.push_back("0010");
    }
    else if(m_number_of_feeds == 9) {
        colon_selectors.push_back("0001");
        colon_selectors.push_back("0010");
        colon_selectors.push_back("0011");
    }
    else if(m_number_of_feeds > 9) {
        colon_selectors.push_back("0001");
        colon_selectors.push_back("0010");
        colon_selectors.push_back("0011");
        colon_selectors.push_back("0100");
    }

    switch(stage_number) {
        case 1:
            vd_selector = "0000"; 
            id_selector = "0001";
            vg_selector = "0010";
            break;
        case 2:
            vd_selector = "0011";
            id_selector = "0100";
            vg_selector = "0101";
            break;
        case 3:
            vd_selector = "0110";
            id_selector = "0111";
            vg_selector = "1000";
            break;
        case 4:
            vd_selector = "1001";
            id_selector = "1010";
            vg_selector = "1011";
            break;
        case 5:
            vd_selector = "1100";
            id_selector = "1101";
            vg_selector = "1110";
            break;
        default:
            throw ReceiverControlEx("ReceiverControl error: invalid stage number.");
    }

    // 3. faccio un numero di richieste pari al numero di colon_selectors, e dalle
    //    grandezze lette prelevo solo la parte relativa al mio numero di feeds (solo
    //    quelle dei feeds esistenti)

    // The following is dummy code
    std::vector<double> left, right;
    StageValues values;
    switch(quantity) {
        case DRAIN_VOLTAGE:
            values.left_channel = left; // TODO: assign the values of the quantity specified
            values.right_channel = right; // TODO: assign the values of the quantity specified
            break;
        case DRAIN_CURRENT:
            values.left_channel = left; // TODO: assign the values of the quantity specified
            values.right_channel = right; // TODO: assign the values of the quantity specified
            break;
        case GATE_VOLTAGE:
            values.left_channel = left; // TODO: assign the values of the quantity specified
            values.right_channel = right; // TODO: assign the values of the quantity specified
            break;
        default:
            throw ReceiverControlEx("ReceiverControl::stageValues(): the quantity requested does not exist.");
    }

    return values;
}


void ReceiverControl::turnLeftLNAsOn() throw (ReceiverControlEx)
{
    try {
        makeRequest(
                m_lna_board_ptr,       // Pointer to the LNA board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_08,    // Port Number 08
                0x00  // The value to set: 0x00 to turn the left LNAs ON
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing turnLeftLNAsOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::turnLeftLNAsOff() throw (ReceiverControlEx)
{
    try {
        makeRequest(
                m_lna_board_ptr,       // Pointer to the LNA board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_08,    // Port Number 08
                0x01  // The value to set: 0x01 to turn the left LNAs OFF
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing turnLeftLNAsOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::turnRightLNAsOn() throw (ReceiverControlEx)
{
    try {
        makeRequest(
                m_lna_board_ptr,       // Pointer to the LNA board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_09,    // Port Number 09
                0x00  // The value to set: 0x00 to turn the right LNAs ON
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing turnRightLNAsOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::turnRightLNAsOff() throw (ReceiverControlEx)
{
    try {
        makeRequest(
                m_lna_board_ptr,       // Pointer to the LNA board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_09,    // Port Number 09
                0x01  // The value to set: 0x01 to turn the right LNAs OFF
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing turnRightLNAsOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


std::vector<BYTE> ReceiverControl::makeRequest(MicroControllerBoard *board_ptr, BYTE command, size_t len, ...) 
    throw (MicroControllerBoardEx)
{
    va_list parameters; // A place to store the list of arguments
    va_start(parameters, static_cast<BYTE>(len)); // Initializing arguments to store all values after len
    std::vector<BYTE> vparams;
    for (size_t i=0; i < len; i++) // Loop until all numbers are added
        // Adds the next value in argument list to sum.
        vparams.push_back(static_cast<BYTE>(va_arg(parameters, int))); 
    va_end(parameters);  // Clean up the list

    m_reliable_comm ? board_ptr->send(command, vparams) \
                    : board_ptr->send(command | MCB_CMD_TYPE_NOCHECKSUM, vparams);
    vparams.clear();
    vparams = board_ptr->receive();

    return vparams; // Return the vector of parameters
}


double ReceiverControl::get_value(const std::vector<BYTE> parameters, const size_t RAW_INDEX)
{

    // 32 bit floating point length
    const size_t TYPE_LEN = 4; 
    union Bytes2Float {
        float value;
        BYTE buff[TYPE_LEN];
    } uvalue;

    std::vector<BYTE>::size_type sidx = TYPE_LEN * RAW_INDEX; 
    std::vector<BYTE>::size_type vidx = TYPE_LEN-1, idx = sidx;
    // parameters[sidx] is the index of the first byte in the requested data.
    // For instance, we get the vacuum value from the AD24 port, at the port number AD10;
    // the vacuum data type is a 32 bit floating poing, so the data is stored in a range of 4 
    // bytes. The AD10 has index 2 in the AD24, so the first of the 4 bytes of our
    // value in the parameter vector has index RAW_INDEX * 4.
    for( ; idx < sidx + TYPE_LEN; idx++) {
        uvalue.buff[vidx] = parameters[idx];
        --vidx;
    }
    return uvalue.value;
}

