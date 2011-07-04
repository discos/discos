/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef m__RECEIVER_CONTROL__H
#define m__RECEIVER_CONTROL__H

#include <string>
#include <vector>
#include <IRA>
#include "MicroControllerBoard.h"

class ReceiverControlEx {

public:
   ReceiverControlEx(const std::string& msg) : m_msg(msg) {}
  ~ReceiverControlEx() {}
   std::string what() const { return(m_msg); }

private:
   const std::string m_msg;
};


/** 
 * This class performs a high level library to comunicate via TCP/IP
 * to receivers controlled by the board designed in Medicina (BO, Italy) 
 * by Franco Fiocchi <f.fiocchi@ira.inaf.it>
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * INAF, Osservatorio Astronomico di Cagliari
 * <p>There are two board for each receiver: the first one allows to
 * commnicate to the dewar either reading the status bits and voltage values
 * or writing some configurations. The second one supplies the power to 
 * the feed LNAs (Low Noise Amplifier), and also allows to read the values
 * of VDs (drain voltage), IDs (drain current) and VGs (gate voltage).</p>
 * <p>The user of this class just have to know its interface without to see
 * which board perform the request or other low level details.</p>
 * <p>Once you instantiate the ReceiverControl, you can execute the
 * following operations:</p>
 * <ul>
 *     <li>doube vacuum(): return the vacuum value inside the dewar</li>
 *     <li>double lowTemperature(): return the low cryogenic temperature value</li>
 *     <li>double highTemperature(): return the high cryogenic temperature value</li>
 *     <li>void setCoolHeadOn(): set to ON the cool head</li>
 *     <li>void setCoolHeadOff(): set to OFF the cool head</li>
 *     <li>bool isCoolHeadOn(): return true if the cool head is ON</li>
 *     <li>void setVacuumSensorOn(): set to ON the vacuum sensor</li>
 *     <li>void setVacuumSensorOff(): set to OFF the vacuum sensor</li>
 *     <li>bool isVacuumSensorOn(): return true if the vacuum sensor is ON</li>
 *     <li>bool isRemoteEnable(): return true if the remote command is enabled</li>
 *     <li>void setVacuumPumpOn(): set to ON the vacuum pump</li>
 *     <li>void setVacuumPumpOff(): set to OFF the vacuum pump</li>
 *     <li>bool isVacuumPumpOn(): return true if the vacuum pump is ON</li>
 *     <li>void setVacuumValveOn(): open the valve IMMEDIATELY</li>
 *     <li>void setVacuumValveOff(): disable the valve</li>
 *     <li>bool isVacuumValveOn(): return true if the vacuum valve is opened</li>
 *     <li>void setCalibrationOn(): set the noise mark to ON</li>
 *     <li>void setCalibrationOff(): set the noise mark to OFF</li>
 *     <li>void setRemoteOn(): enable the remote command</li>
 *     <li>void setRemoteOff(): disable the remote command</li>
 *     <li>bool isRemoteEnable(): return true if the remote command is enable</li>
 *     <li>void selectLO1(): select the Local Oscillator 1</li>
 *     <li>void selectLO2(): select the Local Oscillator 2</li>
 *     <li>void setReliableCommOn(): set the reliable communication to/from the 
 *     board to ON</li>
 *     <li>void setReliableCommOff(): set the reliable communication to/from the 
 *     board to OFF</li>
 *     <li>bool isReliableCommOn(): return true if the the communiaction to the
 *     board is sets to be reliable</li>
 *     <li>bool isLNABoardConnectionOK(): return true if the connection to the LNA
 *     board is OK</li>
 *     <li>bool isDewarBoardConnectionOK(): return true if the connection to the 
 *     dewar board is OK</li>
 * </ul>
 * 
 */
class ReceiverControl {

public:

	/** 
	 * Constructor
	 * @param dewar_ip the IP address of the dewar control board
	 * @param dewar_port the port of the dewar control board
	 * @param lna_ip the IP address of the LNA control board
	 * @param lna_port the port of the LNA control board
     * @param reliable_comm when it is true then the communication
     * to the board is reliable because there is a checksum field in
     * both request and answer. It is true to default.
     * @throw ReceiverControlEx
	*/
    ReceiverControl(
            const std::string dewar_ip,
            const unsigned short dewar_port, 
            const std::string lna_ip, 
            const unsigned short lna_port, 
            const BYTE dewar_madd=0x7D, // Dewar board master address
            const BYTE dewar_sadd=0x7F, // Dewar board slave address
            const BYTE lna_madd=0x7D,   // LNA board master address
            const BYTE lna_sadd=0x7F,   // LNA board slave address
            bool reliable_comm=true
    ) throw (ReceiverControlEx);

    
    /** Destructor */
    ~ReceiverControl();


    /** Set the noise mark generator to ON 
     *  @throw ReceiverControlEx
     */
    void setCalibrationOn() throw (ReceiverControlEx);


    /** Set the noise mark generator to OFF
     *  @throw ReceiverControlEx
     */
    void setCalibrationOff() throw (ReceiverControlEx);


    // When do I convert the voltage values to the requested values? Inside this library
    // or outside it?


private:
    
	/** Send the request to the board and receive the answer
	 * @param command the command code of the request
	 * @param len the number of parameters to send
	 * @param ... the parameters
     * @return a vector of parameters, empty if there are not some
     * @throw MicroControllerBoardEx
	*/
    std::vector<BYTE> makeRequest(const BYTE command, size_t len, ...) throw(MicroControllerBoardEx);

    /** If m_reliable_comm is true then a checksum byte is added to the request
     *  and to the answer during the communication to and from the board.
     *  So m_reliable_comm == true means we have a reliable communication.
     */
    bool m_reliable_comm;

    /** Dewar MicroControllerBoard pointer */
    MicroControllerBoard *m_dewar_board_ptr;

    /** LNA MicroControllerBoard pointer */
    MicroControllerBoard *m_lna_board_ptr;

};



#endif
