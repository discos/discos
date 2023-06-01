// $Id: CommandLine.cpp,v 1.3 2011-05-12 14:14:31 c.migoni Exp $

#include <LogFilter.h>
#include "CommandLine.h"
/****************************************************************************************************************************/
#include <CustomLoggerUtils.h>
/****************************************************************************************************************************/

#define _CHECK_ERRORS(ROUTINE) \
	else if (res==FAIL) { \
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error); \
		dummy.setCode(m_Error.getErrorCode()); \
		dummy.setDescription((const char*)m_Error.getDescription()); \
		m_Error.Reset(); \
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,ROUTINE); \
	} \
	else if (res==WOULDBLOCK) { \
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,ROUTINE); \
	} \
	else if (res==DECODE_ERROR) { \
		_THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,ROUTINE); \
	} \
	else { \
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,ROUTINE); \
	} 

#define DECODE_ERROR -100

_IRA_LOGFILTER_IMPORT;

CCommandLine::CCommandLine(ContainerServices *service): CSocket(),
    m_services(service)
{
	AUTO_TRACE("CCommandLine::CCommandLine()");
	m_bTimedout=false;
	m_reiniting=false;
	ACS::Time init=0;
	m_lastUpdate.value(init);
	m_Error.Reset();
	m_configuration=NULL;
	m_backendStatus=0;
	setStatus(NOTCNTD);
	m_setTpiIntegration=true;
    
    m_sectionsNumber=0;
    m_inputsNumber=0;
}

CCommandLine::~CCommandLine()
{
	AUTO_TRACE("CMedicinaMountSocket::~CMedicinaMountSocket()");
	m_Error.Reset();
	// if the backend is transferring data...make a try to inform the backend before closing the connection
	/*if (getIsBusy()) {
		WORD len;
		char sBuff[SENDBUFFERSIZE];
		len=CProtocol::stopAcquisition(sBuff); // get the buffer
		sendBuffer(sBuff,len);
	}*/
	Close(m_Error);
}

void CCommandLine::Init(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl,
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,ComponentErrors::CDBAccessExImpl)
{
    int res;
	//WORD len;
	//char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];

	AUTO_TRACE("CCommandLine::Init()");
	m_configuration=config;
	// this will create the socket in blocking mode.....
	if (Create(m_Error,STREAM)==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::Init()");
	}
	// the first time, perform a blocking connection....
	if (Connect(m_Error,m_configuration->getAddress(),m_configuration->getPort())==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::Init()");
	}	
	else {
		setStatus(CNTD);
	}
	// set socket send buffer!!!!
	int Val=SENDBUFFERSIZE;
	if (setSockOption(m_Error,SO_SNDBUF,&Val,sizeof(int))==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::Init()");
	}
	// set socket in non-blocking mode.
	if (setSockMode(m_Error,NONBLOCKING)==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::Init()");
	}

    /* After the first connection, the server send the protocol version string */
    res=receiveBuffer(rBuff,RECBUFFERSIZE);
    //printf("connect = %s\n", rBuff);
}

void CCommandLine::stopDataAcquisition() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
		ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl,BackendsErrors::BackendFailExImpl)
{
	AUTO_TRACE("CCommandLine::stopDataAcquisition()");
	/*****************************************************************/
	// since both suspend and stop data acquisition are mapped into Command::stop message to the backend
	// It happens (@ the end of a scan) that the backend receives two Command::stop messages. Even if this is not
	// an issue for the backend, this leads to an unwanted behviour of the control software. The thrown exception, infact,
	// leads to skip the first subscan of the next scan. Temporarly workround if to catch the exception here. A debug messages is sent. 
	/*****************************************************************/
	try {
		Message reply = sendBackendCommand(Command::stop());
	}
	catch (...) {
		ACS_LOG(LM_FULL_INFO,"CCommandLine::stopDataAcquisition()",(LM_DEBUG,"backend error while issuing a stop ascquisition"));		
	}
	ACS_LOG(LM_FULL_INFO,"CCommandLine::stopDataAcquisition()",(LM_NOTICE,"TRANSFER_JOB_STOPPED"));
	clearStatusField(CCommandLine::BUSY); // unsets the component status to busy
	clearStatusField(CCommandLine::SUSPEND); // unsets the component status to transfer job suspended......
}

void CCommandLine::stopDataAcquisitionForced() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
		ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl)
{
}


