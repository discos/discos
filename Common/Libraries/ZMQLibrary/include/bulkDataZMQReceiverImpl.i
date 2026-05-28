// bulkDataZMQReceiverImpl.i


template<class TCallback>
BulkDataZMQReceiverImpl<TCallback>::BulkDataZMQReceiverImpl() {
    // Constructor implementation (if needed)
}
  
template <class TCallback>
BulkDataZMQReceiverImpl<TCallback>::~BulkDataZMQReceiverImpl() {
    // Ensure the receiver is closed and resources are cleaned up
    closeReceiver();
}
 
template <class TCallback>  
void BulkDataZMQReceiverImpl<TCallback>::openReceiver(CORBA::Long flowNumber) {
    ACS_TRACE("BulkDataZMQReceiverImpl::openReceiver");
    try {
        m_config.init(const_cast<maci::ContainerServices*>(getContainerServices()));
    }
    catch (ACSErr::ACSbaseExImpl& E) {
        _EXCPT(ACSBulkDataError::AVOpenReceiverErrorExImpl,dummy,"BulkDataZMQReceiverImpl::openReceiver");
        _ADD_EXTRA(dummy,"Reason","Failed to initialize configuration");
		throw dummy.getAVOpenReceiverErrorEx();
    }    
    try {
        m_zmqconfig = m_config.getConfiguration(flowNumber);
    }
    catch (ACSErr::ACSbaseExImpl& E) {
        _EXCPT(ACSBulkDataError::AVOpenReceiverErrorExImpl,dummy,"BulkDataZMQReceiverImpl::openReceiver");
        _ADD_EXTRA(dummy,"Reason","Failed to retrieve configuration");
        throw dummy.getAVOpenReceiverErrorEx();  
    }    
    if (!m_subscriber.initialize(m_zmqconfig,&m_callback)) {
        _EXCPT(ACSBulkDataError::AVOpenReceiverErrorExImpl,dummy,"BulkDataZMQReceiverImpl::openReceiver");
        _ADD_EXTRA(dummy,"Reason",m_subscriber.getLastError());
        throw dummy.getAVOpenReceiverErrorEx();
    }
    if (m_subscriber.isInitialized()) {
        m_connected = true;
    } else {
        _EXCPT(ACSBulkDataError::AVOpenReceiverErrorExImpl,dummy,"BulkDataZMQReceiverImpl::openReceiver");
        _ADD_EXTRA(dummy,"Reason","Subscriber failed to initialize");
        throw dummy.getAVOpenReceiverErrorEx();
    }
}   
    
template <class TCallback>
void BulkDataZMQReceiverImpl<TCallback>::closeReceiver() {
    if (m_connected) {
        m_subscriber.close();
        m_connected = false;
    }
}

template <class TCallback>
BulkDataZMQSubscriber* BulkDataZMQReceiverImpl<TCallback>::getReceiver() {
    return &m_subscriber;
}