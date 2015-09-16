// $Id: CommandLine.cpp,v 1.3 2011-05-12 14:14:31 a.orlati Exp $

#include <LogFilter.h>
#include "CommandLine.h"
/*****************************************************************************************************************************/
#include <CustomLoggerUtils.h>
/*****************************************************************************************************************************/

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

CCommandLine::CCommandLine(): CSocket()
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
}

CCommandLine::~CCommandLine()
{
	AUTO_TRACE("CMedicinaMountSocket::~CMedicinaMountSocket()");
	m_Error.Reset();
	// if the backend is transferring data...make a try to inform the backend before closing the connection
	if (getIsBusy()) {
		WORD len;
		char sBuff[SENDBUFFERSIZE];
		len=CProtocol::stopAcquisition(sBuff); // get the buffer
		sendBuffer(sBuff,len);
	}
	Close(m_Error);
}

void CCommandLine::Init(CConfiguration *config) throw (ComponentErrors::SocketErrorExImpl,
		ComponentErrors::ValidationErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,ComponentErrors::CDBAccessExImpl)
{
	AUTO_TRACE("CCommandLine::Init()");
	m_configuration=config;
	if (!initializeConfiguration(m_configuration->getConfiguration())) { // throw (ComponentErrors::CDBAccessExImpl)
		IRA::CString msg;
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::Init()");
		msg.Format("the requested configuration %s is not known",(const char *)m_configuration->getConfiguration());
		impl.setReason((const char *)msg);
		throw impl;
	}
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
	m_hostAddress=m_configuration->getDataIPAddress();
	if (m_hostAddress=="") {
		if ((m_hostName=getHostName())=="") {
			_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"CCommandLine::Init()");
		}
		if ((m_hostAddress=getAddrbyHostName(m_hostName))=="") {
			_THROW_EXCPT(ComponentErrors::SocketErrorExImpl,"CCommandLine::Init()");
		}
	}	
	//Waits a bit so that everything can settle down
	IRA::CIRATools::Wait(0,200000);
	try {
		stopDataAcquisitionForced();  // this will force the backend to a normal status in case the connection came from an abnormal close
		setDefaultConfiguration(); //could throw exceptions........
	}
	catch (ACSErr::ACSbaseExImpl& ex) { // these are not real error so we do not want to give up
		
	}
}

void CCommandLine::stopDataAcquisition() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
		ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl)
{
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	AUTO_TRACE("CCommandLine::stopDataAcquisition()");
	if (!getIsBusy()) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::stopDataAcquisition()");
		impl.setReason("transfer job cannot be stopped in this configuration");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::stopDataAcquisition()");
	}
	len=CProtocol::stopAcquisition(sBuff); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::stopDataAcquisition()");
		} 
		ACS_LOG(LM_FULL_INFO,"CCommandLine::stopDataAcquisition()",(LM_INFO,"TRANSFER_JOB_STOPPED"));
		clearStatusField(CCommandLine::BUSY); // sets the component status to busy
		clearStatusField(CCommandLine::SUSPEND); // sets the component status to transfer job suspended......
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::stopDataAcquisition()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::stopDataAcquisition()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::stopDataAcquisition()");
	}
}

void CCommandLine::stopDataAcquisitionForced() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
		ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::NotAllowedExImpl)
{
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	AUTO_TRACE("CCommandLine::stopDataAcquisitionForced()");
	len=CProtocol::stopAcquisition(sBuff); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::stopDataAcquisitionForced()");
		} 
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::stopDataAcquisition()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::stopDataAcquisition()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::stopDataAcquisition()");
	}
}


void CCommandLine::startDataAcquisition() throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConnectionExImpl,
		BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl)
{
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	AUTO_TRACE("CCommandLine::startDataAcquisition()");
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::startDataAcquisition()");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::startDataAcquisition()");
	}
	len=CProtocol::startAcquisition(sBuff,m_commonSampleRate,m_calPeriod,m_configuration->getDataPort(),
			m_hostAddress); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::startDataAcquisition()");
		} 
		ACS_LOG(LM_FULL_INFO,"CCommandLine::startDataAcquisition()",(LM_INFO,"TRANSFER_JOB_STARTED"));
		m_setTpiIntegration=true; // force the configuration of integration time for the next call to getSample();
		IRA::CIRATools::getTime(m_acquisitionStartEpoch);
		setStatusField(CCommandLine::BUSY); // sets the component status to busy
		setStatusField(CCommandLine::SUSPEND); // sets the component status to transfer job suspended......
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::startDataAcquisition()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::startDataAcquisition()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::startDataAcquisition()");
	}
}

