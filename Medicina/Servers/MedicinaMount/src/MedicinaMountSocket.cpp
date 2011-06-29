// $Id: MedicinaMountSocket.cpp,v 1.16 2011-03-28 10:12:15 a.orlati Exp $
#include <LogFilter.h>
#include "MedicinaMountSocket.h"
#include <IRATools.h>

#define _STACKSIZE 2500


#define _LAUNCH_BUFFER(BUFFER,LENGTH,ROUTINE) { \
	int Res; \
	BYTE Err; \
	Res=launchCommand(BUFFER,LENGTH); \
	if (Res>0) { \
		if (!CACUInterface::isAck(BUFFER,Err)) { \
			_EXCPT(NakExImpl,dummy,ROUTINE); \
			dummy.setCode(Err); \
			dummy.setMessage((const char *)CACUInterface::messageFromError(Err)); \
			throw dummy; \
		} \
	} \
	else if (Res==FAIL) {  \
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error); \
		dummy.setCode(m_Error.getErrorCode()); \
		dummy.setDescription((const char*)m_Error.getDescription()); \
		m_Error.Reset(); \
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,ROUTINE); \
	} \
	else if (Res==WOULDBLOCK) { \
		_THROW_EXCPT(TimeoutExImpl,ROUTINE); \
	} \
	else { \
		_THROW_EXCPT(ConnectionExImpl,ROUTINE); \
	} \
}

#define _CHECK_ERRORS(ROUTINE) \
	else if (Res==FAIL) { \
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error); \
		dummy.setCode(m_Error.getErrorCode()); \
		dummy.setDescription((const char*)m_Error.getDescription()); \
		m_Error.Reset(); \
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,ROUTINE); \
	} \
	else if (Res==WOULDBLOCK) { \
		_THROW_EXCPT(TimeoutExImpl,ROUTINE); \
	} \
	else { \
		_THROW_EXCPT(ConnectionExImpl,ROUTINE); \
	} 

using namespace AntennaErrors;
using namespace ComponentErrors;

_IRA_LOGFILTER_DECLARE;

CMedicinaMountSocket::CMedicinaMountSocket(): CSocket()
{
	AUTO_TRACE("CMedicinaMountSocket::CMedicinaMountSocket()");
	setStatus(Antenna::ACU_NOTCNTD);
	m_bBusy=false;
	m_bTimedout=false;
	m_bStopped=false;
	m_trackStack=new CTimeTaggedCircularArray(_STACKSIZE,false);
	m_trackStack->empty();
	m_commandSection=CACUInterface::NEUTRAL;
}

CMedicinaMountSocket::~CMedicinaMountSocket()
{
	AUTO_TRACE("CMedicinaMountSocket::~CMedicinaMountSocket()");
}

void CMedicinaMountSocket::cleanUp()
{
	Close(m_Error);
	if (m_trackStack!=NULL) delete m_trackStack;
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;	
}

void CMedicinaMountSocket::Init(CConfiguration *config) throw (SocketErrorExImpl)
{
	AUTO_TRACE("CMedicinaMountSocket::Init()");
	m_configuration=config;
	_IRA_LOGFILTER_ACTIVATE(m_configuration->repetitionCacheTime(),m_configuration->expireCacheTime());
	// this will create the socket in blocking mode.....
	if (Create(m_Error,STREAM)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CMedicinaMountSocket::Init()");
	}
	// the first time, perform a blocking connection....
	if (Connect(m_Error,m_configuration->ipAddress(),m_configuration->ACUPort())==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CMedicinaMountSocket::Init()");
	}	
	else {
		setStatus(Antenna::ACU_CNTD);
	}
	// set socket send buffer!!!!
	int Val=100;
	if (setSockOption(m_Error,SO_SNDBUF,&Val,sizeof(int))==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CMedicinaMountSocket::Init()");
	}
	// set socket in non-blocking mode.
	if (setSockMode(m_Error,NONBLOCKING)==FAIL) {
		_EXCPT_FROM_ERROR(IRALibraryResourceExImpl,dummy,m_Error);
		dummy.setCode(m_Error.getErrorCode());
		dummy.setDescription((const char*)m_Error.getDescription());
		m_Error.Reset();
		_THROW_EXCPT_FROM_EXCPT(SocketErrorExImpl,dummy,"CMedicinaMountSocket::Init()");
	}
	m_oscDirection=0;
	m_oscAlarm=false;
	m_oscNumber=0;
	m_oscStop=false;
	m_oscTime=0;	
	m_oscStopTime=0;
	m_oscMode=CACUInterface::STANDBY;
	m_lastScanEpoch=0;
}

CACUInterface::TAxeModes CMedicinaMountSocket::getAzimuthMode() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getAzimuthMode()");
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getAzimuthMode()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.azimuthMode();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getAzimuthMode()");
}

CACUInterface::TAxeModes CMedicinaMountSocket::getElevationMode() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	AUTO_TRACE("CMedicinaMountSocket::getElevationMode()");
	int Res;
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getElevationMode()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.elevationMode();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getElevationMode()");
}

double CMedicinaMountSocket::getAzimuth() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getAzimuth()");
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getAzimuth()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.azimuth();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getAzimuth()");
}

double CMedicinaMountSocket::getElevation() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getElevation()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getElevation()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.elevation();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getElevation()");
}

double CMedicinaMountSocket::getElevationRate() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getElevationRate()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getElevationRate()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.elevationRate();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getElevationRate()");
}

double CMedicinaMountSocket::getAzimuthRate() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getAzimuthRate()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getAzimuthRate()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.azimuthRate();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getAzimuthRate()");
}

void CMedicinaMountSocket::getEncodersCoordinates(double& az,double& el,double& azOff,double& elOff,ACS::Time& time,
		CACUInterface::TAntennaSection& section) throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getEncodersCoordinates()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getEncodersCoordinates()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		TIMEVALUE tm;
		m_Data.absTime(tm);
		time=tm.value().value;
		az=m_Data.azimuth();
		el=m_Data.elevation();
		azOff=m_Data.getElevationOffset();
		elOff=m_Data.getAzimuthOffset();
		section=m_Data.getSection();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getEncodersCoordinates()");
}

