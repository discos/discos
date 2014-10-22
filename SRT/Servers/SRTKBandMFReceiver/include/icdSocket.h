/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __ICDSOCKET_H__
#define __ICDSOCKET_H__

#include <IRA>
#include "acstime.h" 
#include "utils.h" 
#include <ComponentErrors.h>
#include <DerotatorErrors.h>
#include <sys/time.h>

#define ICD_BUFF_LEN 17    // Buffer length: request and answer have the same length
#define ICD_RDWORD_LEN 4   // Length of `readdata` word
#define ICD_FBRD_INDEX 12  // Index of the first byte containing the `readdata` word
#define ICD_SIGN_INDEX 8   // Index of the first byte containing the sign in a position response
#define ICD_CMDDATA_LEN 4  // Number of bytes of commanddata

#define END_CMD 0x0D       // Exadecimal coding of ASCII <CR> char

// The following coding is related to a polling message
#define POL_LEN 3          // Polling message length
#define STR_POL 0x23       // Exadecimal coding of ASCII <#> char. 
#define ICD_ADD 0x31       // Exadecimal coding of ASCII <1> char. 1 is the icd address, mapped to 4001

#define READ 0             // Used to identify a read command
#define WRITE 1            // Used to identify a write command

#define IMS_FLTSIG_IDX 5
#define EMS_SIGN_IDX 6
#define WARNING_IDX 7
#define X_END_IDX 6        // Index of x_end bit in the first byte of fbstatusword
#define X_ERR_IDX 7        // Index of x_err bit in the first byte of fbstatusword

#define POWERON_ACTION 0

/****************** DEBUG ********************/
// #define DEBUG_TSS
// #define DEBUG_SPE
/*************** END DEBUG *******************/

// Fields of the ICD verbose status 
enum VerboseStatusFields {

    POL_ERR = 0,        /*!< polling error: polling request is different to polling response */
    WRONG_RFLAG = 1,    /*!< wrong response flag: (rf != sf)  */
    CMDERR = 2,         /*!< cmderr */
    WRONG_RCODE = 3,    /*!< wrong response code */

    /**
     * IMS_FLTSIG (Internal monitoring signals, FltSig)
     * fb-statusword[5]:  00100000 -> 0x20 -> (2: 0x32, 0: 0x30)
     *     - 0: no error detected
     *     - 1: error detected
     * To know the error cause look at parameter Status.FltSign_SR (28:18)
     */
    IMS_FLTSIG = 4,

    /**
     * EMS_SIGN (External monitoring signals, Sign). 
     * fb-statusword[6]:  01000000 -> 0x40 -> (4: 0x34, 0: 0x30)
     *     - 0: no error detected
     *     - 1: error detected
     * To know the error cause look at parameter Status.Sign_SR (28:15)
     */
    EMS_SIGN = 5,

    /**
     * WARNING 
     * fb-statusword[7]:  10000000 -> 0x80 -> (8: 0x38, 0: 0x30)
     *     - 0: no warning message
     *     - 1: warning message
     * To know the warning cause look at parameter Status.WarnSig (28:10)
     */
    WARNING = 6,

    /**
     * INIT_ERR (Initialization Error)
     * - 0: no initialization error detected
     * - 1: an error has occurred during the initialization of the ICD. 
     *      I turn this bit on when the last bits of fb-statusword are:
     *      * 0010: not ready to switch on
     *      * 0011: switch on disabled
     */
    INIT_ERR = 7,

    /**
     * QS_ACTIVE (Quick Stop active)
     * - 0: quick stop inactive
     * - 1: quick stop active. The last bits of fb-statusword are:
     *      * 0111
     */
    QS_ACTIVE = 8,

    /**
     * NOT_OP_ENABLE (Not Operation Enable)
     * - 0: unit is working in set operating mode 
     * - 1: unit is not working in set operating mode
     * When the unit is set to Operation Enable, then we can set the position and drive the ICD
     */
    NOT_OP_ENABLE = 9,

