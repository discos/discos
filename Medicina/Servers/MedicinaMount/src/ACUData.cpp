// $Id: ACUData.cpp,v 1.6 2010-10-14 12:08:28 a.orlati Exp $

#include "ACUData.h"
#include <math.h>

CACUData::CACUData()
{
	m_MonitorTime.reset();
	m_lastTime.reset();
	m_lastAzimuthPosition=m_lastElevationPosition=0;
	m_elevationOff=m_azimuthOff=0.0;
	m_commandedAzimuth=m_commandedElevation=0;	
	m_isCommanded=false;
	m_lastCommandedMode=CACUInterface::STANDBY;
}

CACUData::~CACUData()
{
}

void CACUData::setMonitorBuffer(BYTE* buffer,WORD size)
{
	m_lastAzimuthPosition=azimuth();
	m_lastElevationPosition=elevation();
	time(m_lastTime);
	memcpy(m_MonitorData,buffer,size);
}

double CACUData::azimuth() const
{
	int Value;
	memcpy(&Value,(m_MonitorData+10),4);
	return ((double)Value)/1000000.0;
}

double CACUData::elevation() const
{
	int Value;
	memcpy(&Value,(m_MonitorData+14),4);
	return ((double)Value)/1000000.0;	
}

double CACUData::azimuthRate()
{
	TIMEDIFFERENCE tm;
	double diff;
	double rate;
	time(tm);
	if (tm==m_lastTime.value()) return 0;
	diff=azimuth()-m_lastAzimuthPosition;
	tm-=m_lastTime.value();
	rate=diff/((double)IRA::CIRATools::timeMicroSeconds(tm)/1000000.0);
	return rate;	
}

double CACUData::elevationRate()
{
	TIMEDIFFERENCE tm;
	double diff;
	double rate;
	time(tm);
	if (tm==m_lastTime.value()) return 0;
	diff=elevation()-m_lastElevationPosition;
	tm-=m_lastTime.value();
	rate=diff/((double)IRA::CIRATools::timeMicroSeconds(tm)/1000000.0);
	return rate;
}

double CACUData::elevationError() const
{
	int Value;
	memcpy(&Value,(m_MonitorData+22),4);
	return ((double)Value)/1000000.0;
}

double CACUData::azimuthError() const
{
	int Value;
	memcpy(&Value,(m_MonitorData+18),4);
	return ((double)Value)/1000000.0;
}

CACUInterface::TAxeModes CACUData::azimuthMode() const
{
	BYTE Value;
	memcpy(&Value,(m_MonitorData+26),1);
	return CACUInterface::getMode(Value);
}

CACUInterface::TAxeModes CACUData::elevationMode() const
{
	BYTE Value;
	memcpy(&Value,(m_MonitorData+27),1);
	return CACUInterface::getMode(Value);
}

WORD CACUData::elevationServoStatus() const
{
	WORD Value;
	memcpy(&Value,(m_MonitorData+30),2);
	return Value;
}
	
WORD CACUData::azimuthServoStatus() const
{
	WORD Value;
	memcpy(&Value,(m_MonitorData+28),2);
	return Value;	
}

CACUInterface::TAntennaSection CACUData::getSection() const
{
	WORD Value;
	memcpy(&Value,(m_MonitorData+36),2);
	return CACUInterface::getSection(Value);	
}
	
WORD CACUData::getFreeStackPositions() const
{
	WORD Value;
	memcpy(&Value,(m_MonitorData+34),2);
	return Value;		
}

WORD CACUData::servoSystemStatus() const
{
	WORD Value,Tmp;
	Value=0;
	memcpy(&Tmp,(m_MonitorData+32),2);
	if ((Tmp&CACUInterface::DOORINTERLOCK)==CACUInterface::DOORINTERLOCK) Value=Value|1U;
	if ((Tmp&CACUInterface::SAFE)==CACUInterface::SAFE) Value=Value|2U;		
	if ((Tmp&CACUInterface::EMERGENCY)==CACUInterface::EMERGENCY) Value=Value|4U;
	if ((Tmp&CACUInterface::TRANSFERERROR)==CACUInterface::TRANSFERERROR) Value=Value|8U;
	if ((Tmp&CACUInterface::TIMEERROR)==CACUInterface::TIMEERROR) Value=Value|16U;
	if ((Tmp&CACUInterface::RFINHIBIT)==CACUInterface::RFINHIBIT) Value=Value|32U;
	if ((Tmp&CACUInterface::REMOTE)==CACUInterface::REMOTE) Value=Value|64U;
	return Value;		
}