void CMedicinaMountSocket::getAntennaErrors(double& azErr,double& elErr,ACS::Time& time) throw (ConnectionExImpl,
		SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getAntennaErrors()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getAntennaErrors()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		TIMEVALUE tm;
		m_Data.absTime(tm);
		time=tm.value().value;
		azErr=m_Data.azimuthError();
		elErr=m_Data.elevationError();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getAntennaErrors()");
}

double CMedicinaMountSocket::getElevationError() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getElevationError()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getElevationError()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.elevationError();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getElevationError()");
}

double CMedicinaMountSocket::getAzimuthError() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getAzimuthError()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getAzimuthError()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.azimuthError();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getAzimuthError()");
}

ACS::TimeInterval CMedicinaMountSocket::getTime() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getTime()");
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getTime()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		TIMEDIFFERENCE tm;
		m_Data.time(tm);
		return tm.value().value;
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getTime()");
}

ACS::Time CMedicinaMountSocket::getAbsTime() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getAbsTime()");
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getAbsTime()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		TIMEVALUE tm;
		m_Data.absTime(tm);
		return tm.value().value;
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getAbsTime()");
}

/*bool CMedicinaMountSocket::isTracking() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::isTracking()");	
	// use the minimum of azErr and elErr validities
	DWORD MinValidity=GETMIN(m_configuration->propertyRefreshTime(),m_configuration->propertyRefreshTime());
	DWORD MinValidity1=GETMIN(m_configuration->propertyRefreshTime(),m_configuration->propertyRefreshTime());
	MinValidity=GETMIN(MinValidity,MinValidity1);
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::isTracking()");
	}
	Res=loadMonitorData(MinValidity);
	if (Res>0) { // load OK
		return m_Data.isTracking(m_configuration->trackingPrecision());
	}
	_CHECK_ERRORS("CMedicinaMountSocket::isTracking()");	
}*/

WORD CMedicinaMountSocket::getElevationServoStatus() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getElevationServoStatus()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getElevationServoStatus()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.elevationServoStatus();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getElevationServoStatus()");
}
	
WORD CMedicinaMountSocket::getAzimuthServoStatus() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getAzimuthServoStatus()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getAzimuthServoStatus()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.azimuthServoStatus();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getAzimuthServoStatus()");	
}

WORD CMedicinaMountSocket::getServoSystemStatus() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getServoSystemStatus()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getServoSystemStatus()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.servoSystemStatus();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getServoSystemStatus()");	
}

CACUInterface::TAntennaSection CMedicinaMountSocket::getSection() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	AUTO_TRACE("CMedicinaMountSocket::getSection()");
	int Res;
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getSection()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.getSection();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getSection()");
}
	
WORD CMedicinaMountSocket::getFreeStackPositions() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getFreeStackPositions()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getFreeStackPositions()");
	}
	Res=loadMonitorData(m_configuration->propertyRefreshTime());
	if (Res>0) { // load OK
		return m_Data.getFreeStackPositions();
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getFreeStackPositions()");
}

Management::TSystemStatus CMedicinaMountSocket::getMountStatus() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	int Res;
	AUTO_TRACE("CMedicinaMountSocket::getMountStatus()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::getMountStatus()");
	}
	DWORD MinValidity=GETMIN(m_configuration->propertyRefreshTime(),m_configuration->propertyRefreshTime());
	DWORD MinValidity1=GETMIN(MinValidity,m_configuration->propertyRefreshTime());
	Res=loadMonitorData(MinValidity1);
	if (Res>0) { // load OK
		int st=m_Data.getError();
		if (st==2) return Management::MNG_FAILURE;
		else if (st==1) return Management::MNG_WARNING;
		else return Management::MNG_OK;
	}
	_CHECK_ERRORS("CMedicinaMountSocket::getMountStatus()");	
}

void CMedicinaMountSocket::setPositionOffsets(const double& azOff,const double& elOff) throw (TimeoutExImpl,NakExImpl,ConnectionExImpl,
  SocketErrorExImpl,ValueOutofRangeExImpl,AntennaBusyExImpl,PropertyErrorExImpl,ValidationErrorExImpl)
{
	BYTE sBuffer[ACU_BUFFERSIZE];
	WORD Len;
	double oldAzOff,oldElOff;
	bool ptEnabled;
	AUTO_TRACE("CMedicinaMountSocket::setPositionOffsets()");
	CACUInterface::TAxeModes azMode,elMode;
	try {
		azMode=getAzimuthMode();
		elMode=getElevationMode();
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(PropertyErrorExImpl,foo,E,"CMedicinaMountSocket::setPositionOffsets()");
		foo.setPropertyName("Modes");
		foo.setReason("It cannot be read");
		throw foo;	
	}
	if ((elMode==CACUInterface::PROGRAMTRACK) && (azMode==CACUInterface::PROGRAMTRACK)) {
		ptEnabled=true;
	}
	else if ((azMode==CACUInterface::PRESET) && (azMode==CACUInterface::PRESET)) {
		ptEnabled=false;
	}
	else {  //all others modes does not allows for offsets..............
		_EXCPT(ValidationErrorExImpl,dummy,"MedicinaMountSocket::setPositionOffsets()");
		dummy.setReason("Offsets are not allowed in present mode");
		throw dummy;		
		///
	}
	if (ptEnabled) {
		TIMEVALUE now;
		unsigned pos;
		long az,el;
		double azimuth,elevation;
		int dayTime;
	    short yearDay;
		CIRATools::getTime(now);
		m_trackStack->purge(now);
		if (!m_trackStack->isEmpty()) {
			oldAzOff=m_Data.getAzimuthOffset();
			oldElOff=m_Data.getElevationOffset();
			m_trackStack->addOffsets(azOff-oldAzOff,elOff-oldElOff,now);
			pos=0;
			while (m_trackStack->getPoint(pos,azimuth,elevation,now)) {
				az=(long)(azimuth*1000000.0);
				el=(long)(elevation*1000000.0);
				dayTime=(((now.hour()*60LL+now.minute())*60+now.second())*1000+now.microSecond()/1000);	
				yearDay=(short)now.dayOfYear();
				pos++;
				if (pos==0) {
					m_lastScanEpoch=now.value().value;
					Len=CACUInterface::getProgramTrackBuffer(sBuffer,az,el,0,0,dayTime,yearDay,true,true); //first point...clear the stack
				}
				else {
					Len=CACUInterface::getProgramTrackBuffer(sBuffer,az,el,0,0,dayTime,yearDay,true,false);
				}
				_LAUNCH_BUFFER(sBuffer,Len,"CMedicinaMountSocket::setPositionOffsets()");											
			}				
		}
	}
	else {
		long az,el;
		oldAzOff=m_Data.getAzimuthOffset();
		oldElOff=m_Data.getElevationOffset();
		double azimuth,elevation;
		azimuth=m_Data.getCommandedAzimuth();
		elevation=m_Data.getCommandedElevation();
		azimuth+=azOff-oldAzOff;
		elevation+=elOff-oldElOff;
		az=(long)(azimuth*1000000.0);
		el=(long)(elevation*1000000.0);
		Len=CACUInterface::getPresetBuffer(sBuffer,az,el);
		_LAUNCH_BUFFER(sBuffer,Len,"CMedicinaMountSocket::setPositionOffsets()");
		m_Data.setCommandedAzimuth(azimuth);
		m_Data.setCommandedElevation(elevation);
	}	
	//finally store the new offsets
	m_Data.setAzimuthOffset(azOff);
	m_Data.setElevationOffset(elOff);

}