ACS::Time CCommandLine::resumeDataAcquisition(const ACS::Time& startT) throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
		BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl)
{
	int res;
	WORD len;
	TIMEVALUE now;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	TIMEVALUE epoch;
	long waitSec,waitMicro;
	DDWORD diff;
	ACS::Time expectedTime;
	AUTO_TRACE("CCommandLine::resumeDataAcquisition()");
	if ( !(m_backendStatus & (1 << SUSPEND)) || !getIsBusy()) {
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::resumeDataAcquisition()");
		impl.setReason("transfer job cannot be resumed in present configuration");
		throw impl;
	}
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
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::resumeDataAcquisition()");
	}
	len=CProtocol::resumeAcquisition(sBuff); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::resumeDataAcquisition()");
		} 
		ACS_LOG(LM_FULL_INFO,"CCommandLine::resumeDataAcquisition()",(LM_INFO,"TRANSFER_JOB_RESUMED"));
		clearStatusField(SUSPEND);
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::resumeDataAcquisition()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::resumeDataAcquisition()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::resumeDataAcquisition()");
	}
	return expectedTime;
}

void CCommandLine::suspendDataAcquisition() throw (BackendsErrors::ConnectionExImpl,ComponentErrors::NotAllowedExImpl,
			BackendsErrors::NakExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl)
{
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	AUTO_TRACE("CCommandLine::suspendDataAcquisition()");
	if ((m_backendStatus & (1 << SUSPEND)) || !getIsBusy()) { //not suspended....running
		_EXCPT(ComponentErrors::NotAllowedExImpl,impl,"CCommandLine::resumeDataAcquisition()");
		impl.setReason("transfer job cannot be suspended in present configuration");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::suspendDataAcquisition()");
	}
	len=CProtocol::suspendAcquisition(sBuff); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::suspendDataAcquisition()");
		} 
		ACS_LOG(LM_FULL_INFO,"CCommandLine::suspendDataAcquisition()",(LM_INFO,"TRANSFER_JOB_SUSPENDED"));
		setStatusField(SUSPEND);
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::suspendDataAcquisition()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::suspendDataAcquisition()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::suspendDataAcquisition()");
	}
}

void CCommandLine::setAttenuation(const long&inputId, const double& attenuation) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,BackendsErrors::NakExImpl,
		ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::setAttenuation()");
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	double newAtt,newBW;
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setAttenuation()");
		throw impl;
	}
	if (inputId>=0) {
		if (inputId>=m_sectionsNumber) {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setAttenuation()");
			impl.setReason("the input identifier is out of range");
			throw impl;
		}
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setAttenuation()");
		impl.setReason("the input identifier is out of range");
		throw impl;	
	}
	if (attenuation>=0) { // // the user ask for a new value
		if  (attenuation<MIN_ATTENUATION) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setAttenuation()");
			impl.setValueName("attenuation");
			impl.setValueLimit(MIN_ATTENUATION);
			throw impl;						
		}
		else if (attenuation>MAX_ATTENUATION) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setAttenuation()");
			impl.setValueName("attenuation");
			impl.setValueLimit(MAX_ATTENUATION);
			throw impl;									
		}
		newAtt=attenuation;
	}
	else {
		newAtt=m_attenuation[inputId];
	}
	newBW=m_bandWidth[inputId];
	len=CProtocol::setConfiguration(sBuff,inputId,m_input[inputId],newAtt,newBW,m_boards); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::setAttenuation()");
		} 
		ACS_LOG(LM_FULL_INFO,"CCommandLine::setAttenuation()",(LM_NOTICE,"INPUT_CONFIGURED %ld,ATT=%lf",inputId,newAtt));
		/***********************************************************************************************************************************/
		/***********************************************************************************************************************************/
		//CUSTOM_LOG(LM_FULL_INFO,"CCommandLine::setAttenuation()",(LM_NOTICE,"INPUT_CONFIGURED %ld,ATT=%lf",inputId,newAtt));
		m_attenuation[inputId]=newAtt;
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::setAttenuation()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::setAttenuation()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setAttenuation()");
	}		
}