void CCommandLine::startDataAcquisition() throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConnectionExImpl,
		BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl)
{
	AUTO_TRACE("CCommandLine::startDataAcquisition()");
	/* if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::startDataAcquisition()");
		throw impl;
	}*/
	//Message reply = sendBackendCommand(Command::start());
	ACS_LOG(LM_FULL_INFO,"CCommandLine::startDataAcquisition()",(LM_NOTICE,"ACQUISTION_STARTED"));
	setStatusField(CCommandLine::BUSY); // sets the component status to busy
	setStatusField(CCommandLine::SUSPEND); // sets the component status to transfer job suspended......
}

ACS::Time CCommandLine::resumeDataAcquisition(const ACS::Time& startT) throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
		BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::BackendFailExImpl)
{
	TIMEVALUE now;
	TIMEVALUE epoch;
	long waitSec,waitMicro;
	DDWORD diff;
	ACS::Time expectedTime;
	AUTO_TRACE("CCommandLine::resumeDataAcquisition()");
	/*if ( !(m_backendStatus & (1 << SUSPEND)) || !getIsBusy()) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::resumeDataAcquisition()");
		impl.setReason("transfer job cannot be resumed in present configuration");
		throw impl;
	}*/
	// check that the backend latency in preparing the data transfer is respected......
	if (startT==0) { // immediate start
		IRA::CIRATools::getTime(epoch); //...so take the present time
	}
	else {
		epoch.value(startT);
		IRA::CIRATools::getTime(now);
		if (startT<=now.value().value) {
			ACS_LOG(LM_FULL_INFO,"TotalPowerImpl::sendData()",(LM_WARNING,"START_TIME_ALREADY_ELAPSED"));
		}
	}
	diff=IRA::CIRATools::timeDifference(epoch,m_acquisitionStartEpoch);
	if (diff<m_configuration->getDataLatency()) { //wait until the latency time is elapsed......
		double waitTime;
		waitMicro=m_configuration->getDataLatency()-diff;
		waitTime=((double)waitMicro/1000000.0);
		waitSec=(long)waitTime;
		waitMicro%=1000000;
		ACS_LOG(LM_FULL_INFO,"CCommandLine::resumeDataAcquisition()",(LM_WARNING,"BACKEND_LATENCY_NOT_MATCHED,WAITING %3.2lf usec",waitTime));
		IRA::CIRATools::Wait(waitSec,waitMicro);
		expectedTime=m_acquisitionStartEpoch.value().value+m_configuration->getDataLatency()*10;
	}
	else {
		expectedTime=epoch.value().value;
	}
    Message request = Command::start();
    Message reply = sendBackendCommand(request);
    if(reply.is_success_reply()) {
		ACS_LOG(LM_FULL_INFO,"CCommandLine::resumeDataAcquisition()",(LM_NOTICE,"TRANSFER_JOB_RESUMED"));
		clearStatusField(SUSPEND);
	}
	return expectedTime;
}

void CCommandLine::suspendDataAcquisition() throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
			BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::BackendFailExImpl)
{
	AUTO_TRACE("CCommandLine::suspendDataAcquisition()");
	/*if ((m_backendStatus & (1 << SUSPEND)) || !getIsBusy()) { //not suspended....running
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::suspendDataAcquisition()");
		impl.setReason("transfer job cannot be suspended in present configuration");
		throw impl;
	}*/
    Message request = Command::stop();
    Message reply = sendBackendCommand(request);
    if(reply.is_success_reply()) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::suspendDataAcquisition()",(LM_NOTICE,"TRANSFER_JOB_SUSPENDED"));
		setStatusField(SUSPEND);
	}
}

void CCommandLine::setAttenuation(const long&inputId, const double& attenuation) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,BackendsErrors::NakExImpl,
		ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::setAttenuation()");
}

void CCommandLine::setConfiguration(const long& inputId,const double& freq,const double& bw,const long& feed,const long& pol, const double& sr,const long& bins) throw (
		ComponentErrors::ValidationErrorExImpl,BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
		ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::ValueOutofRangeExImpl,
		BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::setConfiguration()");
}

void CCommandLine::getZeroTPI(DWORD *tpi) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
		BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getZeroTPI()");
}

void CCommandLine::getTpi(ACS::doubleSeq& tpi) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getTpi()");
}

void CCommandLine::getZero(ACS::doubleSeq& tpi) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getZero()");
}

void CCommandLine::getSample(ACS::doubleSeq& tpi,bool zero) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getSample()");
}

void CCommandLine::getRms(ACS::doubleSeq& rms) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getRms()");
}

void CCommandLine::setDefaultConfiguration(const IRA::CString & config) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ReplyNotValidExImpl,BackendsErrors::BackendFailExImpl)
{
	AUTO_TRACE("CCommandLine::setDefaultConfiguration()");
}