double CMedicinaMountSocket::getCommandedAzimuth(bool ptEnabled)
{
	if (ptEnabled) {
		TIMEVALUE now;
		double azimuth,elevation;
		CIRATools::getTime(now);
		m_trackStack->selectPoint(now,azimuth,elevation);
		return azimuth;
	}
	else { 
		return m_Data.getCommandedAzimuth();
	}				
}
	
double CMedicinaMountSocket::getCommandedElevation(bool ptEnabled)
{
	if (ptEnabled) {
		TIMEVALUE now;
		double azimuth,elevation;
		CIRATools::getTime(now);
		m_trackStack->selectPoint(now,azimuth,elevation);
		return elevation;
	}
	else { 
		return m_Data.getCommandedElevation();
	}			
}

void CMedicinaMountSocket::timeTransfer(TIMEVALUE& now) throw (ConnectionExImpl,NakExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	WORD Len;
	BYTE sBuffer[ACU_BUFFERSIZE];
	long micro;
	int dayTime;
	short yearDay;
	short year;
	AUTO_TRACE("CMedicinaMountSocket::timeTransfer()");
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::timeTransfer()");
	}
	if (now.microSecond()>500000) {
		micro=1100000-now.microSecond();
		now.normalize(true);
		now.second(now.second()+1);
		CIRATools::Wait(0,micro);
	}
	//we add 1 to seconds because the ACU will sync to the next second
	now.second(now.second()+1);
	dayTime=(((now.hour()*60LL+now.minute())*60+now.second())*1000+now.microSecond()/1000);	
	yearDay=(short)now.dayOfYear();
	year=(short)now.year();
	Len=CACUInterface::getTimeTransferBuffer(sBuffer,dayTime,yearDay,year);
	_LAUNCH_BUFFER(sBuffer,Len,"CMedicinaMountSocket::timeTransfer");	
}

void CMedicinaMountSocket::Stop() throw (TimeoutExImpl,NakExImpl,ConnectionExImpl,SocketErrorExImpl,AntennaBusyExImpl)
{
	if (isBusy()) setStopped(true);
	Mode(CACUInterface::STOP,CACUInterface::STOP,true);	
}

void CMedicinaMountSocket::Mode(CACUInterface::TAxeModes azMode,CACUInterface::TAxeModes elMode,bool force) 
  throw (TimeoutExImpl,NakExImpl,ConnectionExImpl,SocketErrorExImpl,AntennaBusyExImpl)
{
	WORD Len;
	BYTE sBuffer[ACU_BUFFERSIZE];
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::Mode()");
	}
	if (!force) {
		if (isBusy()) {
			_THROW_EXCPT(AntennaBusyExImpl,"CMedicinaMountSocket::Mode()");
		}
	}
	Len=CACUInterface::getModeSelectionBuffer(sBuffer,azMode,elMode);
	_LAUNCH_BUFFER(sBuffer,Len,"CMedicinaMountSocket::Mode()");
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	m_Data.setLastCommandedMode(azMode); 
	m_Data.setAzimuthOffset(0.0);
	m_Data.setElevationOffset(0.0);
	m_lastScanEpoch=now.value().value;
}

void CMedicinaMountSocket::failureReset() throw (TimeoutExImpl,NakExImpl,ConnectionExImpl,SocketErrorExImpl)
{
	WORD Len;
	BYTE sBuffer[ACU_BUFFERSIZE];	
	Len=CACUInterface::getFailureResetBuffer(sBuffer);	
	_LAUNCH_BUFFER(sBuffer,Len,"CMedicinaMountSocket::failureReset()");
}

