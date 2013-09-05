
#include "ACUProtocol.h"
//#include <acstimeEpochHelper.h>
#include <IRATools.h>
#include <math.h>
#include <DateTime.h> 


// little endian 
#define  STARTFLAG 0x1DFCCF1A   
#define  ENDFLAG 0xA1FCCFD1

#define TS_ACUTIME_NAME "ACU"
#define TS_IRIGB_NAME "IRIG-B"
#define TS_EXTERNAL_NAME "EXT"



#define MODE_COMMAND_ID 1
#define PARAMETER_COMMAND_ID 2
#define PROGRAMTRACK_COMMAND_ID 4
#define SYSTEM_COMMAND_ID 3

#define PROGRAMTRACK_INTERPOLATION_MODE 4
#define PROGRAMTRACK_TRACKING_MODE 1
#define PROGRAMTRACK_LOAD_MODE_NEW_TABLE 1
#define PROGRAMTRACK_LOAD_MODE_APPEND_TABLE 2

#define STATUS_BUFFER CACUProtocol::SOCKET_RECV_BUFFER*3

using namespace Protocol_Internal;

CACUProtocol::CACUProtocol()
{
	m_syncBuffer=new CCircularArray(STATUS_BUFFER);
	m_bbufferStarted=false;
}

CACUProtocol::~CACUProtocol()
{
	delete m_syncBuffer;
}

WORD CACUProtocol::preset(const double& azPos,const double& elPos,const double& azRate,const double& elRate,BYTE * & buff,TCommand *& command,WORD& commNumber)
{	
	return modeCommand(MODE_PRESET_ABS,MODE_PRESET_ABS,azPos,azRate,elPos,elRate,buff,command,commNumber);
}

WORD CACUProtocol::rate(const double& azRate,const double& elRate,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	double az,el;
	double azFactor,elFactor;
	if (azRate==0.0) {
		azFactor=0.0;
		az=0.0;
	}
	else {
		if (azRate<0) {
			azFactor=-1.0;
            az=-azRate;
        }
        else {
        	azFactor=1.0;
        	az=azRate;
        }
	}
	if (elRate==0.0) {
		elFactor=0.0;
		el=0.0;
	}
	else {
		if (elRate<0) {
			elFactor=-1.0;
            el=-elRate;
        }
        else {
        	elFactor=1.0;
        	el=elRate;
        }
	}
	return modeCommand(MODE_RATE,MODE_RATE,azFactor,az,elFactor,el,buff,command,commNumber);
}

WORD CACUProtocol::programTrack(const double& azMaxRate,const double& elMaxRate,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	return modeCommand(MODE_PROGRAMTRACK,MODE_PROGRAMTRACK,0.0,azMaxRate,0.0,elMaxRate,buff,command,commNumber);
}

WORD CACUProtocol::activate(bool azimuth,bool elevation,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	if (azimuth && elevation) {
		return modeCommand(MODE_ACTIVE,MODE_ACTIVE,0.0,0.0,0.0,0.0,buff,command,commNumber);
	}
	else if (azimuth) {
		return modeCommand(MODE_ACTIVE,MODE_IGNORE,0.0,0.0,0.0,0.0,buff,command,commNumber);
	}
	else if (elevation) {
		return modeCommand(MODE_IGNORE,MODE_ACTIVE,0.0,0.0,0.0,0.0,buff,command,commNumber);
	}
	else return 0;
}

WORD CACUProtocol::deactivate(bool azimuth,bool elevation,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	if (azimuth && elevation) {
		return modeCommand(MODE_INACTIVE,MODE_INACTIVE,0.0,0.0,0.0,0.0,buff,command,commNumber);
	}
	else if (azimuth) {
		return modeCommand(MODE_INACTIVE,MODE_IGNORE,0.0,0.0,0.0,0.0,buff,command,commNumber);
	}
	else if (elevation) {
		return modeCommand(MODE_IGNORE,MODE_INACTIVE,0.0,0.0,0.0,0.0,buff,command,commNumber);
	}
	else return 0;
}

WORD CACUProtocol::stop(BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	return modeCommand(MODE_STOP,MODE_STOP,0.0,0.0,0.0,0.0,buff,command,commNumber);
}

WORD CACUProtocol::resetErrors(BYTE *& buff)
{
	//return modeCommand(MODE_RESET,MODE_RESET,0.0,0.0,0.0,0.0,buff,command,commNumber);
	return systemCommand(buff);
}

