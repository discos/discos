// $Id: CommonData.cpp,v 1.8 2011-06-01 18:24:44 a.orlati Exp $

#include "CommonData.h"
#include <LogFilter.h>

using namespace IRA;

_IRA_LOGFILTER_IMPORT;

CCommonData::CCommonData() : m_statusBuffer(NULL), m_trackStack(CACUProtocol::PROGRAMTRACK_STACK_POSITIONS,false)
{
	DWORD size=CACUProtocol::MESSAGE_FRAME_START_BYTE;
	
	m_statusBuffer=new BYTE[CACUProtocol::SOCKET_RECV_BUFFER];
	
	m_generalStatus=new CACUProtocol::TGeneralStatus(m_statusBuffer,size);
	size+=CACUProtocol::TGeneralStatus::SIZE;
	m_azimuthStatus=new CACUProtocol::TAxisStatus(m_statusBuffer,size);
	size+=CACUProtocol::TAxisStatus::SIZE;
	m_elevationStatus=new CACUProtocol::TAxisStatus(m_statusBuffer,size);
	size+=CACUProtocol::TAxisStatus::SIZE;
	m_cableWrap=new CACUProtocol::TAxisStatus(m_statusBuffer,size);
	size+=CACUProtocol::TAxisStatus::SIZE;
	for (int i=0;i<CACUProtocol::AZIMUTH_MOTORS;i++) {
		m_azimuthMotors[i]=new CACUProtocol::TMotorStatus(m_statusBuffer,size);
		size+=CACUProtocol::TMotorStatus::SIZE;
	}
	for (int i=0;i<CACUProtocol::ELEVATION_MOTORS;i++) {
		m_elevationMotors[i]=new CACUProtocol::TMotorStatus(m_statusBuffer,size);
		size+=CACUProtocol::TMotorStatus::SIZE;		
	}
	m_cableWrapMotor=new CACUProtocol::TMotorStatus(m_statusBuffer,size);
	size+=CACUProtocol::TMotorStatus::SIZE;
	m_pointingStatus=new CACUProtocol::TPointingStatus(m_statusBuffer,size);
	size+=CACUProtocol::TPointingStatus::SIZE;

	m_bBusy=false;
	m_statusSocketState=m_controlSocketState=Antenna::ACU_NOTCNTD;
	m_lastCommandSize=0;
	m_commandedAzimuthMode=m_commandedElevationMode=Antenna::ACU_UNKNOWN;
	m_mountStatus=Management::MNG_OK;
	m_commandedAzOff=m_commandedElOff=0.0;
	m_commandedSection=Antenna::ACU_NEUTRAL;
	m_trackStack.empty();
	m_statusWord=m_azimuthStatusWord=m_elevationStatusWord=0;
	for (WORD j=0;j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1;++j) m_motorsStatusWord[j]=0; 
}

CCommonData::~CCommonData()
{
	delete m_generalStatus;
	delete m_azimuthStatus;
	delete m_elevationStatus;
	delete m_cableWrap;
	delete m_pointingStatus;
	for (int i=0;i<CACUProtocol::AZIMUTH_MOTORS;i++) {
		delete m_azimuthMotors[i];
	}	
	for (int i=0;i<CACUProtocol::ELEVATION_MOTORS;i++) {
		delete m_elevationMotors[i];
	}		
	delete m_cableWrapMotor;
	if (m_statusBuffer) {
		delete [] m_statusBuffer;
		m_statusBuffer=NULL;
	}
}

void CCommonData::setStatusLineState(const Antenna::TStatus& s)
{
	m_statusSocketState=s;
}

void CCommonData::setControlLineState(const Antenna::TStatus& s)
{
	// the busy flag is set only of the previous state was ACU_CNTD...otherwise is ignored
	if (s==Antenna::ACU_BSY) {
		if (m_controlSocketState==Antenna::ACU_CNTD) {
			m_bBusy=true;
		}
	}
	else {
		m_controlSocketState=s;
	}	
}

const Antenna::TStatus CCommonData::getControlLineState() const
{
	if (m_bBusy && (m_controlSocketState==Antenna::ACU_CNTD)) {
		return Antenna::ACU_BSY;
	}
	else {
		return m_controlSocketState;
	}
}