void CCommandLine::setTargetFileName(const char *conf)
{
    m_targetFileName = (const char*)conf;
}

void CCommandLine::targetSweep()
{
	AUTO_TRACE("CCommandLine::targetSweep()");

    /*	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setup()");
		throw impl;
	}*/
    
    try {
        Message request = Command::setConfiguration(string("targetSweep"));
    	Message reply = sendBackendCommand(request);
    	if(reply.is_success_reply()) {
      }
        ACS_LOG(LM_FULL_INFO,"CCommandLine::targetSweep()",(LM_NOTICE,"BACKEND_SRTMistral_TARGETSWEEP OK")); 
    }
	catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::targetSweep()",(LM_NOTICE,"BACKEND_SRTMistral_TARGETSWEEP ERROR")); 
	}   
}

void CCommandLine::vnaSweep()
{
	AUTO_TRACE("CCommandLine::vnaSweep()");

    /*	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setup()");
		throw impl;
	}*/
    
    try {
        Message request = Command::setConfiguration(string("vnaSweep"));
    	Message reply = sendBackendCommand(request);
    	if(reply.is_success_reply()) {
        }
        ACS_LOG(LM_FULL_INFO,"CCommandLine::vnaSweep()",(LM_NOTICE,"BACKEND_SRTMistral_VNASWEEP OK")); 
    }
	catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::vnaSweep()",(LM_NOTICE,"BACKEND_SRTMistral_VNASWEEP ERROR")); 
	}
}

void CCommandLine::sendTargetFileName() throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl)
{
	AUTO_TRACE("CCommandLine::sendTargetFileName()");
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	// I do not check for backend busy because this is a call done at the initialization and never repeated
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::sendTargetFileName()");
	}
	strcpy (sBuff,"?set-filename,");
	strcat (sBuff,(const char*)m_targetFileName);
	strcat (sBuff,"\r\n");
	len = strlen (sBuff);

	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
        //printf("set-filename = %s\n",rBuff);
	}
	if (res>0) { // operation was ok.
		//if (!CProtocol::setConfiguration(rBuff)) {
		//	_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::setTargetFileName()");
		//}
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::sendTargetFileName()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::sendTargetFileName()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::sendTargetFileName()");
	}
	ACS_LOG(LM_FULL_INFO,"CCommandLine::sendTargetFileName()",(LM_NOTICE,"targetFileName SENT"));
}

void CCommandLine::setup(const char *conf) throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConfigurationErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,ComponentErrors::CDBAccessExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ReplyNotValidExImpl,BackendsErrors::BackendFailExImpl)
{
	AUTO_TRACE("CCommandLine::setup()");

    /*	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setup()");
		throw impl;
	}*/
    
	if (!initializeConfiguration(conf)) {
		_EXCPT(BackendsErrors::ConfigurationErrorExImpl,impl,"CCommandLine::setup()");
		throw impl;
	}
    try {
        //Message request = Command::setConfiguration(string((const char*)conf));
        Message request = Command::setConfiguration(string("setup"));
    	Message reply = sendBackendCommand(request);
    	if(reply.is_success_reply()) {
            m_inputsNumber = 2;
            m_sectionsNumber = 1;
        }
        ACS_LOG(LM_FULL_INFO,"CCommandLine::setup()",(LM_NOTICE,"BACKEND_SRTMistral_INITIALIZED OK, CONFIGURATION: %s",conf)); 
    }
	catch (...) {
        ACS_LOG(LM_FULL_INFO,"CCommandLine::setup()",(LM_NOTICE,"BACKEND_SRTMistral_INITIALIZATION ERROR, CONFIGURATION: %s",conf)); 
	}   
}

void CCommandLine::checkTime() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::MalformedAnswerExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::checkTime()");
	}
	/*Message request = Command::time();
    Message reply = sendBackendCommand(request);
    string _time = reply.get_argument<string>(0);
    if(reply.is_success_reply())
        strcpy(time, _time.c_str());
    return _time.length();*/
    
    /*char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	WORD len;
	int res;
	///the check of th ebusy status is not done because this time chek must be performed also during sampling.
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::checkTime()");
	}
	len=CProtocol::checkBackendTime(sBuff); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		bool sync;
		if (!CProtocol::checkBackendTime(rBuff,m_configuration->getTimeTollerance(),sync)) {
			_THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"CCommandLine::checkTime()");
		} else if (!sync) {
			_IRA_LOGFILTER_LOG(LM_WARNING,"CCommandLine::checkTime()","BACKEND_CLOCK_NOT_SYNCHRONIZED");
			setStatusField(TIME_SYNC);
		}
		else {
			clearStatusField(TIME_SYNC);
		}
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::checkTime()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::checkTime()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::checkTime()");
	}*/
}

