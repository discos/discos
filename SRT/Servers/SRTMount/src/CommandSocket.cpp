#include "CommandSocket.h"
#include <LogFilter.h>
#include <TimeoutSync.h>

using namespace IRA;
using namespace ComponentErrors;
using namespace AntennaErrors;

_IRA_LOGFILTER_IMPORT;

CCommandSocket::CCommandSocket()
{
	m_pConfiguration=NULL;
	m_pData=NULL;
	m_bTimedout=false;
}

CCommandSocket::~CCommandSocket()
{
}

void CCommandSocket::cleanUp()
{
	//before closing we have to make sure there is no one waiting for services of this object...so we acquire the mutex in blocking mode
	DDWORD timeo=0;
	IRA::CTimeoutSync guard(&m_syncMutex,timeo);
	guard.acquire();
	CError err;
	Close(err);
	guard.release();
	CSecAreaResourceWrapper<CCommonData> commonData=m_pData->Get();
	commonData->setControlLineState(Antenna::ACU_NOTCNTD);
}

void CCommandSocket::init(CConfiguration *config,IRA::CSecureArea<CCommonData> *data) throw (SocketErrorExImpl)
{
	m_pConfiguration=config;
	m_pData=data;
	CSecAreaResourceWrapper<CCommonData> commonData=m_pData->Get();
	commonData->setControlLineState(Antenna::ACU_NOTCNTD);
	connectSocket(); // throw (SocketErrorExImpl)
	m_ptSize=0;
	m_lastScanEpoch=0;
	m_currentScanStartEpoch=0;
}

void CCommandSocket::stop() throw (TimeoutExImpl,SocketErrorExImpl,ConnectionExImpl,AntennaBusyExImpl,NakExImpl)
{
	// Stop command does not need to check the connection or the busy flag, before
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::stop()");
		throw ex;
	}
	// I'm sure there is just one thread beyond this part of code.......
	if (checkIsBusy()) { // if it is busy a stow or unstow procedure has been started
		CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
		data->clearControlLineBusyState();
	}
	if (checkConnection()) {
		activate_command(); //make sure the axis are active.....
		stop_command();
	}
}

void CCommandSocket::activate() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::activate()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::activate()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::activate()");
		throw ex;
	}
	activate_command();
}

void CCommandSocket::deactivate() throw (TimeoutExImpl,SocketErrorExImpl,ConnectionExImpl,AntennaBusyExImpl,NakExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::deactivate()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::deactivate()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::deactivate()");
		throw ex;
	}
	deactivate_command();
}

void CCommandSocket::resetErrors() throw (TimeoutExImpl,SocketErrorExImpl,ConnectionExImpl,AntennaBusyExImpl,NakExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::resetErrors()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::resetErrors()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::resetErrors()");
		throw ex;
	}
	// I'm sure there is just one thread beyond this part of code....... 
	resetErrors_command();
}

void CCommandSocket::setOffsets(const double& azOff,const double& elOff) throw (TimeoutExImpl,SocketErrorExImpl,ConnectionExImpl,AntennaBusyExImpl,NakExImpl,OperationNotPermittedExImpl)
{
	Antenna::TCommonModes azMode,elMode;
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::setOffsets()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::setOffsets()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::setOffsets()");
		throw ex;
	}
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	data->getActualMode(azMode,elMode);
	if ((azMode==Antenna::ACU_PROGRAMTRACK) && (elMode==Antenna::ACU_PROGRAMTRACK)) {
	}
	else if ((azMode==Antenna::ACU_PRESET) && (elMode==Antenna::ACU_PRESET)) {
	}
	else {
		_EXCPT(AntennaErrors::OperationNotPermittedExImpl,impl,"CCommandSocket::setOffsets()");
		impl.setReason("Current mode does not need offsets");
		throw impl;
	}
	data.Release();
	setOffsets_command(azOff,elOff);	
}

void CCommandSocket::setTime(const ACS::Time& time) throw (TimeoutExImpl,SocketErrorExImpl,ConnectionExImpl,AntennaBusyExImpl,NakExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::setTime()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::setTime()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::setTime()");
		throw ex;
	}
	setTime_command(time);
}

