#ifndef _BULKDATAZMQ_RECEIVER_IMPL_H
#define _BULKDATAZMQ_RECEIVER_IMPL_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <Cplusplus11Helper.h>

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
C11_IGNORE_WARNING("-Wmisleading-indentation")
C11_IGNORE_WARNING("-Wcatch-value=")
#include <baciDB.h>
#include <baciCharacteristicComponentImpl.h>
#include <baci.h>
#include "bulkDataZMQReceiverS.h"
C11_IGNORE_WARNING_POP

//#include "ACSBulkDataStatus.h"


#include "bulkDataZMQCallback.h"
#include "bulkDataZMQConfiguration.h"
#include "bulkDataZMQPubSub.h"
#include <ACSBulkDataError.h>



//#include "bulkDataReceiver.h"


namespace bulkdataZMQImpl
{ 

/** 

 */


template<class TCallback>
class BulkDataZMQReceiverImpl :  public baci::CharacteristicComponentImpl, public virtual POA_bulkdataZMQ::BulkDataZMQReceiver
{
  public:
    
    /**
     * Constructor
     */
    BulkDataZMQReceiverImpl();
  
    /**
     * Destructor
     */
    virtual ~BulkDataZMQReceiverImpl();

    virtual void openReceiver(CORBA::Long flowNumber) ;
    
    virtual void closeReceiver();

    void resetReceiver();

    BulkDataZMQSubscriber* getReceiver();

  protected: 

  private:

    /**
    * Flag indicating whether the receiver is connected to a sender.
    */
    bool m_connected;

    /**
    * Subscriber object responsible for receiving bulk data via ZeroMQ.
    * Handles the actual data reception and socket management.
    */
    BulkDataZMQSubscriber m_subscriber;

    /**
    * Configuration object for ZeroMQ settings and container services.
    * Manages connection parameters and service references.
    */
    CZMQConfiguration<maci::ContainerServices> m_config;
    /**
     * Configuration struct for ZeroMQ settings, such as endpoint, flow number, etc.
     */
    bulkdataZMQImpl::TZMQConfig m_zmqconfig;

    TCallback m_callback; ///< Pointer to the callback interface for handling received data and events

};

#include "bulkDataZMQReceiverImpl.i"


};

#endif /*!_BULKDATAZMQ_RECEIVER_IMPL_H*/