void CCommandLine::setTime()  throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
		BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::setTime()");
	//int res;
	//WORD len;
	//char sBuff[SENDBUFFERSIZE];
	//char rBuff[RECBUFFERSIZE];
/*	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setTime()");
		throw impl;
	}*/
	/*len=CProtocol::setBackendTime(sBuff); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		bool sync;
		if (!CProtocol::decodeBackendTime(rBuff,sync)) {
			_THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"CCommandLine::setTime()");
			setStatusField(TIME_SYNC);
		} else if (!sync) {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::setTime()",(LM_WARNING,"CLOCK_NOT_SYNCHRONIZED"));
			setStatusField(TIME_SYNC);
		}
		else {
			clearStatusField(TIME_SYNC);
			ACS_LOG(LM_FULL_INFO,"CCommandLine::setTime()",(LM_NOTICE,"TIME_IS_SYNCHRONIZED"));
		}
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::setTime()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::setTime()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setTime()");
	}
    */
}

void CCommandLine::activateCalSwitching(const long& interleave) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::NotAllowedExImpl)
{
	AUTO_TRACE("CCommandLine::activateCalSwitching()");
	/*int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setTime()");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setTime()");
	}
    Message request = Command::calOn(interleave);
    Message reply = sendBackendCommand(request);
    if(reply.is_success_reply()){
        // TBD
    }
	if (!m_calSwitchingEnabled) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::activateCalSwitching()");
		throw impl;
	}
	if (interleave>=0) {
		m_calPeriod=interleave;
		if (m_calPeriod>0) {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::activateCalSwitching()",(LM_NOTICE,"CALIBRATION_DIODE_SWITCHING_ON"));
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::activateCalSwitching()",(LM_NOTICE,"CALIBRATION_DIODE_SWITCHING_OFF"));			
		}
	}*/
}

void CCommandLine::setEnabled(const ACS::longSeq& en) throw (BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::setEnabled()");
	/*int bound;
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setEnabled()");
		throw impl;
	}
	if ((long)en.length()>=m_sectionsNumber) {
		bound=m_sectionsNumber;
	}
	else {
		bound=en.length();
	}
	for (int i=0;i<bound;i++) {
		if (en[i]>0) m_enabled[i]=true;
		else if (en[i]==0) m_enabled[i]=false;
	}*/	
}

void CCommandLine::setIntegration(const long& integration)  throw (BackendsErrors::BackendBusyExImpl, ComponentErrors::ValueOutofRangeExImpl)
{
	AUTO_TRACE("CCommandLine::setIntegration()");
	/*if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setIntegration()");
		throw impl;
	}*/
    /* m_integration = integration;
    if (m_integration>=MIN_INTEGRATION && m_integration <= MAX_INTEGRATION) {
        Message request = Command::setIntegration(integration);
        Message reply = sendBackendCommand(request);
        if (reply.is_success_reply()) {
            // TBD
            ACS_LOG(LM_FULL_INFO,"CCommandLine::setIntegration()",(LM_NOTICE,"INTEGRATION is now %ld (millisec)",m_integration));
        }
    }
    else {
        _EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setIntegration()");
        impl.setValueName("integration");
		throw impl;
    }*/
}

void CCommandLine::getAttenuation(ACS::doubleSeq& att) throw (ComponentErrors::SocketErrorExImpl,
		ComponentErrors::TimeoutExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::getAttenuation()");
    att.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		att[i]=m_attenuation[i];
	}
}

void CCommandLine::getAttenuationAttr(ACS::doubleSeq& att) throw (ComponentErrors::SocketErrorExImpl,
		ComponentErrors::TimeoutExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::getAttenuation()");
    att.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		att[i]=m_attenuation[i];
	}
}

void CCommandLine::getFrequency(ACS::doubleSeq& freq) const
{
	freq.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		freq[i]=m_frequency[i];
	}
}

void CCommandLine::getFrequencyAttr(ACS::doubleSeq& freq) const
{
	freq.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		freq[i]=m_frequency[i];
	}
}

void CCommandLine::getBackendStatus(DWORD& status)
{
	AUTO_TRACE("CCommandLine::getBackendStatus()");
    try {
        Message request = Command::status();
        Message reply = sendBackendCommand(request);
        if(reply.is_success_reply()) {
            string _status = reply.get_argument<string>(2);
            //strcpy(status, _status.c_str());
            status=atof(_status.c_str());
            /* if (status == 0)
					clearStatusField(BUSY);
            if (status == 1)
					setStatusField(BUSY); */
        }
    }
    catch (...) {
         ACS_LOG(LM_FULL_INFO,"CCommandLine::getBackendStatus()",(LM_NOTICE,"BACKEND_SRTMistral_GET_STATUS_ERROR"));
    }
    //return _status.length(); 
    return status; 
}

