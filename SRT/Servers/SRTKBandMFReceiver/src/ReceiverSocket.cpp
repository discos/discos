/************************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
 *
 *
 *  Metadata
 *  ========
 *  Created: Tue May 18 11:24:41 CEST 2010
 *  
 *
 *  External functions
 *  ==================
 *  split, strip, startswith, endswith: functions used to manipulate strings (utils.h)
\*************************************************************************************/

#include "ReceiverSocket.h"
#include <limits>
#include <string>
#include <bitset>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <baciDB.h>
#include <ManagmentDefinitionsS.h>
#include <new>
#include <SRTKBandMFParameters.h>
#include <vector>
#include <Talk.h>
#include <conversions.def>

using namespace std;


ReceiverSocket::ReceiverSocket(const RCDBParameters *cdb_ptr) : CSocket(), m_cdb_ptr(cdb_ptr)
{
    m_request_id = 0;
    m_soStat = NOTRDY;
}


ReceiverSocket::~ReceiverSocket() {
    Close(m_Error);
}


void ReceiverSocket::Init() throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::SocketErrorExImpl) {

    AUTO_TRACE("ReceiverSocket::Init()");
    
    if(Create(m_Error, STREAM) == FAIL) {
        ACS_SHORT_LOG((LM_ERROR, "Error creating socket"));
        Close(m_Error);
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "ReceiverSocket::Init(), I cannot create socket");
        throw exImpl;
    }

    if (Connect(m_Error, m_cdb_ptr->IP, m_cdb_ptr->PORT) == FAIL) {
        ACS_SHORT_LOG((LM_ERROR, "Error connecting socket on %s:%d", 
                 (const char *)m_cdb_ptr->IP, m_cdb_ptr->PORT)
        );
        m_soStat=TOUT;
        Close(m_Error);
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "ReceiverSocket::Init(); Error connecting socket");
        throw exImpl;
    } 
    else {
        ACS_SHORT_LOG((LM_INFO, "ReceiverSocket::Init; conneted on %s:%d", 
                 (const char *)m_cdb_ptr->IP, m_cdb_ptr->PORT)
        );
        
        if(setSockMode(m_Error, BLOCKINGTIMEO, m_cdb_ptr->TIMEOUT) != SUCCESS) {
            ACS_SHORT_LOG((LM_ERROR, "Error configuring socket"));
            m_soStat=NOTRDY;
            Close(m_Error);
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                    "ReceiverSocket::Init(); Error configuring the socket");
            throw exImpl;
        }
    }
}


void ReceiverSocket::cal_on() throw (ComponentErrors::SocketErrorExImpl) {
    AUTO_TRACE("ReceiverSocket::cal_on()");
    make_talk(CAL_ON); 
}


void ReceiverSocket::cal_off() throw (ComponentErrors::SocketErrorExImpl) {
    AUTO_TRACE("ReceiverSocket::cal_off()");
    make_talk(CAL_OFF); 
}


void ReceiverSocket::set_mode(const char *mode ) throw (ComponentErrors::SocketErrorExImpl) {
    AUTO_TRACE("ReceiverSocket::set_mode()");

    Mode MODE = SDISH;

    // strcmp returns 0 if they are equal
    if(!strcmp(mode, "VLBI"))
        MODE = VLBI;

    make_talk(SET_MODE, MODE); 
}


double ReceiverSocket::vacuum(void) throw (ComponentErrors::SocketErrorExImpl)
{
    AUTO_TRACE("ReceiverSocket::vacuum()");
    // The last byte (+1) stores the paramiter size.
    vbyte answer = make_talk(VACUUM, NONE, FIXED_AL + 1, true); 
    
    return GET_VACUUM(get_value(answer, APARAMS_IDX + VACUUM_PARAMS_LEN));
}


vbyte ReceiverSocket::make_talk(Command cmd, Mode mode, const size_t AL, bool is_with_params) 
{
    vector< vbyte > requests;
    vbyte answer;

    Talk talk(cmd, mode);
    talk.make_request(&requests, &m_request_id, mode);

    for(vector< vbyte >::size_type idx = 0; idx != requests.size(); idx++) {
        // Send a position request
        sendBuffer(requests[idx]);
        try {
            // Set the answer received
            answer = receiveBuffer(AL, is_with_params); 
            talk.verify(&requests[idx], &answer);
        }
        catch(ComponentErrors::SocketErrorEx &ex) {
            ACS_SHORT_LOG((LM_ERROR, "An error occour evaluating the answer"));
            throw;
        }
    }

    return answer;
}


double ReceiverSocket::get_value(const vbyte answer, const vbyte::size_type param_idx)
{

    const size_t F32_LEN = 4;
    union Bytes2Float {
        float value;
        BYTE buff[F32_LEN];
    } uvalue;

    vbyte::size_type sidx = param_idx + F32_LEN * VACUUM_IDX; 
    for(vbyte::size_type idx = sidx, vidx = F32_LEN-1; idx < sidx + F32_LEN; idx++) {
        uvalue.buff[vidx] = answer[idx];
        --vidx;
    }
    return uvalue.value;
}


void ReceiverSocket::sendBuffer(const vbyte request) throw (ComponentErrors::SocketErrorExImpl) 
{

    size_t bytesNum = 0;  // Number of bytes sent for each call to Send
    size_t bytesSent = 0; // Total bytes sent
    const size_t msg_size = request.size();

    // Create a dynamic array from a vector
    BYTE *const msg_ptr = new BYTE[msg_size]();
    for(size_t idx = 0; idx != msg_size; ++idx)
        msg_ptr[idx] = request[idx];

    while(bytesSent < msg_size) {
        // Return the total number of bytes sent
        if ((bytesNum = Send(m_Error, (const void *)(msg_ptr + bytesSent), msg_size - bytesSent)) < 0) 
            break ;
        else bytesSent += bytesNum;
    }

    delete [] msg_ptr;

    if (bytesSent != msg_size) {
        ACS_SHORT_LOG((LM_ERROR, "Socket problem, not all bytes sent."));
        m_soStat = NOTRDY;
    }   
}
 

const vbyte ReceiverSocket::receiveBuffer(const size_t msg_size, bool is_with_params) throw (ComponentErrors::SocketErrorExImpl) {

    BYTE msg[BUFF_LEN];
    vbyte answer;
    size_t param_size = 0;
    
    // Receive the response form the device one byte at once 
    for(size_t i=0; i < msg_size; i++)
        if(Receive(m_Error, (void *)(&msg[i]), 1) == 1)
            answer.push_back(msg[i]);
        else 
            break;
    if(is_with_params) {
        param_size = answer[answer.size()-1];
        for(size_t i = 0; i < param_size; i++)
            if(Receive(m_Error, (void *)(&msg[i]), 1) == 1)
                answer.push_back(msg[i]);
            else 
                break;
    }

    if((msg_size + param_size) != answer.size()) {
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, "Error: unexpected answer length.");
        throw exImpl;
    }

    return answer;
}


