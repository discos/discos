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
#include "Definitions.h"
#include <IRA>

#define MCB_TOUT 200000 // 2 seconds
#define MCB_BUFF_LIMIT 2048
// MCB_HT_COUNTER means Header or Terminator COUNTER
#define MCB_HT_COUNTER 10 // The maximum number of attempts we wait for a header or terminator
#define MCB_BASE_ANSWER_LENGTH 6 // The minimum lenght of the answer. It should to be at least 1

#define MCB_CMD_SOH 0x01 // Request header (master -> slave)
#define MCB_CMD_STX 0x02 // Answer header (slave -> master)
// Short commands don't have a terminator byte
#define MCB_CMD_ETX 0x03 // Answer terminator, only for extended commands (slave -> master)
#define MCB_CMD_EOT 0x04 // Request terminator, only for extended commands (master -> slave)

#define MCB_CMD_TYPE_EXTENDED      0x41 // A
#define MCB_CMD_TYPE_ABBREVIATED  (CMD_TYPE_EXTENDED | CMD_TYPE_NOCHECKSUM) // 0x61 // a

#define MCB_CMD_TYPE_INQUIRY       0x00 // Answer with data
#define MCB_CMD_TYPE_RESET         0x01
#define MCB_CMD_TYPE_VERSION       0x02 // Answer with data
#define MCB_CMD_TYPE_SAVE          0x03
#define MCB_CMD_TYPE_RESTORE       0x04
#define MCB_CMD_TYPE_GET_ADDR      0x05 // Answer with data
#define MCB_CMD_TYPE_SET_ADDR      0x06
#define MCB_CMD_TYPE_GET_TIME      0x07 // Answer with data
#define MCB_CMD_TYPE_SET_TIME      0x08
#define MCB_CMD_TYPE_GET_FRAME     0x09 // Answer with data
#define MCB_CMD_TYPE_SET_FRAME     0x0A
#define MCB_CMD_TYPE_GET_PORT      0x0B // Answer with data
#define MCB_CMD_TYPE_SET_PORT      0x0C
#define MCB_CMD_TYPE_GET_DATA      0x0D // Answer with data
#define MCB_CMD_TYPE_SET_DATA      0x0E 
#define MCB_CMD_TYPE_ERROR         0x0F

#define MCB_CMD_INQUIRY            (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_INQUIRY)   // A - 0x41
#define MCB_CMD_RESET              (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_RESET)     // B - 0x42
#define MCB_CMD_VERSION            (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_VERSION)   // C - 0x43
#define MCB_CMD_SAVE               (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SAVE)      // D - 0x44
#define MCB_CMD_RESTORE            (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_RESTORE)   // E - 0x45
#define MCB_CMD_GET_ADDR           (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_ADDR)  // F - 0x46
#define MCB_CMD_SET_ADDR           (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_ADDR)  // G - 0x47
#define MCB_CMD_GET_TIME           (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_TIME)  // H - 0x48
#define MCB_CMD_SET_TIME           (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_TIME)  // I - 0x49
#define MCB_CMD_GET_FRAME          (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_FRAME) // J - 0x4A
#define MCB_CMD_SET_FRAME          (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_FRAME) // K - 0x4B
#define MCB_CMD_GET_PORT           (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_PORT)  // L - 0x4C
#define MCB_CMD_SET_PORT           (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_PORT)  // M - 0x4D
#define MCB_CMD_GET_DATA           (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_GET_DATA)  // N - 0x4E
#define MCB_CMD_SET_DATA           (MCB_CMD_TYPE_EXTENDED + MCB_CMD_TYPE_SET_DATA)  // O - 0x4F

#define MCB_CMD_TYPE_MIN_EXT       MCB_CMD_INQUIRY
#define MCB_CMD_TYPE_MAX_EXT       MCB_CMD_SET_DATA

#define MCB_CMD_TYPE_NOCHECKSUM    0x20
#define MCB_CMD_TYPE_MIN_ABB       (MCB_CMD_TYPE_MIN_EXT | MCB_CMD_TYPE_NOCHECKSUM)
#define MCB_CMD_TYPE_MAX_ABB       (MCB_CMD_TYPE_MAX_EXT | MCB_CMD_TYPE_NOCHECKSUM)

// Index of the command and id byte, both in answer and request
#define MCB_CMD_HEADER             0
#define MCB_CMD_SLAVE              1
#define MCB_CMD_MASTER             2
#define MCB_CMD_COMMAND            3
#define MCB_CMD_COMMAND_ID         4


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
 * designed in Medicina (BO, Italy) by Franco Fiocchi <f.fiocchi@ira.inaf.it>.
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * INAF, Osservatorio Astronomico di Cagliari
 */
class MicroControllerBoard {

public:

	/** 
	 * Constructor
	 * @param master_address the physical address of the master
	 * @param slave_address the physical address of the slave
	 * @param slave_address the physical address of the slave
	 * @param IP the IP address of the board
     * @param port the port used for the socket connection to the board
     * @throw MicroControllerBoardEx
	*/
    MicroControllerBoard(const BYTE master_address, const BYTE slave_address,
            const std::string IP="", const unsigned short port=8000, DWORD timeout=MCB_TOUT) 
        throw (MicroControllerBoardEx);


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
    /** @var contains error information */
    CError m_Error;

};



#endif
