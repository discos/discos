/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include "icdSocket.h"
#include <limits>
#include <string>
#include <bitset>
#include "../test/icd_response_test.h"


// The master device flags a new command for the compact drive by changing the
// signal of the sf bit:
//  - (sf == 0) => (requestdata == 0x30)
//  - (sf == 1) => (requestdata == 0x38)
static int sf = 0 ; // sendflag


icdSocket::icdSocket(
        CString IP, 
        DWORD port, 
        DWORD timeout,
        double ICD_CF, 
        double ICD_REFERENCE,
        double ICD_MAX_VALUE,
        double ICD_MIN_VALUE,
        double ICD_POSITION_EXPIRE_TIME):
    CSocket(),
    m_ICD_IP(IP),
    m_ICD_PORT(port),
    m_ICD_TIMEO(timeout),
    m_ICD_CF(ICD_CF),
    m_ICD_REFERENCE(ICD_REFERENCE),   
    m_ICD_MAX_VALUE(ICD_MAX_VALUE),
    m_ICD_MIN_VALUE(ICD_MIN_VALUE), 
    m_ICD_POSITION_EXPIRE_TIME(ICD_POSITION_EXPIRE_TIME) {

    m_soStat = NOTRDY;
    m_icd_verbose_status = 0;
    m_icd_summary_status = 0;
    m_timeLastPositionRequest = -100.0;
}

icdSocket::~icdSocket() {
    Close(m_Error);
}

void icdSocket::Init() throw (ComponentErrors::SocketErrorExImpl) {

    AUTO_TRACE("icdSocket; creating derotator icd socket");
    
    if(Create(m_Error, STREAM) == FAIL) {
        ACS_DEBUG("icdSocket", "Error creating socket");
        Close(m_Error);
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "icdSocket::Init(), I can't create derotator icd socket");
        throw exImpl;
    }

    ACS_SHORT_LOG((LM_INFO, "icdSocket; conneting socket..."));
    if (Connect(m_Error, m_ICD_IP, m_ICD_PORT) == FAIL) {
        ACS_SHORT_LOG((
                    LM_ERROR, "Error connecting socket on %s port %d", 
                    (const char*)m_ICD_IP, m_ICD_PORT
                    ));
        m_soStat=TOUT;
        Close(m_Error);
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "icdSocket::Init(); Error connecting socket");
        throw exImpl;
    } 
    else {
        ACS_SHORT_LOG((
                    LM_INFO, "icdSocket:socket on %s port %d connected", 
                    (const char*)m_ICD_IP, m_ICD_PORT
                    ));
        
        if(setSockMode(m_Error, BLOCKINGTIMEO, m_ICD_TIMEO, m_ICD_TIMEO) != SUCCESS) {
            ACS_DEBUG("icdSocket", "Error configuring the socket");
            m_soStat=NOTRDY;
            Close(m_Error);
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                    "icdSocket::Init(); Error configuring the socket");
            throw exImpl;
        }
    }

    setTargetPosition(getURSPosition());
}

double icdSocket::getURSPosition() {

    AUTO_TRACE("icdSocket::getURSPosition()");
    
    // Return the User Reference System position in degree.
    return getPosition() + m_ICD_REFERENCE;
}   