void CCommandLine::setConfiguration(const long& inputId,const double& freq,const double& bw,const long& feed,const long& pol, const double& sr,const long& bins) throw (
		ComponentErrors::ValidationErrorExImpl,BackendsErrors::ConnectionExImpl,BackendsErrors::NakExImpl,
		ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,ComponentErrors::ValueOutofRangeExImpl,
		BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::setConfiguration()");
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	double newBW,newAtt,newSR;
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setConfiguration()");
		throw impl;
	}
	if (inputId>=0) {  //check the section id is in valid rages
		if (inputId>=m_sectionsNumber) {
			_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setReason("the section identifier is out of range");
			throw impl;
		}
	}
	else {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandLine::setConfiguration()");
		impl.setReason("the section identifier is out of range");
		throw impl;
	}
	if (bw>=0) { // the user ask for a new value
		if (bw<MIN_BAND_WIDTH)  {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("bandWidth");
			impl.setValueLimit(MIN_BAND_WIDTH);
			throw impl;			
		}
		else if (bw>MAX_BAND_WIDTH) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("bandWidth");
			impl.setValueLimit(MAX_BAND_WIDTH);
			throw impl;						
		}
		newBW=bw;
	}
	else { // else keep the present value
		newBW=m_bandWidth[inputId];
	}
	if (sr>=0) {// the user ask for a new value
		if (sr>MAX_SAMPLE_RATE) {
			_EXCPT(ComponentErrors::ValueOutofRangeExImpl,impl,"CCommandLine::setConfiguration()");
			impl.setValueName("sampleRate");
			impl.setValueLimit(MAX_SAMPLE_RATE);
			throw impl;			
		}
		newSR=sr;
	}
	else {
		newSR=m_sampleRate[inputId];
	}
	newAtt=m_attenuation[inputId];
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setConfiguration()");
	}
	len=CProtocol::setConfiguration(sBuff,inputId,m_input[inputId],newAtt,newBW,m_boards); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::setConfiguration()");
		} 
		m_bandWidth[inputId]=newBW;
		for (int j=0;j<m_sectionsNumber;j++) m_sampleRate[j]=newSR; //the given sample rate is taken also for all the others
		m_commonSampleRate=newSR;
		m_integration=0;
		// log warning about configuration that are ignored.
		if (freq>=0) {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::setConfiguration()",(LM_WARNING,"CANNOT_CHANGE_FREQUENCY"));
		}
		if (feed>=0) {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::setConfiguration()",(LM_WARNING,"CANNOT_CHANGE_FEED"));			
		}		
		if (bins>=0) {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::setConfiguration()",(LM_WARNING,"CANNOT_CHANGE_BINS_NUMBER"));			
		}
		if (pol>=0) {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::setConfiguration()",(LM_WARNING,"CANNOT_CHANGE_POLARIZATION"));
		}
		IRA::CString temp;
		if (m_polarization[inputId]==Backends::BKND_LCP) {  //FULL STOKE not possible....
			temp="LCP";
		}
		else {
			temp="RCP";
		}
		ACS_LOG(LM_FULL_INFO,"CCommandLine::setConfiguration()",(LM_NOTICE,"SECTION_CONFIGURED %ld,FREQ=%lf,BW=%lf,FEED=%d,POL=%s,SR=%lf,BINS=%d",inputId,m_frequency[inputId],newBW,m_feedNumber[inputId],
				(const char *)temp,newSR,m_bins[inputId]));		
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::setConfiguration()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::setConfiguration()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setConfiguration()");
	}	
}