void CCommonData::getActualMode(Antenna::TCommonModes& azMode,Antenna::TCommonModes& elMode) const
{
	CACUProtocol::TCommandAnswers azModeAnswer,elModeAnswer;
	azModeAnswer=m_azimuthStatus->modeCommandStatus().executedModeCommandAnswer();
	elModeAnswer=m_elevationStatus->modeCommandStatus().executedModeCommandAnswer();
	if ((azModeAnswer==CACUProtocol::COMMAND_EXECUTED) || (azModeAnswer==CACUProtocol::COMMAND_ACTIVE)) {
		azMode=CACUProtocol::modesProtocol2IDL(m_azimuthStatus->modeCommandStatus().executedModeCommand());
	}
	else {
		azMode=Antenna::ACU_UNKNOWN;
	}
	if ((elModeAnswer==CACUProtocol::COMMAND_EXECUTED) || (elModeAnswer==CACUProtocol::COMMAND_ACTIVE)) {
		elMode=CACUProtocol::modesProtocol2IDL(m_elevationStatus->modeCommandStatus().executedModeCommand());
	}
	else {
		elMode=Antenna::ACU_UNKNOWN;
	}	
}

void CCommonData::addProgramTrackStackPoint(const double& az,const double& el,const ACS::Time& time)
{
	TIMEVALUE timeVal(time);
	m_trackStack.addPoint(az,el,timeVal);
}

void CCommonData::getLastProgramTrackPoint(double& az,double& el) const 
{ 
	az=m_trackStack.getLastAzimuth(); 
	el=m_trackStack.getLastElevation(); 
}

void CCommonData::storeLastCommand(const CACUProtocol::TCommand* command,const WORD& size)
{
	for (WORD i=0;i<size;i++) m_lastCommand[i]=command[i];
	m_lastCommandSize=size;
}

bool CCommonData::checkLastCommand(CACUProtocol::TCommandAnswers& answer,bool& error)
{
	return CACUProtocol::checkCommandAnswer(m_lastCommand,m_lastCommandSize,answer,error);
}

void CCommonData::getMotorsPosition(ACS::doubleSeq& positions)
{
	positions.length(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1);
	for (WORD j=0;j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1;++j) {
		if (j<CACUProtocol::AZIMUTH_MOTORS) {
			positions[j]=m_azimuthMotors[j]->actualPosition();
		}
		else if (j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS) {
			positions[j]=m_elevationMotors[j-CACUProtocol::AZIMUTH_MOTORS]->actualPosition();
		}
		else {
			positions[j]=m_cableWrapMotor->actualPosition();
		}
	}
}

void CCommonData::getMotorsSpeed(ACS::doubleSeq& speeds)
{
	speeds.length(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1);
	for (WORD j=0;j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1;++j) {
		if (j<CACUProtocol::AZIMUTH_MOTORS) {
			speeds[j]=m_azimuthMotors[j]->actualVelocity();
		}
		else if (j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS) {
			speeds[j]=m_elevationMotors[j-CACUProtocol::AZIMUTH_MOTORS]->actualVelocity();
		}
		else {
			speeds[j]=m_cableWrapMotor->actualVelocity();
		}
	}
}

void CCommonData::getMotorsStatus(ACS::longSeq& status)
{
	status.length(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1);
	for (WORD j=0;j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1;++j) {
		status[j]=m_motorsStatusWord[j];
	}
}

void CCommonData::getMotorsTorque(ACS::doubleSeq& torques)
{
	torques.length(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1);
	for (WORD j=0;j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1;++j) {
		if (j<CACUProtocol::AZIMUTH_MOTORS) {
			torques[j]=m_azimuthMotors[j]->actualTorque();
		}
		else if (j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS) {
			torques[j]=m_elevationMotors[j-CACUProtocol::AZIMUTH_MOTORS]->actualTorque();
		}
		else {
			torques[j]=m_cableWrapMotor->actualTorque();
		}
	}
}

void CCommonData::getMotorsUtilization(ACS::doubleSeq& utilization)
{
	utilization.length(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1);
	for (WORD j=0;j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS+1;++j) {
		if (j<CACUProtocol::AZIMUTH_MOTORS) {
			utilization[j]=m_azimuthMotors[j]->utilization();
		}
		else if (j<CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS) {
			utilization[j]=m_elevationMotors[j-CACUProtocol::AZIMUTH_MOTORS]->utilization();
		}
		else {
			utilization[j]=m_cableWrapMotor->utilization();
		}
	}
}

