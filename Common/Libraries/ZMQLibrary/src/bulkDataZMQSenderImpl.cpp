#include "bulkDataZMQSenderImpl.h"

using namespace bulkdataZMQImpl;

BulkDataZMQSenderImpl::BulkDataZMQSenderImpl(const ACE_CString& name,maci::ContainerServices* containerServices) : 
  baci::CharacteristicComponentImpl(name,containerServices)
{
    ACS_TRACE("BulkDataZMQSenderImpl::BulkDataZMQSenderImpl()");
    m_connected=false;
    m_receiverReference=0;
}


BulkDataZMQSenderImpl::~BulkDataZMQSenderImpl(){
    if (m_connected) {
        m_publisher.close();
    }   
}

void BulkDataZMQSenderImpl::cleanUp() {
    ACS_TRACE("BulkDataZMQSenderImpl::cleanUp()");
    if (!CORBA::is_nil(m_receiverReference)) {
        CORBA::release(m_receiverReference);
        m_receiverReference = 0;
    }
}

void BulkDataZMQSenderImpl::connect(bulkdataZMQ::BulkDataZMQReceiver_ptr receiver) {
    ACS_TRACE("BulkDataSenderImpl::connect");
    try {
        m_config.init(const_cast<maci::ContainerServices*>(getContainerServices()));
    }
    catch (ACSErr::ACSbaseExImpl& E) {
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
        _ADD_EXTRA(dummy,"Reason","Failed to initialize configuration");
		throw dummy.getAVConnectErrorEx();
    }    
    try {
        m_zmqconfig = m_config.getConfiguration(getContainerServices()->getName());
    }
    catch (ACSErr::ACSbaseExImpl& E) {
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
        _ADD_EXTRA(dummy,"Reason","Failed to retrieve configuration");
        throw dummy.getAVConnectErrorEx();  
    }    
    if (!m_publisher.initialize(m_zmqconfig)) {
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
        _ADD_EXTRA(dummy,"Reason",m_publisher.getLastError());
        throw dummy.getAVConnectErrorEx();
    }
    if (!m_publisher.isInitialized()) {
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
        _ADD_EXTRA(dummy,"Reason","Publisher failed to initialize");
        throw dummy.getAVConnectErrorEx();
    }
    if (CORBA::is_nil(receiver)){
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
        _ADD_EXTRA(dummy,"Reason","Received nil reference for Receiver component");
        throw dummy.getAVConnectErrorEx();
    }
    m_receiverReference = bulkdataZMQ::BulkDataZMQReceiver::_duplicate(receiver);
    try {
        m_receiverReference->openReceiver(m_zmqconfig.flowNumber);
    }
    catch (const ACSBulkDataError::AVOpenReceiverErrorEx& e) {
        _ADD_BACKTRACE(ACSBulkDataError::AVConnectErrorExImpl,dummy,e,"BulkDataZMQSenderImpl::connect");
        throw dummy.getAVConnectErrorEx();  
    } 
    catch (...) {
        _EXCPT(ACSBulkDataError::AVConnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::connect");
        _ADD_EXTRA(dummy,"Reason","Unknown error occurred while opening receiver.");
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
    if (CORBA::is_nil(m_receiverReference)){
        _EXCPT(ACSBulkDataError::AVDisconnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::disconnect");
        _ADD_EXTRA(dummy,"Reason","Received nil reference for Receiver component");
        throw dummy.getAVDisconnectErrorEx();
    }
    try {
        m_receiverReference->closeReceiver();
    }
    catch (const ACSBulkDataError::AVCloseReceiverErrorEx& e) {
        _ADD_BACKTRACE(ACSBulkDataError::AVDisconnectErrorExImpl,dummy,e,"BulkDataZMQSenderImpl::disconnect");
        throw dummy.getAVDisconnectErrorEx();  
    } 
    catch (...) {
        _EXCPT(ACSBulkDataError::AVDisconnectErrorExImpl,dummy,"BulkDataZMQSenderImpl::disconnect");
        _ADD_EXTRA(dummy,"Reason","Unknown error occurred while opening receiver.");
        throw dummy.getAVDisconnectErrorEx();
    } 
}

BulkDataZMQPublisher* BulkDataZMQSenderImpl::getSender() {
    return &m_publisher;
}