void CCommandLine::getZeroTPI(DWORD *tpi) throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
		BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getZeroTPI()");
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	long integration;
	bool busy=getIsBusy();
	long waitTime=0;
	if (busy) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::getZeroTPI()");
		throw impl;
	}
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
	}
	integration=(long)round(1.0/ (m_commonSampleRate*1000.0));
	res=getConfiguration(); // refresh the m_currentSampleRate..........
	if (res>0) { // load OK
		// do nothing
	}
	_CHECK_ERRORS("CommandLine::getZeroTPI()");
	//if the requested integration is not equal to the current sample rate (given is milliseconds as a sample period)
	// or the integration time must be forced...and the backend is not busy. Then set the correct sample rate....
	if ((integration!=m_currentSampleRate) || (m_setTpiIntegration)) {
		len=CProtocol::setIntegrationTime(sBuff,integration); // get the buffer
		if ((res=sendBuffer(sBuff,len))==SUCCESS) {
			res=receiveBuffer(rBuff,RECBUFFERSIZE);
		}
		if (res>0) { // operation was ok.
			if (!CProtocol::isAck(rBuff)) {
					_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getZeroTPI()");
			} 
		}
		else if (res==FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
			dummy.setCode(m_Error.getErrorCode());
			dummy.setDescription((const char*)m_Error.getDescription());
			m_Error.Reset();
			_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getZeroTPI()");
		}
		else if (res==WOULDBLOCK) {
			_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getZeroTPI()");
		}
		else {
			_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
		}
		m_setTpiIntegration=false;
		waitTime=1000000+integration*2000; // //wait time in microseconds...we should wait 2 seconds plus twice the integration time (which is given in milliseconds).
	}
	//  connect the backend to the 50ohm
	len=CProtocol::setZeroInput(sBuff,true); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getZeroTPI()");
		} 
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getZeroTPI()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getZeroTPI()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
	}
	waitTime+=integration*2000;
	if (waitTime>0) IRA::CIRATools::Wait(waitTime); 
	// now read the total power
	len=CProtocol::getSample(sBuff); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::decodeData(rBuff,tpi,m_configuration->getBoardsNumber(),m_sectionsNumber,m_boards)) {
			_THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"CCommandLine::getZeroTPI()");
		}
		for (int j=0;j<m_sectionsNumber;j++) {
			m_tpiZero[j]=(double)tpi[j]/(double)integration;
		}
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getZeroTPI()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getZeroTPI()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
	}
	// now return to the default attenuation level
	len=CProtocol::setZeroInput(sBuff,false); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
			_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getZeroTPI()");
		} 
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getZeroTPI()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getZeroTPI()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getZeroTPI()");
	}
}

void CCommandLine::getTpi(ACS::doubleSeq& tpi) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	getSample(tpi,false);
}

void CCommandLine::getZero(ACS::doubleSeq& tpi) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	getSample(tpi,true);
}