double icdSocket::getPosition() throw (ComponentErrors::SocketErrorExImpl) {

    BYTE buff[] = { // 8, 0, 0, 6, 0, 0, 1, F, 0, 0, 0, 0, 0, 0, 0, 0, CR
        0x38, 0x30, 0x30, 0x36, 0x30, 0x30, 0x31, 0x46,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x0D
    }; // This message is an ICD ``actual position request`` (Status.p_act)

    timeval now;

    AUTO_TRACE("icdSocket::getPosition()");
    
    // If responseCheck (called by make_talk) finds an error it set a bit on 
    // pattern property to communicate the problem to client. 
    // After it logs the error code trows an exception
    try {
        if(isPositionUpToDate()) {
            // Make a conversation to and from icd. The response is stored in buff
            return m_icd_position; // Return the last value of icd position in degree.
        }
        else {
            make_talk(buff, true) ;
            gettimeofday(&now, NULL);
            m_timeLastPositionRequest = now.tv_sec + now.tv_usec / 1000000.0;
        }
    }
    catch (...) {
    }

    // Check the status and set the status pattern
    fbstatuswordCheck(buff);

    // icd_position is the icd position **in steps**
    // For instance, if the ICD response is:
    //     xxxxxxxxFFFFFDDC
    // that means the sign is negative and the correct value of
    // icd_position is:
    //    (FDDC - FFFF)h = 64988 - 65535
    // A response like the follow:
    //     xxxxxxxx00000DDC
    // means that the sign is positive, so the correct value is:
    //     (0DDC - 0000)h = 3548 - 0
    long icd_position = hex2dec(buff, ICD_FBRD_INDEX, ICD_RDWORD_LEN) \
                      - hex2dec(buff, ICD_SIGN_INDEX, ICD_RDWORD_LEN);

    // Conversion from position unit (step) to angle unit (degree) 
    m_icd_position = m_ICD_CF*icd_position ;

    return m_icd_position; // Return the icd position in degree.
}   


void icdSocket::setPositiveDir() throw (ComponentErrors::SocketErrorExImpl) {

    BYTE buff[] = { // 8, 4, 0, 6, 0, 0, 1, C, 0, 0, 0, 0, 0, 0, 0, 0, CR
        0x38, 0x34, 0x30, 0x36, 0x30, 0x30, 0x31, 0x43,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x0D
    }; // Set a positive rotation way (Motion.invertDir, 28:6 - 1C:6h, value: 0)

    AUTO_TRACE("icdSocket::setPositiveDir()");
    
    // If responseCheck (called by make_talk) finds an error it set a bit on 
    // pattern property to communicate the problem to client. 
    // After it logs the error code trows an exception
    try {
        // Make a conversation to and from icd. The response is stored in buff
        make_talk(buff, true) ;
    }
    catch (...) {
    }
}   


void icdSocket::setNegativeDir() throw (ComponentErrors::SocketErrorExImpl) {

    BYTE buff[] = { // 8, 4, 0, 6, 0, 0, 1, C, 0, 0, 0, 0, 0, 0, 0, 1, CR
        0x38, 0x34, 0x30, 0x36, 0x30, 0x30, 0x31, 0x43,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31,
        0x0D
    }; // Set a negative rotation way (Motion.invertDir, 28:6 - 1C:6h, value: 1)

    AUTO_TRACE("icdSocket::setNegativeDir()");
    
    // If responseCheck (called by make_talk) finds an error it set a bit on 
    // pattern property to communicate the problem to client. 
    // After it logs the error code trows an exception
    try {
        // Make a conversation to and from icd. The response is stored in buff
        make_talk(buff, true) ;
    }
    catch (...) {
    }
}   


double icdSocket::getTargetPosition() { return m_icd_target_position; }