void CCommandSocket::setTimeOffset(const double& seconds) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,
		AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::setTimeOffset()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::setTimeOffset()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::setTimeOffset()");
		throw ex;
	}
	setTimeOffset_command(seconds);	
}

void CCommandSocket::programTrack(const double& az,const double& el,const ACS::Time& time,bool clear) throw (ComponentErrors::ComponentErrorsExImpl,AntennaErrors::AntennaErrorsExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	double currentAz,dummy;
	double azOff,elOff;
	double finalAz,finalEl;
	long section;
	Antenna::TCommonModes azMode,elMode;
	try {
		if (!checkConnection()) {
			_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::programTrack()");
		}
		if (checkIsBusy()) {
			_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::programTrack()");
		}
		IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
		if (!guard.acquire()) {
			_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::programTrack()");
			throw ex;
		}
		CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
		data->getActualMode(azMode,elMode);
		if ((azMode!=Antenna::ACU_PROGRAMTRACK) || (elMode!=Antenna::ACU_PROGRAMTRACK)) {
			_EXCPT(AntennaErrors::OperationNotPermittedExImpl,impl,"CCommandSocket::programTrack()");
			impl.setReason("program track mode not configured");
			throw impl;
		}
		if (clear) {  //new scan!!!!
			m_ptSize=0;
			m_lastScanEpoch=0;
			data->clearProgramTrackStack();
		}
		data->getCommandedOffsets(azOff,elOff);
		if (data->isProgramTrackStackEmpty()) {
			currentAz=data->azimuthStatus()->actualPosition();   //if no position commanded yet, the current azimuth is the hardware azimuth of the telescope
			// the offsets is included in the position read from encoders
		}
		else {
			// at the moment this is not comprehensive of the offsets
			data->getLastProgramTrackPoint(currentAz,dummy); // else we refer to the last commanded azimuth
			currentAz+=azOff;
		}
		// this is my target position
		finalAz=az+azOff;
		finalEl=el+elOff;
		// transform the azimuth into a real azimuth..considering the wrap
		if (data->getCommandedSector()==Antenna::ACU_CCW) section=-1;
		else if (data->getCommandedSector()==Antenna::ACU_CW) section=1;
		else section=0;
		finalAz=IRA::CIRATools::getHWAzimuth(currentAz,finalAz,m_pConfiguration->azimuthLowerLimit(),m_pConfiguration->azimuthUpperLimit(),section,m_pConfiguration->cwLimit());
	
		// check that the position are inside the hardware limits
		if (finalAz<m_pConfiguration->azimuthLowerLimit()) {
			finalAz=m_pConfiguration->azimuthLowerLimit();
		}
		else if (finalAz>m_pConfiguration->azimuthUpperLimit()) {
		finalAz=m_pConfiguration->azimuthUpperLimit();
		}
		if (finalEl<m_pConfiguration->elevationLowerLimit()) {
			finalEl=m_pConfiguration->elevationLowerLimit();
		}
		else if (finalEl>m_pConfiguration->elevationUpperLimit()) {
			finalEl=m_pConfiguration->elevationUpperLimit();
		}
		// now get rid of the offsets
		finalAz-=azOff;
		finalEl-=elOff;
		data->setCommandedSector(Antenna::ACU_NEUTRAL);
		// add the point into the cache and to commanded point stack!
		m_ptTable[m_ptSize].azimuth=finalAz;
		m_ptTable[m_ptSize].elevation=finalEl;
		m_ptTable[m_ptSize].timeMark=time;
		data->addProgramTrackStackPoint(m_ptTable[m_ptSize].azimuth,m_ptTable[m_ptSize].elevation,m_ptTable[m_ptSize].timeMark); // add commanded points to stack
		m_ptSize++;	
		if (m_ptSize>=CACUProtocol::PROGRAMTRACK_TABLE_MINIMUM_LENGTH) { // the cache is full...time to send data points to the ACU
			if (m_lastScanEpoch==0) {
				m_currentScanStartEpoch=m_ptTable[0].timeMark;
				len=m_protocol.loadProgramTrack(m_currentScanStartEpoch,m_ptTable,m_ptSize,true,m_pConfiguration->azimuthRateUpperLimit(),m_pConfiguration->elevationRateLowerLimit(),message,command,commNum);
			}
			else {
				len=m_protocol.loadProgramTrack(m_currentScanStartEpoch,m_ptTable,m_ptSize,false,m_pConfiguration->azimuthRateUpperLimit(),m_pConfiguration->elevationRateLowerLimit(),message,command,commNum);
			}
			m_ptSize=0; // clear the cache
			if (len==0) {
				// ERRORE
			}
			else {
				sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
				data->storeLastCommand(command,commNum);
				IRA::CIRATools::getTime(now);
			}
			data.Release(); // this is fundamental in order to avoid starvation of other thread......
			// waits for the ACU to respond....or eventually raise a timeout
			waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
			m_lastScanEpoch=now.value().value;
		}
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) { // in case of error we need to start the tracking from the scatch in order to avoid different time gaps
		//clear
		m_ptSize=0;
		m_lastScanEpoch=0;
		CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
		data->clearProgramTrackStack();
		throw ex; // let the exception go up....
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		//clear
		m_ptSize=0;
		m_lastScanEpoch=0;
		CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
		data->clearProgramTrackStack();
		throw ex; // let the exception go up....
	}
}

