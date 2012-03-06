/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
 *
 *  External functions
 *  ==================
 *    * split, strip, startswith, endswith: functions used to manipulate 
 *      strings (utils.h)
 *
\*******************************************************************************/

#include "WPServoSocket.h"
#include "libCom.h" 
#include <limits>
#include <string>
#include <bitset>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <baciDB.h>
#include <ManagmentDefinitionsS.h>
#include <iostream>
#include <new>
#include <MinorServoErrors.h>
#include <MSParameters.h>
#include "macros.def"

// Initialize the singleton socket pointer
WPServoSocket* WPServoSocket::m_instance_ptr_ = NULL;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

WPServoSocket* WPServoSocket::getSingletonInstance(
        const CDBParameters *const cdb_ptr, 
        ExpireTime * const expire_ptr
        ) 
{
    try {
        // Make an instance only if there is not any. Thread safe singleton pattern
        pthread_mutex_lock(&mutex); 
        if (m_instance_ptr_ == NULL)
            m_instance_ptr_ = new WPServoSocket(cdb_ptr, expire_ptr);
        pthread_mutex_unlock(&mutex);
    }
    catch(...) {
        pthread_mutex_unlock(&mutex);
    }

    return m_instance_ptr_;
}
     

WPServoSocket::WPServoSocket(
        const CDBParameters *const cdb_ptr, 
        ExpireTime *const expire_ptr
        ): CSocket(), m_cdb_ptr(cdb_ptr) 
{
    m_soStat = NOTRDY;
    m_raw_answer = "";
    Init();
}


WPServoSocket::~WPServoSocket() { 
    Close(m_Error); 
    delete m_instance_ptr_;
}


void WPServoSocket::Init() throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::SocketErrorExImpl) {

    AUTO_TRACE("WPServoSocket; creating minorServo socket");
    
    if(Create(m_Error, STREAM) == FAIL) {
        Close(m_Error);
        THROW_EX(ComponentErrors, SocketErrorEx, "Error creating socket", false);
    }

    if (Connect(m_Error, m_cdb_ptr->SERVER_IP, m_cdb_ptr->SERVER_PORT) == FAIL) {
        ACS_SHORT_LOG(
            (
                 LM_ERROR, 
                 "Error connecting socket on %s:%d", 
                 (const char *)m_cdb_ptr->SERVER_IP, 
                 m_cdb_ptr->SERVER_PORT
            )
        );
        m_soStat=TOUT;
        Close(m_Error);
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "WPServoSocket::Init(); Error connecting socket");
        throw exImpl;
    } 
    else {
        ACS_SHORT_LOG(
            (
                 LM_INFO, 
                 "WPServoSocket::Init; conneted on %s:%d", 
                 (const char *)m_cdb_ptr->SERVER_IP, 
                 m_cdb_ptr->SERVER_PORT
            )
        );
        
        // if(setSockMode(m_Error, BLOCKINGTIMEO, m_cdb_ptr->SERVER_TIMEOUT) != SUCCESS) {
        if(setSockMode(m_Error, NONBLOCKING) != SUCCESS) {
            m_soStat=NOTRDY;
            Close(m_Error);
            THROW_EX(ComponentErrors, SocketErrorEx, "Error configuring the socket", false);
        }
    }

}


void WPServoSocket::sendBuffer(string msg_str) throw (ComponentErrors::SocketErrorExImpl) {

    size_t num_bytes = 0, sent_bytes = 0;  
    const BYTE *msg = (const BYTE *)(msg_str.c_str());
    size_t len = strlen((const char *)(msg));

    while(sent_bytes < len) {
        // Send returns the total number of bytes sent
        if ((num_bytes = Send(m_Error, (const void *)(msg + sent_bytes), len - sent_bytes)) < 0) 
            break ;
        else sent_bytes += num_bytes;
    }

    if (sent_bytes != len) {
        ACS_SHORT_LOG((LM_ERROR, "Socket problem, not all bytes sent."));
        m_soStat = NOTRDY;
    }   
}
 

void WPServoSocket::receiveBuffer(string & answer, string closer, string headers) throw (ComponentErrors::SocketErrorExImpl) {

    BYTE msg[BUFF_LIMIT] = {0x00};
    ostringstream msg_stream;
    msg_stream.precision(__UTILS_STREAM_PREC__);
    string header;
    
    // Receive the response form the minorServo one byte at once 
    for(size_t i=0; i<BUFF_LIMIT; i++) {
        if(Receive(m_Error, (void *)(&msg[i]), 1) == 1) {
            msg_stream.clear();
            msg_stream.str("");
            msg_stream << msg[i];
            m_raw_answer += msg_stream.str();
            if(endswith(m_raw_answer, closer))
                break;
            for(string::size_type idx=0; idx != headers.size(); idx++) {
                header = headers[idx];
                if(endswith(m_raw_answer, header)) {
                    m_raw_answer = header;
                    break;
                }
            }
        }
    }

    answer.clear();
    if((!m_raw_answer.empty()) && endswith(m_raw_answer, closer)) {
            // Retrieve the position index of closer character
            string::size_type idx = m_raw_answer.find(closer);
            // The answer is the string sent by MinorServo without closer character.
            answer = m_raw_answer.substr(0, idx);
            m_raw_answer.clear();
    }
}