void icdSocket::setPosition(double position) throw (ComponentErrors::SocketErrorExImpl, MinorServoErrors::PositioningErrorEx) {

    AUTO_TRACE("icdSocket::setPosition()");

    BYTE buff[] = { // 8, 4, 0, 1, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, CR
        0x38, 0x34, 0x30, 0x31, 0x30, 0x30, 0x32, 0x33,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x0D
    }; // This message is an ICD ``set absolute position`` (PTP.p_absPTP, 35:1)

    double sh_position;
    sh_position = position - m_ICD_REFERENCE;

    const double max_rel = m_ICD_MAX_VALUE + m_ICD_REFERENCE;
    const double min_rel = m_ICD_MIN_VALUE + m_ICD_REFERENCE;

    m_icd_summary_status &= ~(1 << W) ;
    setTargetPosition(position);

    if(sh_position > m_ICD_MAX_VALUE) {
        sh_position = m_ICD_MAX_VALUE;
        // position = max_rel;
        ACS_SHORT_LOG((LM_WARNING, "# You are trying to set a position out of range (%.2f°, %.2f°)", min_rel, max_rel));
        // ACS_SHORT_LOG((LM_WARNING, "# The component is setting the position to max allowed value: %.2f°", max_rel)); 
        m_icd_summary_status |=  (1 << W);
        return;
    }
    else
        if(sh_position < m_ICD_MIN_VALUE) {
            sh_position = m_ICD_MIN_VALUE;
            // position = min_rel;
            ACS_SHORT_LOG((LM_WARNING, "# You are trying to set a position out of range (%.2f°, %.2f°)", min_rel, max_rel));
            // ACS_SHORT_LOG((LM_WARNING, "# The component is setting the position to min allowed value: %.2f°", min_rel)); 
            m_icd_summary_status |=  (1 << W);
            return;
        }
    
    if(sh_position < 0)
        setNegativeDir();

    if(sh_position >= 0)
        setPositiveDir();

    unsigned long dec_steps = static_cast<unsigned long>(sh_position/m_ICD_CF + 0.5); // No shifted, rounded

    unsigned char hex_steps[ICD_CMDDATA_LEN + 1];
    dec2hexStr(dec_steps, hex_steps, ICD_CMDDATA_LEN + 1);
    int i = 0;
    while(1) {
        if(hex_steps[i] == '\0') break;
        i++;
    }

    int k=0;
    for(int j=i-1; j>=0; j--) {
        buff[15-j] = hex_steps[k];
        k++;
    }
    
    try {
        // Make a conversation to and from icd. The response is stored in buff
        make_talk(buff, true) ;
        m_icd_verbose_status &= ~(1 << WRONG_POSITION) ;
    }
    // If responseCheck (called by make_talk) finds an error it set a bit on 
    // pattern property to communicate the problem to client. 
    // After it logs the error code trows an exception
    catch (...) {
        m_icd_verbose_status |=  (1 << WRONG_POSITION);
        MinorServoErrors::PositioningErrorExImpl exUnk(__FILE__, __LINE__, 
                "Cannot set derotator position");
	    throw exUnk.getPositioningErrorEx();
        
    }
}   
    

void icdSocket::driveEnable() throw (ComponentErrors::SocketErrorExImpl) {

    AUTO_TRACE("icdSocket::driveEnable()");

    BYTE buff[] = { // 8, 4, 0, 1, 0, 0, 1, C, 0, 0, 0, 0, 0, 0, 0, 2, CR
        0x38, 0x34, 0x30, 0x31, 0x30, 0x30, 0x31, 0x43,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32,
        0x0D
    }; // This message is an ICD ``driveCtrl - driveEnable`` (Commands.driveCtrl, 28:1)

    // If responseCheck (called by make_talk) finds an error it set a bit on 
    // pattern property to communicate the problem to client. 
    // After it logs the error code trows an exception
    try {
        // Make a conversation to and from icd. The response is stored in buff
        make_talk(buff, true) ;
    }
    catch (...) {
    }
}   