WORD CACUProtocol::stow(const double& stowSpeed,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	return modeCommand(MODE_IGNORE,MODE_DRIVETOSTOW,0.0,0.0,0.0,stowSpeed*0.5,buff,command,commNumber);
}

WORD CACUProtocol::unstow(BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	return modeCommand(MODE_IGNORE,MODE_UNSTOW,0.0,0.0,0.0,0.0,buff,command,commNumber);
}

WORD CACUProtocol::positionOffsets(const double& azOff,const double& elOff,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	BYTE msg[CACUProtocol::SOCKET_SEND_BUFFER];
	BYTE *nextMsg;
	TCommand * nextCommand;
	WORD len=0;
	commNumber=2;
	command=new TCommand[commNumber];
	len=parameterCommand(PAR_ABS_POS_OFFSET,SUBSYSTEM_ID_AZIMUTH,azOff,0.0,msg,command);
	nextMsg=msg+len;
	nextCommand=command+1;
	len+=parameterCommand(PAR_ABS_POS_OFFSET,SUBSYSTEM_ID_ELEVATION,elOff,0.0,nextMsg,nextCommand);	
	return packMessage(msg,len,commNumber,buff);
}

WORD CACUProtocol::loadProgramTrack(const ACS::Time& startEpoch,const TProgramTrackPoint *seq,const WORD& size,bool newTable,const double& azRate,const double& elRate,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	BYTE msg[CACUProtocol::SOCKET_SEND_BUFFER];
	WORD len=0;
	TUINT32 counter;
	double mjd;
	//ACS::Time startUt;
	long long timeDiff;
	long castTimeDiff;
	command=NULL;
	commNumber=1;
	if (newTable && (size<PROGRAMTRACK_TABLE_MINIMUM_LENGTH)) { // for a new table at least five points are required!
		return 0;
	}
	copyData<TUINT16>(msg,PROGRAMTRACK_COMMAND_ID,len);
	copyData<TUINT16>(msg,SUBSYSTEM_ID_POINTING,len);
	counter=getMillisOfTheDay()+1;
	copyData<TUINT32>(msg,counter,len);
	copyData<TUINT16>(msg,PAR_PROGRAM_TRACK_TABLE,len);
	copyData<TUINT16>(msg,PROGRAMTRACK_INTERPOLATION_MODE,len);
	copyData<TUINT16>(msg,PROGRAMTRACK_TRACKING_MODE,len);  // tracking mode: azimuth/elevation
	if (newTable) {
		copyData<TUINT16>(msg,PROGRAMTRACK_LOAD_MODE_NEW_TABLE,len);  // load mode: start a new table
	}
	else {
		copyData<TUINT16>(msg,PROGRAMTRACK_LOAD_MODE_APPEND_TABLE,len);  // load mode: new entries will be attached to the existing table
	}
	copyData<TUINT16>(msg,size,len);  // sequence length
	//startUt=seq[0].timeMark;
	mjd=time2MJD(startEpoch);
	copyData<TREAL64>(msg,mjd,len);  // startTime as modified julian date
	copyData<TREAL64>(msg,azRate,len);  // max speed in azimuth..
	copyData<TREAL64>(msg,elRate,len);  // max speed in elevation...
	for (WORD i=0;i<size;i++) {
		timeDiff=(seq[i].timeMark-startEpoch); // 100 ns
		timeDiff=(timeDiff/10000); // milliseconds
		castTimeDiff=(long)timeDiff;
		copyData<TINT32>(msg,castTimeDiff,len);
		copyData<TREAL64>(msg,seq[i].azimuth,len);
		copyData<TREAL64>(msg,seq[i].elevation,len);
	}
	command=new TCommand;
	command->subsystem=SUBSYSTEM_ID_POINTING;  ///this is due to a difference of new 
	command->command=PAR_PROGRAM_TRACK_TABLE;
	command->counter=counter;
	command->answer=NO_COMMAND;
	command->parameterCommand=true;	
	command->error=false;
	command->executed=false;	
	return packMessage(msg,len,commNumber,buff);	
}

