/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __WPSERVOTALK_H__
#define __WPSERVOTALK_H__

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
#include <WPUtils.h>

using namespace IRA;

class WPServoSocket;
class RequestDispatcher;
class SocketListener;


/** This class can be instantiate only once (singleton design pattern) */
class WPServoTalker {
    
friend class WPServoSocket;
friend class RequestDispatcher;
friend class SocketListener;
friend class WPStatusUpdater;

public:

    /**
    * Private constructor of WPServoTalker. The constructor is private because this class
    * is a singleton, so there is only one way to get access to it, by getSingletonInstance
    * method.
    *
    * @param cdb_ptr pointer to struct of CDB parameters
    * @param expire_ptr pointer to the ExpireTime structure
    * @param cmd_list pointer to a list of vectors of commanded positions.
    * @param offsets pointer to a struct of vectors whose items will add to the commanded position.
    */
    WPServoTalker(
            const CDBParameters *const cdb_ptr, 
            ExpireTime *const expire_ptr, 
            CSecureArea< map<int, vector<PositionItem> > > *cmd_list,
            const Offsets *const offsets
    ) throw (ComponentErrors::MemoryAllocationExImpl);


    virtual ~WPServoTalker();


    /**
     * This member function sets the minorServo actual position
     * @param positions reference to doubleSeq positions
     * @param timestamp reference to positions timestamp
     *
     * @throw ComponentErrors::SocketErrorExImpl, MinorServoErrors::CommunicationErrorEx, 
     */
    void getActPos(
            ACS::doubleSeq &positions, 
            ACS::Time &timestamp
    ) throw (
        ComponentErrors::SocketErrorExImpl, 
        MinorServoErrors::CommunicationErrorEx
        );


    /**
     * This member function get the general status parameters
     * @param status_par reference to StatusParameters
     * @param timestamp reference to positions timestamp
     *
     * @throw ComponentErrors::SocketErrorExImpl, MinorServoErrors::CommunicationErrorEx
     */
    void getStatus(
        StatusParameters &status_par, 
        ACS::Time &timestamp
    ) throw (
        ComponentErrors::SocketErrorExImpl, 
        MinorServoErrors::CommunicationErrorEx
        );


    /**
     * This member function sets the minorServo commanded position
     * @param positions reference to doubleSeq positions
     * @param timestamp reference to positions timestamp
     * @param exe_time execution time
     * @param is_dummy true if want to insert the position in the
     * commanded position list, but without command it to the MSCU
     *
     * @throw ComponentErrors::SocketErrorExImpl, MinorServoErrors::PositioningErrorEx,
     * MinorServoErrors::CommunicationErrorEx
     */
    void setCmdPos(
            const ACS::doubleSeq &positions, 
            ACS::Time &timestamp,
            const ACS::Time exe_time=0,
            const bool is_dummy=false
    ) throw (
        ComponentErrors::SocketErrorExImpl, 
        MinorServoErrors::PositioningErrorEx, 
        MinorServoErrors::CommunicationErrorEx
        );

    
    void getAppStatus(
        unsigned long &status, 
        ACS::Time &timestamp
    ) throw (
        ComponentErrors::SocketErrorExImpl, 
        MinorServoErrors::CommunicationErrorEx
        );


    /**
     * This member function sets a generic action request
     * @param code the code corresponding with a specific request
     * @param exe_time execution time
     *
     * @throw ComponentErrors::SocketErrorExImpl, MinorServoErrors::CommunicationErrorEx, 
     */
    void action(
            const unsigned int code, 
            const ACS::Time exe_time=0
    ) throw (
        ComponentErrors::SocketErrorExImpl, 
        MinorServoErrors::CommunicationErrorEx
        );


    /**
     * This member function get the minorServo slaves parameters
     * @param parameters reference
     * @param timestamp reference
     * @param index index of parameter request
     * @param sub_index sub-index of parameter request
     * @param from_plc true if the slave is the PLC
     *
     * @throw ComponentErrors::SocketErrorExImpl, MinorServoErrors::CommunicationErrorEx
     */
    void getParameter(
            ACS::doubleSeq &parameters, 
            ACS::Time &timestamp,
            unsigned int index,
            unsigned int sub_index,
            bool from_plc = false
    ) throw (
        ComponentErrors::SocketErrorExImpl, 
        MinorServoErrors::CommunicationErrorEx
        );


private:

    /**
     * This member function look for a response with the same key of the command.
     * @param request_id the request identification number
     * @param starting_time the time in which the method starts its execution
     * @param cmd_idx an iden
     * @param process_enable a boolean used for enabling the answer processing
     * @param *parameters pointer to input parameters
     * @param from_plc true if the slave is the PLC
     * @param slave slave address
     *
     * @throw MinorServoErrors::CommunicationErrorEx
     */
    ACS::Time look_for_a_response(
            const unsigned long request_id,
            const double starting_time, 
            const unsigned int cmd_idx, 
            bool process_enabled=false, 
            ACS::doubleSeq *parameters=NULL,
            unsigned long *udata=NULL,
            const int slave=-1,
            StatusParameters *status_par=NULL
    ) throw (MinorServoErrors::CommunicationErrorEx); 

    
    /** @var pointer to struct of CDB parameters. */
    const CDBParameters *const m_cdb_ptr;

    /** @var pointer to struct of offsets to add to the commanded position */
    const Offsets *const m_offsets;

    CSecureArea< map<int, vector<PositionItem> > > *m_cmdPos_list;

    /**
    * This map has the command number as a key and the response as a value.
    * @var the map of outstanding responses 
    */
    CSecureArea<map<int, string> > *m_responses;


    /** @var requests to send */
    CSecureArea<vector<string> > *m_requests;


    /** @var counter that defines an id for messages sent */
    CSecureArea<unsigned int> *m_cmd_number;
    
    
    /** @var contains error information */
    CError m_Error;
    
};


#endif