void CCommandLine::getSample(ACS::doubleSeq& tpi,bool zero) throw (ComponentErrors::TimeoutExImpl,
		BackendsErrors::ConnectionExImpl,ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::getSample()");
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	long integration;
	bool busy=getIsBusy();
	long waitTime=0;
	
	if (busy) { // if the backend is sampling data 
		if (zero) {   // if the zero tpi....i can only return the last available measure...since i cannot change the configuration and switch to 50Ohm
			tpi.length(m_sectionsNumber);
			for (int j=0;j<m_sectionsNumber;j++) {
				tpi[j]=m_tpiZero[j];  //it should already be normalized to 1 millisec of integration
			}
			return;
		}
	}
	integration=(long)round(1.0/(m_commonSampleRate*1000.0));
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
	}
	res=getConfiguration(); // refresh the m_currentSampleRate..........
	if (res>0) { // load OK
		// do nothing
	}
	_CHECK_ERRORS("CommandLine::getSample()");
	//if the requested integration is not equal to the current samplerate (given is milliseconds as a sample period)
	// or the integration time must be forced...and the backend is not busy. Then set the correct sample rate....
	if (((integration!=m_currentSampleRate) || (m_setTpiIntegration)) && !busy) {
		len=CProtocol::setIntegrationTime(sBuff,integration); // get the buffer
		if ((res=sendBuffer(sBuff,len))==SUCCESS) {
			res=receiveBuffer(rBuff,RECBUFFERSIZE);
		}
		if (res>0) { // operation was ok.
			if (!CProtocol::isAck(rBuff)) {
				_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getSample()");
			} 
		}
		else if (res==FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
			dummy.setCode(m_Error.getErrorCode());
			dummy.setDescription((const char*)m_Error.getDescription());
			m_Error.Reset();
			_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getSample()");
		}
		else if (res==WOULDBLOCK) {
			_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getSample()");
		}
		else {
			_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
		}
		m_setTpiIntegration=false;
		waitTime=1000000+integration*2000; // //wait time in microseconds...we should wait 2 seconds plus twice the integration time (which is given in milliseconds).
	}
	// if the Tpizero has been requested...than connect the backend to the 50ohm
	if (zero) {
		len=CProtocol::setZeroInput(sBuff,true); // get the buffer
		if ((res=sendBuffer(sBuff,len))==SUCCESS) {
			res=receiveBuffer(rBuff,RECBUFFERSIZE);
		}
		if (res>0) { // operation was ok.
			if (!CProtocol::isAck(rBuff)) {
				_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getSample()");
			} 
		}
		else if (res==FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
			dummy.setCode(m_Error.getErrorCode());
			dummy.setDescription((const char*)m_Error.getDescription());
			m_Error.Reset();
			_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getSample()");
		}
		else if (res==WOULDBLOCK) {
			_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getSample()");
		}
		else {
			_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
		}
		waitTime+=integration*2000;
	}
	if (waitTime>0) IRA::CIRATools::Wait(waitTime); 
	// now read the total power
	len=CProtocol::getSample(sBuff); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		DWORD data[MAX_SECTION_NUMBER];
		if (!CProtocol::decodeData(rBuff,data,m_configuration->getBoardsNumber(),m_sectionsNumber,m_boards)) {
			_THROW_EXCPT(BackendsErrors::MalformedAnswerExImpl,"CCommandLine::getSample()");
		}
		tpi.length(m_sectionsNumber);
		for (int j=0;j<m_sectionsNumber;j++) {
			tpi[j]=(double)data[j]/(double)integration;
			if (zero) m_tpiZero[j]=tpi[j]; // in case of tpiZero we store it......
		}
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getSample()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getSample()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
	}
	// now return to the default attenuation level
	if (zero) {
		len=CProtocol::setZeroInput(sBuff,false); // get the buffer
		if ((res=sendBuffer(sBuff,len))==SUCCESS) {
			res=receiveBuffer(rBuff,RECBUFFERSIZE);
		}
		if (res>0) { // operation was ok.
			if (!CProtocol::isAck(rBuff)) {
				_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::getSample()");
			} 
		}
		else if (res==FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
			dummy.setCode(m_Error.getErrorCode());
			dummy.setDescription((const char*)m_Error.getDescription());
			m_Error.Reset();
			_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::getSample()");
		}
		else if (res==WOULDBLOCK) {
			_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::getSample()");
		}
		else {
			_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getSample()");
		}
	}
}