void CCommandLine::getSampleRate(ACS::doubleSeq& sr) const
{
	sr.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		sr[i]=m_sampleRate[i];
	}
}

void CCommandLine::getTsys(ACS::doubleSeq& tsys) const
{
	/*tsys.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		tsys[i]=m_tsys[i];
	}*/
	tsys.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		tsys[i]=m_tsys[i];
	}	
}

void CCommandLine::getKCRatio(ACS::doubleSeq& ratio) const
{
	ratio.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		ratio[i]=m_KCratio[i];
	}		
}

void CCommandLine::getBins(ACS::longSeq& bins) const
{
	bins.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		bins[i]=m_bins[i];
	}
}

void CCommandLine::getPolarization(ACS::longSeq& pol) const
{
	//pol.length(m_sectionsNumber);
	pol.length(0);
}

void CCommandLine::getFeed(ACS::longSeq& feed) const 
{
	feed.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		feed[i]=m_feedNumber[i];
	}
}

void CCommandLine::getFeedAttr(ACS::longSeq& feed) const 
{
    int i;

	feed.length(m_sectionsNumber);
    
    for (int i=0;i<m_sectionsNumber;i++)
	    feed[i]=m_feedNumber[i];
}

void CCommandLine::getIFs(ACS::longSeq& ifs) const
{
	ifs.length(m_inputsNumber);
	for (int i=0;i<m_inputsNumber;i++) {
		ifs[i]=m_ifNumber[i];
	}	
}

void CCommandLine::getInputSection(ACS::longSeq& inpSection) const
{
	inpSection.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
		inpSection[i]=m_inputSection[i];
	}
}

void CCommandLine::getInputSectionAttr(ACS::longSeq& inpSection) const
{
    long index=0;

    index = m_sectionsNumber;

    inpSection.length(index);
	for (int i=0;i<index;i++) {
		inpSection[i]=m_inputSection[i];
	}
}

void CCommandLine::getBandWidth(ACS::doubleSeq& bw) throw (ComponentErrors::SocketErrorExImpl,
		ComponentErrors::TimeoutExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::getBandWidth()");
    bw.length(m_inputsNumber);
    for (int i=0;i<m_inputsNumber;i++) {
        bw[i]=m_bandWidth[i];
    }
}

void CCommandLine::getBandWidthAttr(ACS::doubleSeq& bw) const
{
	AUTO_TRACE("CCommandLine::getBandWidthAttr()");

	bw.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		bw[i]=m_bandWidth[i];
	}
}
void CCommandLine::getTime(ACS::Time& tt) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::getTime()");
    TIMEDIFFERENCE bkndTime;
    TIMEVALUE tm;
    long long seconds;
	long second,minute,hour,day;
    try {
        Message request = Command::time();
        Message reply = sendBackendCommand(request);
        if(reply.is_success_reply()) {
            seconds=reply.get_argument<long long>(0);
            day=seconds/86400;
	        seconds%=86400;
	        hour=seconds/3600;
	        seconds%=3600;
	        minute=seconds/60;
	        second=seconds % 60;
	        bkndTime.day(day);
	        bkndTime.hour(hour);
	        bkndTime.minute(minute);
	        bkndTime.second(second);
            tm.reset();
	        tm.normalize(true);
	        tm.year(1970);
	        tm.month(1);
	        tm.day(1);
            tm+=bkndTime.value();
            tt=tm.value().value;
        }
    }
    catch (...) {
         ACS_LOG(LM_FULL_INFO,"CCommandLine::getTime()",(LM_NOTICE,"BACKEND_SRTMistral_GET_TIME_ERROR"));
    }
}

void CCommandLine::fillMainHeader(Backends::TMainHeader& bkd)
{
	bkd.sections=m_sectionsNumber;
	bkd.beams=m_beams;
	//resultingSampleRate(m_integration,m_commonSampleRate,intTime);
	//bkd.integration=intTime;
	bkd.integration=m_integration;
	bkd.sampleSize=m_sampleSize;
	bkd.noData=true;
}