void icdSocket::getDriveStatus() throw (ComponentErrors::SocketErrorExImpl) {

    BYTE buff[] = { 
        // 8, 0, 0, 2, 0, 0, 1, C, 0, 0, 0, 0, 0, 0, 0, 0, CR
        0x38, 0x30, 0x30, 0x32, 0x30, 0x30, 0x31, 0x43,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x0D
    }; // This message is a icd ``operating status request`` (Status.driveStat, 28:2) 

    AUTO_TRACE("icdSocket::getDriveStatus()");
    
    try {
        // make_talk doesn't call responseCheck due the second parameter is false
        // Make a conversation to and from icd. The response is stored in buff
        make_talk(buff, false) ;
    }
    catch (...) {
        return ;
    }

#ifdef DEBUG_TSS
    /************************************ TURN STATUS SIGNALS ON *************************************/
    //------------------ Use this code only to simulate a wrong ICD behavior ---------------------//
            
            // The integer CODE parameter is one of the follows:
            //   
            // IMS_FLTSIG      (4)    error detected by internal monitoring signals 
            // EMS_SIGN        (5)    error detected by external monitoring signals 
            // WARNING         (6)    warning signal detected
            // INIT_ERR        (7)    initialization error
            // QS_ACTIVE       (8)    quick stop active
              
            // Set the code you want to test
            int code = IMS_FLTSIG ;
            // int code = EMS_SIGN ;
            // int code = WARNING ;
            // int code = INIT_ERR ;
            // int code = QS_ACTIVE ;

            turnStatusSignalsOn(buff, code);

    /******************************************* END TEST *********************************************/
#endif

    // Check the status and set the status pattern
    fbstatuswordCheck(buff) ;

}


void icdSocket::getProcessingStatus() throw (ComponentErrors::SocketErrorExImpl) {

    BYTE buff[] = { 
        // 8, 0, 0, 3, 0, 0, 1, C, 0, 0, 0, 0, 0, 0, 0, 0, CR
        0x38, 0x30, 0x30, 0x33, 0x30, 0x30, 0x31, 0x43,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x0D
    }; // This message is a icd ``processing status request`` (Status.xMode_act, 28:3) 

    AUTO_TRACE("icdSocket::getProcessingStatus()");
    
    try {
        // make_talk doesn't call responseCheck due the second parameter is false
        // Make a conversation to and from icd. The response is stored in buff
        make_talk(buff, false) ;
    }
    catch (...) {
        return ;
    }

    // Check the status and set the status pattern
    fbstatuswordCheck(buff) ;

}


void icdSocket::getStatePTP() throw (ComponentErrors::SocketErrorExImpl) {

    BYTE buff[] = { 
        // 8, 0, 0, 2, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, CR
        0x38, 0x30, 0x30, 0x32, 0x30, 0x30, 0x32, 0x33,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x0D
    }; // This message is a icd ``PTP operation status request`` (PTP.StatePTP, 35:2) 

    AUTO_TRACE("icdSocket::getStatePTP()");
    
    try {
        // If responseCheck (called by make_talk) finds an error it set a bit on 
        // pattern property to communicate the problem to client. 
        // After it logs the error code trows an exception
        make_talk(buff, true) ;
    }
    catch (...) {
        return ;
    }

    // Check the status and set the status pattern
    fbstatuswordCheck(buff) ;

}


unsigned long icdSocket::getVerboseStatus(void) {

    // Upgrade the verbose status pattern
    getDriveStatus();

    return m_icd_verbose_status ;
}


unsigned long icdSocket::getSummaryStatus(void) {

    unsigned long vstatus = getVerboseStatus();

    // The W bit is set in setPosition and in the verbose status management
    // The S bit is set in the verbose status management

    // off: OFF
    bool off = ((vstatus >> NOT_OP_ENABLE) & 1) || ((vstatus >> INIT_ERR) & 1);
    // Failure: F
    bool failure = ((vstatus >> IMS_FLTSIG) & 1) || ((vstatus >> EMS_SIGN) & 1) ||
                   ((vstatus >> MOVING_ERR) & 1) || ((vstatus >> WRONG_POSITION)) ;
    // Communication Error: CE
    bool comm_error = ((vstatus >> POL_ERR) & 1) || ((vstatus >> WRONG_RFLAG) & 1) ||
                      ((vstatus >> CMDERR) & 1) || ((vstatus >> WRONG_RCODE) & 1) ;

    // Not Ready: NR
    bool not_ready = false;

    // Switch OFF pattern bit
    if(off)
        m_icd_summary_status |=  (1 << OFF);
    else
        m_icd_summary_status &= ~(1 << OFF) ;

    // Switch F pattern bit
    if(failure)
        m_icd_summary_status |=  (1 << F);
    else
        m_icd_summary_status &= ~(1 << F) ;

    // Switch CE pattern bit
    if(comm_error)
        m_icd_summary_status |=  (1 << CE);
    else
        m_icd_summary_status &= ~(1 << CE) ;

    // Switch NR pattern bit
    if(not_ready)
        m_icd_summary_status |=  (1 << NR);
    else
        m_icd_summary_status &= ~(1 << NR) ;

    return m_icd_summary_status ;
}