void CCommandLine::setDefaultConfiguration() throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl)
{
	AUTO_TRACE("CCommandLine::setDefaultConfiguration()");
	int res;
	WORD len;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	// I do not check for backend busy because this is a call done at the initialization and never repeated
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setDefaultConfiguration()");
	}
	if (m_defaultInputSize==1) {
		len=CProtocol::setConfiguration_broadcast(sBuff,m_defaultInput[0],m_attenuation[0],m_bandWidth[0]); // get the buffer
		if ((res=sendBuffer(sBuff,len))==SUCCESS) {
			res=receiveBuffer(rBuff,RECBUFFERSIZE);
		}
		if (res>0) { // operation was ok.
			if (!CProtocol::isAck(rBuff)) {
				_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::setDefaultConfiguration()");
			}
		}
		else if (res==FAIL) {
			_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
			dummy.setCode(m_Error.getErrorCode());
			dummy.setDescription((const char*)m_Error.getDescription());
			m_Error.Reset();
			_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::setDefaultConfiguration()");
		}
		else if (res==WOULDBLOCK) {
			_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::setDefaultConfiguration()");
		}
		else {
			_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setDefaultConfiguration()");
		}
	}
	else {
		for (DWORD k=0;k<m_defaultInputSize;k++) {
			if (m_sections[k]==-1) {
				//***************************************************************************
				// This should be changed in order to read the Attenuation on board basis and to apply proper db for each board
				//****************************************************************************
				len=CProtocol::setConfiguration(sBuff,(long)k,m_defaultInput[k],m_defaultInputDB[k],m_defaultInputBW[k]);
			}
			else {
				len=CProtocol::setConfiguration(sBuff,(long)k,m_defaultInput[k],m_attenuation[m_sections[k]],m_bandWidth[m_sections[k]]);
			}
			if ((res=sendBuffer(sBuff,len))==SUCCESS) {
				res=receiveBuffer(rBuff,RECBUFFERSIZE);
			}
			if (res>0) { // operation was ok.
				if (!CProtocol::isAck(rBuff)) {
					_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::setDefaultConfiguration()");
				}
			}
			else if (res==FAIL) {
				_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
				dummy.setCode(m_Error.getErrorCode());
				dummy.setDescription((const char*)m_Error.getDescription());
				m_Error.Reset();
				_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::setDefaultConfiguration()");
			}
			else if (res==WOULDBLOCK) {
				_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::setDefaultConfiguration()");
			}
			else {
				_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setDefaultConfiguration()");
			}
		}
	}
	IRA::CIRATools::Wait(0,200000);
	long integration=(long)round(1.0/ (m_commonSampleRate*1000.0));
	len=CProtocol::setIntegrationTime(sBuff,integration); // get the buffer
	if ((res=sendBuffer(sBuff,len))==SUCCESS) {
		res=receiveBuffer(rBuff,RECBUFFERSIZE);
	}
	if (res>0) { // operation was ok.
		if (!CProtocol::isAck(rBuff)) {
				_THROW_EXCPT(BackendsErrors::NakExImpl,"CCommandLine::setDefaultConfiguration()");
		} 
	}
	else if (res==FAIL) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(ComponentErrors::SocketErrorExImpl,dummy,"CCommandLine::setDefaultConfiguration()");
	}
	else if (res==WOULDBLOCK) {
		_THROW_EXCPT(ComponentErrors::TimeoutExImpl,"CCommandLine::setDefaultConfiguration()");
	}
	else {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::setDefaultConfiguration()");
	}
	ACS_LOG(LM_FULL_INFO,"CCommandLine::setDefaultConfiguration()",(LM_INFO,"DEFAULTS_ARE_SET"));
	m_setTpiIntegration=false;	
	
}

void CCommandLine::setup(const char *conf) throw (BackendsErrors::BackendBusyExImpl,BackendsErrors::ConfigurationErrorExImpl,ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,ComponentErrors::CDBAccessExImpl)
{
	AUTO_TRACE("CCommandLine::setup()");
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setup()");
		throw impl;
	}
	if (!initializeConfiguration(conf)) {
		_EXCPT(BackendsErrors::ConfigurationErrorExImpl,impl,"CCommandLine::setup()");
		throw impl;
	}
	setDefaultConfiguration(); //could throw exceptions........
	ACS_LOG(LM_FULL_INFO,"CCommandLine::setup()",(LM_NOTICE,"BACKEND_INITIALIZED: %s",conf)); 
}

void CCommandLine::checkTime() throw (BackendsErrors::ConnectionExImpl,BackendsErrors::MalformedAnswerExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl)
{
	char sBuff[SENDBUFFERSIZE];
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
	}
}

void CCommandLine::setTime()  throw (ComponentErrors::TimeoutExImpl,BackendsErrors::ConnectionExImpl,
		ComponentErrors::SocketErrorExImpl,BackendsErrors::NakExImpl,BackendsErrors::MalformedAnswerExImpl,
		BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::setTime()");
	int res;
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
	len=CProtocol::setBackendTime(sBuff); // get the buffer
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
}

void CCommandLine::activateCalSwitching(const long& interleave) throw (BackendsErrors::BackendBusyExImpl,ComponentErrors::NotAllowedExImpl)
{
	AUTO_TRACE("CCommandLine::activateCalSwitching()");
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::activateCalSwitching()");
		throw impl;
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
	}
}

void CCommandLine::setEnabled(const ACS::longSeq& en) throw (BackendsErrors::BackendBusyExImpl)
{
	int bound;
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
	}	
	ACS_LOG(LM_FULL_INFO,"CCommandLine::setEnabled()",(LM_NOTICE,"CHANGED_ENABLED_CHANNEL"));
}

