#ifndef _BULKDATAZMQ_SENDER_IMPL_H
#define _BULKDATAZMQ_SENDER_IMPL_H

#include <Cplusplus11Helper.h>

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
C11_IGNORE_WARNING("-Wmisleading-indentation")
C11_IGNORE_WARNING("-Wcatch-value=")
#include <baciDB.h>
C11_IGNORE_WARNING_POP

#include "bulkDataZMQSenderS.h"
#include "bulkDataZMQReceiverC.h"
#include "bulkDataZMQPubSub.h"
#include "bulkDataZMQConfiguration.h"
#include <ACSBulkDataError.h>

namespace bulkdataZMQImpl
{ 

class BulkDataZMQSenderImpl : public virtual POA_bulkdataZMQ::BulkDataZMQSender
{    
  public:
    /**
     * Constructor
     */
    BulkDataZMQSenderImpl();
  
    /**
     * Destructor
     */
    virtual ~BulkDataZMQSenderImpl();
  

    /**
     *  Negotiate and initialize connection with the Receiver object.
     *  @param receiver reference of the Receiver Component.
     *  @param services pointer to the Container Services for accessing necessary resources.
     *  @throw ACSBulkDataError::AVConnectErrorEx
     *  @return void
    */
    virtual void connect(bulkdataZMQ::BulkDataZMQReceiver_ptr receiver,const maci::ContainerServices* services);

    /**
     *  Initialize connection with the given ZeroMQ configuration.
     *  @param conf ZeroMQ configuration object.
     *  @throw ACSBulkDataError::AVConnectErrorEx
     *  @return void
     */
    virtual void connect(const bulkdataZMQImpl::TZMQConfig& conf);
    
    
    /**
     *  Closes the connection with the Receiver object and cleans up resources.
     *  @throw ACSBulkDataError::AVDisconnectErrorEx
     *  @return void
    */
    virtual void disconnect();

  

  protected:
    /**
     * @brief Provides access to the underlying BulkDataZMQPublisher instance for sending data.
     * @return A pointer to the BulkDataZMQPublisher used for data transmission.
     * @note This allows the implementation to utilize the publisher's send capabilities while maintaining encapsulation 
     */
    virtual const BulkDataZMQPublisher* getSender();

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

  
    //virtual void startSend(CORBA::ULong flownumber, const char *param, size_t len);

    //void sendData(CORBA::ULong flownumber, ACE_Message_Block *buffer);
    //void sendData(CORBA::ULong flownumber, const char *buffer, size_t len); 
    //virtual void paceData()  =0;


    //void stopSend(CORBA::ULong flownumber);
 
  private:

};

};

#endif /* _BULKDATAZMQ_SENDER_IMPL_H */