void CCommandSocket::changeMode(const Antenna::TCommonModes& azMode,const Antenna::TCommonModes& elMode) throw (TimeoutExImpl,SocketErrorExImpl,ConnectionExImpl,AntennaBusyExImpl,
		NakExImpl,ValidationErrorExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::changeMode()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::changeMode()");
	}
	// check that the commanded mode is accepted for both azimuth and elevation
	if ((azMode!=Antenna::ACU_PRESET) && (azMode!=Antenna::ACU_PROGRAMTRACK) && (azMode!=Antenna::ACU_RATE)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandSocket::changeMode()");
		impl.setReason("azimuth mode is not allowed");
		throw impl;
	}
	if ((elMode!=Antenna::ACU_PRESET) && (elMode!=Antenna::ACU_PROGRAMTRACK) && (elMode!=Antenna::ACU_RATE)) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandSocket::changeMode()");
		impl.setReason("elevation mode is not allowed");
		throw impl;
	}	
	if (elMode!=azMode) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandSocket::changeMode()");
		impl.setReason("azimuth and elevation modes are different");
		throw impl;		
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::changeMode()");
		throw ex;
	} 
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (data->checkIsStowed()) {
		_EXCPT(ComponentErrors::ValidationErrorExImpl,impl,"CCommandSocket::changeMode()");
		impl.setReason("please, unstow the telescope first");
		throw impl;
	}
	data.Release();
	activate_command(); //make sure the axis are active.....
	changeMode_command(azMode);
	setOffsets_command(0.0,0.0); // reset the commanded offsets
}

void CCommandSocket::stow() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::stow()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::stow()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::stow()");
		throw ex;
	}
	activate_command(); //make sure the axis are active.....
	stow_command();
}

void CCommandSocket::unstow() throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::ConnectionExImpl,AntennaErrors::AntennaBusyExImpl,AntennaErrors::NakExImpl)
{
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::unstow()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::unstow()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::unstow()");
		throw ex;
	}
	//activate_command(); //make sure the axis are active.....
	unstow_command();	
}