WORD CACUProtocol::setTime(const ACS::Time& time,const IRA::CString& timeSource,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	BYTE msg[CACUProtocol::SOCKET_SEND_BUFFER];
	TIMEVALUE refTime(time);
	IRA::CDateTime dateTime(refTime);
	WORD len=0;
	commNumber=1;
	command=new TCommand;
	len=parameterCommand(PAR_TIME_SOURCE,SUBSYSTEM_ID_POINTING,(double)str2TimeSource(timeSource),(double)dateTime.getMJD(),msg,command);
	return packMessage(msg,len,commNumber,buff);
}

WORD CACUProtocol::setTimeOffset(const double& seconds,BYTE *& buff,TCommand *& command,WORD& commNumber)
{
	BYTE msg[CACUProtocol::SOCKET_SEND_BUFFER];
	WORD len=0;
	double time=seconds/**1000000*/;
	commNumber=1;
	command=new TCommand;
	len=parameterCommand(PAR_PROGRAM_TRACK_TIME_CORRECTION,SUBSYSTEM_ID_POINTING,time,0.0,msg,command);	
	return packMessage(msg,len,commNumber,buff);
}

int CACUProtocol::syncBuffer(BYTE * inBuffer,const WORD& inLen,BYTE *outBuffer)
{
	DWORD start=0;
	DWORD msgLen=0;
	int res;

	m_syncBuffer->add(inBuffer,inLen); // copy the incoming message into the sync buffer
	
	if (!m_bbufferStarted) { // no start byte found yet.....
		if (findStartFlag(start)) {
			m_bbufferStarted=true;			
		}
		// it erase <start> bytes, which is the scanned (without success) bytes from the sync buffer.
		m_syncBuffer->erase(start);
	}
	if (m_bbufferStarted) {
		//the first byte is always the byte 0
		if ((res=findStatusMessageLenght(msgLen))>0) { //success on finding message length
			if ((res=findEndFlag(msgLen))>0) { // check is the stop flag is exactly where expected 
				m_syncBuffer->pop(outBuffer,msgLen);
				m_bbufferStarted=false;
				return msgLen;
			}
			else if (res<0) {
				m_syncBuffer->erase(sizeof(TDWORD)); //discard the bytes of the start flag
				m_bbufferStarted=false;
				return -1;
			}
			else {
				return 0;
			}
		}
		else if (res<0) { // the status message contains errors
			m_syncBuffer->erase(sizeof(TDWORD)); //discard the bytes of the start flag
			m_bbufferStarted=false;
			return -1;
		}
		else { //res==0 buffer could not be completed yet
			return 0;
		}
	}
	return 0;
}

CACUProtocol::TModes CACUProtocol::modesIDL2Protocol(const Antenna::TCommonModes& mode)
{
	switch (mode) {
		case Antenna::ACU_STANDBY: return MODE_INACTIVE;
		case Antenna::ACU_PRESET: return MODE_PRESET_ABS;
		case Antenna::ACU_RATE: return MODE_RATE;
		case Antenna::ACU_STOP: return MODE_STOP;
		case Antenna::ACU_PROGRAMTRACK: return MODE_PROGRAMTRACK;
		case Antenna::ACU_UNSTOW: return MODE_UNSTOW;
		case Antenna::ACU_STOW: return MODE_DRIVETOSTOW;
		default: return MODE_IGNORE;
	}
}

Antenna::TCommonModes CACUProtocol::modesProtocol2IDL(const CACUProtocol::TModes& mode)
{
	switch (mode) {
		case MODE_INACTIVE: return Antenna::ACU_STANDBY; // these next two modes are frequent SRT ACU mode....but is no use for the control system, so I map both into a STANDYBY.
		case MODE_ACTIVE: return Antenna::ACU_STANDBY;
		case MODE_PRESET_ABS: return Antenna::ACU_PRESET;
		case MODE_PRESET_REL: return Antenna::ACU_UNKNOWN;
		case MODE_RATE: return Antenna::ACU_RATE;
		case MODE_POSITIONTRACK: return Antenna::ACU_UNKNOWN;
		case MODE_STOP: return Antenna::ACU_STOP;
		case MODE_PROGRAMTRACK: return Antenna::ACU_PROGRAMTRACK;
		case MODE_RESET: return Antenna::ACU_UNKNOWN;
		case MODE_STOW: return Antenna::ACU_UNKNOWN;
		case MODE_UNSTOW: return Antenna::ACU_UNSTOW;
		case MODE_DRIVETOSTOW: return Antenna::ACU_STOW;
		default: return Antenna::ACU_UNKNOWN;
	}
}