void CMedicinaMountSocket::programTrack(double az,double el,TIMEVALUE& time,bool clear) throw (TimeoutExImpl,ValueOutofRangeExImpl,NakExImpl,ConnectionExImpl,
  SocketErrorExImpl,AntennaBusyExImpl,OperationNotPermittedExImpl,PropertyErrorExImpl)
{
	long azimuth=0;
	long elevation=0;
	double lastAzimuth;
	double azOff,elOff;
	BYTE sBuffer[ACU_BUFFERSIZE];
	WORD Len;
	int dayTime;
	short yearDay;
	CACUInterface::TAxeModes azMode,elMode;
	AUTO_TRACE("CMedicinaMountSocket::programTrack()");
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::programTrack()");
	}
	if (isBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CMedicinaMountSocket::programTrack()");
	}
	try {
		azMode=getAzimuthMode();
		elMode=getElevationMode();
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(PropertyErrorExImpl,foo,E,"CMedicinaMountSocket::programTrack()");
		foo.setPropertyName("Modes");
		foo.setReason("It cannot be read");
		throw foo;		
	}
	if (azMode!=CACUInterface::PROGRAMTRACK) {
		_EXCPT(OperationNotPermittedExImpl,dummy,"MedicinaMountSocket::progamTrack()");
		dummy.setReason("Azimuth is not in program track mode");
		throw dummy;
	}
	if (elMode!=CACUInterface::PROGRAMTRACK) {
		_EXCPT(OperationNotPermittedExImpl,dummy,"MedicinaMountSocket::progamTrack()");
		dummy.setReason("Elevation is not in program track mode");
		throw dummy;
	} 	
	if (clear) {
		m_trackStack->empty();
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		m_lastScanEpoch=now.value().value;
	}
	// Before all, let's get the current azimuth
	if (m_trackStack->isEmpty()) {  // if the trajectory is not set yet.....the azimuth is the real one (where the mount is)
		try {
			lastAzimuth=getAzimuth();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(PropertyErrorExImpl,foo,E,"CMedicinaMountSocket::programTrack()");
			foo.setPropertyName("azimuth");
			foo.setReason("It cannot be read");
			throw foo;
		}
	}
	else { // else the azimuth is the azimuth of the last data point of the trajectory (the mount could not be there yet)
		lastAzimuth=m_trackStack->getLastAzimuth();
	}
	azOff=az+m_Data.getAzimuthOffset();
	elOff=el+m_Data.getElevationOffset();
	// Let's compute the azimuth coordinate to be loaded with respect to the real mount range limits.
	long section;
	if (m_commandSection==CACUInterface::CCW) section=-1;
	else if (m_commandSection==CACUInterface::CW) section=1;
	else section=0;
	azOff=CIRATools::getHWAzimuth(lastAzimuth,azOff,m_configuration->azimuthLowerLimit(),m_configuration->azimuthUpperLimit(),section,m_configuration->cwLimit());
	if (azOff<m_configuration->azimuthLowerLimit()) {
		azOff=m_configuration->azimuthLowerLimit();
	}
	if (azOff>m_configuration->azimuthUpperLimit()) {
		azOff=m_configuration->azimuthUpperLimit();
	}
	if (elOff<m_configuration->elevationLowerLimit()) {
		elOff=m_configuration->elevationLowerLimit();
	}
	if	(elOff>m_configuration->elevationUpperLimit()) {
		elOff=m_configuration->elevationUpperLimit();
	}	
	//change preferred section to NUETRAL
	m_commandSection=CACUInterface::NEUTRAL;
	// add the point to the local stack.
	if (m_trackStack->addPoint(azOff,elOff,time)) {					
		azimuth=(long)(azOff*1000000.0);
		elevation=(long)(elOff*1000000.0);
		dayTime=(((time.hour()*60LL+time.minute())*60+time.second())*1000+time.microSecond()/1000);	
		yearDay=(short)time.dayOfYear();
		Len=CACUInterface::getProgramTrackBuffer(sBuffer,azimuth,elevation,0,0,dayTime,yearDay,true,clear);
		_LAUNCH_BUFFER(sBuffer,Len,"CMedicinaMountSocket::programTrack()");
	}
	else {
		_EXCPT(OperationNotPermittedExImpl,foo,"CMedicinaMountSocket::programTrack()");
		foo.setReason("Time stamp for program track was already elapsed");
		throw foo;
	}
}

void CMedicinaMountSocket::Preset(double Azim,double Elev) throw (TimeoutExImpl,ValueOutofRangeExImpl,NakExImpl,ConnectionExImpl,
  SocketErrorExImpl,AntennaBusyExImpl,PropertyErrorExImpl,OperationNotPermittedExImpl)
{
	WORD Len;
	BYTE sBuffer[ACU_BUFFERSIZE];
	double lastAzimuth;
	double azOff,elOff;
	long az,el;
	CACUInterface::TAxeModes azMode,elMode;
	AUTO_TRACE("CMedicinaMountSocket::Preset()");
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::Preset()");
	}
	if (isBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CMedicinaMountSocket::Preset()");
	}
	try {
		azMode=getAzimuthMode();
		elMode=getElevationMode();
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(PropertyErrorExImpl,foo,E,"CMedicinaMountSocket::Preset()");
		foo.setPropertyName("Modes");
		foo.setReason("It cannot be read");
		throw foo;		
	}
	if (azMode!=CACUInterface::PRESET) {
		_EXCPT(OperationNotPermittedExImpl,dummy,"MedicinaMountSocket::Preset()");
		dummy.setReason("Azimuth is not in preset mode");
		throw dummy;
	}
	if (elMode!=CACUInterface::PRESET) {
		_EXCPT(OperationNotPermittedExImpl,dummy,"MedicinaMountSocket::Preset()");
		dummy.setReason("Elevation is not in preset mode");
		throw dummy;
	} 
	if (m_Data.getIsCommanded()) {
		lastAzimuth=m_Data.getCommandedAzimuth();
	}
	else {
		try {
			lastAzimuth=getAzimuth();
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(PropertyErrorExImpl,foo,E,"CMedicinaMountSocket::Preset()");
			foo.setPropertyName("azimuth");
			foo.setReason("It cannot be read");
			throw foo;
		}
	}
	// Before all, let's add the offsets for both axis. 
	azOff=Azim+m_Data.getAzimuthOffset();
	elOff=Elev+m_Data.getElevationOffset();
	// Let's compute the azimuth coordinate to be loaded with respect to the real mount range limits
	long section;
	if (m_commandSection==CACUInterface::CCW) section=-1;
	else if (m_commandSection==CACUInterface::CW) section=1;
	else section=0;
	azOff=CIRATools::getHWAzimuth(lastAzimuth,azOff,m_configuration->azimuthLowerLimit(),m_configuration->azimuthUpperLimit(),section,m_configuration->cwLimit());
	if (azOff<m_configuration->azimuthLowerLimit()) {
		azOff=m_configuration->azimuthLowerLimit();
	}
	if (azOff>m_configuration->azimuthUpperLimit()) {
		azOff=m_configuration->azimuthUpperLimit();
	}
	if (elOff<m_configuration->elevationLowerLimit()) {
		elOff=m_configuration->elevationLowerLimit();
	}
	if	(elOff>m_configuration->elevationUpperLimit()) {
		elOff=m_configuration->elevationUpperLimit();
	}	
	//change preferred section to NUETRAL
	m_commandSection=CACUInterface::NEUTRAL;
	// check if the el offset brought the elevation over the limits
	az=(long)(azOff*1000000.0);
	el=(long)(elOff*1000000.0);
	m_Data.setCommandedAzimuth(azOff);
	m_Data.setCommandedElevation(elOff);
	Len=CACUInterface::getPresetBuffer(sBuffer,az,el);
	_LAUNCH_BUFFER(sBuffer,Len,"CMedicinaMountSocket::Preset()");
}