void CCommandLine::setIntegration(const long& integration)  throw (BackendsErrors::BackendBusyExImpl)
{
	AUTO_TRACE("CCommandLine::setIntegration()");
	if (getIsBusy()) {
		_EXCPT(BackendsErrors::BackendBusyExImpl,impl,"CCommandLine::setIntegration()");
		throw impl;
	}
	if (integration>=0) {
		long result;
		bool out;
		out=resultingSampleRate(integration,m_commonSampleRate,result);
		m_integration=result;
		if (!out) {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::setIntegration()",(LM_NOTICE,"INTEGRATION is now %ld (millisec)",m_integration));
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CCommandLine::setIntegration()",(LM_WARNING,"INTEGRATION is rounded to %ld (millisec)",m_integration));
		}
	}
}

void CCommandLine::getAttenuation(ACS::doubleSeq& att) throw (ComponentErrors::SocketErrorExImpl,
		ComponentErrors::TimeoutExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::getAttenuation()");
	int res;
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getAttenuation()");
	}
	res=getConfiguration();
	if (res>0) { // load OK
		att.length(m_sectionsNumber);
		for (int i=0;i<m_sectionsNumber;i++) {
			att[i]=m_attenuation[i];
		}
	}
	_CHECK_ERRORS("CommandLine::getAttenuation()");
}

void CCommandLine::getFrequency(ACS::doubleSeq& freq) const
{
	freq.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		freq[i]=m_frequency[i];
	}
}

void CCommandLine::getBackendStatus(DWORD& status) const
{
	status=m_backendStatus;
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
	tsys.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
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
	pol.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		pol[i]=(long)m_polarization[i];
	}
}

void CCommandLine::getFeed(ACS::longSeq& feed) const 
{
	feed.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		feed[i]=m_feedNumber[i];
	}
}

void CCommandLine::getIFs(ACS::longSeq& ifs) const
{
	ifs.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		ifs[i]=m_ifNumber[i];
	}	
}

void CCommandLine::getInputSection(ACS::longSeq& inpSection) const
{
	inpSection.length(m_sectionsNumber);
	for (int i=0;i<m_sectionsNumber;i++) {
		inpSection[i]=m_inputSection[i];
	}
}

void CCommandLine::getBandWidth(ACS::doubleSeq& bw) throw (ComponentErrors::SocketErrorExImpl,
		ComponentErrors::TimeoutExImpl,BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::getBandWidth()");
	int res;
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getBandWidth()");
	}
	res=getConfiguration();
	if (res>0) { // load OK
		bw.length(m_sectionsNumber);
		for (int i=0;i<m_sectionsNumber;i++) {
			bw[i]=m_bandWidth[i];
		}
	}
	_CHECK_ERRORS("CommandLine::getBandWidth()");
}

void CCommandLine::getTime(ACS::Time& tt) throw (ComponentErrors::SocketErrorExImpl,ComponentErrors::TimeoutExImpl,
		BackendsErrors::MalformedAnswerExImpl,BackendsErrors::ConnectionExImpl)
{
	AUTO_TRACE("CCommandLine::getTime()");
	int res;
	if (!checkConnection()) {
		_THROW_EXCPT(BackendsErrors::ConnectionExImpl,"CCommandLine::getTime()");
	}
	res=getConfiguration();
	if (res>0) { // load OK
		tt=m_backendTime.value().value;
	}
	_CHECK_ERRORS("CommandLine::getTime()");
}

void CCommandLine::fillMainHeader(Backends::TMainHeader& bkd)
{
	long chs=0;
	long intTime;
	// count the available channels.......
	for(int i=0;i<m_sectionsNumber;i++) {
		if (m_enabled[i]) chs++;
	}
	bkd.sections=chs;
	bkd.beams=m_beams;
	resultingSampleRate(m_integration,m_commonSampleRate,intTime);
	bkd.integration=intTime;
	//bkd.integration=m_integration;
	bkd.sampleSize=m_sampleSize;
	bkd.noData=false;
}