IRA::CString CACUProtocol::modesIDL2String(const Antenna::TCommonModes& mode)
{
	switch (mode) {
		case Antenna::ACU_STANDBY: return "STANDBY";
		case Antenna::ACU_PRESET: return "PRESET";
		case Antenna::ACU_RATE: return "RATE";
		case Antenna::ACU_STOP: return "STOP";
		case Antenna::ACU_PROGRAMTRACK: return "PROGRAM_TRACK";
		case Antenna::ACU_UNSTOW: return "UNSTOW";
		case Antenna::ACU_STOW: return "STOW";
		default: return "UNKNOWN";
	}	
}

IRA::CString CACUProtocol::commandAnswer2String(const TCommandAnswers& answer)
{
	switch (answer) {
		case COMMAND_EXECUTED: return "COMMAND EXECUTED";
		case COMMAND_ACTIVE: return "COMMAND IS ACTIVE";
		case COMMAND_EXECUTION_ERROR: return "ERROR DURING EXECUTION";
		case COMMAND_IN_WRONG_MODE: return "MODE IS WRONG OR ALREADY ACTIVE";
		case COMMAND_INVALID_PARAMETERS: return "INVALID ARGUMENTS";
		case COMMAND_ACCEPTED: return "COMMAND ACCEPTED";
		default: return "NO COMMAND"; //NO_COMMAND
	}
}

double CACUProtocol::time2MJD(const ACS::Time& time)
{
	TIMEVALUE timeVal(time);
	IRA::CDateTime dt(timeVal); // dut1 is always zero...here want to convert a UT to a MJD;
	return (double)dt.getMJD();
}

CACUProtocol::TTimeSources CACUProtocol::str2TimeSource(const IRA::CString ts)
{
	if (ts==TS_ACUTIME_NAME) {
		return TS_ACUTIME;
	}
	else if (ts==TS_IRIGB_NAME) {
		return TS_IRIGB;
	}
	else { // if (ts==TS_EXTERNAL_NAME) {
		return TS_EXTERNAL;
	}
}

WORD CACUProtocol::systemCommand(BYTE *& outBuff) const
{
	BYTE msg[CACUProtocol::SOCKET_SEND_BUFFER];
	WORD len=0;
	TUINT32 counter;
	WORD commNumber=1;
	copyData<TUINT16>(msg,SYSTEM_COMMAND_ID,len);
	copyData<TUINT16>(msg,4,len);
	counter=getMillisOfTheDay()+1;
	copyData<TUINT32>(msg,counter,len);
	copyData<TUINT16>(msg,9,len);
	copyData<TREAL64>(msg,0,len);
	copyData<TREAL64>(msg,0,len);	
	return packMessage(msg,len,commNumber,outBuff);
}


WORD CACUProtocol::modeCommand(const TModes& azMode,const TModes& elMode,const double& azP1,const double& azP2,const double& elP1,const double& elP2,BYTE *& outBuff,TCommand *& command,WORD& commNumber) const
{
	BYTE msg[CACUProtocol::SOCKET_SEND_BUFFER];
	WORD len=0,comm=0;
	TUINT32 counter;
	commNumber=0;
	command=NULL;
	if (azMode!=MODE_IGNORE) {
		commNumber++;
	}
	if (elMode!=MODE_IGNORE) {
		commNumber++;
	}
	command=new TCommand[commNumber];
	if (azMode!=MODE_IGNORE) {
		copyData<TUINT16>(msg,MODE_COMMAND_ID,len);
		copyData<TUINT16>(msg,SUBSYSTEM_ID_AZIMUTH,len);
		counter=getMillisOfTheDay()+1;
		copyData<TUINT32>(msg,counter,len);
		copyData<TUINT16>(msg,azMode,len);
		copyData<TREAL64>(msg,azP1,len);
		copyData<TREAL64>(msg,azP2,len);	
		command[comm].subsystem=SUBSYSTEM_ID_AZIMUTH;
		command[comm].command=azMode;
		command[comm].counter=counter;
		command[comm].answer=NO_COMMAND;
		command[comm].parameterCommand=false;
		command[comm].error=false;
		command[comm].executed=false;
		comm++;
	}
	if (elMode!=MODE_IGNORE) {
		copyData<TUINT16>(msg,MODE_COMMAND_ID,len);
		copyData<TUINT16>(msg,SUBSYSTEM_ID_ELEVATION,len);
		counter=getMillisOfTheDay()+1;
		copyData<TUINT32>(msg,counter,len);	
		copyData<TUINT16>(msg,elMode,len);
		copyData<TREAL64>(msg,elP1,len);
		copyData<TREAL64>(msg,elP2,len);
		command[comm].subsystem=SUBSYSTEM_ID_ELEVATION;
		command[comm].command=elMode;
		command[comm].counter=counter;
		command[comm].answer=NO_COMMAND;
		command[comm].parameterCommand=false;	
		command[comm].error=false;
		command[comm].executed=false;		
	}
	return packMessage(msg,len,commNumber,outBuff);
}

