/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __LIBCOM_H__
#define __LIBCOM_H__

#include <sstream>
#include <IRA>
#include <hexlib.h>
#include "acstime.h" 
#include "utils.h" 
#include "WPServoSocket.h"
#include <ComponentErrors.h>
#include <sys/time.h>
#include <baciCharacteristicComponentImpl.h>
#include <map>

using namespace IRA;

const string commands[] = {"getpos", "getspar", "setpos", "setup", "stow", "calibrate", "getappstatus", "getstatus", "clean", "disable",
                           "clremergency"};

// Header of request
const string req_header("#");
// Header of ack answer
const string ack_header("?");
// Header of first timed answer
const string timed_ack_header("@");
// Header of ack answer
const string nak_header("!");
const string headers = ack_header + timed_ack_header + nak_header;

// The MinorServo answer has a syntax like this:
//
//     "?command_sent> real_answer"
// 
// In that case the answer separator is ``>``. The string ``answer_separators`` can
// contain more than one character and the split function performs a splitting when it
// matches the first occurrence of one of the ``answer_separators`` characters. 
const string answer_separators(">");

// The possibles separators among response parameters
const string parameter_separators(",");

// The possibles request identification separators
const string reqid_separators("=");

// The possibles command separators 
const string command_separators(":");

// The closer characters of messages
const string message_closer = "\r\n";


/**
 * This function makes and returns a request (string) to send to minor servo.
 * @param cmd_idx index of array of commands
 * @param cdb_ptr pointer to CDB structure
 * @param cmd_number reference to Socket member that stores the command identification number.
 * @param slave slave address
 * @param index index of command parameter request 
 * @param sub_index sub index of command parameter request 
 * @param exe_time execution time
 * @param positions pointer to doubleSeq positions
 *
 * @throw ComponentErrors::SocketErrorEx
 * @return the request to send to minor servo (string)
 */
string make_request(
        const unsigned int cmd_idx, 
        const CDBParameters *const cdb_ptr, 
        CSecureArea<unsigned int> *cmd_number,
        const int slave=-1,
        const int index=-1,
        const int sub_index=-1,
        const ACS::Time exe_time=0,
        const ACS::doubleSeq *positions=NULL
) throw (ComponentErrors::SocketErrorExImpl);


/**
 * Verify whether the answer is valid.
 * @param answer the answer received from minor servo
 * @return true if the answer is as expected, false if it is a NAK or a wrong answer
 * @throw ComponentErrors::SocketErrorEx 
 */
 bool verify(string answer) throw (ComponentErrors::SocketErrorExImpl);


/**
 * Process the answer and set requested data.
 * @param answer the answer received from minor servo
 * @param data a reference to requested data (double sequence)
 * @param udata a reference to requested data (unsigned long)
 * @param status_par a reference to requested data (StatusParameters)
 * @param cmd_idx index of array of commands
 * @param cdb_ptr pointer to CDB structure
 * @param timestamp a reference to answer timestamp
 * @param slave slave address
 * @throw ComponentErrors::SocketErrorEx if the answer is a NAK
 */
 void process(
         string answer, 
         ACS::doubleSeq *data, 
         unsigned long *udata, 
         StatusParameters *status_par, 
         const unsigned int cmd_idx, 
         const CDBParameters *const cdb_ptr, 
         ACS::Time &timestamp,
         const int slave=-1
) throw (ComponentErrors::SocketErrorExImpl);

/**
 * Return the request id of a message string.
 * @param message the message to parse
 * @return the request id of a message
 */
 unsigned long get_request_id(const string message);


/**
 * Return the minor servo address of a message string.
 * @param message the message to parse
 * @return the minor servo address of a message string
 */
 unsigned short get_msaddress(const string message);


/**
 * This member function convert the real positions to virtual positions.
 * @param positions reference to doubleSeq positions
 * @param servo_address address of minor servo
 * @param zero the zero of the real reference system
 */
void real2virtual(ACS::doubleSeq &positions, DWORD servo_address, const double zero);

/**
 * This member function convert the virtual positions to real positions.
 * @param positions reference to doubleSeq positions
 * @param servo_address address of minor servo
 * @param zero the zero of the real reference system
 */
void virtual2real(ACS::doubleSeq &positions, DWORD servo_address, const double zero);


#endif