void CCommandSocket::preset(const double& az,const double& el) throw (TimeoutExImpl,SocketErrorExImpl,ConnectionExImpl,AntennaBusyExImpl,NakExImpl,OperationNotPermittedExImpl)
{
	double newAz,newEl;
	double azOff,elOff;
	double finalEl,finalAz;
	double currentAz;
	long section;
	Antenna::TCommonModes azMode,elMode;
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::preset()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::preset()");
	}
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::preset()");
		throw ex;
	} 
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	data->getActualMode(azMode,elMode);
	if ((azMode!=Antenna::ACU_PRESET) || (elMode!=Antenna::ACU_PRESET)) {  // check that the configured mode is preset
		_EXCPT(AntennaErrors::OperationNotPermittedExImpl,impl,"CCommandSocket::preset()");
		impl.setReason("preset mode not configured");
		throw impl;
	}
	data->getCommandedOffsets(azOff,elOff); // computes the effective position...included the offsets
	newAz=az+azOff; newEl=el+elOff;
	currentAz=data->azimuthStatus()->actualPosition(); 
	if (data->getCommandedSector()==Antenna::ACU_CCW) section=-1;
	else if (data->getCommandedSector()==Antenna::ACU_CW) section=1;
	else section=0;
	finalAz=IRA::CIRATools::getHWAzimuth(currentAz,newAz,m_pConfiguration->azimuthLowerLimit(),m_pConfiguration->azimuthUpperLimit(),section,m_pConfiguration->cwLimit());
	finalEl=newEl;
	data->setCommandedSector(Antenna::ACU_NEUTRAL); // put again the requested sector to neutral....
	// check that the position are inside the hardware limits
	if (finalAz<m_pConfiguration->azimuthLowerLimit()) {
		finalAz=m_pConfiguration->azimuthLowerLimit();
	}
	else if (finalAz>m_pConfiguration->azimuthUpperLimit()) {
		finalAz=m_pConfiguration->azimuthUpperLimit();
	}
	if (finalEl<m_pConfiguration->elevationLowerLimit()) {
		finalEl=m_pConfiguration->elevationLowerLimit();
	}
	else if (finalEl>m_pConfiguration->elevationUpperLimit()) {
		finalEl=m_pConfiguration->elevationUpperLimit();
	}
	finalAz-=azOff; finalEl-=elOff;
	data.Release();
	preset_command(finalAz,finalEl);
}

void CCommandSocket::rates(const double& azRate,const double& elRate) throw (TimeoutExImpl,SocketErrorExImpl,ConnectionExImpl,AntennaBusyExImpl,NakExImpl,OperationNotPermittedExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	Antenna::TCommonModes azMode,elMode;
	double az,el;
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CCommandSocket::rates()");
	}
	if (checkIsBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CCommandSocket::rates()");
	}
	if (azRate<m_pConfiguration->azimuthRateLowerLimit()) az=m_pConfiguration->azimuthRateLowerLimit();
	else if (azRate>m_pConfiguration->azimuthRateUpperLimit()) az=m_pConfiguration->azimuthRateUpperLimit();
	else az=azRate;
	if (elRate<m_pConfiguration->elevationRateLowerLimit()) el=m_pConfiguration->elevationRateLowerLimit();
	else if (elRate>m_pConfiguration->elevationRateUpperLimit()) el=m_pConfiguration->elevationRateUpperLimit();
	else el=elRate;
	IRA::CTimeoutSync guard(&m_syncMutex,m_pConfiguration->controlSocketResponseTime(),m_pConfiguration->controlSocketDutyCycle());
	if (!guard.acquire()) {
		_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::rates()");
		throw ex;
	}
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	data->getActualMode(azMode,elMode);
	if ((azMode!=Antenna::ACU_RATE) || (elMode!=Antenna::ACU_RATE)) {
		_EXCPT(AntennaErrors::OperationNotPermittedExImpl,impl,"CCommandSocket::rates()");
		impl.setReason("rate mode not configured");
		throw impl;
	}
	len=m_protocol.rate(az,el,message,command,commNum);
	if (len==0) {
		// ERRORE
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is fundamental in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::rates()",(LM_NOTICE,"RATES_COMMANDED %lf,%lf degrees/sec",az,el));
}

void CCommandSocket::forceSector(const Antenna::TSections& section)
{
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	data->setCommandedSector(section);
}

bool CCommandSocket::checkIsUnstowed()
{
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	return data->checkIsUnstowed();
}

ACS::TimeInterval CCommandSocket::checkIsUnstowed(const ACS::Time& startTime,bool& timeout,bool& stopped,bool& unstowed)
{
	TIMEVALUE now;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	IRA::CIRATools::getTime(now);
	timeout=stopped=unstowed=false;
	ACS::TimeInterval remainT=startTime+m_pConfiguration->unstowTimeout()*10-now.value().value; // compute the remaining time before timeout
	if  (data->checkIsUnstowed()) {
		data->clearControlLineBusyState();
		ACS_LOG(LM_FULL_INFO,"CCommandSocket::checkIsUnstowed()",(LM_NOTICE,"ANTENNA_UNSTOWED"));
		data.Release();
		unstowed=true;
		return remainT;
	}
	else {
		if (!data->isBusy()) {
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::checkIsUnstowed()",(LM_NOTICE,"UNSTOW_STOPPED_BY_USER"));
			stopped=true;
		}
		else if (remainT<0) {
			data->clearControlLineBusyState();
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::checkIsUnstowed()",(LM_NOTICE,"UNSTOW_TIMEOUT"));
			timeout=true;
		}
		return remainT;
	}
}