int CACUData::getError() const
{
	WORD servoSystem,elServo,azServo;
	servoSystem=servoSystemStatus();
	elServo=elevationServoStatus();
	azServo=azimuthServoStatus();
	if ((servoSystem&1U)>0) return 2;                               //DOORINTERLOCK
	else if ((servoSystem&2U)>0) return 2;                         //SAFE switch 
	else if ((servoSystem&4U)>0) return 2;                         //EMERGENCY pushbutton
	else if ((servoSystem&64U)==0) return 2;                       // the ACU is not in REMOTE   
	else if ((azServo&CACUInterface::EMERGENCYLIMIT)==CACUInterface::EMERGENCYLIMIT) return 2;   
	else if ((azServo&CACUInterface::OPERLIMITCCW)==CACUInterface::OPERLIMITCCW) return 2;
	else if ((azServo&CACUInterface::OPERLIMITCW)==CACUInterface::OPERLIMITCW) return 2;
	else if ((azServo&CACUInterface::SERVOFAILURE)==CACUInterface::SERVOFAILURE) return 2;
	else if ((azServo&CACUInterface::BRAKEFAILURE)==CACUInterface::BRAKEFAILURE) return 2;
	else if ((azServo&CACUInterface::ENCODERFAILURE)==CACUInterface::ENCODERFAILURE) return 2;
	else if ((azServo&CACUInterface::MOTIONFAILURE)==CACUInterface::MOTIONFAILURE) return 2;
	else if ((azServo&CACUInterface::AXISDISABLED)==CACUInterface::AXISDISABLED) return 2;
	else if ((azServo&CACUInterface::COMPUTERDISABLED)==CACUInterface::COMPUTERDISABLED) return 2;
	else if ((elServo&CACUInterface::EMERGENCYLIMIT)==CACUInterface::EMERGENCYLIMIT) return 2;   
	else if ((elServo&CACUInterface::OPERLIMITCCW)==CACUInterface::OPERLIMITCCW) return 2;
	else if ((elServo&CACUInterface::OPERLIMITCW)==CACUInterface::OPERLIMITCW) return 2;
	else if ((elServo&CACUInterface::SERVOFAILURE)==CACUInterface::SERVOFAILURE) return 2;
	else if ((elServo&CACUInterface::BRAKEFAILURE)==CACUInterface::BRAKEFAILURE) return 2;
	else if ((elServo&CACUInterface::ENCODERFAILURE)==CACUInterface::ENCODERFAILURE) return 2;
	else if ((elServo&CACUInterface::MOTIONFAILURE)==CACUInterface::MOTIONFAILURE) return 2;
	else if ((elServo&CACUInterface::AXISDISABLED)==CACUInterface::AXISDISABLED) return 2;
	else if ((elServo&CACUInterface::COMPUTERDISABLED)==CACUInterface::COMPUTERDISABLED) return 2;
	else if ((azServo&CACUInterface::PRELIMITCCW)==CACUInterface::PRELIMITCCW) return 1;
	else if ((azServo&CACUInterface::PRELIMITCW)==CACUInterface::PRELIMITCW) return 1;
	else if ((azServo&CACUInterface::MOTOROVERSPEED)==CACUInterface::MOTOROVERSPEED) return 1;
	else if ((azServo&CACUInterface::AUXILIARYMODE)==CACUInterface::AUXILIARYMODE) return 1;
	else if ((elServo&CACUInterface::PRELIMITCCW)==CACUInterface::PRELIMITCCW) return 1;
	else if ((elServo&CACUInterface::PRELIMITCW)==CACUInterface::PRELIMITCW) return 1;
	else if ((elServo&CACUInterface::MOTOROVERSPEED)==CACUInterface::MOTOROVERSPEED) return 1;
	else if ((elServo&CACUInterface::AUXILIARYMODE)==CACUInterface::AUXILIARYMODE) return 1;
	else if ((servoSystem&8U)>0) return 1;  // TRANSFERERROR
	else if ((servoSystem&16U)>0) return 1; // TIMEERROR
	else return 0;
}

void CACUData::absTime(TIMEVALUE& tm) const
{
	//*************************************////
	//* This is a fix for an identified issue related to to readout of ACU time.
	// The issue was originated by the 64bit platform when memcpying into a long (8bytes)
	// instead of int (4 bytes)
	//long timeofday,hour,minute,second,micro;
	int timeofday;
	long hour,minute,second,micro;
	short dayofyear;
	memcpy(&timeofday,(m_MonitorData+4),4);
	memcpy(&dayofyear,(m_MonitorData+8),2);
	IRA::CIRATools::getTime(tm);
	hour=timeofday/3600000;
	timeofday=timeofday%3600000;
	minute=timeofday/60000;
	timeofday=timeofday%60000;
	second=timeofday/1000;
	timeofday=timeofday%1000;
	micro=timeofday*1000;
	tm.normalize(true);
	tm.dayOfYear(dayofyear);	
	tm.hour(hour);
	tm.minute(minute);
	tm.second(second);
	tm.microSecond(micro);
}

void CACUData::time(TIMEDIFFERENCE& tm) const
{
	int timeofday;
	long hour,minute,second,micro;
	short dayofyear;
	memcpy(&timeofday,(m_MonitorData+4),4);
	memcpy(&dayofyear,(m_MonitorData+8),2);
	tm.reset();
	hour=timeofday/3600000;
	timeofday=timeofday%3600000;
	minute=timeofday/60000;
	timeofday=timeofday%60000;
	second=timeofday/1000;
	timeofday=timeofday%1000;
	micro=timeofday*1000;
	tm.normalize(true);
	tm.day(dayofyear);	
	tm.hour(hour);
	tm.minute(minute);
	tm.second(second);
	tm.microSecond(micro);
}

bool CACUData::isTracking(double Precision) const
{
	double skyErr,azErr,elErr;
	CACUInterface::TAxeModes azMode,elMode;
	if (getError()>1) return false;
	azErr=azimuthError();
	elErr=elevationError();
	azMode=azimuthMode();
	elMode=elevationMode();
	azErr=azErr*cos(elErr*DEG2RAD);
	skyErr=sqrt(azErr*azErr+elErr*elErr);
	if ((azMode!=CACUInterface::PRESET) && (azMode!=CACUInterface::PROGRAMTRACK) && (azMode!=CACUInterface::RATE) && (azMode!=CACUInterface::STARTRACK)
	  && (azMode!=CACUInterface::POSITIONTRACK)) {
		return false;	
	}
	if ((elMode!=CACUInterface::PRESET) && (elMode!=CACUInterface::PROGRAMTRACK) && (elMode!=CACUInterface::RATE) && (elMode!=CACUInterface::STARTRACK)
	  && (elMode!=CACUInterface::POSITIONTRACK)) {
		return false;	
	}	
	if(skyErr<=Precision) {
		return true;
	}
	else {
		return false;
	}
}
