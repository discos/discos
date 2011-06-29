/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __RECEIVER_CONTROL__H
#define __RECEIVER_CONTROL__H

#include <string>
#include <vector>
#include <IRA>
#include "ReceiverControl.h"

class ReceiverBoardEx {

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
 * what board perform the request or other low level details</p>
 * <p>Once you instantiate the ReceiverControl, you can execute the
 * following operations:</p>
 * <ul>
 *     <li>...</li>
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
    ) throw (ReceiverControlEx);


	/** Destructor */
    ~ReceiverControl();

private:
    // ...

};



#endif