ACS::TimeInterval CCommandSocket::checkIsStowed(const ACS::Time& startTime,bool& timeout,bool& stopped,bool& stowed)
{
	TIMEVALUE now;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	IRA::CIRATools::getTime(now);
	timeout=stopped=stowed=false;
	ACS::TimeInterval remainT=startTime+m_pConfiguration->stowTimeout()*10-now.value().value; // compute the remaining time before timeout
	if  (data->checkIsStowed()) {
		data->clearControlLineBusyState();
		ACS_LOG(LM_FULL_INFO,"CCommandSocket::checkIsStowed()",(LM_NOTICE,"ANTENNA_STOWED"));
		stowed=true;
		return remainT;
	}
	else {
		if (!data->isBusy()) {
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::checkIsStowed()",(LM_NOTICE,"UNSTOW_STOPPED_BY_USER"));
			stopped=true;
		}
		else if (remainT<0) {
			data->clearControlLineBusyState();
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::checkIsStowed()",(LM_NOTICE,"STOW_TIMEOUT"));
			timeout=true;
		}
		return remainT;
	}
}

void CCommandSocket::connectSocket() throw (SocketErrorExImpl)
{
	IRA::CError error;	
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (data->getControlLineState()==Antenna::ACU_NOTCNTD) {
		ACS_LOG(LM_FULL_INFO,"CCommandSocket::connectSocket()",(LM_NOTICE,"COMMAND_SOCKET_CONNECTING"));	
		createSocket(); // throw (SocketErrorExImpl)
		CSocket::OperationResult res=Connect(error,m_pConfiguration->ipAddress(),m_pConfiguration->commandPort());
		if (res==FAIL) {
			_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
			dummy.setCode(error.getErrorCode());
			dummy.setDescription((const char*)error.getDescription());
			data->setControlLineState(Antenna::ACU_NOTCNTD);
			_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CStatusSocket::connectSocket()");
		}	
		else if (res==WOULDBLOCK) {
			data->setControlLineState(Antenna::ACU_CNTDING);
		}
		else {
			data->setControlLineState(Antenna::ACU_CNTD);
		}
	}
}

// ********************* PROTECTED  **************************************/

void CCommandSocket::onConnect(int ErrorCode)
{
	CError Tmp;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (data->getControlLineState()==Antenna::ACU_CNTDING) {
		if (ErrorCode==0) {
			if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) {
				if (data->isBusy()) data->setControlLineState(Antenna::ACU_BSY);
				else data->setControlLineState(Antenna::ACU_CNTD);
				ACS_LOG(LM_FULL_INFO,"CCommandSocket::onConnect()",(LM_NOTICE,"COMMAND_SOCKET_CONNECTED"));			
			}
		}
		else {
			data->setControlLineState(Antenna::ACU_NOTCNTD);
			ACS_DEBUG_PARAM("CStatusSocket::onConnect()","Reconnection failed, exit code is %d",ErrorCode);
			//ACS_LOG(LM_FULL_INFO,"CStatusSocket::onConnect()",(LM_NOTICE,"Reconnection failed, exit code is %d",ErrorCode));
		}
	}
}

// ********************* PRIVATE **************************************/

void CCommandSocket::stow_command() throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	len=m_protocol.stow(m_pConfiguration->elevationRateUpperLimit(),message,command,commNum);  // get the buffer to be sent!
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (len==0) {
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is fundamental in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	CSecAreaResourceWrapper<CCommonData> dataAgain=m_pData->Get();
	dataAgain->setControlLineState(Antenna::ACU_BSY);
	ACS_LOG(LM_FULL_INFO,"CStatusSocket::stow()",(LM_NOTICE,"SEND_ANTENNA_TO_STOW"));
}

