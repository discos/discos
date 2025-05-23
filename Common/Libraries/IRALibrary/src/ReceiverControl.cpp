#include "ReceiverControl.h"

using IRA::ReceiverControl; 
using IRA::ReceiverControlEx;
// using IRA::ReceiverControl::FetValues;
// using IRA::ReceiverControl::FetValue;
// using IRA::ReceiverControl::StageValues;
using IRA::any2string;

// SETMODE_SLEEP_TIME is the time to wait until the bit commutation is stable
const unsigned int SETMODE_SLEEP_TIME = 100000; // 0.1 seconds
const unsigned int SWITCH_SLEEP_TIME = 100000; // 0.1 seconds


ReceiverControl::ReceiverControl(
        const std::string dewar_ip,
        const unsigned short dewar_port, 
        const std::string lna_ip, 
        const unsigned short lna_port, 
        const unsigned int guard_time,
        const unsigned short number_of_feeds,
        const std::string switch_ip, 
        const unsigned short switch_port, 
        const BYTE dewar_madd,
        const BYTE dewar_sadd,
        const BYTE lna_madd,
        const BYTE lna_sadd,
        const BYTE switch_madd,
        const BYTE switch_sadd,
        bool reliable_comm
): 
    m_dewar_ip(dewar_ip),
    m_dewar_port(dewar_port),
    m_lna_ip(lna_ip),
    m_lna_port(lna_port),
    m_guard_time(guard_time),
    m_number_of_feeds(number_of_feeds),
    m_switch_ip(switch_ip),
    m_switch_port(switch_port),
    m_dewar_madd(dewar_madd),
    m_dewar_sadd(dewar_sadd),
    m_lna_madd(lna_madd),
    m_lna_sadd(lna_sadd),
    m_switch_madd(switch_madd),
    m_switch_sadd(switch_sadd),
    m_reliable_comm(reliable_comm), 
    m_dewar_board_ptr(NULL),
    m_lna_board_ptr(NULL),
    m_switch_board_ptr(NULL)
{
    pthread_mutex_init(&m_request_mutex, NULL);
    openConnection();
}


ReceiverControl::~ReceiverControl() 
{
    if(m_dewar_board_ptr != NULL)
        delete m_dewar_board_ptr;

    if(m_lna_board_ptr != NULL)
        delete m_lna_board_ptr;

    if(m_switch_board_ptr != NULL)
        delete m_switch_board_ptr;

    pthread_mutex_destroy(&m_request_mutex);
    pthread_mutex_destroy(&m_lna_mutex);
    pthread_mutex_destroy(&m_dewar_mutex);
    pthread_mutex_destroy(&m_switch_mutex);
}


void ReceiverControl::setCalibrationOn(
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number, 
        const BYTE value
        )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type, 
                port_type,  
                port_number, 
                value 
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCalibrationOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setCalibrationOff(
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number, 
        const BYTE value
        )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type, 
                port_type,  
                port_number, 
                value 
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCalibrationOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isCalibrationOn(
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number
        )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type, 
                port_number 
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isCalibrationOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isCalibrationOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setExtCalibrationOn(
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number, 
        const BYTE value
        )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type, 
                port_type,  
                port_number, 
                value 
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setExtCalibrationOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setExtCalibrationOff(
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number, 
        const BYTE value
        )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type, 
                port_type,  
                port_number, 
                value 
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setExtCalibrationOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isExtCalibrationOn(
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number
        )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type, 
                port_number 
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isExtCalibrationOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isExtCalibrationOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