void CCommandLine::fillChannelHeader(Backends::TSectionHeader *chHr,const long& size)
{
	long index=0;
	for (int i=0;i<m_sectionsNumber;i++) {
		if (m_enabled[i]) {
			if (index<size) {
				chHr[index].id=i;
				chHr[index].bins=m_bins[i];
				chHr[index].polarization=m_polarization[i];
				chHr[index].bandWidth=m_bandWidth[i];
				chHr[index].frequency=m_frequency[i];
				chHr[index].attenuation[0]=m_attenuation[i]; // we have always one inputs....so just the first position is significant
				chHr[index].attenuation[1]=0.0;  // not significant....placeholder				
				chHr[index].IF[0]=m_ifNumber[i];
				chHr[index].IF[1]=0;  // not significant
				chHr[index].sampleRate=/*m_sampleRate[i];*/m_commonSampleRate;
				chHr[index].feed=m_feedNumber[i];
				chHr[index].inputs=1;				
				index++;
			}
		}
	}
}

void CCommandLine::saveTsys(const ACS::doubleSeq& tsys,const ACS::doubleSeq& ratio)
{
	if (tsys.length()==(unsigned)m_sectionsNumber) {
		for (int i=0;i<m_sectionsNumber;i++) {
			m_tsys[i]=tsys[i];
		}
	}
	if (ratio.length()==(unsigned)m_sectionsNumber) {
		for (int i=0;i<m_sectionsNumber;i++) {
			m_KCratio[i]=ratio[i];
		}
		ACS_LOG(LM_FULL_INFO,"CCommandLine::saveTsys()",(LM_INFO,"KELVIN_COUNTS_CONVERSION_FACTOR_SET"));
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
					stopDataAcquisitionForced(); // ask the backend to exit the data transfering mode......
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

int CCommandLine::getConfiguration()
{
	TIMEVALUE Now;
	char sBuff[SENDBUFFERSIZE];
	char rBuff[RECBUFFERSIZE];
	WORD len;
	int rBytes;
	OperationResult Res;
	CIRATools::getTime(Now);
	if (CIRATools::timeDifference(m_lastUpdate,Now)>m_configuration->getPropertyRefreshTime()) { //check if we have to go to the backend!
		// prepare the buffer........
		len=CProtocol::askBackendConfiguration(sBuff);
		//send and wait for the answer...........
		if ((Res=sendBuffer(sBuff,len))==SUCCESS) {
			rBytes=receiveBuffer(rBuff,RECBUFFERSIZE);
			if (rBytes>0) {
				if (CProtocol::decodeBackendConfiguration(rBuff,m_sectionsNumber,m_configuration->getBoardsNumber(),m_attenuation,m_bandWidth,m_input,m_backendTime,m_currentSampleRate,m_boards)) {
					CIRATools::getTime(m_lastUpdate);
				}
				else {
					return DECODE_ERROR;
				}
			}
			// this could be 0 (communication fell down), FAIL error (m_Error set accordingly) , WOULDBLOCK timeout, >0 ok
			return rBytes;
		}
		else {  // send fails....m_Error already set by sendBuffer
			return Res;
		}
	}
	// the last value is still valid..nothing to do so return a value that is grater than zero
	return SENDBUFFERSIZE; 
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
						stopDataAcquisitionForced(); // ask the backend to exit the data transfering mode......
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
			m_defaultInput[k]=setup.inputPort[k];
			m_defaultInputDB[k]=setup.inputPortDB[k];
			m_defaultInputBW[k]=setup.inputPortBW[k];
			m_sections[k]=-1;
		}
		m_defaultInputSize=setup.inputPorts; // this should be 1 or the number of installed boards
		m_beams=setup.beams;
		m_calSwitchingEnabled=setup.calSwitchEnabled;
		for (i=0;i<m_sectionsNumber;i++) {
			m_boards[i]=setup.section_boards[i];
			m_sections[m_boards[i]]=i;
			m_input[i]=m_defaultInput[m_boards[i]];
			m_attenuation[i]=m_defaultInputDB[m_boards[i]];
			m_bandWidth[i]=m_defaultInputBW[m_boards[i]];
			m_polarization[i]=setup.polarizations[i];
			m_ifNumber[i]=setup.ifs[i];
			m_feedNumber[i]=setup.feed[i];
			m_inputSection[i]=i; // input 0 belongs to section 0 and so on.....
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
		m_bins[i]=BINSNUMBER;
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