void CMedicinaMountSocket::Rate(double azRate,double elRate) throw (TimeoutExImpl,ValueOutofRangeExImpl,NakExImpl,ConnectionExImpl,
  SocketErrorExImpl,AntennaBusyExImpl,PropertyErrorExImpl,OperationNotPermittedExImpl)
{
	WORD Len;
	BYTE sBuffer[ACU_BUFFERSIZE];
	long az,el;
	CACUInterface::TAxeModes azMode,elMode;
	AUTO_TRACE("CMedicinaMountSocket::Rate()");	
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::Rate()");
	}
	if (isBusy()) {
		_THROW_EXCPT(AntennaBusyExImpl,"CMedicinaMountSocket::Rate()");
	}
	try {
		azMode=getAzimuthMode();
		elMode=getElevationMode();
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(PropertyErrorExImpl,foo,E,"CMedicinaMountSocket::Rate()");
		foo.setPropertyName("Modes");
		foo.setReason("It cannot be read");
		throw foo;		
	}
	if (azMode!=CACUInterface::RATE) {
		_EXCPT(OperationNotPermittedExImpl,dummy,"MedicinaMountSocket::Rate()");
		dummy.setReason("Azimuth is not in rate  mode");
		throw dummy;
	}
	if (elMode!=CACUInterface::RATE) {
		_EXCPT(OperationNotPermittedExImpl,dummy,"MedicinaMountSocket::Rate()");
		dummy.setReason("Elevation is not in rate  mode");
		throw dummy;
	}
	if (azRate<m_configuration->azimuthRateLowerLimit()) {
		azRate=m_configuration->azimuthRateLowerLimit();
	}
	else if (azRate>m_configuration->azimuthRateUpperLimit()) {
		azRate=m_configuration->azimuthRateUpperLimit();
	}
	if (elRate<m_configuration->elevationRateLowerLimit()) {
		elRate=m_configuration->elevationRateLowerLimit();
	}
	else if	(elRate>m_configuration->elevationRateUpperLimit()) {
		elRate=m_configuration->elevationRateUpperLimit();
	}
	az=(long)(azRate*1000000.0);
	el=(long)(elRate*1000000.0);
	Len=CACUInterface::getRateBuffer(sBuffer,az,el);
	_LAUNCH_BUFFER(sBuffer,Len,"CMedicinaMountSocket::Rate()");
}

double CMedicinaMountSocket::getHWAzimuth(double destination,const CACUInterface::TAntennaSection& commandedSection)
{
	AUTO_TRACE("CMedicinaMountSocket::getHWAzimuth()");
	long section;
	double pos=m_Data.azimuth();
	double dest=destination+m_Data.getAzimuthOffset(); // the destination will be added with the currently commanded azimuth offset, so we take it into consideration
	if (commandedSection==CACUInterface::CCW) section=-1;
	else if (commandedSection==CACUInterface::CW) section=1;
	else section=0;
	return CIRATools::getHWAzimuth(pos,dest,m_configuration->azimuthLowerLimit(),m_configuration->azimuthUpperLimit(),section,m_configuration->cwLimit());
}

void CMedicinaMountSocket::detectOscillation() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl,NakExImpl,AntennaBusyExImpl)
{
	TIMEVALUE now;
	double azError=getAzimuthError(); // throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
	IRA::CIRATools::getTime(now);
	CACUInterface::TAxeModes mode=m_Data.getLastCommandedMode();
	if (m_oscStop) { // if the oscillation has been detected.....during previuos iteration
		if (now.value().value>=m_oscStopTime+m_configuration->oscillationRecoveryTime()) { // the time to wait for trying to recover has elapsed.....
			ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::detectOscillation()",(LM_NOTICE,"OSCILLATION_RECOVERY"));
			Mode(m_oscMode,m_oscMode); // throw (TimeoutExImpl,NakExImpl,ConnectionExImpl,SocketErrorExImpl,AntennaBusyExImpl)
			m_oscStop=false;
			m_oscAlarm=false; // clear the oscillation detection.....
		}
		return; // the exit;
	}
	if ((mode==CACUInterface::STOW) || (mode==CACUInterface::STANDBY) || (mode==CACUInterface::STOP) || (mode==CACUInterface::UNSTOW)) {
		m_oscStop=false;
		m_oscAlarm=false;
		return;
	}
	if (now.value().value<m_lastScanEpoch+20000000) {
		m_oscStop=false;
		m_oscAlarm=false;		
		return;
	}
	if (!m_oscStop) {  // this cycle is done only if the oscillation has not been detected yet.
		if (azError>m_configuration->oscillationThreshold()) {  //if the error si beyond a threshold...possible alarm
			if (!m_oscAlarm) { // if the alarm was not triggered yet.....do it
				m_oscAlarm=true;
				m_oscTime=now.value().value;
				m_oscDirection=1; // positive direction;
				m_oscNumber=0;
			}
			else { //alarm already triggered 
				if (m_oscTime<now.value().value+m_configuration->oscillationAlarmDuration()) { // if the alarm time windows is still opened
					if (m_oscDirection<0) { // the previuos direction was negative
						m_oscNumber++;
						m_oscDirection=-1;
						if (m_oscNumber>m_configuration->oscillationNumberThreashold()) { // if the number of directions changes is beyound the threshold, during the alarm validity time....oscillation detected!
							m_oscStop=true; //...so stop it!
						}
					}
				}
			}
		}
		else if (azError<-m_configuration->oscillationThreshold()) {  //if the error si beyond a threshold...possible alarm
			if (!m_oscAlarm) { // if the alarm was not triggered yet.....do it
				m_oscAlarm=true;
				m_oscTime=now.value().value;
				m_oscDirection=-1; // positive direction;
				m_oscNumber=0;
			}
			else { //alarm already triggered 
				if (m_oscTime<now.value().value+m_configuration->oscillationAlarmDuration()) { // if the alarm time windows is still opened
					if (m_oscDirection>0) { // the previuos direction was positive
						m_oscNumber++;
						m_oscDirection=1;
						if (m_oscNumber>m_configuration->oscillationNumberThreashold()) { // if the number of directions changes is beyound the threshold, during the alarm validity time....oscillation detected!
							m_oscStop=true; //...so stop it!
						}
					}
				}
			}
		}
		else { // the error is inside the limits
			if (m_oscTime>=now.value().value+m_configuration->oscillationAlarmDuration()) { // if the alarm time window is elapsed
				m_oscAlarm=false; // reset the alarm...all seems to be ok.
				m_oscTime=0;
			}
		}	
	}
	if (m_oscStop) { // if the oscillation has been detected durint current iteration.....stop the antenna
		ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::detectOscillation()",(LM_CRITICAL,"OSCILLATION_DETECTED"));
		m_oscMode=mode; ///store the current mode, in order to recommand it for ascillation recovery;
		m_oscStopTime=now.value().value;
		Stop(); //throw (TimeoutExImpl,NakExImpl,ConnectionExImpl,SocketErrorExImpl,AntennaBusyExImpl)
	}
}

