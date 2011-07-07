#include "ReceiverControl.h"
#include "ReceiverConversions.def"


ReceiverControl::ReceiverControl(
        const std::string dewar_ip,
        const unsigned short dewar_port, 
        const std::string lna_ip, 
        const unsigned short lna_port, 
        const BYTE dewar_madd,
        const BYTE dewar_sadd,
        const BYTE lna_madd,
        const BYTE lna_sadd,
        bool reliable_comm
) throw (ReceiverControlEx) : 
    m_reliable_comm(reliable_comm), 
    m_dewar_board_ptr(NULL),
    m_lna_board_ptr(NULL)
{

    try {
        m_dewar_board_ptr = new MicroControllerBoard(dewar_ip, dewar_port, dewar_madd, dewar_sadd);
        m_dewar_board_ptr->openConnection();
    }
    catch(MicroControllerBoardEx& ex) {
        throw ReceiverControlEx("Dewar MicroControllerBoard error: " + ex.what());
    }

    try {
        m_lna_board_ptr = new MicroControllerBoard(lna_ip, lna_port, lna_madd, lna_sadd);
        m_lna_board_ptr->openConnection();
    }
    catch(MicroControllerBoardEx& ex) {
        throw ReceiverControlEx("LNA MicroControllerBoard error: " + ex.what());
    }
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
                MCB_CMD_GET_DATA,      // Command to send
                3,                     // Number of parameters
                MCB_CMD_DATA_TYPE_B01, // Data type: 1 bit
                MCB_PORT_TYPE_DIO,     // Port type: Digital IO
                MCB_PORT_NUMBER_11     // Port Number 11
        );

        // In that case makeRequest should return just one parameter (1 bit: ON, OFF)
        if(parameters.size() != 1)
            throw ReceiverControlEx("RecieverControl::isCalibrationOn(): wrong number of parameters.");

        return false ? parameters[0] == 0 : true;
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error performing setCalibrationOff().\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


double ReceiverControl::vacuum() throw (ReceiverControlEx) 
{
    try {

        std::vector<BYTE> parameters = makeRequest(
                MCB_CMD_GET_DATA,       // Command to send
                3,                      // Number of parameters
                MCB_CMD_DATA_TYPE_F32,  // Data type: 32 bit floating point
                MCB_PORT_TYPE_AD24,     // Port type: AD24
                MCB_PORT_RANGE_08_15    // Port Number from 08 to 15
        );

        const size_t VCM_SENSOR_RAW_IDX = 2; // Index of the vacuum in the AD24 port
        // Return the vacuum value in mbar for a given voltage VALUE
        return GET_VACUUM(get_value(parameters, VCM_SENSOR_RAW_IDX));
    }
    catch(MicroControllerBoardEx& ex) {
        std::string error_msg = "ReceiverControl: error getting the vacuum.\n";
        throw ReceiverControlEx(error_msg + ex.what());
    }
}


std::vector<BYTE> ReceiverControl::makeRequest(BYTE command, size_t len, ...) throw(MicroControllerBoardEx)
{
    va_list parameters; // A place to store the list of arguments
    va_start(parameters, static_cast<BYTE>(len)); // Initializing arguments to store all values after len
    std::vector<BYTE> vparams;
    for (size_t i=0; i < len; i++) // Loop until all numbers are added
        // Adds the next value in argument list to sum.
        vparams.push_back(static_cast<BYTE>(va_arg(parameters, int))); 
    va_end(parameters);  // Clean up the list

    m_reliable_comm ? m_dewar_board_ptr->send(command, vparams) \
                    : m_dewar_board_ptr->send(command | MCB_CMD_TYPE_NOCHECKSUM, vparams);
    vparams.clear();
    vparams = m_dewar_board_ptr->receive();

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