double ReceiverControl::vacuum(
        double (*converter)(double voltage),
        const BYTE data_type,     
        const BYTE port_type,    
        const BYTE port_number, 
        const size_t raw_index
        )
{
    try {

        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,      // Pointer to the dewar board
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // Return the vacuum value in mbar for a given voltage VALUE
        return converter != NULL ? converter(get_value(parameters, raw_index)) : \
                            get_value(parameters, raw_index);
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error getting the vacuum.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


double ReceiverControl::vertexTemperature(
        double (*converter)(double voltage),
        const BYTE data_type,     
        const BYTE port_type,       
        const BYTE port_number,  
        const size_t raw_index                      
        )
{
    try {

        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,      // Pointer to the dewar board
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // Return the vertex temperature in Kelvin for a given voltage VALUE
        return converter != NULL ? converter(get_value(parameters, raw_index)) : \
                            get_value(parameters, raw_index);
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error getting the vertex temperature.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


double ReceiverControl::cryoTemperature(
        const short temperature_id,
        double (*converter)(double voltage),
        const BYTE data_type,     
        const BYTE port_type,    
        const BYTE port_number
        )
{

    const size_t raw_index = temperature_id;

    try {

        // AD8 (id=0), AD9 (id=1), AD11 (id=3), AD12 (id=4)
        if(temperature_id < 0 || temperature_id > 4) 
            throw ReceiverControlEx("Invalid temperature_id: " + any2string(temperature_id));

        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,      // Pointer to the dewar board
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // Return the temperature value in Kelvin for a given voltage VALUE if converter != NULL, the
        // value in voltage (before the conversion) otherwise.
        return converter != NULL ? converter(get_value(parameters, raw_index)) : \
                            get_value(parameters, raw_index);
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error getting the " \
                                "cryogenic temperature n. " + any2string(raw_index) + ".\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setCoolHeadOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCoolHeadOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setCoolHeadOff(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCoolHeadOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isCoolHeadSetOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isCoolHeadSetOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isCoolHeadSetOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isCoolHeadOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isCoolHeadOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isCoolHeadOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumSensorOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumSensorOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumSensorOff(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumSensorOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isVacuumSensorOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isVacuumSensorOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isVacuumSensorOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumPumpOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumPumpOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumPumpOff(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumPumpOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isVacuumPumpOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isVacuumPumpOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isVacuumPumpOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::hasVacuumPumpFault(
        const BYTE data_type,
        const BYTE port_type,
        const BYTE port_number
        )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::hasVacuumPumpFault(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing hasVacuumPumpFault().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumValveOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumValveOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVacuumValveOff(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setVacuumValveOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isVacuumValveOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isVacuumValveOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isVacuumValveOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isRemoteOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isRemoteOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isRemoteOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::selectLO1(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing selectLO1().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isLO1Selected(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isLO1Selected(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isLO1Selected().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::selectLO2(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing selectLO2().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isLO2Selected(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isLO2Selected(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isLO2Selected().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isLO2Locked(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isLO2Locked(): wrong number of parameters.");

        return parameters.front() == 0 ? true : false;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isLO2Locked().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setSingleDishMode(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number_sd,
            const BYTE port_number_vlbi,
            const BYTE value_sd,
            const BYTE value_vlbi
            )
{
    try {
        pthread_mutex_lock(&m_dewar_mutex); 
        // Turn OFF the VLBI mode on port number 14
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number_vlbi,
                value_vlbi
        );

        // Turn OFF the single dish mode on port number 13
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number_sd,
                !value_sd
        );

        // Now the bits 13 and 14 are set to 1, meaning no current is now traversing any switch magnet and no mode is currently selected
        // We need to set the bit 13 to 0, which will send a TTL impulse to the desired magnet switch, selecting the desired mode
        
        usleep(3 * SETMODE_SLEEP_TIME);
        // Turn ON the single dish mode on port number 13
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number_sd,
                value_sd
        );

        // We can switch off the TTL impulse after some time, setting the relative bit to 1 again
        usleep(3 * SETMODE_SLEEP_TIME);
        // Turn OFF the single dish mode on port number 13
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number_sd,
                !value_sd
        );
        pthread_mutex_unlock(&m_dewar_mutex); 
    }
    catch(MicroControllerBoardEx& ex) {
        pthread_mutex_unlock(&m_dewar_mutex); 
        std::string error_msg = "ReceiverControl: error performing setSigleDishOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isSingleDishModeOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isSingleDishModeOn(): wrong number of parameters.");

        return parameters.front() == 0 ? false : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isSingleDishModeOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setVLBIMode(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number_vlbi,
            const BYTE port_number_sd,
            const BYTE value_vlbi,
            const BYTE value_sd
            )
{
    try {
        pthread_mutex_lock(&m_dewar_mutex); 
        // Turn OFF the single dish mode on port number 13
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number_sd,
                value_sd
        );

        // Turn OFF the VLBI mode on port number 14
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number_vlbi,
                !value_vlbi
        );

        // Now the bits 13 and 14 are set to 1, meaning no current is now traversing any switch magnet and no mode is currently selected
        // We need to set the bit 14 to 0, which will send a TTL impulse to the desired magnet switch, selecting the desired mode
        
        usleep(3 * SETMODE_SLEEP_TIME);
        // Turn ON the VLBI mode on port number 14
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number_vlbi,
                value_vlbi
        );

        // We can switch off the TTL impulse after some time, setting the relative bit to 1 again
        usleep(3 * SETMODE_SLEEP_TIME);
        // Turn OFF the VLBI mode on port number 14
        makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number_vlbi,
                !value_vlbi
        );
        pthread_mutex_unlock(&m_dewar_mutex); 
    }
    catch(MicroControllerBoardEx& ex) {
        pthread_mutex_unlock(&m_dewar_mutex); 
        std::string error_msg = "ReceiverControl: error performing setVLBIMode().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


bool ReceiverControl::isVLBIModeOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number
            )
{
    try {
        std::vector<BYTE> parameters = makeRequest(
                m_dewar_board_ptr,     // Pointer to the dewar board
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                data_type,
                port_type,
                port_number
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("ReceiverControl::isVLBIModeOn(): wrong number of parameters.");

        return parameters.front() == 1 ? true : false;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing isVLBIModeOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::openConnection(void)
{
    try {
        m_dewar_board_ptr = new MicroControllerBoard(m_dewar_ip, m_dewar_port, m_dewar_madd, m_dewar_sadd);
        m_dewar_board_ptr->openConnection();
        pthread_mutex_init(&m_dewar_mutex, NULL);
    }
    catch(MicroControllerBoardEx& ex) {
        throw ReceiverControlEx("Dewar MicroControllerBoard error: " + ex.what());
    }

    try {
        m_lna_board_ptr = new MicroControllerBoard(m_lna_ip, m_lna_port, m_lna_madd, m_lna_sadd);
        m_lna_board_ptr->openConnection();
        pthread_mutex_init(&m_lna_mutex, NULL);
    }
    catch(MicroControllerBoardEx& ex) {
        throw ReceiverControlEx("LNA MicroControllerBoard error: " + ex.what());
    }

    if(!m_switch_ip.empty() && m_switch_port != 0)
        try {
            m_switch_board_ptr = new MicroControllerBoard(m_switch_ip, m_switch_port, m_switch_madd, m_switch_sadd);
            m_switch_board_ptr->openConnection();
            pthread_mutex_init(&m_switch_mutex, NULL);
        }
        catch(MicroControllerBoardEx& ex) {
            throw ReceiverControlEx("Switches MicroControllerBoard error: " + ex.what());
        }

}


void ReceiverControl::closeConnection(void)
{
        if(m_dewar_board_ptr != NULL) {
            m_dewar_board_ptr->closeConnection();
        }

        if(m_lna_board_ptr != NULL) {
            m_lna_board_ptr->closeConnection();
        }

        if(m_switch_board_ptr != NULL) {
            m_switch_board_ptr->closeConnection();
        }
}


bool ReceiverControl::isLNABoardConnectionOK(void)
{
    return (m_lna_board_ptr->getConnectionStatus() == CSocket::NOTCREATED) ? false : true;
}


bool ReceiverControl::isDewarBoardConnectionOK(void)
{
    return (m_dewar_board_ptr->getConnectionStatus() == CSocket::NOTCREATED) ? false : true;
}


bool ReceiverControl::isSwitchBoardConnectionOK(void)
{
    return (m_switch_board_ptr != NULL && m_switch_board_ptr->getConnectionStatus() == CSocket::NOTCREATED) ? false : true;
}


ReceiverControl::FetValues ReceiverControl::fetValues(
        unsigned short feed_number, 
        unsigned short stage_number,
        double (*currentConverter)(double voltage),
        double (*voltageConverter)(double voltage)
        )
{
    std::string column_selector;           // EN03: the signal that addresses the column multiplexing of AD24
    std::string vd_selector;               // A03: it allows to select the value requested for a given stadium
    std::string id_selector;               // A03: it allows to select the value requested for a given stadium
    std::string vg_selector;               // A03: it allows to select the value requested for a given stadium
    size_t FEED_LIDX, FEED_RIDX;           // Indexes of the feed channels in the AD24 port, for a given column
    
    FetValues values;

    if(feed_number >= m_number_of_feeds)
        throw ReceiverControlEx("ReceiverControl error: invalid feed number.");

    // Select the column in which there is the given feed
    switch(feed_number) {
        case 0:
        case 2:
        case 4:
        case 6:
            column_selector = "0001";
            break;
        case 1:
        case 3:
        case 5:
        case 7:
            column_selector = "0010";
            break;
        case 8:
        case 10:
        case 12:
        case 14:
            column_selector = "0011";
            break;
        case 9:
        case 11:
        case 13:
        case 15:
            column_selector = "0100";
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

    std::bitset<8> vd_request(column_selector + vd_selector);
    std::bitset<8> id_request(column_selector + id_selector);
    std::bitset<8> vg_request(column_selector + vg_selector);

    try {

        pthread_mutex_lock(&m_lna_mutex); 
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
        pthread_mutex_unlock(&m_lna_mutex); 

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
        pthread_mutex_unlock(&m_lna_mutex); 
        std::string error_msg = "ReceiverControl: error getting LNA values.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


ReceiverControl::StageValues ReceiverControl::stageValues(
        FetValue quantity, 
        unsigned short stage_number,
        double (*converter)(double voltage)
        )
{
	IRA::CString quant;
    // Each item is a EN03 value: the signal that addresses the column multiplexing of AD24
    std::vector<std::string> column_selectors;  
    std::string vd_selector;               // A03: it allows to select the value requested for a given stadium
    std::string id_selector;               // A03: it allows to select the value requested for a given stadium
    std::string vg_selector;               // A03: it allows to select the value requested for a given stadium
    std::string quantity_selector;         // A03: it allows to select the value requested for a given stadium
    
    if(m_number_of_feeds == 1)
        column_selectors.push_back("0001");
    else if(m_number_of_feeds <= 8)  {
        column_selectors.push_back("0001");
        column_selectors.push_back("0010");
    }
    else if(m_number_of_feeds == 9) {
        column_selectors.push_back("0001");
        column_selectors.push_back("0010");
        column_selectors.push_back("0011");
    }
    else if(m_number_of_feeds > 9) {
        column_selectors.push_back("0001");
        column_selectors.push_back("0010");
        column_selectors.push_back("0011");
        column_selectors.push_back("0100");
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

    switch(quantity) {
        case DRAIN_VOLTAGE:
            quantity_selector = vd_selector;
            quant="VD";
            break;
        case DRAIN_CURRENT:
            quantity_selector = id_selector;
            quant="ID";
            break;
        case GATE_VOLTAGE:
        		quant="VG";
            quantity_selector = vg_selector;
            break;
        default:
            throw ReceiverControlEx("ReceiverControl::stageValues(): the quantity requested does not exist.");
    }

    // Left channel values, right channel values, left channel disorderly values, right channel disorderly values
    std::vector<double> lvalues, rvalues, ldvalues, rdvalues;
    std::vector<double> ldvalues_first, rdvalues_first, ldvalues_second, rdvalues_second;
    std::vector<BYTE> parameters;

    try {
        pthread_mutex_lock(&m_lna_mutex); 
        for(std::vector<std::string>::iterator iter=column_selectors.begin(); iter!=column_selectors.end(); iter++) {
            std::bitset<8> value(*iter + quantity_selector);

            makeRequest(
                    m_lna_board_ptr,                           // Pointer to the LNA board socket
                    MCB_CMD_SET_DATA,                          // Command to send
                    4,                                         // Number of parameters
                    MCB_CMD_DATA_TYPE_U08,                     // Data type: unsigned 08 bit
                    MCB_PORT_TYPE_DIO,                         // Port type: Digital IO
                    MCB_PORT_NUMBER_00_07,                     // Port Number from 00 to 07
                    static_cast<BYTE>(value.to_ulong())        // Value to set                   
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
            pthread_mutex_unlock(&m_lna_mutex); 

            if(parameters.size() != AD24_LEN * AD24_TYPE_LEN)
                throw MicroControllerBoardEx("Error: wrong number of parameters received.");

            for(std::vector<BYTE>::size_type idx=0; idx<AD24_LEN; idx+=2) {
                ldvalues.push_back(get_value(parameters, idx));    // Push the left value channel
                rdvalues.push_back(get_value(parameters, idx+1));  // Push the right value channel
            }

            if(ldvalues.empty() || rdvalues.empty())
                throw MicroControllerBoardEx("Error: no data received.");
        }
        pthread_mutex_unlock(&m_lna_mutex); 
    }
    catch(MicroControllerBoardEx& ex) {
        pthread_mutex_unlock(&m_lna_mutex); 
        std::string error_msg = "ReceiverControl: error getting LNA values.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }

    if(m_number_of_feeds == 1) { 
        if(ldvalues.size() != 4 || rdvalues.size() != 4)  // Just one feed (the feed number 0) means just one column 
            throw ReceiverControlEx("Error: mismatch between number of feeds and number of parameters");
        lvalues.push_back(ldvalues.front());
        rvalues.push_back(rdvalues.front());
    }
    else // In this case we are sure we must add at least the first 8 items (2 columns)
        for(std::vector<BYTE>::size_type idx=0; idx<=3; idx++) {
            lvalues.push_back(ldvalues[idx]);   // Add the item of the first column
            lvalues.push_back(ldvalues[idx+4]); // Add the item of the second column
            rvalues.push_back(rdvalues[idx]);   // Add the item of the first column
            rvalues.push_back(rdvalues[idx+4]); // Add the item of the second column
        }

    if(m_number_of_feeds == 9) { // Exactly nine feeds (number_of_feeds == 9)
        if(ldvalues.size() != 12 || rdvalues.size() != 12)  // Nine feeds means three columns
            throw ReceiverControlEx("Error: mismatch between number of feeds and number of parameters");
        lvalues.push_back(ldvalues[8]);
        rvalues.push_back(rdvalues[8]);
    }
    else 
        if(ldvalues.size() > 9) {
            if(ldvalues.size() != 16 || rdvalues.size() != 16)  // More than nine feeds means four columns
                throw ReceiverControlEx("Error: mismatch between number of feeds and number of parameters");
            std::vector<BYTE>::size_type offset=8;
            for(std::vector<BYTE>::size_type idx=0; idx<=3; idx++) {
                lvalues.push_back(ldvalues[offset+idx]);   // Add the item of the third column
                lvalues.push_back(ldvalues[offset+idx+4]); // Add the item of the fourth column
                rvalues.push_back(rdvalues[offset+idx]);   // Add the item of the third column
                rvalues.push_back(rdvalues[offset+idx+4]); // Add the item of the fourth column
            }
        }
    

    if(lvalues.size() < m_number_of_feeds || rvalues.size() < m_number_of_feeds)
        throw ReceiverControlEx("Error: the vector size doesn't match the number of feeds.");

    // Add the first "number_of_feeds" converted items of lvalues and rvalues
    StageValues values;
    try {
        for(size_t idx=0; idx<m_number_of_feeds; idx++) {
            (values.left_channel).push_back(converter != NULL ? converter(lvalues[idx]) : lvalues[idx]);
            (values.right_channel).push_back(converter != NULL ? converter(rvalues[idx]) : rvalues[idx]);                   
        }
    }
    catch(...) {
        throw ReceiverControlEx("ReceiverControl error: unexpected exception occurs performing the conversion.");
    }

    return values;
}


void ReceiverControl::turnLeftLNAsOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_lna_board_ptr,       // Pointer to the LNA board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing turnLeftLNAsOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::turnLeftLNAsOff(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_lna_board_ptr,       // Pointer to the LNA board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing turnLeftLNAsOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::turnRightLNAsOn(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_lna_board_ptr,       // Pointer to the LNA board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing turnRightLNAsOn().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::turnRightLNAsOff(
            const BYTE data_type,
            const BYTE port_type,
            const BYTE port_number,
            const BYTE value
            )
{
    try {
        makeRequest(
                m_lna_board_ptr,       // Pointer to the LNA board
                MCB_CMD_SET_DATA,      // Command to send
                4,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                value
        );
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing turnRightLNAsOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}

void ReceiverControl::setPath( 
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number, 
        const size_t width,
        const std::vector<BYTE> parameters
        )
{
    if(parameters.size() % width != 0)
        throw ReceiverControlEx("ReceiverControl: wrong number of parameters.\n");

    try {
        pthread_mutex_lock(&m_switch_mutex); 
        for(size_t i=0; i < parameters.size() / width; i++) {
            makeRequest(
                    m_switch_board_ptr,      // Pointer to the board
                    MCB_CMD_SET_DATA,        // Command to send
                    3 + width,               // Total number of parameters
                    data_type,
                    port_type,
                    port_number,
                    parameters[0 + width*i], // Address MSB
                    parameters[1 + width*i], // Address LSB
                    parameters[2 + width*i], // Board MSB
                    parameters[3 + width*i], // Board LSB
                    parameters[4 + width*i], // Command MSB
                    parameters[5 + width*i]  // Command LSB
            );
            usleep(2 * SWITCH_SLEEP_TIME);
        }
        pthread_mutex_unlock(&m_switch_mutex); 
    }
    catch(MicroControllerBoardEx& ex) {
        pthread_mutex_unlock(&m_switch_mutex); 
        std::string error_msg = "ReceiverControl: error performing setPath().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setColdLoadPath(
        unsigned short feed_id,
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number
        )
{
    try {
        const BYTE two_high_bits = 0x03; // Binary value 11
        const BYTE value = (two_high_bits << feed_id * 2); // Move the bits to the feed

        pthread_mutex_lock(&m_switch_mutex); 
        // Set the cool load path
        makeRequest(
                m_switch_board_ptr,    // Pointer to the switch board
                MCB_CMD_SET_DATA,      // Command to send
                5,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                0x00,
                0x10
        );
                
        usleep(2 * SWITCH_SLEEP_TIME);

        // Set the link to the two channels of the feed
        makeRequest(
                m_switch_board_ptr,    // Pointer to the switch board
                MCB_CMD_SET_DATA,      // Command to send
                5,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                0x00,
                value + 0x10
        );
                
        usleep(2 * SWITCH_SLEEP_TIME);
        
        // Set again the cool load path
        makeRequest(
                m_switch_board_ptr,    // Pointer to the switch board
                MCB_CMD_SET_DATA,      // Command to send
                5,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                0x00,
                0x10
        );
                
        pthread_mutex_unlock(&m_switch_mutex); 
    }
    catch(MicroControllerBoardEx& ex) {
        pthread_mutex_unlock(&m_switch_mutex); 
        std::string error_msg = "ReceiverControl: error performing setColdLoadPath().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


void ReceiverControl::setSkyPath(
        unsigned short feed_id,
        const BYTE data_type, 
        const BYTE port_type, 
        const BYTE port_number
        )
{
    try {
        const BYTE two_high_bits = 0x03; // Binary value 11
        const BYTE value = (two_high_bits << feed_id * 2); // Move the bits to the feed

        pthread_mutex_lock(&m_switch_mutex); 
        // Set the sky path
        makeRequest(
                m_switch_board_ptr,    // Pointer to the switch board
                MCB_CMD_SET_DATA,      // Command to send
                5,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                0x00,
                0x00
        );
                
        usleep(2 * SWITCH_SLEEP_TIME);

        // Set the link to the two channels of the feed
        makeRequest(
                m_switch_board_ptr,    // Pointer to the switch board
                MCB_CMD_SET_DATA,      // Command to send
                5,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                0x00,
                value
        );
                
        usleep(2 * SWITCH_SLEEP_TIME);
        
        // Set again the sky path
        makeRequest(
                m_switch_board_ptr,    // Pointer to the switch board
                MCB_CMD_SET_DATA,      // Command to send
                5,                     // Number of parameters
                data_type,
                port_type,
                port_number,
                0x00,
                0x00
        );
        pthread_mutex_unlock(&m_switch_mutex); 
    }
    catch(MicroControllerBoardEx& ex) {
        pthread_mutex_unlock(&m_switch_mutex); 
        std::string error_msg = "ReceiverControl: error performing setColdLoadPath().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


std::vector<BYTE> ReceiverControl::makeRequest(MicroControllerBoard *board_ptr, BYTE command, size_t len, ...) 
{
    pthread_mutex_lock(&m_request_mutex); 
    va_list parameters; // A place to store the list of arguments
    va_start(parameters, static_cast<BYTE>(len)); // Initializing arguments to store all values after len
    std::vector<BYTE> vparams;
    for (size_t i=0; i < len; i++) // Loop until all numbers are added
        // Adds the next value in argument list to sum.
        vparams.push_back(static_cast<BYTE>(va_arg(parameters, int))); 
    va_end(parameters);  // Clean up the list
    try {
        m_reliable_comm ? board_ptr->send(command, vparams) \
                        : board_ptr->send(command | MCB_CMD_TYPE_NOCHECKSUM, vparams);
        vparams.clear();
        vparams = board_ptr->receive();
    }
    catch(MicroControllerBoardEx& ex) {
        pthread_mutex_unlock(&m_request_mutex); 
        throw;
    }
    pthread_mutex_unlock(&m_request_mutex); 

    return vparams; // Return the vector of parameters
}


double ReceiverControl::get_value(const std::vector<BYTE> parameters, const size_t RAW_INDEX)
{

    union Bytes2Float {
        float value;
        BYTE buff[AD24_TYPE_LEN];
    } uvalue;

    std::vector<BYTE>::size_type sidx = AD24_TYPE_LEN * RAW_INDEX; 
    std::vector<BYTE>::size_type vidx = AD24_TYPE_LEN-1, idx = sidx;
    // parameters[sidx] is the index of the first byte in the requested data.
    // For instance, we get the vacuum value from the AD24 port, at the port number AD10;
    // the vacuum data type is a 32 bit floating poing, so the data is stored in a range of 4 
    // bytes. The AD10 has index 2 in the AD24, so the first of the 4 bytes of our
    // value in the parameter vector has index RAW_INDEX * 4.
    for( ; idx < sidx + AD24_TYPE_LEN; idx++) {
        uvalue.buff[vidx] = parameters[idx];
        --vidx;
    }
    return uvalue.value;
}

