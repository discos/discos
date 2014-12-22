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

#define MAX_AXIS_NUMBER 32

CConfiguration::CConfiguration()
{
	m_procTable=NULL;
	m_minorServoMappings=0;
	m_axis=NULL;
}

CConfiguration::~CConfiguration()
{
	if (m_axis!=NULL) {
		delete [] m_axis;
	}
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

IRA::CString CConfiguration::getServoNameFromAxis(const Management::TScanAxis& axis) const
{
	for (WORD i=0;i<m_minorServoMappings;i++) {
		if (m_axis[i].axis==axis) {
			return m_axis[i].servoName;
		}
	}
	return "";
}

Management::TScanAxis CConfiguration::getAxisFromServoName(const IRA::CString& servoName) const
{
	for (WORD i=0;i<m_minorServoMappings;i++) {
		if (m_axis[i].servoName==servoName) {
			return m_axis[i].axis;
		}
	}
	return Management::MNG_NO_AXIS;
}

void CConfiguration::init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl)
{
	IRA::CString check;
	IRA::CString componentName;
	IRA::CString strVal;
	IRA::CString fieldPath;
	componentName=Services->getName();
	componentName=componentName+"/MinorServoMapping";

	try {
		m_axis=new TMinorServoAxis[MAX_AXIS_NUMBER];
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CConfiguration::init()");
		throw dummy;
	}

	for(;;) {
		if (m_minorServoMappings==0) fieldPath=componentName;
		else fieldPath.Format("%s%d",(const char *)componentName,m_minorServoMappings);
		if (!CIRATools::getDBValue(Services,"axis",strVal,"alma/",fieldPath)) {
			break;
		}
		ACS_DEBUG_PARAM("CConfiguration::Init()","axis: %s",(const char *)strVal);
		m_axis[m_minorServoMappings].axis=str2Axis(strVal);
		if (!CIRATools::getDBValue(Services,"servoName",strVal,"alma/",fieldPath)) {
			break;
		}
		ACS_DEBUG_PARAM("CConfiguration::Init()","servoName: %s",(const char *)strVal);
		m_axis[m_minorServoMappings].servoName=strVal;
		m_minorServoMappings++;
		if (m_minorServoMappings>=MAX_AXIS_NUMBER) break;
	}
	ACS_DEBUG_PARAM("CConfiguration::Init()","Total minor servo axis: %d",m_minorServoMappings);


	_GET_STRING_ATTRIBUTE("AntennaBossInterface","Antenna Boss component interface is ",m_antennaBossComp);
	_GET_STRING_ATTRIBUTE("ObservatoryInterface","Observatory component interface is ",m_observatoryComp);
	_GET_STRING_ATTRIBUTE("ReceiversBossInterface","Receivers Boss component interface is ",m_receiversBossComp);
	_GET_STRING_ATTRIBUTE("MinorServoBossInterface","Minor Servo Boss component interface is ",m_minorServoBossComp);
	_GET_STRING_ATTRIBUTE("ActiveSurfaceBossInterface","Active Surface Boss component interface is ",m_activeSurfaceBossComp);
	_GET_STRING_ATTRIBUTE("CustomLoggerInterface","Custom logger component interface is ",m_custoLoggerComp);
	_GET_STRING_ATTRIBUTE("WeatherStationInstance","Weather station component interface is ",m_weatherComp);
	_GET_STRING_ATTRIBUTE("DefaultBackendInstance","Default backend component instance is ",m_defaultBackendInst);
	_GET_STRING_ATTRIBUTE("DefaultDataReceiverInstance","Default data receiver component instance is ",m_defaultDataReceiverInst);
	_GET_STRING_ATTRIBUTE("SchedDir","Schedule files path is ",m_schedDir);
	_GET_STRING_ATTRIBUTE("DataDir","Data files path is ",m_dataDir);
	_GET_STRING_ATTRIBUTE("SystemDataDir","System data files path is ",m_systemDataDir);
	_GET_STRING_ATTRIBUTE("LogDir","Log files path is ",m_logDir);
	//_GET_STRING_ATTRIBUTE("SystemLogDir","System log files path is ",m_systemLogDir);
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

Management::TScanAxis CConfiguration::str2Axis(const IRA::CString& axis) const
{
	if (axis=="SUBR_Z") {
		return Management::MNG_SUBR_Z;
	}
	else 	if (axis=="SUBR_X") {
		return Management::MNG_SUBR_X;
	}
	else 	if (axis=="SUBR_Y") {
		return Management::MNG_SUBR_Y;
	}
	else 	if (axis=="SUBR_ROTY") {
		return Management::MNG_SUBR_ROTY;
	}
	else 	if (axis=="SUBR_ROTX") {
		return Management::MNG_SUBR_ROTX;
	}
	else 	if (axis=="PFP_Z") {
		return Management::MNG_PFP_Z;
	}
	else /*	if (axis=="PFP_Y")*/ {
		return Management::MNG_PFP_Y;
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