void CCommandLine::fillChannelHeader(Backends::TSectionHeader *chHr,const long& size)
{
	long index=0;
	for (int i=0;i<m_sectionsNumber;i++) {
	//for (int i=0;i<m_inputsNumber;i++) {
                //printf("i = %d\n", i);
		//if (m_enabled[i]) {
		//	if (index<size) {
				chHr[index].id=i;
				chHr[index].bins=m_bins[i];
				chHr[index].bandWidth=m_bandWidth[i];
				chHr[index].frequency=m_frequency[i];
				chHr[index].attenuation[0]=m_attenuation[i];
				chHr[index].attenuation[1]=m_attenuation[i];
				chHr[index].sampleRate=/*m_sampleRate[i];*/m_commonSampleRate;
                /*if (m_SK77S==true || m_SK03S==true || m_SK06S==true || m_SC00S==true || m_SK00S==true || m_SL00S==true || m_SP00S==true) { */
                    chHr[index].inputs=2;
				    chHr[index].feed=m_feedNumber[2*i];
				    chHr[index].polarization=m_polarization[2*i];
				    chHr[index].IF[0]=0;
				    chHr[index].IF[1]=1;
                /* }
                else {
                    chHr[index].inputs=1;
				    chHr[index].feed=m_feedNumber[i];
				    chHr[index].polarization=m_polarization[i];
				    chHr[index].IF[0]=m_ifNumber[i];
                } */
                //printf("feed = %d\n", chHr[index].feed);
                //printf("pol = %d\n", m_polarization[i]);
				index++;
		//	}
		//}
	}
}

