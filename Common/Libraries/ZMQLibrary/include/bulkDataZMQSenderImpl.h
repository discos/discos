#ifndef _BULKDATAZMQ_SENDER_IMPL_H
#define _BULKDATAZMQ_SENDER_IMPL_H

#include <Cplusplus11Helper.h>

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
C11_IGNORE_WARNING("-Wmisleading-indentation")
C11_IGNORE_WARNING("-Wcatch-value=")
#include <baciDB.h>
#include <baciCharacteristicComponentImpl.h>
#include <baci.h>
C11_IGNORE_WARNING_POP

#include "bulkDataZMQSenderS.h"
#include "bulkDataZMQReceiverC.h"
#include "bulkDataZMQPubSub.h"
#include "bulkDataZMQConfiguration.h"
#include <ACSBulkDataError.h>


namespace bulkdataZMQImpl
{ 

class BulkDataZMQSenderImpl : public baci::CharacteristicComponentImpl, public virtual POA_bulkdataZMQ::BulkDataZMQSender
{    
  public:
  
    /**
     * Constructor
     * @param containerServices pointer to the container services for resource management and service access
     * @param name component name
     */
    BulkDataZMQSenderImpl(const ACE_CString& name,maci::ContainerServices* containerServices);


    /**
     * Destructor
     */
    virtual ~BulkDataZMQSenderImpl();
  
    void cleanUp();

    /**
     *  Negotiate and initialize connection with the Receiver object.
     *  @param receiver reference of the Receiver Component.
     *  @throw ACSBulkDataError::AVConnectErrorEx
     *  @return void
    */
    virtual void connect(bulkdataZMQ::BulkDataZMQReceiver_ptr receiver);

    /**
     *  Initialize connection with the given ZeroMQ configuration.
     *  @param conf ZeroMQ configuration object.
     *  @throw ACSBulkDataError::AVConnectErrorEx
     *  @return void
     */
    virtual void connect(const bulkdataZMQImpl::TZMQConfig& conf);
    
   
    /** 
     *  This is here in order to be compliant with CORCA AVSystem based implementations.
     *  @throw ACSBulkDataError::AVStartSendErrorEx
     *  @return void
     *  @htmlonly
     <br><hr>
     @endhtmlonly
    */
    //virtual void startSend() =0;

    /**
     *  This is here in order to be compliant with CORCA AVSystem based implementations.
     *  @throw ACSBulkDataError::AVPaceDataErrorEx
     *  @return void
     *  @htmlonly
     <br><hr>
     @endhtmlonly
    */
    //virtual void paceData()  =0;

    /** 
     * This is here in order to be compliant with CORCA AVSystem based implementations.
     *  @throw ACSBulkDataError::AVStopSendErrorEx
     *  @return void
     *  @htmlonly
     <br><hr>
     @endhtmlonly
    */
    //virtual void stopSend() =0; 
    
    /**
     *  Closes the connection with the Receiver object and cleans up resources.
     *  @throw ACSBulkDataError::AVDisconnectErrorEx
     *  @return void
    */
    virtual void disconnect();

    /**
     * @brief Provides access to the underlying BulkDataZMQPublisher instance for sending data.
     * @return A pointer to the BulkDataZMQPublisher used for data transmission.
     * @note This allows the implementation to utilize the publisher's send capabilities while maintaining encapsulation 
     */
    virtual BulkDataZMQPublisher* getSender();
  protected:
   
  private:

    bulkdataZMQ::BulkDataZMQReceiver_var m_receiverReference; ///< CORBA reference to the connected Receiver component
   
    /**
    * Flag indicating whether the sender is connected to a receiver.
    * @see connect()
    * @see disconnect()
    */
    bool m_connected;

    /**
    * Publisher object responsible for sending bulk data via ZeroMQ.
    * Handles the actual data transmission and socket management.
    */
    BulkDataZMQPublisher m_publisher;

    /**
    * Configuration object for ZeroMQ settings and container services.
    * Manages connection parameters and service references.
    */
    CZMQConfiguration<maci::ContainerServices> m_config;

    bulkdataZMQImpl::TZMQConfig m_zmqconfig;

};

};

#endif /* _BULKDATAZMQ_SENDER_IMPL_H */
