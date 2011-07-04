#include "ReceiverControl.h"


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
        std::vector<BYTE> parameters = makeRequest(
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
        std::vector<BYTE> parameters = makeRequest(
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


std::vector<BYTE> ReceiverControl::makeRequest(BYTE command, size_t len, ...) throw(MicroControllerBoardEx)
{
    va_list parameters; // A place to store the list of arguments
    va_start(parameters, len); // Initializing arguments to store all values after num
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
