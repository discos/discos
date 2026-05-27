#ifndef _BULKDATAZMQPUBSUB_H
#define _BULKDATAZMQPUBSUB_H

#include "bulkDataZMQConfiguration.h"
#include "bulkDataZMQ.h"
#include "bulkDataZMQCallback.h"
#include <ACSBulkDataError.h>


namespace bulkdataZMQImpl {

/**
 * @class BulkDataZMQPublisher
 * @brief Specialized implementation of ZmqSender for BulkData transfer.
 * 
 * This class extends the base ZmqSender to provide specific functionality 
 * required by the BulkData system.
 */
class BulkDataZMQPublisher : public ZmqSender {
public:
    /**
     * @brief Default constructor for BulkDataZMQPublisher.
     */
    BulkDataZMQPublisher() : ZmqSender() {}

    /**
     * @brief Destructor for BulkDataZMQPublisher.
     */
    virtual ~BulkDataZMQPublisher();


    virtual bool initialize(const bulkdataZMQImpl::TZMQConfig& conf);

    /**
     * @brief Starts sending data asynchronously.
     * @param flownumber The flow number for the data stream.
     * @param param The data to send.
     * @param len The length of the data.
     * @throws AVStartSendErrorExImpl exception if sending fails to start.
     */
    virtual void startSend(CORBA::ULong flownumber, const char *param, size_t len);
    /**
     * @brief Sends data from an ACE_Message_Block.
     * @param flownumber The flow number for the data stream.
     * @param buffer The ACE_Message_Block containing the data to send.
     * @throws AVSendFrameErrorExImpl exception if sending fails.
     */
    virtual void sendData(CORBA::ULong flownumber, ACE_Message_Block *buffer);
    /**
     * @brief Sends raw data from a buffer.
     * @param flownumber The flow number for the data stream.
     * @param buffer Pointer to the data buffer.
     * @param len The length of the data in bytes.
     * @throws AVSendFrameErrorExImpl exception if sending fails.
     */
    virtual void sendData(CORBA::ULong flownumber, const char *buffer, size_t len); 
    /**
     * @brief Stops sending data for a given flow number.
     * @param flownumber The flow number to stop sending.
     * @throws AVStopSendErrorExImpl exception if stopping fails.
     */
    virtual void stopSend(CORBA::ULong flownumber);

protected:
    bulkdataZMQImpl::TZMQConfig m_configuration;

};

/**
 * @class BulkDataZMQSubscriber
 * @brief Specialized implementation of ZmqReceiver for BulkData transfer.
 * 
 * This class extends the base ZmqReceiver to handle incoming data streams
 * in the BulkData framework, supporting both synchronous and asynchronous 
 * message processing via ZeroMQ SUB sockets.
 */
class BulkDataZMQSubscriber : public ZmqReceiver {
public:
    /**
     * @brief Default constructor for BulkDataZMQSubscriber.
     */
    BulkDataZMQSubscriber() : ZmqReceiver() {}

    /**
     * @brief Destructor for BulkDataZMQSubscriber.
     */
    virtual ~BulkDataZMQSubscriber();

    virtual bool initialize(const bulkdataZMQImpl::TZMQConfig& conf, bulkdataZMQImpl::BulkDataZMQCallback* callback);
    
    /**
     * @brief Receives data synchronously, stripping the type marker.
     * @param out_data Output vector to store the received data without the marker.
     * @param timeout_ms Timeout in milliseconds.
     * @return true if data was received successfully, false on error or timeout.
     */
    bool receive(std::vector<uint8_t>& out_data, long timeout_ms);
    
    /**
     * @brief Callback method called when a message is received.
     * @param data The received data.
     */
    void onMessageReceived(const std::vector<uint8_t>& data);

protected:
    bulkdataZMQImpl::TZMQConfig m_configuration;
    bulkdataZMQImpl::BulkDataZMQCallback* m_callback; // User-provided callback for processing received messages
};

} // namespace bulkdataZMQImpl

#endif /*!_BULKDATAZMQPUBSUB_H*/