    /**
     * MOVING (Status bit for monitoring the process status).
     * x_end is the second bit (bit 14) of fbstatusword. 
     * - 0: motor movement complete
     * - 1: motor movement active
     */
    MOVING = 10,

    /**
     * MOVING_ERR (Status bit related to x_err).
     * x_err is the first bit (bit 15) of fbstatusword. I think the manual documentation is wrong, so
     * the correct values of x_err are the follows:
     * - 0: An asynchronous error occurs during processing
     * - 1: No error occurs during processing
     */
    MOVING_ERR = 11,

    /**
     * WRONG_POSITION
     * - 0: No error occurs retrieving ICD position
     * - 1: An error occurs retrieving ICD position
     */
    WRONG_POSITION = 12,
};


// Fields of the ICD summary status 
enum SummaryStatusFields {
    OFF = 0,  /*!< power OFF */
    F = 1,    /*!< Failure  */
    CE = 2,   /*!< Communiction Error*/
    NR = 3,   /*!< Not Ready */
    S = 4,    /*!< Slewing  */
    W = 5     /*!< Warning */
};


using namespace IRA;

class icdSocket: public CSocket {

public:

    /**
     * Derotator socket constructor.
     * @arg IP IP address of icd
     * @arg port port number of icd
     * @arg timeout timeout
     * @arg degree/step conversion factor
     * @arg reference position (zero position) of ICD in degrees
     * @arg max absolute position value allowed
     * @arg min absolute position value allowed
     */
    icdSocket(CString IP, DWORD port, DWORD timeout, double ICD_CF, 
            double REFERENCE_POSITION, double MAX_VALUE, double MIN_VALUE,
            double TrackingDelta, double ICD_POSITION_EXPIRE_TIME);


    virtual ~icdSocket();

    
   /**
    * Create a socket and make a connection to it 
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void Init() throw (ComponentErrors::SocketErrorExImpl) ;


    /// Return the icd position as degree angle, in the user reference system
    /**
     * This member function returns the icd position as degree angle.
     * To get a icd position I send a "actual position" request (`p_act` command).
     * The response from the ICD contains the position in its four last bytes (the first byte
     * is identified as ICD_FBRD_INDEX parameter and the length of `readdata` word is identified 
     * as ICD_RDWORD_LEN parameter.
     * Each of these bytes contain an hexadecimal code of ASCII char. For istance,
     * if a byte value is 0x30 then the corresponding char is 0. To make a conversion
     * I first convert the hexadecimal 0x30 in decimal value 48 and after I subtract 48
     * to obtain the related digit (I subtract 55 instead of 48 if the code is related to a letter).
     * Finally I apply the hex2dec, a degree conversion and a shifting of the zero reference in order
     * to return the posizion in the user reference system
     * Default: 14.067166936 (2562 steps)
     *
     * @return sensor position as degree angle
     * @throw ComponentErrors::SocketErrorExImpl
     */
    double getActPosition() throw (
                DerotatorErrors::ValidationErrorExImpl, 
                DerotatorErrors::CommunicationErrorExImpl
    );

    double getCmdPosition() { return m_cmdPosition; }

    double getPositionDiff() throw (ComponentErrors::SocketErrorExImpl);

    bool isReady();
    bool isTracking();
    bool isSlewing();


    /**
     * Return the ICD target position (double) in the user's reference system
     *
     * @return ICD target position (double) in the user's reference system
     * @throw CORBA::SystemException
     */
	double getTargetPosition() ;

    
    /**
     * Return the ICD position (double) in the user's reference system
     *
     * @return ICD position (double) in the user's reference system
     * @throw CORBA::SystemException
     */
    double getURSPosition();