bool CCommonData::checkIsStowed() const
{
	//We just check the elevation since SRT can stow and park only the elevation axis
	// if the telescope is not stowed and the stowpins extracted......
	return (!m_elevationStatus->warnings().stowPinExtracted() && (m_elevationStatus->stowed()));
}

bool CCommonData::checkIsUnstowed() const
{
	//We just check the elevation since SRT can stow and park only the elevation axis
	// if the telescope stowpins are extracted......
	return m_elevationStatus->warnings().stowPinExtracted();	
}

					
void CCommonData::getAntennaErrors(double& azErr,double& elErr,ACS::Time& time)
{
	azErr=azimuthStatus()->commandedPosition()-azimuthStatus()->actualPosition();
	elErr=elevationStatus()->commandedPosition()-elevationStatus()->actualPosition();
	time=pointingStatus()->actualTime();
}

double CCommonData::getHWAzimuth(double destination,const Antenna::TSections& commandedSection,double azLowerLimit,double azUpperLimit,double azCwLimit)
{	
	double currentAz=azimuthStatus()->actualPosition(); // this should be comprehensive of the offsets
	double dest,azOff;
	long section;
	azOff=azimuthStatus()->positionOffset(); // get the commanded offsets in order to add the azimuth offsets to the destination 
	dest=destination+azOff;
	if (commandedSection==Antenna::ACU_CCW) section=-1;
	else if (commandedSection==Antenna::ACU_CW) section=1;
	else section=0;
	return IRA::CIRATools::getHWAzimuth(currentAz,dest,azLowerLimit,azUpperLimit,section,azCwLimit);	
}

void CCommonData::getEncodersCoordinates(double& az,double& el,double& azOff,double& elOff,ACS::Time& time,Antenna::TSections& section)
{
	az=azimuthStatus()->actualPosition();
	el=elevationStatus()->actualPosition();
	azOff=azimuthStatus()->positionOffset();
	elOff=elevationStatus()->positionOffset();
	time=pointingStatus()->actualTime();
	section=pointingStatus()->azimuthSector();
}
						