void CCommandSocket::unstow_command() throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	len=m_protocol.unstow(message,command,commNum);  // get the buffer to be sent!
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (len==0) {
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is fundamental in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	CSecAreaResourceWrapper<CCommonData> dataAgain=m_pData->Get();
	dataAgain->setControlLineState(Antenna::ACU_BSY);
	ACS_LOG(LM_FULL_INFO,"CStatusSocket::stow()",(LM_NOTICE,"EXTRACTING_ANTENNA_STOW_PINS"));
}

void CCommandSocket::deactivate_command() throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	bool deactiveAz=false,deactiveEl=false;
	WORD commNum;
	WORD len;
	TIMEVALUE now;	
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if ((data->azimuthStatus()->axisState()==CACUProtocol::STATE_ACTIVE)) {
		deactiveAz=true;
	}
	if ((data->elevationStatus()->axisState()==CACUProtocol::STATE_ACTIVE)) {
		deactiveEl=true;
	}
	len=m_protocol.deactivate(deactiveAz,deactiveEl,message,command,commNum);  // get the buffer to be sent!
	if (len==0) {
		// ERRORE
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is fundamental in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
}

void CCommandSocket::activate_command() throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	bool activeAz,activeEl;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	Antenna::TCommonModes azMode,elMode;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	data->getActualMode(azMode,elMode);
	if ((data->azimuthStatus()->axisState()!=CACUProtocol::STATE_ACTIVE) && (data->azimuthStatus()->axisState()!=CACUProtocol::STATE_ACTIVATING)) {
		activeAz=true;
	}
	else {
		activeAz=false;
	}
	/*else {
		activeAz=((azMode==Antenna::ACU_STANDBY) || (azMode==Antenna::ACU_UNKNOWN));
	}*/
	if ((data->elevationStatus()->axisState()!=CACUProtocol::STATE_ACTIVE) && (data->elevationStatus()->axisState()!=CACUProtocol::STATE_ACTIVATING) ) {
		activeEl=true;
	}
	else {
		activeEl=false;
	}
	/*else {
		activeEl=((elMode==Antenna::ACU_STANDBY) || (elMode==Antenna::ACU_UNKNOWN));
	}*/
	if (activeAz || activeEl) {
		len=m_protocol.activate(activeAz,activeEl,message,command,commNum);
	}
	else {
		return; // nothing to do
	}
	if (len==0) {
		// ERRORE
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is fundamental in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	IRA::CIRATools::Wait(6,500000); // wait for the real completion of the activate operation
	ACS_LOG(LM_FULL_INFO,"CStatusSocket::activate_command()",(LM_NOTICE,"AXIS_ARE_NOW_ACTIVE"));
}

void CCommandSocket::changeMode_command(const Antenna::TCommonModes& mode) throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len=0;
	TIMEVALUE now;
	double az,el;
	IRA::CString modeName;	
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (mode==Antenna::ACU_PRESET) {
		az=data->azimuthStatus()->actualPosition();   //set the current values for az and el.
		el=data->elevationStatus()->actualPosition();
		len=m_protocol.preset(az,el,0.0001,0.0001,message,command,commNum);  // get the buffer to be sent!     
		modeName=CACUProtocol::modesIDL2String(Antenna::ACU_PRESET);
	}
	else if (mode==Antenna::ACU_RATE) {
		len=m_protocol.rate(0.0,0.0,message,command,commNum);
		modeName=CACUProtocol::modesIDL2String(Antenna::ACU_RATE);
	}
	else if (mode==Antenna::ACU_PROGRAMTRACK){ // programTrack
		len=m_protocol.programTrack(m_pConfiguration->azimuthRateUpperLimit(),m_pConfiguration->elevationRateUpperLimit(),message,command,commNum);
		modeName=CACUProtocol::modesIDL2String(Antenna::ACU_PROGRAMTRACK);
	}
	else if (mode==Antenna::ACU_STOP) {
		
	}
	else if (mode==Antenna::ACU_UNSTOW) {
		
	}
	else if (mode==Antenna::ACU_STOW) {
		len=m_protocol.stow(m_pConfiguration->elevationRateUpperLimit(),message,command,commNum);
		modeName=CACUProtocol::modesIDL2String(Antenna::ACU_STOW);
	}
	if (len==0) {
		// ERRORE
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is crucial in order to avoid starvation of other thread......
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	CSecAreaResourceWrapper<CCommonData> dataAgain=m_pData->Get();
	dataAgain->setCommandedMode(mode,mode);
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::changeMode_command()",(LM_NOTICE,"NEW_ACU_MODE %s",(const char *)modeName));
}

