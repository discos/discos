/**************************************************************\
 *  File Infos
 *  ==========
 *  Class to use for communicating with receiver boards.
 *
 *  Parameters
 *  ----------
 *  A Talk take the following parameters:
 *
 *    + command: a string associated with a command
 *    + id: an unsigned int that identify the command
 *
 *  Methods
 *  -------
 *  A Talk has the following methods:
 *
 *    + ...
 *
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
 *
 *  Metadata
 *  ========
 *  Created: Thu May 20 08:25:36 CEST 2010
\**************************************************************/
#ifndef __TALK_H__
#define __TALK_H__

#include <vector>
#include <string>
#include <IRA>
#include <ComponentErrors.h>

using namespace IRA;
using namespace std;

/* General protocol encoding */
const BYTE SRH = 0x01; // Short Request Header (CMD_SOH)
const BYTE SAH = 0x02; // Short Answer Header (CMD_STX)
const BYTE SGD = 0x6E; // Short Get Data (short CMD_GET_DATA)
const BYTE SSD = 0x6F; // Short Set Data (short CMD_SET_DATA)
#define SADD_IDX 1 // Index position of sender address byte in a request
#define RADD_IDX 2 // Index position of recipient address byte in a request
#define CMD_IDX 3 // Index position of command byte
#define ID_IDX 4 // Index position of command ID byte
#define EXR_IDX 5 // Index position of execution result byte
#define FIXED_AL 6 // Fixed Anwer Length
#define APARAMS_IDX 7 // Index of first answer parameter


/* Data type encoding */
typedef vector<BYTE> vbyte;
const BYTE ONE_BIT_DT = 0x03; // One BIT  Data Type encoding: 1 bit
const BYTE F32_BIT_DT = 0x18; // Float 32 bit data type


/* Data type values */
const BYTE TURN_ON = 0x01;  // Set ONE_BIT_DT to 1
const BYTE TURN_OFF = 0x00; // Set ONE_BIT_DT to 0


/* Port number encoding */
const BYTE DIO_PORT = 0x04; // Digital IO PORT address
const BYTE AD24_PORT = 0x08; // Digital IO PORT address


/* Device Addresses */
const BYTE SADDRESS = 0x7D; // Sender ADDRESS
const BYTE RADDRESS = 0x01; // Recipient ADDRESS


/* Command encoding and command macros */
enum Command {CAL_OFF, CAL_ON, SET_MODE, VACUUM};

const BYTE CAL_PN = 0x0B; // CALibration Port Number: out11 of DIO

const BYTE SET_MODE_PN = 0x0D; // Set Mode Port Number: out13 of DIO
enum Mode {NONE, SDISH, VLBI};

const BYTE VACUUM_PN = 0x60; // Set Mode Port Number: AD8-AD15
#define VACUUM_IDX 2 // Index of AD10 in the answer parameters
#define VACUUM_PARAMS_LEN 3 // Vacuum parameter lenght


/* Execution Result Encoding */
const unsigned short ACK = 0x00; // No error
const unsigned short ERR_CMD = 0x01; // Command not available
const unsigned short ERR_CHKS = 0x02; // Checksum error
const unsigned short ERR_FORM = 0x03; // Error in parameter format
const unsigned short ERR_DATA = 0x04; // Parameter data error
const unsigned short ERR_TOUT = 0x05; // Expired command execution timeout
const unsigned short ERR_ADDR = 0x06; // Wrong address
const unsigned short ERR_TIME = 0x07; // Wrong time
const unsigned short ERR_FRAME_SIZE = 0x08; // Wrong frame size
const unsigned short ERR_DATA_TYPE = 0x09; // Data type unavailable
const unsigned short ERR_PORT_TYPE = 0x0A; // Port type unavailable
const unsigned short ERR_PORT_NUMBER = 0x0B; // Port number unavailable
const unsigned short ERR_DATA_SIZE = 0x0C; // Wrong data size


class Talk {

    public:

        /**
        * Constructor of Talk object.
        *
        * @param command an integral type used as a command identifier
        * @param mode an integral type used as a modecommand identifier
        */
        Talk(Command command, Mode mode=NONE);


        /**
        * Set the request unique identification code.
        *
        * @param request_id reference to Socket member that stores the request 
        * identification number.
        */
        void make_request(std::vector< std::vector<BYTE> > *const requests_ptr, unsigned int *request_id_ptr, Mode mode=NONE);

        void verify(const vector<BYTE> *const request, const vector<BYTE> *const answer) 
            throw (ComponentErrors::SocketErrorExImpl);

    private:
        
        Command m_command;
        Mode m_mode;
};


#endif