void CCommandLine::saveTsys(const ACS::doubleSeq& tsys,const ACS::doubleSeq& ratio)
{
	/*if (tsys.length()==(unsigned)m_sectionsNumber) {
		for (int i=0;i<m_sectionsNumber;i++) {
			m_tsys[i]=tsys[i];
		}
	}
	if (ratio.length()==(unsigned)m_sectionsNumber) {
		for (int i=0;i<m_sectionsNumber;i++) {
			m_KCratio[i]=ratio[i];
		}
		ACS_LOG(LM_FULL_INFO,"CCommandLine::saveTsys()",(LM_NOTICE,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
	}*/
	if (tsys.length()==(unsigned)m_inputsNumber) {
		for (int i=0;i<m_inputsNumber;i++) {
			m_tsys[i]=tsys[i];
		}
		ACS_LOG(LM_FULL_INFO,"CCommandLine::saveTsys()",(LM_NOTICE,"TSYS_SET"));
	}
	if (ratio.length()==(unsigned)m_inputsNumber) {
		for (int i=0;i<m_inputsNumber;i++) {
			m_KCratio[i]=ratio[i];
		}
		ACS_LOG(LM_FULL_INFO,"CCommandLine::saveTsys()",(LM_NOTICE,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
	}
}

bool CCommandLine::resultingSampleRate(const long& integration,const double& sr,long& result)
{
	bool res=false;
	long accumulations;
	double temp;
	temp=(double)integration*1000.0*sr;
	accumulations=(long)round(temp);
	if (accumulations<1) accumulations=1;
	if (((temp-accumulations)>0.0) || ((temp-accumulations)<0.0)) {
		res=true;
	}
	result=(long)round((accumulations/(sr*1000)));
	return res;
}

// Protected Methods

void CCommandLine::onConnect(int ErrorCode)
{
	CError Tmp;
	if (getStatus()==CNTDING) { // if the line is connecting.......
		if (ErrorCode==0) {
			if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) {
				setStatus(CNTD);
				CSocket::setStatus(IRA::CSocket::READY); // force the socket status to be ready.....
				try {
					//stopDataAcquisitionForced(); // ask the backend to exit the data transfering mode......
				}
				catch (ACSErr::ACSbaseExImpl& ex) {
					
				}
				clearStatusField(BUSY);
				m_reiniting=true; // ask to the datasocket to accept connection again
				ACS_LOG(LM_FULL_INFO,"CCommandLine::onConnect()",(LM_NOTICE,"SOCKET_RECONNECTED"));
			}
		}
		else {
			setStatus(NOTCNTD);
			ACS_DEBUG_PARAM("CCommandLine::onConnect()","Reconnection failed, exit code is %d",ErrorCode);	
		}
	}
}

void CCommandLine::onTimeout(WORD EventMask)
{
	if ((EventMask&E_CONNECT)==E_CONNECT) {
		ACS_DEBUG("CCommandLine::onTimeout()","Reconnection timed-out, keep trying....");
	}
}

// private methods

IRA::CSocket::OperationResult CCommandLine::sendBuffer(char *Msg,WORD Len)
{
	int NWrite;
	int BytesSent;
	BytesSent=0;
	while (BytesSent<Len) {
		if ((NWrite=Send(m_Error,(const void *)(Msg+BytesSent),Len-BytesSent))<0) {
			if (NWrite==WOULDBLOCK) {
				setStatus(NOTCNTD);
				_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::sendBuffer()","SOCKET_DISCONNECTED - remote side shutdown");
				return WOULDBLOCK;
			}
			else {
				setStatus(NOTCNTD);
				CString app;
				app.Format("SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
				_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::sendBuffer()",(const char*)app);
				return FAIL;
			}
		}
		else { // success
			BytesSent+=NWrite;
		}
	}
	if (BytesSent==Len) {
		return SUCCESS;
   }
	else {
		_SET_ERROR(m_Error,CError::SocketType,CError::SendError,"CCommandLine::SendBuffer()");
		return FAIL;
	}
}

int CCommandLine::receiveBuffer(char *Msg,WORD Len)
{
	char inCh;
	int res;
	int nRead=0;
	TIMEVALUE Now;
	TIMEVALUE Start;
	CIRATools::getTime(Start);
	while(true) {
		res=Receive(m_Error,&inCh,1);
		if (res==WOULDBLOCK) {
			CIRATools::getTime(Now);
			if (CIRATools::timeDifference(Start,Now)>m_configuration->getCommandLineTimeout()) {
				m_bTimedout=true;
				return WOULDBLOCK;
			}
			else {
				CIRATools::Wait(0,20000);
				continue;
			}
		}
		else if (res==FAIL) { 
			setStatus(NOTCNTD);
			CString app;
			app.Format("SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
			_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::receiveBuffer()",(const char*)app);
			return res;
		}
		else if (res==0) {
			setStatus(NOTCNTD);
			_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::receiveBuffer()","SOCKET_DISCONNECTED - remote side shutdown");			
			return res;
		}
		else {
			if (inCh!=PROT_TERMINATOR_CH) {
				Msg[nRead]=inCh;
				nRead++;
			}
			else {
				Msg[nRead]=0;
				return nRead;
			}
		}
	}
}

int CCommandLine::sendCommand(char *inBuff,const WORD& inLen,char *outBuff)
{
	OperationResult Res;
	if ((Res=sendBuffer(inBuff,inLen))==SUCCESS) {
		int bytes=receiveBuffer(outBuff,RECBUFFERSIZE);
		return bytes;
	}
	else {  // send fails....m_Error already set by sendBuffer
		return Res;
	}
}

Message 
CCommandLine::sendBackendCommand(Message request)
{
    if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::sendBackendCommand()");
	}
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
    	strcpy (sBuff,request.toString(true).c_str());
	int len = strlen (sBuff);
    	int res = 0;
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
        	clearStatusField(CCommandLine::BUSY);
		    setStatusField(SUSPEND);
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::sendBackendCommand()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::sendBackendCommand()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::sendBackendCommand()");
	}
   	Message reply(string(rBuff, res), true);
   	try {
       	reply.validate();
   	}
	catch (BackendProtocolError& bpe) {
		//TODO: somehow the reply is not valid
        	//we cannot proceed we need to throw an exception
		_THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"sendBackendCommand()");
    }
    if(!(reply.is_valid_reply_for(request))) {
        //TODO: this answer is not valid for this request, throw exception
		_THROW_EXCPT(BackendsErrors::ReplyNotValidExImpl,"sendBackendCommand()");
    }
    if(!(reply.is_success_reply())) {
        //TODO: the reply has a FAIL o INVALID code, this must be managed
		_EXCPT(BackendsErrors::BackendFailExImpl,impl,"sendBackendCommand()");
		//impl.setReason("transfer job cannot be resumed in present configuration");
		impl.setReason(reply.get_argument<string>(0).c_str());
		throw impl;
    }
	return reply;
}

//int CCommandLine::getConfiguration(char* configuration)
void CCommandLine::getConfiguration(char* configuration)
{
    Message request = Command::getConfiguration();
    try {
        Message reply = sendBackendCommand(request);
        if(reply.is_success_reply())
        {
		    strcpy(configuration, reply.get_argument<string>(0).c_str());
        }
    }
    catch (...) {
    
    }
}

int CCommandLine::getCommProtVersion(CORBA::String_out version)
{
    Message request = Command::version();
    Message reply = sendBackendCommand(request);
    string _version = reply.get_argument<string>(0);
    if(reply.is_success_reply())
        strcpy(version, _version.c_str());
    return _version.length();
}