void CMedicinaMountSocket::forceUpdate() throw (ConnectionExImpl,SocketErrorExImpl,TimeoutExImpl)
{
	AUTO_TRACE("CMedicinaMountSocket::forceUpdate()");
	int Res;
	if (!checkConnection()) {
		_THROW_EXCPT(ConnectionExImpl,"CMedicinaMountSocket::forceUpdate()");
	}
	Res=loadMonitorData(0);
	if (Res>0) { // load OK
	}
	_CHECK_ERRORS("CMedicinaMountSocket::forceUpdate()");	
}

bool CMedicinaMountSocket::updateLongJobs(WORD job,ACSErr::Completion_out comp)
{
	WORD azStatus=0;
	WORD elStatus=0;
	if (isStopped()) {
		setStopped(false);
		_COMPL(StoppedByUserCompletion,dummy,"CMedicinaMountSocket::updateLongJobs()");
		comp=dummy.outCompletion();
		ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::updateLongJobs()",(LM_NOTICE,"CURRENT_ACTION_STOPPED"));
		return true;
	}
	switch (job) {
		case UNSTOW_ACTION : {
			try {
				azStatus=getAzimuthServoStatus();
			}			
			catch (ACSErr::ACSbaseExImpl& E) {
				_COMPL_FROM_EXCPT(OperationErrorCompletion,__dummy,E,"CMedicinaMountSocket::updateLongJobs()");
				__dummy.setReason("Could not read azimuth servo status");
				_IRA_LOGFILTER_LOG_COMPLETION(__dummy,LM_DEBUG);
				//__dummy.log(LM_DEBUG);
				comp=__dummy.outCompletion();
				return true;
			}
			try {
				elStatus=getElevationServoStatus();
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				_COMPL_FROM_EXCPT(OperationErrorCompletion,__dummy,E,"CMedicinaMountSocket::updateLongJobs()");
				__dummy.setReason("Could not read elevation servo status");
				_IRA_LOGFILTER_LOG_COMPLETION(__dummy,LM_DEBUG);
				//__dummy.log(LM_DEBUG);
				comp=__dummy.outCompletion();
				return true;
			}			
			// check if the stow pin is retracted in both axes.
			if (((azStatus&CACUInterface::STOWPINRETRACTED)==CACUInterface::STOWPINRETRACTED) && 
	  		  ((elStatus&CACUInterface::STOWPINRETRACTED)==CACUInterface::STOWPINRETRACTED)) {
				ComponentErrors::NoErrorCompletion dummy;
				comp=dummy.outCompletion();
				ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::updateLongJobs()",(LM_NOTICE,"ANTENNA_UNSTOWED"));		
				return true;
			}			
			return false;
		}
		case STOW_ACTION : {
			try {
				azStatus=getAzimuthServoStatus();
			}			
			catch (ACSErr::ACSbaseExImpl& E) {
				_COMPL_FROM_EXCPT(OperationErrorCompletion,__dummy,E,"CMedicinaMountSocket::updateLongJobs()");
				__dummy.setReason("Could not read azimuth servo status");
				_IRA_LOGFILTER_LOG_COMPLETION(__dummy,LM_DEBUG);
				//__dummy.log(LM_DEBUG);
				comp=__dummy.outCompletion();
				return true;
			}
			try {
				elStatus=getElevationServoStatus();	
			}			
			catch (ACSErr::ACSbaseExImpl& E) {
				_COMPL_FROM_EXCPT(OperationErrorCompletion,__dummy,E,"CMedicinaMountSocket::updateLongJobs()");
				__dummy.setReason("Could not read elevation servo status");
				_IRA_LOGFILTER_LOG_COMPLETION(__dummy,LM_DEBUG);
				//__dummy.log(LM_DEBUG);
				comp=__dummy.outCompletion();
				return true;
			}			
			// check if the stow pins are retracted in both axes.
			if (((azStatus&CACUInterface::STOWPININSERTED)==CACUInterface::STOWPININSERTED) && 
	  		  ((elStatus&CACUInterface::STOWPININSERTED)==CACUInterface::STOWPININSERTED)) {
				ComponentErrors::NoErrorCompletion dummy;
				comp=dummy.outCompletion();
				ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::updateLongJobs()",(LM_NOTICE,"ANTENNA_STOWED"));						  
				return true;
			}			
			return false;		
		}
		default : {
			return true;
		}
	}		
}