WORD CACUProtocol::parameterCommand(const TParameters& parameterID,const TSubsystems& subSystem,const double& p1,const double& p2,BYTE * outBuff,TCommand* command)
{
	WORD len=0;
	TUINT32 counter;
	copyData<TUINT16>(outBuff,PARAMETER_COMMAND_ID,len);
	copyData<TUINT16>(outBuff,subSystem,len);
	counter=getMillisOfTheDay()+1;
	copyData<TUINT32>(outBuff,counter,len);	
	copyData<TUINT16>(outBuff,parameterID,len);
	copyData<TREAL64>(outBuff,p1,len);
	copyData<TREAL64>(outBuff,p2,len);
	command->subsystem=subSystem;
	command->command=parameterID;
	command->counter=counter;
	command->answer=NO_COMMAND;
	command->parameterCommand=true;	
	command->error=false;
	command->executed=false;	
	return len;
}

WORD CACUProtocol::packMessage(BYTE *msg,const WORD& msgLen,const TUINT32& commands,BYTE *& outBuff) const
{
	TUINT32 bufferLen;
	WORD len=0;
	TUINT32 counter;
	bufferLen=5*sizeof(TUINT32)+msgLen; ///computes the buffer length
	outBuff=NULL;
	if (bufferLen>SOCKET_SEND_BUFFER) {
		return 0;
	}
	outBuff=new BYTE[bufferLen];
	counter=getMillisOfTheDay();
	copyData<TUINT32>(outBuff,STARTFLAG,len); // copy the start flag;
	copyData<TUINT32>(outBuff,bufferLen,len); // now store the length into the buffer;
	copyData<TUINT32>(outBuff,counter,len); //add the command counter as the number of milliseconds of the day
	copyData<TUINT32>(outBuff,commands,len); //add the  number of commands contained in the message
	memcpy((void *)(outBuff+len),(void *)msg,msgLen); // copy the message.....
	len+=msgLen;
	copyData<TUINT32>(outBuff,ENDFLAG,len);
	return bufferLen;
}

void CACUProtocol::matchCommandAnswer(TCommand* command,const WORD& size,const TSubsystems& subsystem,const TUINT16& cmd,const TUINT32& counter,const TCommandAnswers& answer)
{
	if ((answer==NO_COMMAND) || (size==0)) return;
	WORD i=0;
	for (i=0;i<size;i++) {
		if ((command[i].subsystem==subsystem) && (command[i].command==cmd) & (command[i].counter==counter)) { // match the proper answer to the command
			command[i].answer=answer;  // store the answer;
			if (command[i].parameterCommand) { // is a parameter command
				command[i].executed=true;
				if ((answer==COMMAND_IN_WRONG_MODE) || (answer==COMMAND_INVALID_PARAMETERS)) command[i].error=true;
			}
			else {  // mode command
				if ((answer==COMMAND_IN_WRONG_MODE) || (answer==COMMAND_INVALID_PARAMETERS) || (answer==COMMAND_EXECUTION_ERROR)) { // possible error codes, recv or executed command
					command[i].error=true;
					command[i].executed=true;
				}
				else if ((answer==COMMAND_ACTIVE) || (answer==COMMAND_EXECUTED)) {
					command[i].executed=true;
				}
			}
		}
	}
}

bool CACUProtocol::checkCommandAnswer(TCommand* command,const WORD& size,TCommandAnswers& answer,bool& error)
{
	bool executed=true;
	for (WORD i=0;i<size;i++) {
		if (command[i].error) {  // if one single command failed...the all message is considered failed
			error=true;
			answer=command[i].answer;
			return true;
		}
		else {
			executed&=command[i].executed; // if only one of the commands of the message frame did not complete...all the message if not completed yet 
			if (!executed) return false;
		}
	}
	return true; // all commands completed succesfully!!!! 
}

