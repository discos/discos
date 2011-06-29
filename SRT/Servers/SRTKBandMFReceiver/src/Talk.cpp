/********************************************************\
 *  File Infos
 *  ==========
 *  Class to use for communicating with receiver boards.
 *  Look at talk.h for more informations.
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
\********************************************************/

#include <string>
#include <IRA>
#include <cstdlib>
#include <sstream>
#include <ManagmentDefinitionsS.h>
#include <pthread.h>
#include <Talk.h>
#include "utils.h"
#include "macros.def"
#include <vector>

using namespace std;
using namespace IRA;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


Talk::Talk(Command command, Mode mode): m_command(command), m_mode(mode) {
    AUTO_TRACE("Talk::Talk()");
}


void Talk::make_request(vector< vector<BYTE> > *const requests_ptr, unsigned int *request_id_ptr, Mode mode) {

    vector<BYTE> common_req;

    // Make a general request (all bytes are common to every request)
    common_req.push_back(SRH); // Append the command header (index 0)
    common_req.push_back(RADDRESS); // Append the recipient address (1)
    common_req.push_back(SADDRESS); // Append the sender address (2)
    common_req.push_back(0x00); // Initialize to 0x00 the command (3)
    common_req.push_back(0x00); // Initialize to 0x00 the request_id (4)

    requests_ptr->clear(); // Clear the vector

    switch(m_command) {
        // Append specific bytes for a cal_on request
        case CAL_ON: {
            requests_ptr->push_back(common_req);
            ((*requests_ptr)[0]).push_back(0x04); // Append four parameters
            ((*requests_ptr)[0]).push_back(ONE_BIT_DT); // Append the data type (one bit)
            ((*requests_ptr)[0]).push_back(DIO_PORT); // Append the port type (digital IO)
            ((*requests_ptr)[0]).push_back(CAL_PN); // Port number (bit out11 of Digital IO)
            ((*requests_ptr)[0]).push_back(TURN_ON); // Active this bit
            ((*requests_ptr)[0])[CMD_IDX] = SSD; // Short Set Data command
            // The request_id increment heve to be an atomic operation
            pthread_mutex_lock(&mutex); 
            ((*requests_ptr)[0])[ID_IDX] = ++(*request_id_ptr);
            pthread_mutex_unlock(&mutex);
            break ;
        }
        // Append specific bytes for a cal_off request
        case CAL_OFF: {
            requests_ptr->push_back(common_req);
            ((*requests_ptr)[0]).push_back(0x04); // Append four parameters
            ((*requests_ptr)[0]).push_back(ONE_BIT_DT); // Append the data type (one bit)
            ((*requests_ptr)[0]).push_back(DIO_PORT); // Append the port type (digital IO)
            ((*requests_ptr)[0]).push_back(CAL_PN); // Port number (bit out11 of Digital IO)
            ((*requests_ptr)[0]).push_back(TURN_OFF); // Active this bit
            ((*requests_ptr)[0])[CMD_IDX] = SSD; // Short Set Data command
            // The request_id increment heve to be an atomic operation
            pthread_mutex_lock(&mutex); 
            ((*requests_ptr)[0])[ID_IDX] = ++(*request_id_ptr);
            pthread_mutex_unlock(&mutex);
            break ;
        }
        // Append specific bytes for a set_mode request
        case SET_MODE: {
            switch (mode) {
                case SDISH: {
                    /************** First item of requests ***************/
                    requests_ptr->push_back(common_req);
                    ((*requests_ptr)[0]).push_back(0x04); // Append four parameters
                    ((*requests_ptr)[0]).push_back(ONE_BIT_DT); // Append the data type (one bit)
                    ((*requests_ptr)[0]).push_back(DIO_PORT); // Append the port type (digital IO)
                    ((*requests_ptr)[0]).push_back(SET_MODE_PN); // Port number (bit out13 of Digital IO)
                    ((*requests_ptr)[0]).push_back(TURN_ON); // Active this bit
                    ((*requests_ptr)[0])[CMD_IDX] = SSD; // Short Set Data command
                    // The request_id increment heve to be an atomic operation
                    pthread_mutex_lock(&mutex); 
                    ((*requests_ptr)[0])[ID_IDX] = ++(*request_id_ptr);
                    pthread_mutex_unlock(&mutex);
                    /************** Second item of requests ***************/
                    requests_ptr->push_back(common_req);
                    ((*requests_ptr)[1]).push_back(0x04); // Append four parameters
                    ((*requests_ptr)[1]).push_back(ONE_BIT_DT); // Append the data type (one bit)
                    ((*requests_ptr)[1]).push_back(DIO_PORT); // Append the port type (digital IO)
                    ((*requests_ptr)[1]).push_back(SET_MODE_PN + 0x01); // Port number (bit out14 of Digital IO)
                    ((*requests_ptr)[1]).push_back(TURN_OFF); // Pull this bit down
                    ((*requests_ptr)[1])[CMD_IDX] = SSD; // Short Set Data command
                    // The request_id increment heve to be an atomic operation
                    pthread_mutex_lock(&mutex); 
                    ((*requests_ptr)[1])[ID_IDX] = ++(*request_id_ptr);
                    pthread_mutex_unlock(&mutex);
                    break ;
                }
                case VLBI: {
                    /************** First item of requests ***************/
                    requests_ptr->push_back(common_req);
                    ((*requests_ptr)[0]).push_back(0x04); // Append four parameters
                    ((*requests_ptr)[0]).push_back(ONE_BIT_DT); // Append the data type (one bit)
                    ((*requests_ptr)[0]).push_back(DIO_PORT); // Append the port type (digital IO)
                    ((*requests_ptr)[0]).push_back(SET_MODE_PN); // Port number (bit out13 of Digital IO)
                    ((*requests_ptr)[0]).push_back(TURN_OFF); // Pull this bit down
                    ((*requests_ptr)[0])[CMD_IDX] = SSD; // Short Set Data command
                    // The request_id increment heve to be an atomic operation
                    pthread_mutex_lock(&mutex); 
                    ((*requests_ptr)[0])[ID_IDX] = ++(*request_id_ptr);
                    pthread_mutex_unlock(&mutex);
                    /************** Second item of requests ***************/
                    requests_ptr->push_back(common_req);
                    ((*requests_ptr)[1]).push_back(0x04); // Append four parameters
                    ((*requests_ptr)[1]).push_back(ONE_BIT_DT); // Append the data type (one bit)
                    ((*requests_ptr)[1]).push_back(DIO_PORT); // Append the port type (digital IO)
                    ((*requests_ptr)[1]).push_back(SET_MODE_PN + 0x01); // Port number (bit out14 of Digital IO)
                    ((*requests_ptr)[1]).push_back(TURN_ON); // Active this bit
                    ((*requests_ptr)[1])[CMD_IDX] = SSD; // Short Set Data command
                    // The request_id increment heve to be an atomic operation
                    pthread_mutex_lock(&mutex); 
                    ((*requests_ptr)[1])[ID_IDX] = ++(*request_id_ptr);
                    pthread_mutex_unlock(&mutex);
                    break ;
                }
                default: {
                    ACS_SHORT_LOG((LM_ERROR, "SET_MODE: default!!"));
                    break;
                }
            }
            break;
        }
        // Append specific bytes for a vacuum request
        case VACUUM: {
            requests_ptr->push_back(common_req);
            ((*requests_ptr)[0]).push_back(0x03); // Append 3 parameters
            ((*requests_ptr)[0]).push_back(F32_BIT_DT); // Append the data type (32 bits)
            ((*requests_ptr)[0]).push_back(AD24_PORT); // Append the port type (AD24)
            ((*requests_ptr)[0]).push_back(VACUUM_PN); // Port number (AD8-AD15)
            ((*requests_ptr)[0])[CMD_IDX] = SGD; // Short Get Data command
            // The request_id increment heve to be an atomic operation
            pthread_mutex_lock(&mutex); 
            ((*requests_ptr)[0])[ID_IDX] = ++(*request_id_ptr);
            pthread_mutex_unlock(&mutex);
            break ;
        }
    }
}