void CMedicinaMountSocket::updateComponent()
{
	if (getStatus()==Antenna::ACU_CNTD) {		
		try {
			Management::TSystemStatus st=getMountStatus();
			if (st==Management::MNG_FAILURE) {
				_IRA_LOGFILTER_LOG(LM_CRITICAL,"CMedicinaMountSocket::updateComponent()","HW_FAILURE");
			}
			else if (st==Management::MNG_WARNING) {
				_IRA_LOGFILTER_LOG(LM_WARNING,"CMedicinaMountSocket::updateComponent()","HW_WARNING");
			}			     
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(WatchDogErrorExImpl,dummy,E,"CMedicinaMountSocket::updateComponent()");
			dummy.setReason("Can't get mount status information");
			_IRA_LOGFILTER_LOG_EXCEPTION(dummy,LM_ERROR);
		}		
	}
}

// Protected Methods

void CMedicinaMountSocket::onConnect(int ErrorCode)
{
	CError Tmp;
	if (getStatus()==Antenna::ACU_CNTDING) {
		if (ErrorCode==0) {
			if (EventSelect(Tmp,E_CONNECT,false)==SUCCESS) {
				if (isBusy()) setStatus(Antenna::ACU_BSY);
				else setStatus(Antenna::ACU_CNTD);
				_IRA_LOGFILTER_LOG(LM_NOTICE,"CMedicinaMountSocket::onConnect()","MedicinaMount::SOCKET_RECONNECTED");
				//ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::onConnect()",(LM_NOTICE,"MedicinaMount::SOCKET_RECONNECTED"));			
			}
		}
		else {
			setStatus(Antenna::ACU_NOTCNTD);
			ACS_DEBUG_PARAM("CMedicinaMountSocket::onConnect()","Reconnection failed, exit code is %d",ErrorCode);	
		}
	}
}

void CMedicinaMountSocket::onTimeout(WORD EventMask)
{
	if ((EventMask&E_CONNECT)==E_CONNECT) {
		ACS_DEBUG("CMedicinaMountSocket::onTimeout()","Reconnection timed-out, keep trying....");
	}
}

// private methods