void icdSocket::make_talk(BYTE *buff, bool check=true) throw (ComponentErrors::SocketErrorExImpl) {

    // Set the sendflag (this is an acknowledge command)
    set_sendflag(buff) ;

    // Send a polling command to wake up the ICD
    polling();

    // Send a command to ICD
    sendBuffer(buff, ICD_BUFF_LEN) ; 

    // Receive the response in the same buffer used to make a request
    receiveBuffer(buff, ICD_BUFF_LEN);
    
    /*
    for(int i=0; i<ICD_BUFF_LEN-1; i++)
        ACS_SHORT_LOG((LM_INFO, "Buff[%d]: %X", i, buff[i])); 
    */

    // Verify the response correctness
    responseCheck(buff, check);

#ifdef DEBUG_WRT
    /************************************ WRONG RESPONSE TEST *************************************/
    //------------------ Use this code only to simulate a wrong ICD behavior ---------------------//
            
            BYTE wrong_buff[ICD_BUFF_LEN] ; 

            // The integer CODE parameter is one of the follows:
            //   
            // WRONG_RFLAG     (1)     wrong response flag (rf != sf)
            // CMDERR          (2)     cmderr 
            // WRONG_RCODE     (3)     wrong response code 
              
            // Set the code you want to test
            int code = WRONG_RFLAG ;
            // int code = CMDERR ;
            // int code = WRONG_RCODE ;

            wrongResponse(buff, wrong_buff, code);
            ACS_SHORT_LOG((LM_INFO, "#------------ TESTING STATUS CODE %d -----------#", code)); 
            responseCheck(wrong_buff);
            ACS_SHORT_LOG((LM_INFO, "# ICDStatus: %d", m_icd_verbose_status)); 
            ACS_SHORT_LOG((LM_INFO, "#------------------ END TESTING ----------------#")); 

    /********************************** END WRONG RESPONSE TEST ***********************************/
#endif
        
}


void icdSocket::set_sendflag(BYTE *buff) {

    if(sf) {
        buff[0] = 0x30;
        sf = 0;
    }   
    else {
        sf = 1;
        buff[0] = 0x38;
    }
}


void icdSocket::sendBuffer(BYTE *msg, WORD len) throw (ComponentErrors::SocketErrorExImpl) {

    int bytesNum = 0;  // Number of bytes sent for each call to Send
    int bytesSent = 0; // Total bytes sent

    while(bytesSent < len) {
        // Send returns the total number of bytes sent
        if ((bytesNum = Send(m_Error, (const void *)(msg + bytesSent), len - bytesSent)) < 0) 
            break ;
        else bytesSent += bytesNum;
    }

    if (bytesSent != len) {
        ACS_LOG(
                LM_FULL_INFO, 
                "icdSocket::getPosition::sendBuffer, Socket problem, not all bytes sent", 
                (LM_INFO,"getPosition::sendBuffer")
                );
        m_soStat = NOTRDY;

        if(reConnect(bytesSent)) { // try to reconnect
            ACS_LOG(
                    LM_FULL_INFO, 
                    "icdSocket::getPosition::sendBuffer, Error recreating the socket connection", 
                    (LM_INFO,"getPosition::sendBuffer")
                    );
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                    "icdSocket::getPosition::sendBuffer, I can't reconnect to the socket");
            throw exImpl;
        }
    }   
}
 