void Talk::verify(const vector<BYTE> *const request_ptr, const vector<BYTE> *const answer_ptr) 
    throw (ComponentErrors::SocketErrorExImpl) {

    // Raise an exception if the answer doesn't start with SAH
    if(!vstartswith(*answer_ptr, SAH))
        THROW_EX(ComponentErrors, SocketErrorEx, "Wrong answer header", false);

    // Raise an exception if the execution result isn't an ACK
    const unsigned short ex_result = static_cast<unsigned short>((*answer_ptr)[EXR_IDX]);
    if(ex_result) 
        THROW_EX(ComponentErrors, SocketErrorEx, "Execution result error: " + number2str(ex_result), false);

    // Raise an exception if the answer id is different from request id
    const unsigned short answer_id = static_cast<unsigned short>((*answer_ptr)[ID_IDX]);
    const unsigned short request_id = static_cast<unsigned short>((*request_ptr)[ID_IDX]);
    if(answer_id != request_id)
        THROW_EX(ComponentErrors, SocketErrorEx, "Error: mismatch between IDs (request, answer): (" 
                + number2str(request_id) + ", " + number2str(answer_id) + ")", false);

    // Raise an exception if there are some addressing errors between request and answer
    if((*request_ptr)[SADD_IDX] != (*answer_ptr)[RADD_IDX])
        THROW_EX(ComponentErrors, SocketErrorEx, 
                "Error: mismatch between sender in the request and recipient in the answer", false);

    // Raise an exception if there are some addressing errors between request and answer
    if((*request_ptr)[RADD_IDX] != (*answer_ptr)[SADD_IDX])
        THROW_EX(ComponentErrors, SocketErrorEx, 
                "Error: mismatch between recipient in the request and sender in the answer", false);

    // Raise an exception if there is a mismatch between the request and answer command byte
    if((*request_ptr)[CMD_IDX] != (*answer_ptr)[CMD_IDX])
        THROW_EX(ComponentErrors, SocketErrorEx, 
                "Error: mismatch between request and answer command byte", false);

    if((*answer_ptr).size() > FIXED_AL) {
        // The index starts from the first parameter
        for(vector<BYTE>::size_type idx = 0; idx < (*request_ptr)[FIXED_AL-1]; idx++)
            if((*answer_ptr)[FIXED_AL + idx + 1] != (*request_ptr)[FIXED_AL + idx])
                THROW_EX(ComponentErrors, SocketErrorEx, 
                    "Error: mismatch between request and answer parameters", false);
    }
}

