/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include "sensorSocket.h"
#include <limits>
#include <string>
#include <bitset>

sensorSocket::sensorSocket(
        CString IP, 
        DWORD port, 
        DWORD timeout, 
        double PS_CF, 
        long PS_REFERENCE, 
        double ICD_REFERENCE) :
    CSocket(),
    m_PS_IP(IP),
    m_PS_PORT(port),
    m_PS_TIMEO(timeout),
    m_PS_CF(PS_CF),
    m_PS_REFERENCE(PS_REFERENCE),    
    m_ICD_REFERENCE(ICD_REFERENCE) {   

    m_soStat = NOTRDY;
}

sensorSocket::~sensorSocket() {
    Close(m_Error);
}

void sensorSocket::Init() throw (ComponentErrors::SocketErrorExImpl) {

    AUTO_TRACE("sensorSocket; creating derotator sensor socket");
    
    if(Create(m_Error, STREAM) == FAIL) {
        Close(m_Error);
        ACS_DEBUG("sensorSocket", "Error creating socket");
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "sensorSocket::Init(), I can't create derotator sensor socket");
        throw exImpl;
    }

    ACS_SHORT_LOG((LM_INFO, "sensorSocket; conneting socket..."));
    if (Connect(m_Error, m_PS_IP, m_PS_PORT) == FAIL) {
        ACS_SHORT_LOG((
                    LM_ERROR, "Error connecting socket on %s port %d", 
                    (const char*)m_PS_IP, m_PS_PORT
                    ));
        m_soStat=TOUT;
        Close(m_Error);
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "sensorSocket::Init(); Error connecting socket");
        throw exImpl;
    } 
    else {
        ACS_SHORT_LOG((
                    LM_INFO, "sensorSocket:socket on %s port %d connected", 
                    (const char*)m_PS_IP, m_PS_PORT
                    ));
        
        if(setSockMode(m_Error, BLOCKINGTIMEO, m_PS_TIMEO, m_PS_TIMEO) != SUCCESS) {
            ACS_DEBUG("sensorSocket", "Error configuring the socket");
            m_soStat=NOTRDY;
            Close(m_Error);
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                    "sensorSocket::Init(); Error configuring the socket");
            throw exImpl;
        }
    }
}


double sensorSocket::getPosition() throw (ComponentErrors::SocketErrorExImpl) {
    
    BYTE buff[] = {STS, PR, SA, 0x00, ETS}; // This message is a sensor position request (PR)

    int bytesNum = 0;  // Number of bytes sent for each call to Send
    int bytesSent = 0; // Total bytes sent

    AUTO_TRACE("sensorSocket::getPosition()");

    set_checksum(buff);

    while(bytesSent < BUFF_LEN) {
        // Send returns the total number of bytes sent
        if ((bytesNum = Send(m_Error, (const void *)(buff + bytesSent), BUFF_LEN-bytesSent)) < 0) 
            break ;
        else bytesSent += bytesNum;
    }
    
    if (bytesSent != BUFF_LEN) {
        ACS_LOG(
                LM_FULL_INFO, 
                "sensorSocket::getPosition, Socket problem, not all bytes sent", 
                (LM_INFO,"getPosition")
                );
        m_soStat = NOTRDY;

        if(reConnect(bytesSent)) { // try to reconnect
            ACS_LOG(
                    LM_FULL_INFO, 
                    "sensorSocket::getPosition, Error recreating the socket connection", 
                    (LM_INFO,"getPosition")
                    );
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                    "sensorSocket::getPosition(), I can't reconnect to the socket");
            throw exImpl;
        }
    }   
    
    // Receive the response in the same buffer used to make a request
    receiveBuffer(buff, BUFF_LEN);

    unsigned char position_bytes[WORD_LEN] ; // This array stores the position

    // Storing of position bytes: position_bytes <- buff[1], buff[2], buff[3] 
    for(int i=0; i<WORD_LEN; i++) position_bytes[i] = buff[FB_INDEX+i] ;

    // The last item is '\0'
    char binChar[CHAR_BIT*WORD_LEN + 1];

    // The position bytes are stored as a chars, so word2binChar converts the
    // position bytes to binary and save the result in binChar. Every item
    // of binChar array is a char that stands for a bit ('0' or '1').
    word2binChar(binChar, position_bytes, WORD_LEN) ;

    long sensor_length ; // The sensor length in steps (1 step = 0.01 mm)
    string binStr = binChar ;

    // Conversion from binary string to integer. For instance: "1001" -> 9
    sensor_length = bitset<numeric_limits<unsigned long>::digits>(binStr).to_ulong() ; 

    // Conversion from position unit (step) to angle unit (degree) 
    m_sensor_position = -m_PS_CF*(sensor_length - m_PS_REFERENCE);

    return -m_sensor_position; // Return the sensor position in degree.
}   


double sensorSocket::getURSPosition() {

    AUTO_TRACE("sensorSocket::getURSPosition()");
    
    // Return the User Reference System position in degree.
    return getPosition() + m_ICD_REFERENCE;
}   

//---  Protected Methods ---//
    
void sensorSocket::receiveBuffer(BYTE *msg, WORD len) 
    throw (ComponentErrors::SocketErrorExImpl) {

    for(int j=0; j<5*len; j++) {
        if(Receive(m_Error, (void *)(&msg[0]), 1) == 1) {
            if(msg[0] == STS) // 0x02 -> '#' (request header)
                break;
            else
                continue;
        }
    } 
    
    if(msg[0] != STS) {
        ACS_DEBUG("sensorSocket::receiveBuffer()", "no answer header received");
        msg[0] = 0x00;
    }
    else {
        for(size_t i=1; i<len;) {
            if(Receive(m_Error, (void *)(&msg[i]), 1) == 1) {
                i++;
            }
            else
                continue;
        }
    }
}


void sensorSocket::set_checksum(BYTE* buff)
{
    DWORD sum = 0;
    
    for(int i=0; i < CHECKSUM_INDEX; i++) 
        sum += buff[i];       

    buff[CHECKSUM_INDEX] = sum;
}


int sensorSocket::reConnect(int error)
{
    int err = 0;
    
    switch(error) {
        /* check the source of tout & clear the input buffer of USD's*/
        case WOULDBLOCK: {
            m_soStat = READY;        // wasn't a socket problem
            err = clearUSDBuffer();
            if(err == 0) return -1; //it was a USD tout. USD's buffer cleared   
        }
        case FAIL:  
        case 0: {   //socket remotely closed. Try to reconnect...
            Init();
        }
    }
    return err;             
 }


int sensorSocket::clearUSDBuffer()
{
    BYTE buff[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };   //clear USD input buffer
    int err = 0, len = 8, written = 0;
         
    while (written < len) {
         err = Send(m_Error, (const void *)buff, len);
         if (err < 0) break; 
         else written += err;
    }   

    if(err < 0 || written != len) {      
        m_soStat = NOTRDY;
        return err;     
    } 
    m_soStat = READY;
    return 0; //success clearing USD buffer
}


