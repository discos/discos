// $Id: Configuration.cpp,v 1.7 2011-05-20 16:53:09 a.orlati Exp $

#include "Configuration.h"

#define _GET_DWORD_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	DWORD tmpw; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmpw)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpw; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %lu",tmpw); \
	} \
}

#define _GET_STRING_ATTRIBUTE(ATTRIB,DESCR,FIELD) { \
	CString tmps; \
	if (!CIRATools::getDBValue(Services,ATTRIB,tmps)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"::CConfiguration::Init()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmps; \
		ACS_DEBUG_PARAM("CConfiguration::Init()",DESCR" %s",(const char*)tmps); \
	} \
}

CConfiguration::CConfiguration()
{
	m_procTable=NULL;
}

CConfiguration::~CConfiguration()
{
}

void CConfiguration::readProcedures(maci::ContainerServices *services,const IRA::CString& procedureFile,ACS::stringSeq& names,ACS::longSeq& args,ACS::stringSeq *&bodies) throw (
		ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl)
{
	IRA::CError err;
	err.Reset();
	m_procTable=new IRA::CDBTable(services,"Procedure",m_proceduresLocation+"/"+procedureFile);
	if (!m_procTable->addField(err,"name",CDataField::STRING)) {
		err.setExtra("Error adding field name",0);
	}
	if (!m_procTable->addField(err,"args",CDataField::LONGLONG)) {
		err.setExtra("Error adding field args",0);
	}
	else if (!m_procTable->addField(err,"body",CDataField::STRING)) {
		err.setExtra("Error adding field body",0);
	}
	if (!err.isNoError()) {
		delete m_procTable;
		m_procTable=NULL;
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,err);
		dummy.setCode(err.getErrorCode());
		dummy.setDescription((const char*)err.getDescription());
		throw dummy;				
	}
	if (!m_procTable->openTable(err))	{
		delete m_procTable;
		m_procTable=NULL;		
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl,dummy,err);
		throw dummy;
	}
	m_procTable->First();
	bodies=new ACS::stringSeq[m_procTable->recordCount()];
	names.length(m_procTable->recordCount());
	args.length(m_procTable->recordCount());
	for (unsigned i=0;i<m_procTable->recordCount();i++) {
		names[i]=(const char*)((*m_procTable)["name"]->asString());
		args[i]=(long)((*m_procTable)["args"]->asLongLong());
		extractBody((*m_procTable)["body"]->asString(),bodies[i]);
		m_procTable->Next();
	}
	m_procTable->closeTable();
	delete m_procTable;
	m_procTable=NULL;
}

void CConfiguration::init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl)
{
	IRA::CString check;
	_GET_STRING_ATTRIBUTE("AntennaBossInterface","Antenna Boss component interface is ",m_antennaBossComp);
	_GET_STRING_ATTRIBUTE("ObservatoryInterface","Observatory component interface is ",m_observatoryComp);
	_GET_STRING_ATTRIBUTE("ReceiversBossInterface","Receivers Boss component interface is ",m_receiversBossComp);
	_GET_STRING_ATTRIBUTE("MinorServoBossInterface","Minor Servo Boss component interface is ",m_minorServoBossComp);
	_GET_STRING_ATTRIBUTE("CustomLoggerInterface","Custom logger component interface is ",m_custoLoggerComp);
	_GET_STRING_ATTRIBUTE("WeatherStationInstance","Weather station component interface is ",m_weatherComp);
	_GET_STRING_ATTRIBUTE("DefaultBackendInstance","Default backend component instance is ",m_defaultBackendInst);
	_GET_STRING_ATTRIBUTE("DefaultDataReceiverInstance","Default data receiver component instance is ",m_defaultDataReceiverInst);
	_GET_STRING_ATTRIBUTE("SchedDir","Schedule files path is ",m_schedDir);
	_GET_STRING_ATTRIBUTE("DataDir","Data files path is ",m_dataDir);
	_GET_STRING_ATTRIBUTE("SystemDataDir","System data files path is ",m_systemDataDir);
	_GET_STRING_ATTRIBUTE("LogDir","Log files path is ",m_logDir);
	_GET_STRING_ATTRIBUTE("SystemLogDir","System log files path is ",m_systemLogDir);
	_GET_DWORD_ATTRIBUTE("TsysTransitionGap","Gap time between mark switch on/off (uSec) ",m_TsysGapTime);
	_GET_DWORD_ATTRIBUTE("RepetitionCacheTime","Log repetition filter cache time (uSec) ",m_repetitionCacheTime);
	_GET_DWORD_ATTRIBUTE("RepetitionExpireTime","Log repetition filter expire time (uSec) ",m_repetitionExpireTime);
	_GET_DWORD_ATTRIBUTE("ScheduleExecutorSleepTime","Schedule executor sleep time (uSec) ",m_scheduleExecutorSleepTime);
	_GET_STRING_ATTRIBUTE("ProceduresLocation","The CDB location of procedures is",m_proceduresLocation);
	_GET_STRING_ATTRIBUTE("DefaultProceduresFile","The default procedures file is ",m_defaultProceduresFile);
	_GET_STRING_ATTRIBUTE("DefaultProjectCode","The default project code is ",m_defaultProjectCode);
	_GET_STRING_ATTRIBUTE("CheckProjectCode","Check new project codes: ",check);
	check.MakeUpper();
	if (check=="TRUE") {
		m_checkProjectCode=true;
	}
	else {
		m_checkProjectCode=false;
	}
}

void CConfiguration::extractBody(const IRA::CString& body,ACS::stringSeq& commands)
{
	long len=0;
	int start=0;
	IRA::CString token;
	//this methods is not really efficient...I use a double pass algorithm but since I do not know the length of commands I prefer doing that way instead of
	// allocating and deallocating the commands size once each iteration....
	//computes the number of commands inside the body.....
	while (CIRATools::getNextToken(body,start,'\n',token)) {
		if (token.GetLength()>0) {
			len++;
		}
	}
	commands.length(len);
	//now extract and condition the commands...
	start=0;
	len=0;
	while (CIRATools::getNextToken(body,start,'\n',token)) {
		if (token.GetLength()>0) {
			token.ReplaceAll('\t',' ');
			token.RTrim();
			token.LTrim();
			commands[len]=(const char *)token;
			len++;
		}
	}
}