CMedicinaMountSocket::OperationResult CMedicinaMountSocket::sendBuffer(BYTE *Msg,WORD Len)
{
	int NWrite;
	int BytesSent;
	BytesSent=0;
	while (BytesSent<Len) {
		if ((NWrite=Send(m_Error,(const void *)(Msg+BytesSent),Len-BytesSent))<0) {
			if (NWrite==WOULDBLOCK) {
				setStatus(Antenna::ACU_NOTCNTD);
				_IRA_LOGFILTER_LOG(LM_CRITICAL,"CMedicinaMountSocket::sendBuffer()","MedicinaMount::SOCKET_DISCONNECTED - remote side shutdown");
				//ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::sendBuffer()",(LM_CRITICAL,"MedicinaMount::SOCKET_DISCONNECTED - remote side shutdown"));			
				return WOULDBLOCK;
			}
			else {
				setStatus(Antenna::ACU_NOTCNTD);
				CString app;
				app.Format("MedicinaMount::SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
				_IRA_LOGFILTER_LOG(LM_CRITICAL,"CMedicinaMountSocket::sendBuffer()",(const char*)app);
				//ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::sendBuffer()",(LM_CRITICAL,"MedicinaMount::SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription()));
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
		_SET_ERROR(m_Error,CError::SocketType,CError::SendError,"CMedicinaMountSocket::SendBuffer()");
		return FAIL;
	}
}

int CMedicinaMountSocket::receiveBuffer(BYTE *Msg,WORD Len)
{
	int nRead;
	TIMEVALUE Now;
	TIMEVALUE Start;
	CIRATools::getTime(Start);
	while(true) {
		nRead=Receive(m_Error,(void *)Msg,Len);
		if (nRead==WOULDBLOCK) {
			CIRATools::getTime(Now);
			if (CIRATools::timeDifference(Start,Now)>m_configuration->receiveTimeout()) {
				m_bTimedout=true;
				return WOULDBLOCK;
			}
			else {
				CIRATools::Wait(0,10000);
				continue;
			}
		}
		else if (nRead==FAIL) { 
			setStatus(Antenna::ACU_NOTCNTD);
			CString app;
			app.Format("MedicinaMount::SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription());
			_IRA_LOGFILTER_LOG(LM_CRITICAL,"CMedicinaMountSocket::receiveBuffer()",(const char*)app);
			//ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::receiveBuffer()",(LM_CRITICAL,"MedicinaMount::SOCKET_DISCONNECTED - %s",(const char *)m_Error.getFullDescription()));
			return nRead;
		}
		else if (nRead==0) {
			setStatus(Antenna::ACU_NOTCNTD);
			_IRA_LOGFILTER_LOG(LM_CRITICAL,"CMedicinaMountSocket::receiveBuffer()","MedicinaMount::SOCKET_DISCONNECTED - remote side shutdown");
			//ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::receiveBuffer()",(LM_CRITICAL,"MedicinaMount::SOCKET_DISCONNECTED - remote side shutdown"));			
			return nRead;
		}
		else return nRead;
	}
}

int CMedicinaMountSocket::launchCommand(BYTE *Buff,WORD Len)
{
	OperationResult Res;
	if ((Res=sendBuffer(Buff,Len))==SUCCESS) {
		int bytes=receiveBuffer(Buff,ACU_BUFFERSIZE);
		return bytes;
	}
	else {  // send fails....m_Error already set by sendBuffer
		return Res;
	}
}

int CMedicinaMountSocket::loadMonitorData(DWORD updateTime)
{
	TIMEVALUE Now;
	// buffer used to transfer requests to the ACU
	BYTE sBuffer[ACU_BUFFERSIZE];
	WORD Len;
	int rBytes;
	OperationResult Res;
	CIRATools::getTime(Now);	
	if (CIRATools::timeDifference(*m_Data.monitorTime(),Now)>updateTime) { //Parameter must be loaded!!!
		Len=CACUInterface::getMonitorBuffer(sBuffer); //prepare the buffer
		if ((Res=sendBuffer(sBuffer,Len))==SUCCESS) {
			rBytes=receiveBuffer(sBuffer,ACU_BUFFERSIZE);
			if (rBytes>0) {
				m_Data.setMonitorBuffer(sBuffer,41);
				//memcpy(m_Data.monitorBuffer(),sBuffer,41);	
				CIRATools::getTime(*m_Data.monitorTime());
			}
			// this could be 0 (comunication fell down), FAIL error (m_Error set accordingly) , WOULDBLOCK timeout, >0 ok
			return rBytes;
		}
		else {  // send fails....m_Error already set by sendBuffer
			return Res;
		}
	}
	return ACU_BUFFERSIZE;
}

bool CMedicinaMountSocket::checkConnection()
{
	CError Tmp;
	BYTE sBuffer[ACU_BUFFERSIZE];
	int rBytes;
	if (m_bTimedout) {
		rBytes=receiveBuffer(sBuffer,ACU_BUFFERSIZE);
		if (rBytes==WOULDBLOCK) {
			setStatus(Antenna::ACU_NOTCNTD);	// another timeout! something has happend
			_IRA_LOGFILTER_LOG(LM_CRITICAL,"CMedicinaMountSocket::checkConnection()","MedicinaMount::SOCKET_DISCONNECTED - timeout expired");
			//ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::checkConnection()",(LM_CRITICAL,"MedicinaMount::SOCKET_DISCONNECTED - timeout expired"));
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
	if ((getStatus()==Antenna::ACU_CNTD) || (getStatus()==Antenna::ACU_BSY)) {
		return true;
	}
	else if (getStatus()==Antenna::ACU_CNTDING) {
		return false;
	}
	else {  // socket is not connected....
		// try to close the socket, if it is already closed : never mind....
		Close(Tmp);
		Tmp.Reset();
		m_bTimedout=false;
		// this will create the socket in blocking mode.....
		if (Create(Tmp,STREAM)==SUCCESS) {
			// registers the onconnect event....allows 3 seconds to complete!
			if (EventSelect(Tmp,E_CONNECT,true,m_configuration->connectionTimeout())==SUCCESS) {
				OperationResult Res;
				ACS_DEBUG("CMedicinaMountSocket::checkConnection()","Trying to reconnect");
				Res=Connect(Tmp,m_configuration->ipAddress(),m_configuration->ACUPort());
				if (Res==WOULDBLOCK) {
					setStatus(Antenna::ACU_CNTDING);
				}
				else if (Res==SUCCESS) {
					if (isBusy()) setStatus(Antenna::ACU_BSY);
					else setStatus(Antenna::ACU_CNTD);
					_IRA_LOGFILTER_LOG(LM_NOTICE,"CMedicinaMountSocket::checkConnection()","MedicinaMount::SOCKET_RECONNECTED");
					//ACS_LOG(LM_FULL_INFO,"CMedicinaMountSocket::checkConnection()",(LM_NOTICE,"MedicinaMount::SOCKET_RECONNECTED"));						
					return true;
				}
			}
		}
		return false;
	}
}

Antenna::TCommonModes CMedicinaMountSocket::acuModes2IDL(const CACUInterface::TAxeModes& mode)
{	
	if (mode==CACUInterface::STANDBY) {  return Antenna::ACU_STANDBY; }
	else if (mode==CACUInterface::PRESET) {  return Antenna::ACU_PRESET; }
	else if (mode==CACUInterface::STOP) {  return Antenna::ACU_STOP; }
	else if (mode==CACUInterface::PROGRAMTRACK) {  return Antenna::ACU_PROGRAMTRACK; }
	else if (mode==CACUInterface::RATE) {  return Antenna::ACU_RATE; }
	else if (mode==CACUInterface::STOW) {  return Antenna::ACU_STOW; }
	//else if (mode==CACUInterface::POSITIONTRACK) {  return Antenna::ACU_POSITIONTRACK; }
	else if (mode==CACUInterface::UNSTOW) {  return Antenna::ACU_UNSTOW; }
	else { return Antenna::ACU_UNKNOWN;	}
}

CACUInterface::TAxeModes CMedicinaMountSocket::idlModes2ACU(const Antenna::TCommonModes& mode)
{
	if (mode==Antenna::ACU_STANDBY) return CACUInterface::STANDBY;
	else if (mode==Antenna::ACU_STOP) return CACUInterface::STOP;
	else if (mode==Antenna::ACU_PRESET) return CACUInterface::PRESET;
	else if (mode==Antenna::ACU_PROGRAMTRACK) return CACUInterface::PROGRAMTRACK;
	else if (mode==Antenna::ACU_RATE) return CACUInterface::RATE;
	else if (mode==Antenna::ACU_STOW) return CACUInterface::STOW;
	//else if (mode==Antenna::ACU_POSITIONTRACK) return CACUInterface::POSITIONTRACK;
	else if (mode==Antenna::ACU_UNSTOW) return CACUInterface::UNSTOW;
	else return CACUInterface::UNKNOWN;
}

Antenna::TSections CMedicinaMountSocket::acuSection2IDL(const CACUInterface::TAntennaSection& section)
{
	if (section==CACUInterface::CW) return Antenna::ACU_CW;
	else if (section==CACUInterface::CCW) return Antenna::ACU_CCW;
	else return Antenna::ACU_NEUTRAL;
}

CACUInterface::TAntennaSection CMedicinaMountSocket::IDLSection2Acu(const Antenna::TSections& section)
{
	if (section==Antenna::ACU_CW) return CACUInterface::CW;
	else if (section==Antenna::ACU_CCW) return CACUInterface::CCW;
	else return CACUInterface::NEUTRAL;	
}

bool CMedicinaMountSocket::isBusy() 
{
	return (m_bBusy);
}

Antenna::TStatus CMedicinaMountSocket::getStatus() const
{ 
	return m_Status;
}
	
void CMedicinaMountSocket::setStatus(Antenna::TStatus sta)
{ 
	m_Status=sta;
	if (m_Status==Antenna::ACU_BSY) m_bBusy=true;
	else if (m_Status==Antenna::ACU_CNTD) m_bBusy=false;
}

void CMedicinaMountSocket::printBuffer(BYTE *Buff,WORD Len)
{
	for(int i=0;i<Len;i++) printf("%02X ",Buff[i]);
	printf("\n");
}