void CCommonData::reBind()
{
	//* Azimith Mode command status
	CACUProtocol::matchCommandAnswer(m_lastCommand,m_lastCommandSize,CACUProtocol::SUBSYSTEM_ID_AZIMUTH,
																	 m_azimuthStatus->modeCommandStatus().recvModeCommand(),
																	 m_azimuthStatus->modeCommandStatus().recvModeCommandCounter(),
																	 m_azimuthStatus->modeCommandStatus().recvModeCommandAnswer());
	
	CACUProtocol::matchCommandAnswer(m_lastCommand,m_lastCommandSize,CACUProtocol::SUBSYSTEM_ID_AZIMUTH,
																	 m_azimuthStatus->modeCommandStatus().executedModeCommand(),
																	 m_azimuthStatus->modeCommandStatus().executedModeCommandCounter(),
																	 m_azimuthStatus->modeCommandStatus().executedModeCommandAnswer());
	
	//* azimuth parameter command status
	CACUProtocol::matchCommandAnswer(m_lastCommand,m_lastCommandSize,CACUProtocol::SUBSYSTEM_ID_AZIMUTH,
																	 m_azimuthStatus->parameterCommandStatus().parameterCommand(),
																	 m_azimuthStatus->parameterCommandStatus().parameterCommandCounter(),
																	 m_azimuthStatus->parameterCommandStatus().parameterCommandAnswer());

	//* elevation Mode command status
	
	CACUProtocol::matchCommandAnswer(m_lastCommand,m_lastCommandSize,CACUProtocol::SUBSYSTEM_ID_ELEVATION,
																	 m_elevationStatus->modeCommandStatus().recvModeCommand(),
																	 m_elevationStatus->modeCommandStatus().recvModeCommandCounter(),
																	 m_elevationStatus->modeCommandStatus().recvModeCommandAnswer());
	
	CACUProtocol::matchCommandAnswer(m_lastCommand,m_lastCommandSize,CACUProtocol::SUBSYSTEM_ID_ELEVATION,
																	 m_elevationStatus->modeCommandStatus().executedModeCommand(),
																	 m_elevationStatus->modeCommandStatus().executedModeCommandCounter(),
																	 m_elevationStatus->modeCommandStatus().executedModeCommandAnswer());

	//* elevation parameter command status
	CACUProtocol::matchCommandAnswer(m_lastCommand,m_lastCommandSize,CACUProtocol::SUBSYSTEM_ID_ELEVATION,
																	m_elevationStatus->parameterCommandStatus().parameterCommand(),
																	m_elevationStatus->parameterCommandStatus().parameterCommandCounter(),
																	m_elevationStatus->parameterCommandStatus().parameterCommandAnswer());

	// * the pointing parameter command status
	CACUProtocol::matchCommandAnswer(m_lastCommand,m_lastCommandSize,CACUProtocol::SUBSYSTEM_ID_POINTING,
																	m_pointingStatus->parameterCommandStatus().parameterCommand(),
																	m_pointingStatus->parameterCommandStatus().parameterCommandCounter(),
																	m_pointingStatus->parameterCommandStatus().parameterCommandAnswer());	

	// bind general status word
	if ((m_generalStatus->master()==2)) { 
		clearStatusWord(REMOTE_CONTROL_DISABLED);
	}
	else {
		setStatusWord(REMOTE_CONTROL_DISABLED);
	}
	if (m_generalStatus->software_interlock().emergencyStop()) {
		setStatusWord(EMERGENCY_STOP);
	}
	else {
		clearStatusWord(EMERGENCY_STOP);
	}
	if (m_generalStatus->software_interlock().mainPower()) {
		setStatusWord(MAIN_POWER_ERROR);
	}
	else {
		clearStatusWord(MAIN_POWER_ERROR);
	}
	if (m_pointingStatus->clockOnLine() && m_pointingStatus->clockOK()) {
		clearStatusWord(TIME_ERROR);
	}
	else {
		setStatusWord(TIME_ERROR);
	}
	CACUProtocol::TErrorsTracking ptErr=m_pointingStatus->pointingErrors();
	if (ptErr.dataOverflow() || ptErr.timeDistanceFault() || ptErr.noDataAvailable()) {
		setStatusWord(PROGRAM_TRACK_DATA_ERROR);
	}
	else {
		clearStatusWord(PROGRAM_TRACK_DATA_ERROR);
	}
	// bind the azimuth status word
	setAzimuthStatusWord(PRELIMIT_UP,m_azimuthStatus->warnings().preLimitUp());
	setAzimuthStatusWord(PRELIMIT_DOWN,m_azimuthStatus->warnings().preLimitDown());
	setAzimuthStatusWord(FINAL_LIMIT_UP,m_azimuthStatus->warnings().finLimitUp());
	setAzimuthStatusWord(FINAL_LIMIT_DOWN,m_azimuthStatus->warnings().finLimitDown());
	setAzimuthStatusWord(RATE_LIMIT,m_azimuthStatus->warnings().rateLimit());
	setAzimuthStatusWord(STOW_PIN_EXTRACTED,m_azimuthStatus->warnings().stowPinExtracted());
	setAzimuthStatusWord(ENCODER_FAILURE,m_azimuthStatus->errors().encoderFailure());
	setAzimuthStatusWord(BRAKE_ERROR,m_azimuthStatus->errors().brakeError());
	setAzimuthStatusWord(SERVO_ERROR,m_azimuthStatus->errors().servoError());
	setAzimuthStatusWord(AXIS_READY,m_azimuthStatus->axisReady());
	setAzimuthStatusWord(ACTIVE,m_azimuthStatus->axisState()==CACUProtocol::STATE_ACTIVE);
	setAzimuthStatusWord(LOW_POWER_MODE,m_azimuthStatus->lowPowerMode());
	setAzimuthStatusWord(STOWED,m_azimuthStatus->stowed());
	// bind the azimuth status word
	setElevationStatusWord(PRELIMIT_UP,m_elevationStatus->warnings().preLimitUp());
	setElevationStatusWord(PRELIMIT_DOWN,m_elevationStatus->warnings().preLimitDown());
	setElevationStatusWord(FINAL_LIMIT_UP,m_elevationStatus->warnings().finLimitUp());
	setElevationStatusWord(FINAL_LIMIT_DOWN,m_elevationStatus->warnings().finLimitDown());
	setElevationStatusWord(RATE_LIMIT,m_elevationStatus->warnings().rateLimit());
	setElevationStatusWord(STOW_PIN_EXTRACTED,m_elevationStatus->warnings().stowPinExtracted());
	setElevationStatusWord(ENCODER_FAILURE,m_elevationStatus->errors().encoderFailure());
	setElevationStatusWord(BRAKE_ERROR,m_elevationStatus->errors().brakeError());
	setElevationStatusWord(SERVO_ERROR,m_elevationStatus->errors().servoError());
	setElevationStatusWord(AXIS_READY,m_elevationStatus->axisReady());
	setElevationStatusWord(ACTIVE,m_azimuthStatus->axisState()==CACUProtocol::STATE_ACTIVE);
	setElevationStatusWord(LOW_POWER_MODE,m_elevationStatus->lowPowerMode());
	setElevationStatusWord(STOWED,m_elevationStatus->stowed());
	//bind motors status word
	TWORD motorSelection,brakesOpen,powerModuleOk;
	motorSelection=m_azimuthStatus->motorSelection();
	brakesOpen=m_azimuthStatus->brakesOpen();
	powerModuleOk=m_azimuthStatus->powerModuleOk();	
	for(WORD i=0;i<CACUProtocol::AZIMUTH_MOTORS;i++) {
		setMotorsStatusWord(i,MOTOR_SELECTED,(motorSelection & (1 << i))!=0);
		setMotorsStatusWord(i,MOTOR_BRAKE_OPEN,(brakesOpen & (1 << i))!=0);
		setMotorsStatusWord(i,POWER_MODULE_ERROR,(powerModuleOk & (1 << i))==0);
		setMotorsStatusWord(i,MOTOR_ACTIVE,m_azimuthMotors[i]->active());
		setMotorsStatusWord(i,MOTOR_SERVO_ERROR,m_azimuthMotors[i]->servoError());
		setMotorsStatusWord(i,SENSOR_ERROR,m_azimuthMotors[i]->sensorError());
		setMotorsStatusWord(i,BUS_ERROR,m_azimuthMotors[i]->busError());
		//setMotorsStatusWord(i,POSITION_ERROR,m_azimuthMotors[i]->positionError());
	}
	motorSelection=m_elevationStatus->motorSelection();
	brakesOpen=m_elevationStatus->brakesOpen();
	powerModuleOk=m_elevationStatus->powerModuleOk();	
	for(WORD i=0;i<CACUProtocol::ELEVATION_MOTORS;i++) {
		setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+i,MOTOR_SELECTED,(motorSelection & (1 << i))!=0);
		setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+i,MOTOR_BRAKE_OPEN,(brakesOpen & (1 << i))!=0);
		setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+i,POWER_MODULE_ERROR,(powerModuleOk & (1 << i))==0);
		setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+i,MOTOR_ACTIVE,m_elevationMotors[i]->active());
		setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+i,MOTOR_SERVO_ERROR,m_elevationMotors[i]->servoError());
		setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+i,SENSOR_ERROR,m_elevationMotors[i]->sensorError());
		setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+i,BUS_ERROR,m_elevationMotors[i]->busError());
		//setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+i,POSITION_ERROR,m_elevationMotors[i]->positionError());
	}
	motorSelection=m_cableWrap->motorSelection();
	brakesOpen=m_cableWrap->brakesOpen();
	powerModuleOk=m_cableWrap->powerModuleOk();	
	setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS,MOTOR_SELECTED,(motorSelection & (1 << 0))!=0);
	setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS,MOTOR_BRAKE_OPEN,(brakesOpen & (1 << 0))!=0);
	setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS,POWER_MODULE_ERROR,(powerModuleOk & (1 << 0))==0);
	setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS,MOTOR_ACTIVE,m_cableWrapMotor->active());
	setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS,MOTOR_SERVO_ERROR,m_cableWrapMotor->servoError());
	setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS,SENSOR_ERROR,m_cableWrapMotor->sensorError());
	setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS,BUS_ERROR,m_cableWrapMotor->busError());
	//setMotorsStatusWord(CACUProtocol::AZIMUTH_MOTORS+CACUProtocol::ELEVATION_MOTORS,POSITION_ERROR,m_cableWrapMotor->positionError());
	//now let's compose the overall status..get it from : 
	// m_statusSocketState;
	// m_controlSocketState;
	// m_statusWord;
	// m_azimuthStatusWord;
	// m_elevationStatusWord;
	m_mountStatus=Management::MNG_OK; // first of all we reset the status to everything is OK.
	if ((m_statusSocketState==Antenna::ACU_NOTCNTD) || (m_statusSocketState==Antenna::ACU_CNTDING)) {
		setMountStatus(Management::MNG_FAILURE);
	}
	if ((m_controlSocketState==Antenna::ACU_NOTCNTD) || (m_controlSocketState==Antenna::ACU_CNTDING)) {
		setMountStatus(Management::MNG_FAILURE);
	}	
	if (m_statusWord & (1 << STATUS_MESSAGE_SYNC_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","CANNOT_SYNC_STATUS_MESSAGE");
	}
	if (m_statusWord & (1 << STATUS_LINE_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","STATUS_SOCKET_ERROR");
	}
	if (m_statusWord & (1 << CONTROL_LINE_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","CONTROL_SOCKET_ERROR");
	}
	if (m_statusWord & (1 << REMOTE_CONTROL_DISABLED)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","ACU_REMOTE_CONTROL_DISABLED");
	}
	if (m_statusWord & (1 << EMERGENCY_STOP)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","EMERGENCY_STOP");
	}
	if (m_statusWord & (1 << MAIN_POWER_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","MAIN_POWER_ERROR");
	}
	if (m_statusWord & (1 << TIME_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","TIME_ERROR");
	}
	if (m_statusWord & (1 << PROGRAM_TRACK_DATA_ERROR)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","ERROR_IN_PROGRAM_TRACK_DATA");
	}
	if (m_statusWord & (1 << REMOTE_COMMAND_ERROR)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","REMOTE_COMMAND_ERROR");
	}
	if (m_azimuthStatusWord & (1 << LOW_POWER_MODE)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","AZIMUTH_LOW_POWER_MODE");
	}
	if (m_azimuthStatusWord & (1 << PRELIMIT_UP)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","AZIMUTH_AGAINST_UPPER_PRELIMIT");
	}
	if (m_azimuthStatusWord & (1 << PRELIMIT_DOWN)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","AZIMUTH_AGAINST_LOWER_PRELIMIT");
	}
	if (m_azimuthStatusWord & (1 << FINAL_LIMIT_UP)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","AZIMUTH_AGAINST_UPPER_FINALLIMIT");
	}
	if (m_azimuthStatusWord & (1 << FINAL_LIMIT_DOWN)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","AZIMUTH_AGAINST_LOWER_FINALLIMIT");
	}
	if (m_azimuthStatusWord & (1 << RATE_LIMIT)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","AZIMUTH_BEYOUND_RATE_LIMIT");
	}
	if (m_azimuthStatusWord & (1 << ENCODER_FAILURE)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","AZIMUTH_ENCODER_FAILURE");
	}
	if (m_azimuthStatusWord & (1 << BRAKE_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","AZIMUTH_BRAKES_FAILURE");
	}
	if (m_azimuthStatusWord & (1 << SERVO_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","AZIMUTH_SERVO_FAILURE");
	}
	if (m_elevationStatusWord & (1 << LOW_POWER_MODE)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","ELEVATION_LOW_POWER_MODE");
	}
	if ((m_elevationStatusWord & (1 << PRELIMIT_UP)) && !checkIsStowed()) { //the ACU raise this warning also when the Antenna is stowed so we want to avoid to do the same
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","ELEVATION_AGAINST_UPPER_PRELIMIT");
	}
	if (m_elevationStatusWord & (1 << PRELIMIT_DOWN)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","ELEVATION_AGAINST_LOWER_PRELIMIT");
	}
	if (m_elevationStatusWord & (1 << FINAL_LIMIT_UP)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","ELEVATION_AGAINST_UPPER_FINALLIMIT");
	}
	if (m_elevationStatusWord & (1 << FINAL_LIMIT_DOWN)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","ELEVATION_AGAINST_LOWER_FINALLIMIT");
	}
	if (m_elevationStatusWord & (1 << RATE_LIMIT)) {
		setMountStatus(Management::MNG_WARNING);
		_IRA_LOGFILTER_LOG(LM_WARNING,"CCommonData::reBind()","ELEVATION_BEYOUND_RATE_LIMIT");
	}
	if (m_elevationStatusWord & (1 << ENCODER_FAILURE)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","ELEVATION_ENCODER_FAILURE");
	}
	if (m_elevationStatusWord & (1 << BRAKE_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","ELEVATION_BRAKES_FAILURE");
	}
	if (m_azimuthStatusWord & (1 << SERVO_ERROR)) {
		setMountStatus(Management::MNG_FAILURE);
		_IRA_LOGFILTER_LOG(LM_CRITICAL,"CCommonData::reBind()","ELEVATION_SERVO_FAILURE");
	}
}
