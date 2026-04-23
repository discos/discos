#include "bulkDataZMQSenderImpl.h"

using namespace bulkdataZMQImpl;

BulkDataZMQSenderImpl::BulkDataZMQSenderImpl() {
    m_connected=false;
}


BulkDataZMQSenderImpl::~BulkDataZMQSenderImpl(){
    if (m_connected) {
        m_publisher.close();
    }   
}

void BulkDataZMQSenderImpl::connect(bulkdataZMQ::BulkDataZMQReceiver_ptr receiver,const maci::ContainerServices* services) {
    try {
        m_config.init(services);
    }
    catch (ACSErr::ACSbaseExImpl& E) {
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
		throw dummy.getAVConnectErrorEx();
    }    
    
    //m_zmqconfig=m_config.getConfiguration("");
    if (!m_publisher.initialize(m_zmqconfig)) {
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
        _ADD_EXTRA(dummy,"Reason",m_publisher.getLastError());
        throw dummy.getAVConnectErrorEx();
    }
    m_connected=true;
}

void BulkDataZMQSenderImpl::connect(const bulkdataZMQImpl::TZMQConfig& conf) {
    if (!m_publisher.initialize(conf)) {
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
        _ADD_EXTRA(dummy,"Reason",m_publisher.getLastError());
        throw dummy;
    }
    m_connected=true;
}

void BulkDataZMQSenderImpl::disconnect() {
}

const BulkDataZMQPublisher* BulkDataZMQSenderImpl::getSender() {
    return &m_publisher;
}
