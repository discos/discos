#include "bulkDataZMQPubSub.h"
using namespace bulkdataZMQImpl;

BulkDataZMQPublisher::~BulkDataZMQPublisher() {}

bool BulkDataZMQPublisher::initialize(const bulkdataZMQImpl::TZMQConfig& conf) {
    m_configuration = conf;
    // Initialize the base ZmqSender with the endpoint from configuration
    if (!ZmqSender::initialize(std::string((const char*)m_configuration.endPoint))) {
        return false;
    }
    if (m_configuration.tcpNoDelay) {
        //Apparently this is not needded anymore as it is the default behviour of 0MQ
        /*int tcp_nodelay = 1;
        if (zmq_setsockopt(socket, ZMQ_TCP_NODELAY, &tcp_nodelay, sizeof(tcp_nodelay)) != 0) {
            captureError();
            close();
            return false;
        } */ 
    }
    if (m_configuration.cpuAffinity) {
        uint64_t affinity = 1; // Pin to CPU core 0
        if (zmq_setsockopt(socket, ZMQ_AFFINITY, &affinity, sizeof(affinity)) !=0) {
            captureError();
            close();
            return false;
        }   
    } 
    return true;
}

void BulkDataZMQPublisher::startSend(CORBA::ULong flownumber, const char *param, size_t len) {
    // Mark as startSend data with 'S' prefix
    std::vector<uint8_t> markedData;
    markedData.reserve(len + 1);
    markedData.push_back('S'); // Start marker
    markedData.insert(markedData.end(), reinterpret_cast<const uint8_t*>(param), reinterpret_cast<const uint8_t*>(param) + len);
    
    if (!send(markedData.data(), markedData.size())) {
        _EXCPT(ACSBulkDataError::AVSendFrameErrorExImpl,dummy,"BulkDataZMQPublisher::startSend");
        _ADD_EXTRA(dummy,"Reason",getLastError());
        throw dummy;
    }
}

void BulkDataZMQPublisher::sendData(CORBA::ULong flownumber, ACE_Message_Block *buffer) {
    if (buffer != nullptr) {
        // Mark as sendData with 'D' prefix
        std::vector<uint8_t> markedData;
        size_t len = buffer->length();
        markedData.reserve(len + 1);
        markedData.push_back('D'); // Data marker
        markedData.insert(markedData.end(), reinterpret_cast<const uint8_t*>(buffer->rd_ptr()), reinterpret_cast<const uint8_t*>(buffer->rd_ptr()) + len);
        
        if (!send(markedData.data(), markedData.size())) {
            _EXCPT(ACSBulkDataError::AVSendFrameErrorExImpl,dummy,"BulkDataZMQPublisher::sendData");
            _ADD_EXTRA(dummy,"Reason",getLastError());
            throw dummy;
        }
    }
}

void BulkDataZMQPublisher::sendData(CORBA::ULong flownumber, const char *buffer, size_t len) {
    // Mark as sendData with 'D' prefix
    std::vector<uint8_t> markedData;
    markedData.reserve(len + 1);
    markedData.push_back('D'); // Data marker
    markedData.insert(markedData.end(), reinterpret_cast<const uint8_t*>(buffer), reinterpret_cast<const uint8_t*>(buffer) + len);
    
    if (!send(markedData.data(), markedData.size())) {
        _EXCPT(ACSBulkDataError::AVSendFrameErrorExImpl,dummy,"BulkDataZMQPublisher::sendData");
        _ADD_EXTRA(dummy,"Reason",getLastError());
        throw dummy;
    }
}

void BulkDataZMQPublisher::stopSend(CORBA::ULong flownumber) {
    // Mark as stopSend with 'X' prefix
    const char marker = 'X';
    if (!send(reinterpret_cast<const uint8_t*>(&marker), 1)) {
        _EXCPT(ACSBulkDataError::AVStopSendErrorExImpl,dummy,"BulkDataZMQPublisher::stopSend");
        _ADD_EXTRA(dummy,"Reason",getLastError());
        throw dummy;
    }
}   

// ------------------- Subscriber Implementation -------------------

BulkDataZMQSubscriber::~BulkDataZMQSubscriber() {}

bool BulkDataZMQSubscriber::initialize(const bulkdataZMQImpl::TZMQConfig& conf, bulkdataZMQImpl::BulkDataZMQCallback* callback) {
    m_callback = callback;
    m_configuration = conf;
    if (!ZmqReceiver::initialize(std::string((const char*)m_configuration.endPoint), "")) {
        return false;
    }
    if (m_callback) {
        // Start async reception with a lambda that calls the member function
        if (!startAsync([this](const std::vector<uint8_t>& data) {
            this->onMessageReceived(data);
        })) {
            return false; // Failed to start async reception, error message already captured in startAsync
        }
    }
    return true;
}

bool BulkDataZMQSubscriber::receive(std::vector<uint8_t>& out_data, long timeout_ms) {
    std::vector<uint8_t> rawData;
    if (receiveSync(rawData, timeout_ms)) {
        if (!rawData.empty()) {
            // Strip the type marker
            out_data.assign(rawData.begin() + 1, rawData.end());
            return true;
        }
    }
    return false; // Error or timeout, error message already captured in receiveSync
}

void BulkDataZMQSubscriber::onMessageReceived(const std::vector<uint8_t>& data) {
    if (data.empty()) return;
    
    char type = data[0];
    
    if (m_callback) {
        // printBuffer("OnMessageReceived: ", data);
        size_t payload_size = data.size() - 1;
        // 1. Create the message block wrapping the data
        ACE_Message_Block frame(reinterpret_cast<const char*>(data.data() + 1), payload_size); 
        // 2. ADVANCE THE WRITE POINTER so length() reflects the data
        frame.wr_ptr(payload_size); 
        //cout << "Total Length: " << frame.total_length() << ", Current Length: " << frame.length() << std::endl;
        if (type == 'S') {
            m_callback->cbStart(&frame);
        } else if (type == 'D') {
            m_callback->cbReceive(&frame);
        } else if (type == 'X') {
            m_callback->cbStop();
        }
    }
}
