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
) m_reliable_comm(reliable_comm) throw (ReceiverControlEx)
{
    try {
        m_dewar_board = MicroControllerBoard(dewar_ip, dewar_port, dewar_madd, dewar_sadd);
    }
    catch(MicroControllerBoardEx& ex) {
        throw ReceiverErrorEx("Dewar MicroControllerBoard error: " + ex.what());
    }

    try {
        m_lna_board = MicroControllerBoard(lna_ip, lna_port, lna_madd, lna_sadd);
    }
    catch(MicroControllerBoardEx& ex) {
        throw ReceiverErrorEx("LNA MicroControllerBoard error: " + ex.what());
    }

}


void ReceiverControl::setCalibrationOn() throw (ReceiverControlEx)
{
    const BYTE DATA_TYPE = MCB_CMD_DATA_TYPE_B01; // 1 bit
    const BYTE PORT_TYPE = MCB_PORT_TYPE_DIO; // Digital IO
    const BYTE PORT_NUMBER = 
    m_reliable_comm ? send(reliable) : send(no_reliable);
}
