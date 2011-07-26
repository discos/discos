/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __MICRO_CONTROLLER_BOARD__H
#define __MICRO_CONTROLLER_BOARD__H

#include <string>
#include <vector>
#include <IRA>
#include "Definitions.h"
#include "MicroControllerBoardDef.h"


namespace IRA {

class MicroControllerBoardEx {

public:
   MicroControllerBoardEx(const std::string& msg) : m_msg(msg) {}
  ~MicroControllerBoardEx() {}
   std::string what() const { return(m_msg); }

private:
   const std::string m_msg;
};


/** 
 * This class implements the protocol to comunicate via TCP socket to the micro controller board
 * designed in Medicina (BO, Italy) by Franco Fiocchi <f.fiocchi@ira.inaf.it>. The library is thread safe.
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * INAF, Osservatorio Astronomico di Cagliari
 */
class MicroControllerBoard {

public:

	/** 
	 * Constructor
	 * @param IP the IP address of the board
     * @param port the port used for the socket connection to the board
	 * @param master_address the physical address of the master
	 * @param slave_address the physical address of the slave
     * @param timeout the blocking socket timeout
     * @throw MicroControllerBoardEx
	*/
    MicroControllerBoard(std::string IP, unsigned short port=8000,
            BYTE master_address=0x7C, BYTE slave_address=0x7D, 
            DWORD timeout=MCB_TOUT
    ) throw (MicroControllerBoardEx);
     

	/** Destructor */
    ~MicroControllerBoard();


    /** Perform a TCP connection socket to the board
     *  @throw MicroControllerBoardEx
     */
    void openConnection(void) throw (MicroControllerBoardEx);


    /** Return the status of TCP socket connection to the board
     *  @return SocketStatus, an enumeration value with the following meaning:
     *   <ul>
     *       <li>NOTCREATED (0) socket is not created, no operation is possible</li>
     *       <li>READY      (1) socket is ready to perform any operation</li>
     *       <li>ACCEPTING  (2) socket is looking for incoming connection</li>
     *       <li>CONNECTING (3) socket is trying to connect to another machine</li>
     *   </ul>
     */
    CSocket::SocketStatus getConnectionStatus(void);


    /** Close the TCP connection socket to the board */
    void closeConnection(void);


    /** Receive the answer from the board and perform all the checks. Check if:
     *  <ul>
     *      <li>The master address and slave address are right</li>
     *      <li>The command code exists</li>
     *      <li>The command code of the answer is the same as the request</li>
     *      <li>The id of the answer is the same as the reqeust</li>
     *      <li>The number of parameters is as expected</li>
     *      <li>The checksum is right</li>
     *  </ul>
     *  @throw MicroControllerBoardEx
     *  @return a vector<BYTE> storing the requested parameters
     */
    std::vector<BYTE> receive(void) throw (MicroControllerBoardEx);


    /** Send a command to the board.
     *  @param command the command code
     *  @param parameters a vector<BYTE> of parameters to send (it could be empty)
     *  @throw MicroControllerBoardEx
     */
    void send(const BYTE command, std::vector<BYTE> parameters=std::vector<BYTE>()) throw (MicroControllerBoardEx);

    /** Return the IP address of the board */
    std::string getIP() const { return(m_IP); }

    /** Return the port of the board socket connection */
    unsigned short getPort() const { return(m_port); }


private:

    /** Compute the checksum of the vector<BYTE> given as parameter
     *  @param message the vector<BYTE> on which to compute the checksum
     *  @throw MicroControllerBoardEx
     */
    BYTE computeChecksum(std::vector<BYTE> message) throw (MicroControllerBoardEx);

    
    /** Return the any object as a string.
     * @param i: a any object
     * @return the given object as a string
     */
    template<class T>
    std::string any2string(T i) {
        std::ostringstream buffer;
        buffer << i;
        return buffer.str();
    }


    const std::string m_IP;
    const unsigned short m_port;
    const DWORD m_timeout;
    const BYTE m_master_address;
    const BYTE m_slave_address;
    IRA::CSocket *m_socket;
    unsigned long long m_id;
    BYTE m_command_type;
    std::vector<BYTE> m_request;
    std::vector<BYTE> m_answer;
    std::vector<BYTE> m_parameters;
    /** @var contains error information */
    CError m_Error;

};

}

#endif