bool CCommandLine::checkConnection()
{
	CError Tmp;
	char sBuffer[RECBUFFERSIZE];
	int rBytes;
	if (m_bTimedout) { // if a previous time out happend....try if the buffer has still to be received 
		rBytes=receiveBuffer(sBuffer,RECBUFFERSIZE);
		if (rBytes==WOULDBLOCK) { // if the operation would have block again.....declare the disconnection
			setStatus(NOTCNTD);	// another timeout! something has happend
			_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommandLine::checkConnection()","SOCKET_DISCONNECTED - timeout expired");
		}
		else if (rBytes==FAIL) {
			// Nothing to do, this error will be handled below....
		}
		else if (rBytes==0) {
			// Nothing to do, this error will be handled below....
		}
		else {
			m_bTimedout=false; // timeout recovered
		}
	}
	if (getStatus()==CNTD)  {
		return true;
	}
	else if (getStatus()==CNTDING) {
		return false;
	}
	else {  // socket is not connected....
		// try to close the socket, if it is already closed : never known....
		Close(Tmp);
		Tmp.Reset();
		m_bTimedout=false;
		// this will create the socket in blocking mode.....
		if (Create(Tmp,STREAM)==SUCCESS) {
			// Put the socket in non-blocking mode, registers the onconnect event....and allow some seconds to complete!
			if (EventSelect(Tmp,E_CONNECT,true,m_configuration->getConnectTimeout())==SUCCESS) {
				OperationResult Res;
				ACS_DEBUG("CCommandLine::checkConnection()","Trying to reconnect");
				Res=Connect(Tmp,m_configuration->getAddress(),m_configuration->getPort());
				if (Res==WOULDBLOCK) {
					setStatus(CNTDING);
				}
				else if (Res==SUCCESS) {
					setStatus(CNTD);
					try {
						//stopDataAcquisitionForced(); // ask the backend to exit the data transfering mode......
					}
					catch (ACSErr::ACSbaseExImpl& ex) {
						
					}
					clearStatusField(BUSY);
					m_reiniting=true;
					ACS_LOG(LM_FULL_INFO,"CCommandLine::checkConnection()",(LM_NOTICE,"SOCKET_RECONNECTED")); //we do not want to filter this info so no logfilter.....				
					return true;
				}
			}
		}
		return false;
	}
}

bool CCommandLine::initializeConfiguration(const IRA::CString & config) throw (ComponentErrors::CDBAccessExImpl)
{
	int i;
	CConfiguration::TBackendSetup setup;
	if (m_configuration->getSetupFromID(config,setup)) { // throw (ComponentErrors::CDBAccessExImpl)
		m_sectionsNumber=setup.sections;
		for (WORD k=0;k<MAX_BOARDS_NUMBER;k++) {
			//m_defaultInput[k]=setup.inputPort[k];
			m_sections[k]=-1;
		}
		m_defaultInputSize=setup.inputPorts; // this should be 1 or the number of installed boards
		m_beams=setup.beams;
		m_calSwitchingEnabled=setup.calSwitchEnabled;
		for (i=0;i<m_sectionsNumber;i++) {
			m_boards[i]=setup.section_boards[i];
			m_sections[m_boards[i]]=i;
			//m_input[i]=m_defaultInput[m_boards[i]];
			m_polarization[i]=setup.polarizations[i];
            //printf ("m_polarization = %d\n", m_polarization[i]);
			m_ifNumber[i]=setup.ifs[i];
			m_feedNumber[i]=setup.feed[i];
			m_inputSection[i]=i; // input 0 belongs to section 0 and so on.....
			m_attenuation[i]=setup.attenuation;
			m_bandWidth[i]=setup.bandWidth;
			m_bins[i]=setup.bins;
		}		
	}
	else {
		return false;
	}

	// Common configurations.......
	m_integration=DEFAULT_INTEGRATION;  // integration if by default zero...that means the 1/samplerate is the real integration time
	m_currentSampleRate=m_integration;  // this is given in milliseconds as sample period
	m_sampleSize=SAMPLESIZE;
	m_commonSampleRate=DEFAULT_SAMPLE_RATE;
	m_calPeriod=DEFAULT_DIODE_SWITCH_PERIOD;
	for (i=0;i<m_sectionsNumber;i++) {
		m_sampleRate[i]=DEFAULT_SAMPLE_RATE;
		m_frequency[i]=STARTFREQUENCY;
		//m_bins[i]=BINSNUMBER;
		m_enabled[i]=true;
		m_tsys[i]=0.0;
		m_KCratio[i]=1.0;
		m_tpiZero[i]=0.0;
	}
	return true;
}

void CCommandLine::setStatus(TLineStatus sta)
{
	m_status=sta;
	if (m_status!=CNTD) {
		setStatusField(CMDLINERROR);
	}
	else {
		clearStatusField(CMDLINERROR);
	}
}