void icdSocket::receiveBuffer(BYTE *msg, WORD len) throw (ComponentErrors::SocketErrorExImpl) {

    int bytesNum = 0;

    // Receive the response form the icd one byte at once 
    for(int i=0; i<len; i++)
        bytesNum += Receive(m_Error, (void *)(&msg[i]), 1) ;
        
    // Throw a SocketErrorExImpl if it didn't receive "len" bytes
    if (bytesNum != len) { // partial timeout
        ACS_DEBUG("icdSocket::receiveBuffer", "socket problem, not all bytes received");
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "icdSocket::receiveBuffer(), Not all bytes received");
        throw exImpl;
    } 
}


//---  Protected Methods ---//

void icdSocket::polling(void) throw (ComponentErrors::SocketErrorExImpl) {

    BYTE polling[] = { STR_POL, ICD_ADD, END_CMD };
    
    try {
        sendBuffer(polling, POL_LEN);
    }
    catch (...){
        ACS_DEBUG("icdSocket::polling", "ICD polling problem");
        ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                "icdSocket::polling, ICD polling problem");
        throw exImpl;
    }
    BYTE polling_response[POL_LEN];
    receiveBuffer(polling_response, POL_LEN);

#ifdef DEBUG_SPE
    /********************* SWITCH POLLING ERROR ON ***********************/
        wrongPolling(polling);
    /***************************** END TEST ******************************/
#endif
    
    m_icd_verbose_status &= ~(1 << POL_ERR) ;
    for(int i=0; i<POL_LEN; i++)
        if(polling[i] != polling_response[i]) {
            m_icd_verbose_status |=  (1 << POL_ERR) ;
            ACS_SHORT_LOG((LM_ERROR, "# Polling problem!"));
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                    "icdSocket::polling, ICD polling problem");
            throw exImpl;
            break;
        }
}


