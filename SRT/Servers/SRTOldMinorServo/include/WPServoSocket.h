/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __MINORSERVOSOCKET_H__
#define __MINORSERVOSOCKET_H__

#include <sstream>
#include <IRA>
#include "acstime.h" 
#include "utils.h" 
#include <ComponentErrors.h>
#include <sys/time.h>
#include <baciCharacteristicComponentImpl.h>
#include <map>
#include <MSParameters.h>
#include <MinorServoErrors.h>

using namespace IRA;


/** This class can be instantiate only once (singleton design pattern) */
class WPServoSocket: public CSocket {

friend class WPServoTalker;
friend class RequestDispatcher;
friend class SocketListener;
    
public:

    /**
    * This method returns a reference to WPServoSocket instance and it is the only
    * way to get an instance of this class (singleton design pattern)
    *
    * @param cdb_ptr pointer to struct of CDB parameters
    * @param expire_ptr pointer to the ExpireTime structure
    * @return reference of WPServoSocket instance
    */
    static WPServoSocket* getSingletonInstance(const CDBParameters *const cdb_ptr, ExpireTime *const expire_ptr);

    virtual ~WPServoSocket();

    
   /**
    * Create a socket and make a connection to it 
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void Init() throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::SocketErrorExImpl) ;


protected:
    
    /**
    * Send a buffer to minorServo
    * @param msg_str string that will contain the message to send
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void sendBuffer(string msg_str) throw (ComponentErrors::SocketErrorExImpl) ;

    /**
    * Receive a buffer from minorServo
    * @param answer a reference to the string containing the message from the minorServo
    * @param closer the closer string of messages
    * @param header the header string of messages
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void receiveBuffer(string &answer, string closer, string header) throw (ComponentErrors::SocketErrorExImpl);

    /** @var connection status */
    enum soStat { READY, NOTRDY, BROKEN, TOUT, BUSY } m_soStat;   


private:

    /** @var pointer to minor servo socket */
    static WPServoSocket *m_instance_ptr_;

    /** @var m_raw_answer a string containing the partial message received */
    string m_raw_answer;

    /**
    * Private constructor of WPServoSocket. The constructor is private because this class
    * is a singleton, so there is only one way to get access to it, by getSingletonInstance
    * method.
    *
    * @param cdb_ptr pointer to struct of CDB parameters
    * @param expire_ptr pointer to the ExpireTime structure
    */
    WPServoSocket(const CDBParameters *const cdb_ptr, ExpireTime *const expire_ptr);
    
    /** @var pointer to struct of CDB parameters. */
    const CDBParameters *const m_cdb_ptr;

    /** @var contains error information */
    CError m_Error;
    
};


#endif