    /**
     * This member function sets the icd position. The user types the input value as a degree angle (in the URS)
     * ant this method convert it to the number of steps needed to address the ICD. The positioning error
     * is always less then 0.006 degrees.
     * To set a icd position I send a "target position" (`PTP.p_absPTP` command, 35:1).
     * The response from the ICD contains... 
     * @param position target position as a degree angle
     * @throw DerotatorErrors::PositioningErrorEx,
	 * @throw DerotatorErrors::CommunicationErrorEx
     */
    void setPosition(double position) throw (
         DerotatorErrors::PositioningErrorExImpl,
	     DerotatorErrors::CommunicationErrorExImpl
    );
            

    /**
     * This member funcion sets the status pattern signals related to
     * ``operating status request`` (Status.driveStat, 28:2)
     *
     * @throw ComponentErrors::SocketErrorExImpl
     */
    void updateDriveStatus() throw (ComponentErrors::SocketErrorExImpl);


    /**
     * This member funcion sets the status pattern signals related to
     * ``processing status request`` (Status.xMode_act, 28:3)
     *
     * @throw ComponentErrors::SocketErrorExImpl
     */
    void getProcessingStatus() throw (ComponentErrors::SocketErrorExImpl);

    /**
     * This member funcion make a PTP status request (PTP.StatePTP, 35:2)
     *
     * @throw ComponentErrors::SocketErrorExImpl
     */
    void getStatePTP() throw (ComponentErrors::SocketErrorExImpl);
    
    /**
     * This method switches on the power amplifier.
     * The compact drive must be ``ready to switch on`` mode. The power amplifier
     * is turn off when the connection to ICD is lost, so we have to turn the power
     * amplifier on at the initialize of derotator component, so a setPosition call
     * will find the ICD ready to move.
     *
     * @throw ComponentErrors::SocketErrorExImpl
     */
    void driveEnable() throw (ComponentErrors::SocketErrorExImpl);
 

    /**
     * This method switches the power amplifier off
     * @throw ComponentErrors::SocketErrorExImpl
     */
    void driveDisable() throw (ComponentErrors::SocketErrorExImpl);

 
    /** Fault reset
     * @throw ComponentErrors::SocketErrorExImpl
     */
    void reset() throw (ComponentErrors::SocketErrorExImpl);

    
    /**
     * This member funcion calls several status functions:
     * - updateDriveStatus()
     *
     * @return verbose status pattern as an unsigned long
     */
    unsigned long getVerboseStatus(void) ;
    
    /**
     * This member funcion summarizes the ICD status
     *
     * @return summary status pattern as an unsigned long
     */
    unsigned long getSummaryStatus(void) ;


protected:
    
    /** Make a conversation to and from icd 
     *
     * @param buff pointer to the byte buffer that will contain the message to send
     * @param check enable the responseCheck related to actual transmitted command. 
     * Default check value is true. The responseCheck is disable for driveStatus
     * command, otherwise the responseCheck of driveStatus command masks the status 
     * of the previous command.
     */
    void make_talk(BYTE *buff, bool check) throw (ComponentErrors::SocketErrorExImpl);


   /**
    * Switch the sendflag from 0x30 to 0x38 and viceversa. The requestdata for a
    * read command is always 0x80 or 0x00. These bytes will be coded in two bytes:
    * - 0x80 -> (0x38, 0x30) where 0x38 is the hexadecimal code of 8 char and 
    *   0x30 is the hexadecimal code of char 0.
    * - 0x00 -> (0x30, 0x30)
    * The second byte is always 0x30, so we have to set only the first byte.
    *
    * @param msg pointer to the byte buffer that will contain the message to send
    */
    void set_sendflag(BYTE *buff); 