void CCommandSocket::setOffsets_command(const double& azOff,const double& elOff) throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	len=m_protocol.positionOffsets(azOff,elOff,message,command,commNum);
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (len==0) {
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is fundamental in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	CSecAreaResourceWrapper<CCommonData> dataAgain=m_pData->Get();
	dataAgain->setCommandedOffsets(azOff,elOff);
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::setOffsets_command()",(LM_NOTICE,"NEW_POSITION_OFFSETS %lf,%lf",azOff,elOff));	
}

void CCommandSocket::preset_command(const double& az,const double& el) throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	len=m_protocol.preset(az,el,m_pConfiguration->azimuthRateUpperLimit(),m_pConfiguration->elevationRateUpperLimit(),message,command,commNum);  // get the buffer to be sent!
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	if (len==0) {
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is fundamental in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
}

void CCommandSocket::stop_command() throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	len=m_protocol.stop(message,command,commNum);
	if (len==0) {
		// ERRORE
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is crucial in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	CSecAreaResourceWrapper<CCommonData> dataAgain=m_pData->Get();
	dataAgain->setCommandedMode(Antenna::ACU_STOP,Antenna::ACU_STOP);
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::stop_command()",(LM_NOTICE,"MOUNT_STOPPED"));
}

void CCommandSocket::resetErrors_command() throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	WORD len;
	TIMEVALUE now;
	//CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	len=m_protocol.resetErrors(message);
	if (len==0) {
		// ERRORE
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		//data->storeLastCommand(command,commNum);
		//IRA::CIRATools::getTime(now);
	}
	// This command does not expect an answer from the ACU, so we do not wait
	//data.Release(); // this is crucial in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	//waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::resetErrors_command()",(LM_NOTICE,"MOUNT_ERRORS_ACKNOWLEGDED"));
}