void icdSocket::responseCheck(BYTE *buff, bool check=true) throw (ComponentErrors::SocketErrorExImpl) {
    /*******************************************\
     *    (sf == 0) => (requestdata == 0x30)
     *    (sf == 1) => (requestdata == 0x38)
    \*******************************************/
    BYTE responsedata = buff[0];

    if(check) {

        if(sf) {
            switch(responsedata) {
                case 0x38: // Fine! The responsedata is equal to senddata => (cmderr == 0)
                    break ;

                case 0x30: // rf wrong: (cmderr == 0) but (sf != rf) 
                    m_icd_verbose_status |=  (1 << WRONG_RFLAG) ;
                    m_icd_verbose_status &= ~(1 << CMDERR) ;
                    m_icd_verbose_status &= ~(1 << WRONG_RCODE) ;
                    ACS_SHORT_LOG((LM_ERROR, "# WRONG RESPONSE FLAG. First byte: %X", buff[0]));
                    break ;

                case 0x43: // Error: (sf == rf) but (cmderr == 1)
                    m_icd_verbose_status |=  (1 << CMDERR) ;
                    m_icd_verbose_status &= ~(1 << WRONG_RFLAG) ;
                    m_icd_verbose_status &= ~(1 << WRONG_RCODE) ;
                    ACS_SHORT_LOG((LM_ERROR, "# CMDERR - ERROR CODE: (%c%c%c%ch)", 
                                (const char*)buff[ICD_BUFF_LEN-5], 
                                (const char*)buff[ICD_BUFF_LEN-4],
                                (const char*)buff[ICD_BUFF_LEN-3], 
                                (const char*)buff[ICD_BUFF_LEN-2] 
                                ));
                    break ;

                case 0x34: // Error and wrong rf: (sf != rf) and (cmderr == 1)
                    m_icd_verbose_status |=  (1 << CMDERR) ;
                    m_icd_verbose_status |=  (1 << WRONG_RFLAG) ;
                    m_icd_verbose_status &= ~(1 << WRONG_RCODE) ;
                    ACS_SHORT_LOG((LM_ERROR, "# WRONG RESPONSE FLAG. First byte:  %X", buff[0]));
                    ACS_SHORT_LOG((LM_ERROR, "# CMDERR - ERROR CODE: (%c%c%c%ch)", 
                                (const char*)buff[ICD_BUFF_LEN-5], 
                                (const char*)buff[ICD_BUFF_LEN-4],
                                (const char*)buff[ICD_BUFF_LEN-3], 
                                (const char*)buff[ICD_BUFF_LEN-2] 
                                ));
                    break ;

                default:   // Wrong responsedata coding
                    m_icd_verbose_status |=  (1 << CMDERR) ;
                    m_icd_verbose_status |=  (1 << WRONG_RFLAG) ;
                    m_icd_verbose_status |=  (1 << WRONG_RCODE) ;
                    ACS_SHORT_LOG((LM_ERROR, "# WRONG RESPONSEDATA CODE. First Byte: %X", buff[0]));
            }
        }
        else {
            switch(responsedata) {
                case 0x38: // rf wrong: (cmderr == 0) but (sf != rf) 
                    m_icd_verbose_status &=  (1 << WRONG_RFLAG) ;
                    m_icd_verbose_status &= ~(1 << CMDERR) ;
                    m_icd_verbose_status &= ~(1 << WRONG_RCODE) ;
                    ACS_SHORT_LOG((LM_ERROR, "# WRONG RESPONSE FLAG. First byte: %X", buff[0]));
                    break ;

                case 0x30: // Fine! The responsedata is equal to senddata => (cmderr == 0)
                    m_icd_verbose_status = 0;
                    break ;

                case 0x43: // Error and wrong rf: (sf != rf) and (cmderr == 1)
                    m_icd_verbose_status |=  (1 << WRONG_RFLAG) ;
                    m_icd_verbose_status |=  (1 << CMDERR) ;
                    m_icd_verbose_status &= ~(1 << WRONG_RCODE) ;
                    ACS_SHORT_LOG((LM_ERROR, "# WRONG RESPONSE FLAG. First byte:  %X", buff[0]));
                    ACS_SHORT_LOG((LM_ERROR, "# CMDERR - ERROR CODE: (%c%c%c%ch)", 
                                (const char*)buff[ICD_BUFF_LEN-5], 
                                (const char*)buff[ICD_BUFF_LEN-4],
                                (const char*)buff[ICD_BUFF_LEN-3], 
                                (const char*)buff[ICD_BUFF_LEN-2] 
                                ));
                    break ;

                case 0x34: // Error: (sf == rf) but (cmderr == 1)
                    m_icd_verbose_status |=  (1 << CMDERR) ;
                    m_icd_verbose_status &= ~(1 << WRONG_RFLAG) ;
                    m_icd_verbose_status &= ~(1 << WRONG_RCODE) ;
                    ACS_SHORT_LOG((LM_ERROR, "# CMDERR - ERROR CODE: (%c%c%c%ch)", 
                                (const char*)buff[ICD_BUFF_LEN-5], 
                                (const char*)buff[ICD_BUFF_LEN-4],
                                (const char*)buff[ICD_BUFF_LEN-3], 
                                (const char*)buff[ICD_BUFF_LEN-2] 
                                ));
                    break ;

                default:   // Wrong responsedata coding
                    m_icd_verbose_status |=  (1 << CMDERR) ;
                    m_icd_verbose_status |=  (1 << WRONG_RFLAG) ;
                    m_icd_verbose_status |=  (1 << WRONG_RCODE) ;
                    ACS_SHORT_LOG((LM_ERROR, "# WRONG RESPONSEDATA CODE. First Byte: %X", buff[0]));
            }
        }
    }
}