    /**
    * Send a polling command to wake up the ICD
    *
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void polling(void) throw (ComponentErrors::SocketErrorExImpl);


    /**
    * Send a buffer to icd
    * @param msg pointer to the byte buffer that will contain the message to send
    * @param len number of bytes to be send
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void sendBuffer(BYTE *msg, WORD len) throw (ComponentErrors::SocketErrorExImpl) ;

    
    /**
    * Receive a buffer from icd
    * @param msg pointer to the byte buffer that will contain the message from the icd
    * @param len number of bytes to be read
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void receiveBuffer(BYTE *msg, WORD len) throw (ComponentErrors::SocketErrorExImpl) ;


    /**
    * Set a positive rotation way.
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void setPositiveDir(void) throw (ComponentErrors::SocketErrorExImpl) ;


    /**
    * Set a negative rotation way.
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void setNegativeDir(void) throw (ComponentErrors::SocketErrorExImpl) ;


    /**
    * Verify the response correctness. There are several case to evaluate:
    *   - responseflag error: when responseflag is not equal to sendflag
    *   - error: when an error occur (cmderr == 1)
    *   - wrong responsedata coding: when an unexpected coding occur
    *
    * @param buff pointer to the byte buffer that will contain the message to send
    * @param check enable the responseCheck related to actual transmitted command. 
    * Default check value is true. The responseCheck is disable for driveStatus
    * command, otherwise the responseCheck of driveStatus command masks the status 
    * of the previous command.
    */
    void responseCheck(BYTE *buff, bool check) throw (ComponentErrors::SocketErrorExImpl) ;


    /**
     * Check the operating status (fb-statusword) of ICD. Set the status pattern if:
     *   - an error is detected by internal monitoring signals
     *   - an error is detected by external monitoring signals
     *   - there is a warning message
     *   - there is an initialization error (0011 or 0010)
     *   - the quick stop is active
     *
     * @param buff pointer to the byte buffer that will contain the sent message
     */
    void fbstatuswordCheck(BYTE *buff);


    /** 
    * Recovers the socket functionality.
    * In case of broken socket(err=0), close and try to reopen it
    * on timeout  (err=-2) check the LAN/485 converter functionality sending a dummy buffer
    * on fail (err=-3),  close and try to reopen the socket
    * @return 0 on succesfull reconnection
    * -1 on USD tout, and USD's buffer clear
    * -2 socket tout 
    * -3 socket fail
    */
    int reConnect(int);
    

    /** @var connection status */
    enum soStat { READY, NOTRDY, BROKEN, TOUT, BUSY } m_soStat;   


    /**
    *   flush the input buffer of USD's sending 8 dummy bytes
    *   @returns:
    *   0 for socket success write 
    *   -x on socket error
    */
    int clearUSDBuffer();

private:

    /** set the target position */
    void setCmdPosition(double position); 

    /**
     * Evaluate if the position value is up to date.
     *
     * @returns: true if the ICD position is up to date, false otherwise
     */
    bool isPositionUpToDate(void); 

    /** @var read the actual motor position. Status.p_act (Actual Position; Index: 31, Subindex: 6)  */
    double m_actPosition;

    double m_cmdPosition;

    /** @var read the actual ICD status (ACS::ROpattern type)  */
    long m_icd_verbose_status;

    /** @var read the actual ICD summary status (ACS::ROpattern type)  */
    long m_icd_summary_status;
    
    /** @var contains error information */
    CError m_Error;
    
    /** @var the time of the last position request in `second.useconds`  */
    double m_timeLastPositionRequest;

    /** @var the time of the last status request in `second.useconds`  */
    double m_timeLastStatusRequest;
    
    /** @var icd ip address */    
    const CString m_ICD_IP;
    
    /** @var icd listening port */
    const DWORD m_ICD_PORT;
   
    /** @var timeout */
    const DWORD m_ICD_TIMEO;

    /** @var icd conversion factor from step position to angle position */
    const double m_ICD_CF;

    /** @var icd zero position */
    const double m_ICD_REFERENCE;

    /** @var icd position max value*/
    const double m_ICD_MAX_VALUE;

    /** @var icd position min value*/
    const double m_ICD_MIN_VALUE;

    const double m_TRACKING_DELTA;

    /** @var icd position expire time */
    const double m_ICD_POSITION_EXPIRE_TIME;

};

#endif