void CCommandSocket::setTime_command(const ACS::Time& time) throw (TimeoutExImpl,SocketErrorExImpl,NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	IRA::CString timeSource=m_pConfiguration->getTimeSource();
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	len=m_protocol.setTime(time,timeSource,message,command,commNum);
	if (len==0) {
		// ERRORE
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is crucial in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::setTime_command()",(LM_NOTICE,"MOUNT_TIME_SYNCHRONIZED"));	
}

void CCommandSocket::setTimeOffset_command(const double& seconds) throw (ComponentErrors::TimeoutExImpl,ComponentErrors::SocketErrorExImpl,AntennaErrors::NakExImpl)
{
	autoArray<BYTE> message; //it will also free the buffer......
	autoArray<CACUProtocol::TCommand> command;
	WORD commNum;
	WORD len;
	TIMEVALUE now;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	len=m_protocol.setTimeOffset(seconds,message,command,commNum);
	if (len==0) {
		// ERRORE
	}
	else {
		sendCommand(message,len); // throw TimeoutExImpl,SocketErrorExImpl;
		data->storeLastCommand(command,commNum);
		IRA::CIRATools::getTime(now);
	}
	data.Release(); // this is crucial in order to avoid starvation of other thread......
	// waits for the ACU to respond....or eventually raise a timeout
	waitAck(now.value().value); // throw NakExImpl and TimeoutExImpl
	ACS_LOG(LM_FULL_INFO,"CCommandSocket::setTimeOffset_command()",(LM_NOTICE,"DELTA_TIME_SECONDS %lf",seconds));		
}

void CCommandSocket::waitAck(const ACS::Time& commandTime) throw (AntennaErrors::NakExImpl,ComponentErrors::TimeoutExImpl)
{
	TIMEVALUE now;
	ACS::TimeInterval timeout=m_pConfiguration->controlSocketResponseTime()*10;
	CACUProtocol::TCommandAnswers answer;
	bool error=false;
	IRA::CIRATools::getTime(now);
	while (now.value().value<commandTime+timeout) {
		CSecAreaResourceWrapper<CCommonData> data=m_pData->Get(); // take the lock
		if (data->checkLastCommand(answer,error)) { // we have an answer
			if (error) { // problems.......
				printf("Command Answer Error %d, %s\n",answer,(const char *)CACUProtocol::commandAnswer2String(answer));
				_EXCPT(AntennaErrors::NakExImpl,ex,"CCommandSocket::waitAck()");
				ex.setCode((long)answer);
				ex.setMessage((const char *)CACUProtocol::commandAnswer2String(answer));
				data->setStatusWord(CCommonData::REMOTE_COMMAND_ERROR);
				throw ex;
			}
			else {  //success
				data->clearStatusWord(CCommonData::REMOTE_COMMAND_ERROR);
				return;
			}
		}
		else { // wait a little bit before checking again....
			data.Release();
			IRA::CIRATools::Wait(m_pConfiguration->controlSocketDutyCycle());
		}
		IRA::CIRATools::getTime(now); // get current time again
	}
	printf("Command Answer Error: timeout \n");
	_EXCPT(ComponentErrors::TimeoutExImpl,ex,"CCommandSocket::waitAck()");
	throw ex;
}

bool CCommandSocket::checkConnection()
{
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	return ((data->getControlLineState()==Antenna::ACU_CNTD) || (data->getControlLineState()==Antenna::ACU_BSY));
}

bool CCommandSocket::checkIsBusy()
{
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	return data->isBusy();	
}

void CCommandSocket::createSocket() throw (SocketErrorExImpl)
{
	CError error;
	// first of all close the socket.....just to be sure there is nothing pending
	Close(error);
	error.Reset();
	// this will create the socket in blocking mode.....
	if (Create(error,STREAM)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::createSocket()");
	}
	// set socket send buffer!!!!
	int Val=CACUProtocol::SOCKET_SEND_BUFFER;
	if (setSockOption(error,SO_SNDBUF,&Val,sizeof(int))==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::createSocket()");
	}
	// set socket in non-blocking mode.
	if (setSockMode(error,NONBLOCKING)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::createSocket()");
	}
	if (EventSelect(error,E_CONNECT,true,0)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CCommandSocket::createSocket()");
	}	
}

void CCommandSocket::sendCommand(const BYTE *cmd,const WORD& len) throw (TimeoutExImpl,SocketErrorExImpl)
{
	OperationResult res;
	CError err;
	CSecAreaResourceWrapper<CCommonData> data=m_pData->Get();
	res=sendBuffer(cmd,len,err);
	if (res==FAIL) {
		data->setStatusWord(CCommonData::CONTROL_LINE_ERROR);
		_EXCPT_FROM_ERROR(SocketErrorExImpl,dummy,err);
		throw dummy;
	} 
	else if (res==WOULDBLOCK) {
		if (m_bTimedout) { // this is at least the second time the socket times out
			data->setControlLineState(Antenna::ACU_NOTCNTD);  //declare the connection fell down
			ACS_LOG(LM_FULL_INFO,"CCommandSocket::sendCommand()",(LM_CRITICAL,"COMMAND_SOCKET_DISCONNECTED_ON_TIMEOUT")); // logs it
		}
		m_bTimedout=true;
		_THROW_EXCPT(TimeoutExImpl,"CCommandSocket::sendCommand()");
	}
	else if (res==SUCCESS) {
		m_bTimedout=false;
		data->clearStatusWord(CCommonData::CONTROL_LINE_ERROR);
	}
}

CSocket::OperationResult CCommandSocket::sendBuffer(const BYTE *Msg,const WORD& Len,CError& error)
{
	int NWrite;
	int BytesSent;
	BytesSent=0;
	while (BytesSent<Len) {
		if ((NWrite=Send(error,(const void *)(Msg+BytesSent),Len-BytesSent))<=0) {
			if (NWrite==WOULDBLOCK) {
				return WOULDBLOCK;
			}
			else {
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
		_SET_ERROR(error,CError::SocketType,CError::SendError,"CCommandSocket::sendBuffer()");
		return FAIL;
	}
}