void icdSocket::fbstatuswordCheck(BYTE * buff) {

    // The first parameter is the buffer containing the response.
    // The second parameter is the index of the first byte of sub-buffer in which we are interested
    // The third parameter is the number of byte of the sub-buffer
    unsigned long statusword_last = hex2dec(buff, 6, 2); 
    unsigned long statusword_first = hex2dec(buff, 4, 2); 
    unsigned long cos = (statusword_last & 15);
    int x_end = (statusword_first >> X_END_IDX) & 1;
    int x_err = (statusword_first >> X_ERR_IDX) & 1;
   
    // If the ICD is moving then x_err is set to 0 and the related bit is set to 1
    if(x_end) {
        m_icd_verbose_status &= ~(1 << MOVING) ;
        m_icd_summary_status &= ~(1 << S) ;
    }
    else {
        m_icd_verbose_status |=  (1 << MOVING);
        m_icd_summary_status |=  (1 << S);
    }

    // If an asynchronous error occurs during processing, x_err is set to 0 and
    // the related status bit is set to 1
    if(x_err) 
        m_icd_verbose_status |=  (1 << MOVING_ERR);
    else
        m_icd_verbose_status &= ~(1 << MOVING_ERR) ;

    // If an error is detected by internal monitoring signals
    if((statusword_last >> IMS_FLTSIG_IDX) & 1) {
        m_icd_verbose_status |=  (1 << IMS_FLTSIG);
        ACS_SHORT_LOG((LM_ERROR, "IMS Error: Cause via parameter Status.FltSign_SR (28:18)"));
    }
    else
        m_icd_verbose_status &= ~(1 << IMS_FLTSIG) ;

    // If an error is detected by external monitoring signals
    if((statusword_last >> EMS_SIGN_IDX) & 1) {
        m_icd_verbose_status |=  (1 << EMS_SIGN);
        ACS_SHORT_LOG((LM_ERROR, "EMS Error: Cause via parameter Status.Sign_SR (28:15)"));
    }
    else
        m_icd_verbose_status &= ~(1 << EMS_SIGN) ;

    // If there is a warning message
    if((statusword_last >> WARNING_IDX) & 1) {
        m_icd_verbose_status |=  (1 << WARNING);
        m_icd_summary_status |=  (1 << W); // That is set also in setPosition
        ACS_SHORT_LOG((LM_WARNING, "Warning: Cause via parameter Status.WarnSig (28:10)"));
    }
    else
        m_icd_verbose_status &= ~(1 << WARNING) ;

    // COS (the last four bits of fb-statusword)
    // If there is an initialization error (0011 or 0010)
    if((cos == 3) || (cos == 2)) {
        m_icd_verbose_status |=  (1 << INIT_ERR);
        ACS_SHORT_LOG((LM_ERROR, "An error has occurred during the initialization of the ICD"));
    }
    else
        m_icd_verbose_status &= ~(1 << INIT_ERR) ;

    // If the quick stop is active
    if(cos == 7)
        m_icd_verbose_status |=  (1 << QS_ACTIVE);
    else
        m_icd_verbose_status &= ~(1 << QS_ACTIVE) ;

    // If Unit is not working in set operating mode the related status bit is set to 1
    if(cos == 6)
        m_icd_verbose_status &= ~(1 << NOT_OP_ENABLE) ; 
    else
        m_icd_verbose_status |=  (1 << NOT_OP_ENABLE); // Not Operation Enable
}


int icdSocket::reConnect(int error)
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


void icdSocket::setTargetPosition(double position) { m_icd_target_position = position; }


bool icdSocket::isPositionUpToDate(void) {
    timeval now;
    gettimeofday(&now, NULL);
    double actual_time = now.tv_sec + now.tv_usec / 1000000.0;
    double diff = actual_time - m_timeLastPositionRequest;
    return diff < m_ICD_POSITION_EXPIRE_TIME;
}


int icdSocket::clearUSDBuffer()
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


