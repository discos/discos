/********************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\********************************************/
#ifndef __RECEIVERSOCKET_H__
#define __RECEIVERSOCKET_H__

#include <sstream>
#include <IRA>
#include "acstime.h" 
#include <ComponentErrors.h>
#include <sys/time.h>
#include <baciCharacteristicComponentImpl.h>
#include <SRTKBandMFParameters.h>
#include <vector>
#include "Talk.h"

#define BUFF_LEN 60


using namespace IRA;


class ReceiverSocket: public CSocket {
    
public:

    /** Socket constructor
    *
    * @param cdb_ptr pointer to struct of CDB parameters
    */
    ReceiverSocket(const RCDBParameters *cdb_ptr);
    

    virtual ~ReceiverSocket();

    
    /** Create a socket and make a connection to it 
    * 
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void Init() throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::SocketErrorExImpl) ;


    void cal_on() throw (ComponentErrors::SocketErrorExImpl);


    void cal_off() throw (ComponentErrors::SocketErrorExImpl);

    void set_mode(const char *mode) throw (ComponentErrors::SocketErrorExImpl);

    double vacuum(void) throw (ComponentErrors::SocketErrorExImpl);


protected:
        
    vbyte make_talk(Command cmd, Mode mode=NONE, const size_t AL=FIXED_AL, bool is_with_params=false);

    double get_value(const vbyte answer, const vbyte::size_type param_idx);
    
    /** Send a buffer
    * 
    * @param request vector containing the message bytes to send
    * @throw ComponentErrors::SocketErrorExImpl
    */
    void sendBuffer(const vbyte request) throw (ComponentErrors::SocketErrorExImpl) ;

    /** Receive a buffer
    * 
    * @param msg_size the number of bytes of the answer
    * @return a vector of BYTEs containing the answer
    * @throw ComponentErrors::SocketErrorExImpl
    */
    const vbyte receiveBuffer(const size_t msg_size, bool is_with_params=false) throw (ComponentErrors::SocketErrorExImpl);

    /** @var connection status */
    enum soStat { READY, NOTRDY, BROKEN, TOUT, BUSY } m_soStat;   


private:
    
    /** Structure containing the CDB parameters */
    const RCDBParameters *m_cdb_ptr;
    /** Request identification number */
    unsigned int m_request_id;

    /** @var contains error information */
    CError m_Error;
    
};


#endif