TUINT32 CACUProtocol::getMillisOfTheDay() const
{
	long milliSeconds;
	TIMEVALUE now;
	IRA::CIRATools::getTime(now); // get current time
	milliSeconds=now.hour()*3600+now.minute()*60+now.second(); // integer part of seconds of the day
	milliSeconds*=1000; // converted in millis
	milliSeconds+=now.microSecond()/1000; // now add the millisconds of the current second
	return (TUINT32)milliSeconds;
}

int CACUProtocol::findStatusMessageLenght(DWORD& msgLen)
{
	WORD buffLen=m_syncBuffer->getLenght();
	if (buffLen<sizeof(TDWORD)+sizeof(TUINT32)) return 0;
	TUINT32 len;
	m_syncBuffer->get((BYTE *)&len,sizeof(TDWORD),sizeof(TUINT32));
	if ((len==0) || (len>=STATUS_BUFFER)) return -1;
	msgLen=(DWORD)len;
	return 1;
}

bool CACUProtocol::findStartFlag(DWORD& start) const
{
	TDWORD test;
	WORD len=m_syncBuffer->getLenght();
	if (len<sizeof(TDWORD)) return false;
	while (start<=len-sizeof(TDWORD)) {
		m_syncBuffer->get((BYTE *)&test,start,sizeof(TDWORD));
		if (test==STARTFLAG) {
			return true;
		}
		start++;
	}
	return false;
}

int CACUProtocol::findEndFlag(const DWORD& stop)
{
	TDWORD test;
	WORD len=m_syncBuffer->getLenght();
	if (len<stop) return 0; 
	m_syncBuffer->get((BYTE *)&test,stop-sizeof(TDWORD),sizeof(TDWORD));
	if (test==ENDFLAG) {
		return 1;
	}
	return -1;
}

Protocol_Internal::CCircularArray::CCircularArray(const DWORD& dim): m_dimension(dim), m_head(0), m_tail(0),m_lenght(0)
{
	m_buffer=new BYTE[m_dimension];
	reset();
}

Protocol_Internal::CCircularArray::~CCircularArray()
{
	delete [] m_buffer;
}

void Protocol_Internal::CCircularArray:: add(BYTE *buffer,const DWORD& len)
{
	linearToCircular(buffer,len);
}

void Protocol_Internal::CCircularArray::get(BYTE *outBuffer,const DWORD& index,const DWORD& len) const
{
	circularToLinear(outBuffer,index,len);
}

void Protocol_Internal::CCircularArray::pop(BYTE *outBuffer,const DWORD& len)
{
	circularToLinear(outBuffer,0,len);
	erase(len);
}

void Protocol_Internal::CCircularArray::erase(const DWORD& len)
{
	if (len>=m_lenght) {
		reset();
	}
	else {
		m_head=(m_head+len)%m_dimension;
		m_lenght-=len;
	}
}

void Protocol_Internal::CCircularArray::reset()
{
	m_tail=m_head=0;
	m_lenght=0;
}

DWORD Protocol_Internal::CCircularArray::getLenght() const
{
	return m_lenght;
}

void Protocol_Internal::CCircularArray::linearToCircular(BYTE *linear,const DWORD& len)
{
	for (WORD i=0;i<len;i++) {
		if (m_lenght!=0) m_tail=(m_tail+1)%m_dimension;
		m_buffer[m_tail]=linear[i];
		if (m_lenght<m_dimension) {
			m_lenght++;
		}
		//this case handles the case when the data overlap and the new one must replace the older one. In case of the first element (tail=head) this mechanism must not take place (length==0).
		if ((m_head==m_tail)&&(m_lenght>1)) { 
			m_head=(m_tail+1)%m_dimension;
		}
	}
}

void Protocol_Internal::CCircularArray::circularToLinear(BYTE *linear,const DWORD& start,const DWORD& len) const
{
	DWORD index=0;
	for (WORD i=0;i<len;i++) {
		if (start+i<m_lenght) {
			index=(m_head+start+i)%m_dimension;
			linear[i]=m_buffer[index];
		}
	}
}
