#include "Schedule.h"
#include <slamac.h>
#include <ManagementModule.h>

using namespace std;
using namespace Schedule;

#define LINE_SIZE 256
#define COMMENT_CHAR '#'
#define PROCEDURE_SEPARATOR ':'
#define PROCEDURE_START '{'
#define PROCEDURE_STOP '}'
#define PROCEDURE_ARG_OPEN '('
#define PROCEDURE_ARG_CLOSE ')'
#define PROCEDURE_NAME_ARG_SEPARATOR '='
#define PROCEDURE_ARGS_SEPARATOR  ','
#define PROJECT "PROJECT:"
#define OBSERVER "OBSERVER:"
#define SCANLAYOUT "SCANLAYOUT:"
//#define OUTPUTFILE "OUTPUTFILE:"
#define SCANLIST "SCANLIST:"
#define PROCLIST "PROCEDURELIST:"
#define BACKENDLIST "BACKENDLIST:"
#define MODE "MODE:"
#define SCANTAG "SCANTAG:"
#define ELEVATIONLIMITS "ELEVATIONLIMITS"
#define INITPROC "INITPROC:"
#define SCAN_START "SC:"

//#define SIDEREAL "SIDEREAL"
//#define SUN "SUN"
//#define MOON "MOON"
//#define SATELLITE "SATELLITE"
//#define SOLARSYTEMBODY "SOLARSYTEMBODY"
//#define OTF "OTF"
//#define OTFC "OTFC"
//#define	SKYDIP	"SKYDIP"
//#define FOCUS "FOCUS"
#define LSTMODE "LST"
#define SEQMODE "SEQ"

//#define OFFFRAMEEQ "-EQOFFS"
//#define OFFFRAMEHOR "-HOROFFS"
//#define OFFFRAMEGAL "-GALOFFS"

#define ASYNCCHAR '@'

CParser::CParser() 
{
}

CParser::~CParser()
{
	close();
}

bool CParser::open(const IRA::CString& fileName)
{
	m_file.open((const char *)fileName,ifstream::in);
	return m_file.is_open();
}

void CParser::close()
{
	if (m_file.is_open()) {
		m_file.close();
	}
}

bool CParser::parse(CBaseSchedule* unit,DWORD& line,IRA::CString& errorMsg)
{
	char inLine[LINE_SIZE];
	line=0;
	while (!m_file.eof()) {
		m_file.getline(inLine,LINE_SIZE);
		if (m_file.bad()) {
			return false;
		}
		else {
			line++;
		}
		// This code is a workaround in order to cope with the carriage return in files edited in windows machine
		IRA::CString inputLine(inLine);
		inputLine.Replace('\r',' ',0);
		inputLine.RTrim();
		if ((inLine[0]!=COMMENT_CHAR) && (inLine[0]!=0) && (strlen(inLine)!=0)) {
			if (!unit->parseLine(inputLine,line,errorMsg)) {
				return false;
			}
		}
	}
	return true;
}

//CBaseSchedule

CBaseSchedule::CBaseSchedule(const IRA::CString& path,const IRA::CString& fileName) : m_lines(0), m_fileName(""),
		m_filePath(path), m_baseName(""),m_lastError(""),m_config(NULL)
{
	m_parser=new CParser;
	m_fileName=m_filePath+fileName;
	int pos=fileName.Find('.');
	m_baseName=fileName.Mid(0,pos);
}

CBaseSchedule::~CBaseSchedule()
{
	if (m_parser) delete m_parser;
}

bool CBaseSchedule::readAll(bool check)
{
	IRA::CString err;
	if (!m_parser->open(m_fileName)) {
		m_lastError.Format("Cannot open file %s",(const char *)m_fileName);
		return false;
	}
	if (!m_parser->parse(this,m_lines,err)) {
		m_lastError.Format("Parsing error at line %u of file %s, error message: %s",m_lines,(const char *)m_fileName,(const char *)err);
		return false;
	}
	m_parser->close();
	if (check) {
		DWORD line;
		if (!checkConsistency(line,err)) {
			m_lastError.Format("File %s has inconsistencies at line %u, error message %s",(const char*)m_fileName,line,(const char *)err);
			return false;
		}
	}
	return true;
}

// CBackendList

CBackendList::CBackendList(const IRA::CString& path,const IRA::CString& fileName): CBaseSchedule(path,fileName)
{
	m_backend.clear();
	TRecord *tmp=new TRecord;
	tmp->procName=_SCHED_NULLTARGET;
	tmp->target=_SCHED_NULLTARGET;
	tmp->proc.clear();
	m_backend.push_back(tmp);
	m_started=false;
	m_currentRecord=NULL;
}
	
CBackendList::~CBackendList()
{
	TIterator p;
	for(p=m_backend.begin();p<m_backend.end();p++) {
		if ((*p)!=NULL) {
			(*p)->proc.clear();
			delete (*p);
		}
	}
	if (m_currentRecord!=NULL) delete m_currentRecord;
	m_backend.clear();	
}

bool CBackendList::parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg)
{
	IRA::CString workLine=line;
	workLine.RTrim();
	workLine.LTrim();
	if (m_started) { // if the procedure is started
		if (workLine.Find(PROCEDURE_STOP)>=0) { // if the end bracket is found...close the procedure parsing
			m_backend.push_back(m_currentRecord);
			m_started=false;
			m_currentRecord=NULL;
		}
		else {
			m_currentRecord->proc.push_back(workLine);
		}
	}
	else {
		int start=0;
		IRA::CString proc,instance;
		if (!IRA::CIRATools::getNextToken(workLine,start,PROCEDURE_SEPARATOR,proc)) {  // get the procedure name
			errorMsg="procedure name is missing";
			return false;
		}
		if (!IRA::CIRATools::getNextToken(workLine,start,PROCEDURE_START,instance)) {  // get the backend name
			errorMsg="backend instance name is missing";
			return false;
		}
		if (workLine[workLine.GetLength()-1]!=PROCEDURE_START) {
			errorMsg="could not find procedure begin symbol (open bracket)";
			return false;
		}	
		m_currentRecord=new TRecord;
		m_started=true;
		proc.RTrim();
		instance.RTrim();
		m_currentRecord->line=lnNumber;
		m_currentRecord->procName=proc;
		m_currentRecord->target=instance;
	}
	return true;
}

bool CBackendList::checkConsistency(DWORD& line,IRA::CString& errMsg)
{
	 TIterator i,p;
	 for (i=m_backend.begin();i<m_backend.end();i++) { //check for duplicated proc names
		 for (p=i+1;p<m_backend.end();p++) {
			 if ((*i)->procName==(*p)->procName) {
				 line=(*p)->line;
				 errMsg=(*i)->procName+" is duplicated";
				 return false;
			 }
		 }
	 }
	 return true;
}

bool CBackendList::getBackend(const IRA::CString& name,IRA::CString& backend,std::vector<IRA::CString>& proc)
{
	TIterator p;
	for(p=m_backend.begin();p<m_backend.end();p++) {
		if ((*p)->procName==name) {
			backend=(*p)->target;
			proc=(*p)->proc;
			return true;
		}
	}
	m_lastError.Format("Unknown backend procedure %s",(const char *)name);
	return false;
}

bool CBackendList::checkBackend(const IRA::CString& name)
{
	TIterator i;	
	for (i=m_backend.begin();i<m_backend.end();i++) {
		if ((*i)->procName==name) {
			return true;
		}
	}
	return false;
}

//****************************************  CLayoutList **********************************************************************

CLayoutList::CLayoutList(const IRA::CString& path,const IRA::CString& fileName) :  CBaseSchedule(path,fileName)
{
	m_layout.clear();
	TRecord *tmp=new TRecord;
	tmp->layout.clear();
	tmp->layoutName=_SCHED_NULLTARGET;
	tmp->line=0;
	m_layout.push_back(tmp);
	m_started=false;
	m_currentRecord=NULL;
}

CLayoutList::~CLayoutList()
{
	TIterator p;
	for(p=m_layout.begin();p<m_layout.end();p++) {
		if ((*p)!=NULL) {
			(*p)->layout.clear();
			delete (*p);
		}
	}
	m_layout.clear();
	if (m_currentRecord!=NULL) {
		m_currentRecord->layout.clear();
		delete m_currentRecord;
	}
}

bool CLayoutList::parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg)
{
	IRA::CString workLine=line;
	workLine.RTrim();
	workLine.LTrim();
	IRA::CString layoutName;
	if (m_started) { // if the layout definition is started
		if (workLine.Find(PROCEDURE_STOP)>=0) { // if the end bracket is found...close the procedure parsing
			m_layout.push_back(m_currentRecord);
			m_started=false;
			m_currentRecord=NULL;
		}
		else {
			m_currentRecord->layout.push_back(workLine);
		}
	}
	else {
		int start=0;
		if (!IRA::CIRATools::getNextToken(workLine,start,PROCEDURE_START,layoutName)) {  // get the  name
			errorMsg="name of the scan layout missing or incorrect";
			return false;
		}
		if (workLine[workLine.GetLength()-1]!=PROCEDURE_START) { // check the presence of open bracket
			errorMsg="could not find the scan layout start symbol (open bracket)";
			return false;
		}
		m_currentRecord=new TRecord;
		m_started=true;
		layoutName.RTrim();
		m_currentRecord->line=lnNumber;
		m_currentRecord->layoutName=layoutName;
	}
	return true;
}

bool CLayoutList::checkConsistency(DWORD& line,IRA::CString& errMsg)
{
	 TIterator i,p;
	 for (i=m_layout.begin();i<m_layout.end();i++) { //check for duplicated names
		 for (p=i+1;p<m_layout.end();p++) {
			 if ((*i)->layoutName==(*p)->layoutName) {
				 line=(*p)->line;
				 errMsg=(*i)->layoutName+" is duplicated";
				 return false;
			 }
		 }
	 }
	 return true;
}

bool CLayoutList::checkLayout(const IRA::CString& layoutName)
{
	TIterator i;
	for (i=m_layout.begin();i<m_layout.end();i++) {
		if ((*i)->layoutName==layoutName) {
			return true;
		}
	}
	return false;
}

bool CLayoutList::getScanLayout(const IRA::CString& layoutName,ACS::stringSeq& layout)
{
	TIterator p;
	for(p=m_layout.begin();p<m_layout.end();p++) {
		if ((*p)->layoutName==layoutName) {
			WORD len=(*p)->layout.size();
			layout.length(len);
			for (WORD s=0;s<len;s++) {
				layout[s]=(*p)->layout[s];
			}
			return true;
		}
	}
	m_lastError.Format("Unknown layout %s",(const char *)layoutName);
	layout.length(0);
	return false;
}

//******************************************** CProcedureList   **************************************************************************

CProcedureList::CProcedureList(const IRA::CString& path,const IRA::CString& fileName) : CBaseSchedule(path,fileName)
{
	m_procedure.clear();
	/*TRecord *tmp=new TRecord;
	// add the NULL procedure.........in order to allow for the nil
	tmp->procName=_SCHED_NULLTARGET;
	tmp->proc.clear();
	tmp->args=0;
	m_procedure.push_back(tmp);*/
	m_started=false;
	m_currentRecord=NULL;	
}
	
CProcedureList::~CProcedureList()
{
	TIterator p;
	for(p=m_procedure.begin();p<m_procedure.end();p++) {
		if ((*p)!=NULL) {
			(*p)->proc.clear();
			delete (*p);
		}
	}
	if (m_currentRecord!=NULL) {
		m_currentRecord->proc.clear();
		delete m_currentRecord;
	}
	m_procedure.clear();
}

bool CProcedureList::parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg)
{
	IRA::CString workLine=line;
	workLine.RTrim();
	workLine.LTrim();
	IRA::CString orig,proc;
	WORD args;
	if (m_started) { // if the procedure is started
		if (workLine.Find(PROCEDURE_STOP)>=0) { // if the end bracket is found...close the procedure parsing
			m_procedure.push_back(m_currentRecord);
			m_started=false;
			m_currentRecord=NULL;
		}
		else {
			m_currentRecord->proc.push_back(workLine);
		}
	}
	else {
		int start=0;
		if (!IRA::CIRATools::getNextToken(workLine,start,PROCEDURE_START,orig)) {  // get the procedure name
			errorMsg="name of the procedure missing or incorrect";
			return false;
		}
		if (workLine[workLine.GetLength()-1]!=PROCEDURE_START) { // check the presence of open bracket
			errorMsg="could not find the procedure start symbol (open bracket)";
			return false;
		}
		orig.RTrim();
		if (!extractArgument(orig,proc,args,errorMsg)) return false;
		m_currentRecord=new TRecord;
		m_started=true;
		m_currentRecord->line=lnNumber;
		m_currentRecord->procName=proc;
		m_currentRecord->args=args;
	}
	return true;	
}

bool CProcedureList::extractArgument(const IRA::CString& orig,IRA::CString& proc,WORD& args,IRA::CString& errorMsg)
{
	int start;
	int stop=0;
	IRA::CString temp;
	args=0;
	if ((start=orig.Find(PROCEDURE_ARG_OPEN))<0) {
		proc=orig;
		return true;
	}
	else 	{
		if ((stop=orig.Find(PROCEDURE_ARG_CLOSE))<0) {
			proc=orig;
			errorMsg="bracket not matched in procedure argument";
			return false;
		}
		temp=orig.Mid(start+1,stop-(start+1));
		args=temp.ToLong();
		proc=orig.Left(start);
		return true;
	}
}

bool CProcedureList::checkConsistency(DWORD& line,IRA::CString& errMsg)
{	
	 TIterator i,p;
	 for (i=m_procedure.begin();i<m_procedure.end();i++) { //check for duplicated proc names
		 for (p=i+1;p<m_procedure.end();p++) {
			 if ((*i)->procName==(*p)->procName) {
				 line=(*p)->line;
				 errMsg=(*i)->procName+" is duplicated";
				 return false;
			 }
		 }
	 }
	 return true;
}

bool CProcedureList::getProcedure(const IRA::CString& conf,std::vector<IRA::CString>& proc,WORD& args)
{	
	TIterator p;
	for(p=m_procedure.begin();p<m_procedure.end();p++) {
		if ((*p)->procName==conf) {
			proc=(*p)->proc;
			args=(*p)->args;
			return true;
		}
	}
	m_lastError.Format("Unknown procedure %s",(const char *)conf);
	return false;
}

bool CProcedureList::getProcedure(const IRA::CString& conf,ACS::stringSeq& proc,WORD& args)
{
	TIterator p;
	for(p=m_procedure.begin();p<m_procedure.end();p++) {
		if ((*p)->procName==conf) {
			WORD procLen=(*p)->proc.size();
			proc.length(procLen);
			for (WORD s=0;s<procLen;s++) {
				proc[s]=(*p)->proc[s];
			}
			args=(*p)->args;
			return true;
		}
	}
	m_lastError.Format("Unknown procedure %s",(const char *)conf);
	return false;	
}

bool CProcedureList::getProcedure(const WORD& pos,IRA::CString& conf,ACS::stringSeq& proc,WORD& args)
{
	if (pos>=m_procedure.size()) {
		m_lastError.Format("Procedure does not exist %d",pos);
		return false;
	}
	TIterator p=m_procedure.begin()+pos;
	WORD procLen=(*p)->proc.size();
	proc.length(procLen);
	for (WORD s=0;s<procLen;s++) {
		proc[s]=(*p)->proc[s];
	}
	conf=(*p)->procName;
	args=(*p)->args;
	return true;
}


bool CProcedureList::checkProcedure(const IRA::CString& conf,WORD args)
{
	TIterator i;
	IRA::CString token;
	int start=0;
	IRA::CIRATools::getNextToken(conf,start,PROCEDURE_NAME_ARG_SEPARATOR,token);
	for (i=m_procedure.begin();i<m_procedure.end();i++) {
		if ((*i)->procName==token) {
			if (args==(*i)->args) {
				return true;
			}
			else return false;
		}
	}
	return true;
}
//*****************************************************************************************************************************************************

// CScanList


// CSchedule

CSchedule::CSchedule(const IRA::CString& path,const IRA::CString& fileName) : CBaseSchedule(path,fileName),m_projectName(""),m_observer(""),
	m_scanList(""),m_configList(""),m_backendList(""),m_layoutFile(""),m_scanListUnit(NULL),m_preScanUnit(NULL),m_postScanUnit(NULL),m_backendListUnit(NULL),m_layoutListUnit(NULL),
	m_mode(LST),m_repetitions(0),m_lowerElevationLimit(-1.0),m_upperElevationLimit(-1.0),m_scanTag(-1),m_modeDone(false),m_initProc(_SCHED_NULLTARGET),m_initProcArgs(0)
{
	m_schedule.clear();
	m_currentScanDef.valid=false;
	m_currentScanDef.line=m_currentScanDef.id=0;
	m_currentScanDef.backendProc=m_currentScanDef.layout=m_currentScanDef.writerInstance=m_currentScanDef.suffix="";
}

CSchedule::~CSchedule()
{
	TIterator p;
	TScansIterator k;
	for(p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)!=NULL) delete (*p);
	}
	for(k=m_scans.begin();k<m_scans.end();k++) {
		if ((*k)!=NULL) delete (*k);
	}
	m_schedule.clear();
	m_scans.clear();
	if (m_scanListUnit) delete m_scanListUnit;
	if (m_postScanUnit) delete m_postScanUnit;
	if (m_preScanUnit) delete m_preScanUnit;
	if (m_backendListUnit) delete m_backendListUnit;
	if (m_layoutListUnit) delete m_layoutListUnit;
}

bool CSchedule::readAll(bool check)
{
	DWORD line;
	IRA::CString err;
	if (!CBaseSchedule::readAll(false)) { //the check is posticipated at the time of the other file are opened
		return false;
	}
	if (isComplete()) {
		m_scanListUnit=new CScanList(m_filePath,m_scanList);
		if (!m_scanListUnit->readAll(check)) {
			m_lastError=m_scanListUnit->getLastError();
			return false;
		}
		m_postScanUnit=new CProcedureList(m_filePath,m_configList);
		m_preScanUnit=new CProcedureList(m_filePath,m_configList);	
		if (!m_preScanUnit->readAll(check)) {
			m_lastError=m_preScanUnit->getLastError();
			return false;
		}
		if (!m_postScanUnit->readAll(check)) {
			m_lastError=m_postScanUnit->getLastError();
			return false;
		}
		m_backendListUnit=new CBackendList(m_filePath,m_backendList);
		if (!m_backendListUnit->readAll(check)) {
			m_lastError=m_backendListUnit->getLastError();
			return false;
		}
		if (m_layoutFile!="") {
			m_layoutListUnit=new CLayoutList(m_filePath,m_layoutFile);
			if (!m_layoutListUnit->readAll(check)) {
				m_lastError=m_layoutListUnit->getLastError();
				return false;
			}
		}
	}
	else {
		m_lastError.Format("One or more schedule sections are missing in file : %s",(const char *)m_fileName);
		return false;
	}
	if (check) {
		if (!checkConsistency(line,err)) {
			m_lastError.Format("File %s has inconsistencies at line %u, error message: %s",(const char*)m_fileName,line,(const char *)err);
			return false;
		}
	}
	if (!prepareScan(line,err)) {
		if (check) {
			m_lastError.Format("Scan definition error in file %s at line %u, error message: %s",(const char*)m_fileName,line,(const char *)err);
			return false;
		}
	}
	return true;
}

bool CSchedule::getInitProc(IRA::CString& procName)
{
	procName=m_initProc;
	/*if (m_initProc==_SCHED_NULLTARGET) {
		proc.length(0);
		return true;
	}
	if (!m_preScanUnit->getProcedure(m_initProc,proc,args)) {
		m_lastError.Format("Unknown procedure %s",(const char *)m_initProc);
		return false;
	}*/
	return true;
}

bool CSchedule::getSubScan(const DWORD& counter,TRecord& rec)
{
	if ((m_mode==SEQ) || (m_mode==TIMETAGGED)) {
		rec.counter=counter;
		return getSubScan_SEQ(rec);
	}
	else {
		rec.counter=counter;
		rec.lst=0;
		return getSubScan_LST(rec);
	}
}

DWORD CSchedule::getSubScanCounter(const DWORD& scanid,const DWORD& subscanid)
{
	TIterator p;
	TIterator k;
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if  ((*p)->scanid==scanid) {
			k=p;
			while ((k<m_schedule.end()) && ((*k)->scanid==scanid)) {
				if ((*k)->subscanid==subscanid) {
					return (*k)->counter;
				}
				k++;
			}
			return (*p)->counter;
		}
	}
	p=m_schedule.begin();
	return (*p)->counter;
}

DWORD CSchedule::getSubScanCounter(const IRA::CString& scan)
{
	DWORD scanid,subscanid;
	long num=sscanf((const char *)scan,"%u_%u",&scanid,&subscanid);
	if (num==1) { // only scan id is matched
		subscanid=0; // this subscan cannot exist, so it will be  taken the first subscan relative to requested scan
	}
	else if (num==0) { // none are matched
		scanid=subscanid=0; // ....the first subscan in the schedule will be taken
	}
	return getSubScanCounter(scanid,subscanid);
}

void CSchedule::getIdentifiers(const DWORD& counter,DWORD& scanID,DWORD& subScanID)
{
	TIterator p;
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)->counter==counter) {
			scanID=(*p)->scanid;
			subScanID=(*p)->subscanid;
			return;
		}
	}
	p=m_schedule.begin();
	scanID=(*p)->scanid;
	subScanID=(*p)->subscanid;
}

IRA::CString CSchedule::getIdentifiers(const DWORD& counter)
{
	DWORD scanID,subScanID;
	IRA::CString scan;
	getIdentifiers(counter,scanID,subScanID);
	scan.Format("%d_%d",scanID,subScanID);
	return scan;
}

bool CSchedule::getSubScan_SEQ(DWORD& counter,DWORD& scanid,DWORD& subscanid,double& duration,DWORD& scan,IRA::CString& pre,bool& preBlocking,WORD& preArgs,IRA::CString& post,
		bool& postBlocking,WORD& postArgs,IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix,
		IRA::CString& layout,bool& rewind,Management::TScanConfiguration*& scanConf)
{
	if ((m_mode!=SEQ) && (m_mode!=TIMETAGGED)) {
		m_lastError.Format("The current is not a sequence schedule");
		return false;
	}
	if (m_schedule.size()==0) {
		m_lastError.Format("The current schedule is empty");
		return false;
	}
	TIterator p;
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)->counter>counter) {
			scanid=(*p)->scanid;
			subscanid=(*p)->subscanid;
			duration=(*p)->duration;
			scan=(*p)->scan;
			pre=(*p)->preScan;
			post=(*p)->postScan;
			preBlocking=(*p)->preScanBlocking;
			postBlocking=(*p)->postScanBlocking;
			preArgs=(*p)->preScanArgs;
			postArgs=(*p)->postScanArgs;
			bckProc=(*p)->backendProc;
			wrtInstance=(*p)->writerInstance;
			suffix=(*p)->suffix;
			layout=(*p)->layout;
			counter=(*p)->counter;
			rewind=false;
			scanConf=(*p)->scanConf;
			return true;
		}
	}	
	p=m_schedule.begin();
	scanid=(*p)->scanid;
	subscanid=(*p)->subscanid;
	duration=(*p)->duration;
	scan=(*p)->scan;
	pre=(*p)->preScan;
	post=(*p)->postScan;
	preBlocking=(*p)->preScanBlocking;
	postBlocking=(*p)->postScanBlocking;
	preArgs=(*p)->preScanArgs;
	postArgs=(*p)->postScanArgs;
	bckProc=(*p)->backendProc;
	wrtInstance=(*p)->writerInstance;
	suffix=(*p)->suffix;
	layout=(*p)->layout;
	counter=(*p)->counter;
	rewind=true;
	scanConf=(*p)->scanConf;
	return true;
}

bool CSchedule::getSubScan_SEQ(TRecord& rec)
{
	return getSubScan_SEQ(rec.counter,rec.scanid,rec.subscanid,rec.duration,rec.scan,rec.preScan,rec.preScanBlocking,rec.preScanArgs,rec.postScan,rec.postScanBlocking,rec.postScanArgs,rec.backendProc,rec.writerInstance,rec.suffix,
			rec.layout,rec.rewind,rec.scanConf);
}

bool CSchedule::getSubScan_LST(ACS::TimeInterval& lst,DWORD& counter,DWORD&scanid,DWORD& subscanid,double& duration,DWORD& scan,
		IRA::CString& pre,bool& preBlocking,WORD& preArgs,IRA::CString& post,bool& postBlocking,WORD& postArgs,
		IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix,
		IRA::CString& layout,bool& rewind,Management::TScanConfiguration*& scanConf)
{
	if (m_mode!=LST) {
		m_lastError.Format("The current schedule is not LST based");
		return false;
	}
	if (m_schedule.size()==0) {
		m_lastError.Format("The current schedule is empty");
		return false;
	}
	TIterator p;
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)->counter>counter) {
			if ((*p)->lst>lst) {
				lst=(*p)->lst;
				counter=(*p)->counter;
				scanid=(*p)->scanid;
				subscanid=(*p)->subscanid;
				scan=(*p)->scan;
				duration=(*p)->duration;
				pre=(*p)->preScan;
				post=(*p)->postScan;
				preBlocking=(*p)->preScanBlocking;
				postBlocking=(*p)->postScanBlocking;
				preArgs=(*p)->preScanArgs;
				postArgs=(*p)->postScanArgs;
				bckProc=(*p)->backendProc;
				wrtInstance=(*p)->writerInstance;
				suffix=(*p)->suffix;
				layout=(*p)->layout;
				rewind=false;
				scanConf=(*p)->scanConf;
				return true;
			}
		}
	}
	//that means that no scheduled lst are later than the given one...than take the first one which will be the next day.
	p=m_schedule.begin();
	// This is to bypass the following problem in the scheduler:
	// Coming from the last scan of the schedule, if the next (which is the first of the schedule) scan is late.....the scheduler will call again this function
	// again and again until the current lst is elapsed...since this function in this case returns the first scan of the schedule
	if ( (*p)->counter==counter ) {
		p++;
	}
	lst=(*p)->lst;
	counter=(*p)->counter;
	scanid=(*p)->scanid;
	subscanid=(*p)->subscanid;
	scan=(*p)->scan;
	duration=(*p)->duration;
	pre=(*p)->preScan;
	post=(*p)->postScan;
	preBlocking=(*p)->preScanBlocking;
	postBlocking=(*p)->postScanBlocking;	
	preArgs=(*p)->preScanArgs;
	postArgs=(*p)->postScanArgs;
	bckProc=(*p)->backendProc;
	wrtInstance=(*p)->writerInstance;	
	suffix=(*p)->suffix;
	layout=(*p)->layout;
	rewind=true;
	scanConf=(*p)->scanConf;
	return true;
}

bool CSchedule::getSubScan_LST(TRecord& rec)
{
	return getSubScan_LST(rec.lst,rec.counter,rec.scanid,rec.subscanid,rec.duration,rec.scan,rec.preScan,rec.preScanBlocking,rec.preScanArgs,rec.postScan,rec.postScanBlocking,rec.preScanArgs,
			rec.backendProc,rec.writerInstance,rec.suffix,rec.layout,rec.rewind,rec.scanConf);
}

bool CSchedule::prepareScan(DWORD& line,IRA::CString& errMsg)
{
	TIterator p;
	Schedule::CScanList::TRecord rec;
	DWORD scanID=0;
	Management::TScanConfiguration *scanConf;
	Schedule::CScanBinder binder(NULL,false);
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if (scanID!=(*p)->scanid) {
			scanConf=new Management::TScanConfiguration;
			m_scans.push_back(scanConf);
			scanID=(*p)->scanid;
			binder.init(scanConf);
			binder.startScan(scanID);
		}	
		m_scanListUnit->getScan((*p)->scan,rec);
		binder.addSubScan((Antenna::TTrackingParameters*)rec.primaryParameters,
		  						(Antenna::TTrackingParameters*)rec.secondaryParameters,
								(MinorServo::MinorServoScan*)rec.servoParameters,
								(Receivers::TReceiversParameters*)rec.receiversParameters);
		(*p)->scanConf=binder.getScanConf();				
	}
	return true;
}

bool CSchedule::checkConsistency(DWORD& line,IRA::CString& errMsg)
{
	ACS::TimeInterval lst=0;
	TIterator p,i;
	DWORD currentScanID;
	TIterator start;
	std::vector<DWORD> scanIDS;
	// check that the initialization procedure has been defined
	if (!m_postScanUnit->checkProcedure(m_initProc,m_initProcArgs)) {
		line=0;
		errMsg.Format("schedule initialization procedure %s is not defined",(const char *)m_initProc);
		return false;
	}
	scanIDS.clear();
	currentScanID=0;
	start=m_schedule.begin();
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if (currentScanID!=(*p)->scanid) {
			if (!m_scanListUnit->checkTarget(scanIDS)) {
				line=(*start)->line;
				errMsg="this scan seems to have more than one target or source";
				return false;
			}
			scanIDS.clear();
			currentScanID=(*p)->scanid;
			start=p;		
		}		
		scanIDS.push_back((*p)->scan);		
		//check that  the combination scanid/subscanid is unique
		for (i=p+1;i<m_schedule.end();i++) {
				 if  (((*p)->scanid==(*i)->scanid) && ((*p)->subscanid==(*i)->subscanid)) {
					 line=(*i)->line;  //highlight the scan with duplicated couple scanid/subscanid
					 errMsg.Format("subscan %d_%d is duplicated",(*i)->scanid,(*i)->subscanid);
					 return false;
				 }
		}
		if (m_mode==LST) { //check that all the time stamps are increasing....
			if ((*p)->lst<lst) {
				line=(*p)->line;
				errMsg="local sidereal timestamp is not increased from the previous scan";
				return false;
			}
			else {
				lst=(*p)->lst;
			}
		}
		if (!m_scanListUnit->checkScan((*p)->scan) ) {
			line=(*p)->line;
			errMsg.Format("scan %d is not defined",(*p)->scan);
			return false;
		}
		if (!m_preScanUnit->checkProcedure((*p)->preScan,(*p)->preScanArgs)) {
			line=(*p)->line;
			errMsg.Format("arguments number does not match in pre scan procedure %s",(const char *)(*p)->preScan);
			return false;
		}
		if (!m_postScanUnit->checkProcedure((*p)->postScan,(*p)->postScanArgs)) {
			line=(*p)->line;
			errMsg.Format("arguments number does not match in post scan procedure %s",(const char *)(*p)->postScan);
			return false;
		}
		if (!m_backendListUnit->checkBackend((*p)->backendProc)) {
			errMsg.Format("backend configuration procedure %s is not defined",(const char *)(*p)->backendProc);
			line=(*p)->scanLine;
			return false;
		}
		if ((*p)->layout!=_SCHED_NULLTARGET) {
			if (m_layoutListUnit) {
				if (!m_layoutListUnit->checkLayout((*p)->layout)) {
					errMsg.Format("scan layout %s is not defined",(const char *)(*p)->layout);
					line=(*p)->scanLine;
					return false;
				}
			}
			else {
				line=(*p)->line;
				return false;
			}
		}
		//count++;
	}
	if (!m_scanListUnit->checkTarget(scanIDS)) {
		line=(*start)->line;
		errMsg="this scan seems to have more than one target or source";
		return false;
	}
	return true;
}

bool CSchedule::parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg)
{
	int start=0;
	IRA::CString ret;
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) {
		return false;
	}
	else { // token could be extracted
		ret.MakeUpper();
		if (ret==PROJECT) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_projectName)) {
				errorMsg="cannot parse project name";
				return false;
			}
			else return true;
		}
		else if (ret==OBSERVER) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_observer)) {
				errorMsg="cannot parse observer name";
				return false;
			}
			else return true;			
		}
		else if (ret==SCANLIST) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_scanList)) {
				errorMsg="cannot parse scan list file name";
				return false;
			}
			else return true;
		}
		else if (ret==PROCLIST) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_configList)) {
				errorMsg="cannot parse procedure list file name";
				return false;
			}
			else return true;
		}
		else if (ret==BACKENDLIST) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_backendList)) {
				errorMsg="cannot parse backend configurations file name";
				return false;
			}
			else return true;
		}
		else if (ret==SCANLAYOUT) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_layoutFile)) {
				errorMsg="cannot parse scan layouts  file name";
				return false;
			}
			else return true;
		}
		else if (ret==MODE) {
			IRA::CString mode,rep,startTime;
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,mode)) {
				errorMsg="cannot parse schedule mode";
				return false;
			}
			else {
				m_modeDone=true;
				if (mode==LSTMODE) {
					if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,rep)) {
						errorMsg="cannot parse repetitions number for  LST mode";
						return false;
					}
					m_repetitions=rep.ToLong();
					m_mode=LST;
					if (m_repetitions!=0) return true;  // a zero value is not allowed
					else {
						errorMsg="repetitions must be at least 1";
						return false;
					}
				}
				else if (mode==SEQMODE) {
					if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,startTime)) {
						m_mode=SEQ; //if it has not an extra argument the sequential is pure
						return true;
					}
					else {
						//otherwise it is sequential with start time
						if (!parseLST(startTime,m_startLST)) {
							errorMsg="bad format for start local sidereal time for sequential mode";
							return false;
						}
						m_mode=TIMETAGGED;
						return true;
					}
				}
				else {
					errorMsg="unknown schedule mode";
					return false;
				}
			}
		}
		else if (ret==ELEVATIONLIMITS) {
			IRA::CString minEl,maxEl;
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,minEl)) {
				errorMsg="elevation lower limit missing or not correct";
				return false;
			}
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,maxEl)) {
				errorMsg="elevation upper limit missing or not correct";
				return false;
			}
			m_lowerElevationLimit=minEl.ToDouble();
			if ((m_lowerElevationLimit<0) || (m_lowerElevationLimit>90.0)) {
				errorMsg="elevation limits are outside the expected ranges";
				return false;
			}
			m_upperElevationLimit=maxEl.ToDouble();
			if ((m_upperElevationLimit<0) || (m_upperElevationLimit>90.0)) {
				errorMsg="elevation limits are outside the expected ranges";
				return false;
			}
			if (m_lowerElevationLimit>=m_upperElevationLimit) {
				errorMsg="lower elevation limit could not be greater than upper one";
				return false;
			}
			m_lowerElevationLimit*=DD2R;
			m_upperElevationLimit*=DD2R;
			return true;
		}
		else if (ret==SCANTAG) {
			IRA::CString scanTag;
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,scanTag)) {
				errorMsg="cannot parse scan tag";
				return false;
			}			
			else {
				m_scanTag=scanTag.ToLong();
			}
			return true;
		}
		else if (ret==INITPROC) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_initProc)) {
				errorMsg="cannot parse schedule initialization procedure";
				return false;
			}
			else {
				m_initProcArgs=getProcedureArgs(m_initProc);
				return true;
			}
		}
		else { // check the schedule
			return parseScans(line,lnNumber,errorMsg);
		}
	}
	return true;
}

bool CSchedule::parseScans(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errorMsg)
{
	int start=0;
	IRA::CString ret;
	DWORD subscanid,scanid,scan;
	DWORD counter;
	//,prevId;
	double duration;
	char localLST[256],pre[256],post[256];

	IRA::CString storageCommand,backendProc,writerInstance;
	ACS::TimeInterval lst;
	//ACS::Time ut;
	TRecord *p;
	start=0;
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) {
		errorMsg="schedule format error";
		return false;
	}
	else {
		ret.MakeUpper();
		if (ret==SCAN_START) {  //process scan definition
			m_currentScanDef.valid=false;
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) { // scan id
				errorMsg="scan identifier cannot be found";
				return false;
			}
			else {
				DWORD tempId=(DWORD)ret.ToLong();
				if (tempId==0) {
					errorMsg="scan identifier cannot be zero";
					return false;
				}
				if (tempId<=m_currentScanDef.id) { // check that the scan id is increasing
					errorMsg="scan enumeration must increase";
					return false;
				}
				m_currentScanDef.id=tempId;
			}
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) { //suffix
				errorMsg="scan suffix cannot be found";
				return false;
			}
			else {
				m_currentScanDef.suffix=ret;
			}
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) { //backend:datawriter
				errorMsg="backend/datawriter cannot be found";
				return false;
			}
			else {
				int localStart=0;
				if (!IRA::CIRATools::getNextToken(ret,localStart,PROCEDURE_SEPARATOR,m_currentScanDef.backendProc)) {
					errorMsg="backend configuration couls not be found";
					return false;
				}
				if (m_currentScanDef.backendProc!=_SCHED_NULLTARGET) {
					if (!IRA::CIRATools::getNextToken(ret,localStart,PROCEDURE_SEPARATOR,m_currentScanDef.writerInstance)) {
						errorMsg="datawriter instance name could not be found";
						return false;
					}
				}
				else {
					m_currentScanDef.writerInstance=_SCHED_NULLTARGET;
				}
			}
			if (IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) { //layout...not mandatory
				if (m_layoutFile=="") {  // if the layout file has not been given
					errorMsg="scan layout without providing the scan layouts file";
					return false;
				}
				m_currentScanDef.layout=ret;
			}
			else {
				m_currentScanDef.layout=_SCHED_NULLTARGET;
			}
			m_currentScanDef.line=lnNumber;
			m_currentScanDef.valid=true;
			return true;
		}
	}
	if (!m_currentScanDef.valid) { // before processing the subscans at least a scan header must be given
		errorMsg="a subscan must be defined inside a scan";
		return false;
	}
	if (m_mode==LST) {
		if (sscanf((const char *)line,"%u_%u\t%s\t%lf\t%u\t%s\t%s",&scanid,&subscanid,localLST,&duration,&scan,pre,post)!=7) {
			errorMsg="subscan format error";
			return false;
		}
		else {
			if (!parseLST(IRA::CString(localLST),lst)) {
				errorMsg="cannot decode local sidereal timestamp";
				return false;
			}
			if (scanid!=m_currentScanDef.id) {			
				errorMsg="scan identifier differs from the identifier provided in the current scan definition";
				return false;
			}
			if (subscanid==0) {
				errorMsg="subscan identifier cannot be zero";
				return false;
			}
			counter=m_schedule.size()+1; // subscan enumerations must start from 1			
			p=new TRecord;
			p->scanConf=0; //this wil be set in preparescan method of this class
			p->line=lnNumber;
			p->scanLine=m_currentScanDef.line;
			p->subscanid=subscanid;
			p->scanid=m_currentScanDef.id;
			p->counter=counter;
			p->lst=lst;
			p->scan=scan;
			p->duration=duration;
			p->preScanBlocking=isSync(pre);
			p->preScan=IRA::CString(pre);
			p->preScanArgs=getProcedureArgs(p->preScan);
			p->postScanBlocking=isSync(post);
			p->postScan=IRA::CString(post);
			p->postScanArgs=getProcedureArgs(p->postScan);
			p->backendProc=m_currentScanDef.backendProc;
			p->writerInstance=m_currentScanDef.writerInstance;
			p->layout=m_currentScanDef.layout;
			p->suffix=m_currentScanDef.suffix;
			m_schedule.push_back(p);
			return true;
		}
	}
	else if ((m_mode==SEQ) || (m_mode==TIMETAGGED)) {
		if (sscanf((const char *)line,"%u_%u\t%lf\t%u\t%s\t%s",&scanid,&subscanid,&duration,&scan,pre,post)!=6) {
			errorMsg="subscan format error";
			return false;
		}
		else {
			if (scanid!=m_currentScanDef.id) {
				errorMsg="scan identifier differs from the identifier provided in the current scan definition";
				return false;
			}
			if (subscanid==0) {
				errorMsg="subscan identifier cannot be zero";
				return false;
			}
			counter=m_schedule.size()+1; // subscan enumerations must start from 1
			p=new TRecord;
			p->scanConf=0; //this wil be set in prepareScan method of this class 
			p->line=lnNumber;
			p->scanLine=m_currentScanDef.line;
			p->subscanid=subscanid;
			p->scanid=m_currentScanDef.id;
			p->counter=counter;
			p->ut=0;  // this is the indication that the scan must start as soon as possible
			p->scan=scan;
			p->duration=duration;
			p->preScanBlocking=isSync(pre);
			p->preScan=IRA::CString(pre);
			p->preScanArgs=getProcedureArgs(p->preScan);
			p->postScanBlocking=isSync(post);
			p->postScan=IRA::CString(post);
			p->postScanArgs=getProcedureArgs(p->postScan);
			p->backendProc=m_currentScanDef.backendProc;
			p->writerInstance=m_currentScanDef.writerInstance;
			p->layout=m_currentScanDef.layout;
			p->suffix=m_currentScanDef.suffix;
			m_schedule.push_back(p);
			return true;
		}
	}
	return true;
}

bool CSchedule::isComplete()
{
	if ((m_projectName!="") && (m_observer="")  && 	(m_scanList!="") && (m_configList!="")
		&& (m_backendList!="") && (m_schedule.size()>0) && (m_modeDone)) {
		return true;
	}
	else {
		m_lastError.Format("Some mandatory keywords of the schedule are missing");
		return false;
	}
}

WORD CSchedule::getProcedureArgs(const IRA::CString& proc)
{
	IRA::CString temp;
	WORD args=0;
	int start=proc.Find(PROCEDURE_NAME_ARG_SEPARATOR);
	if (start<0) return 0;
	start++;
	while (IRA::CIRATools::getNextToken(proc,start,PROCEDURE_ARGS_SEPARATOR,temp)) {
		args++;
	}
	return args;
}

bool CSchedule::isSync(char *procName) 
{
	if (procName[strlen(procName)-1]==ASYNCCHAR) {
		procName[strlen(procName)-1]=0;
		return false;
	}
	else {
		return true;
	}
}

bool CSchedule::parseLST(const IRA::CString& val,ACS::TimeInterval& lst)
{
	if (!IRA::CIRATools::strToInterval(val,lst,true)) return false;
	else return true;
}

bool CSchedule::parseUT(const IRA::CString& val,ACS::Time& ut)
{
	long hh,mm,ss;
	unsigned yy,doy;
	double sec;
	if (sscanf((const char *)val,"%u-%u-%ld:%ld:%ld.%lf",&yy,&doy,&hh,&mm,&ss,&sec)!=6) {
		return false;
	}
	else {
		if (hh<0 || hh>23) return false;
		if (mm<0 || mm>59) return false;
		if (ss<0 || ss>59) return false;
		if (doy>366) return false;
		TIMEVALUE tt;
		tt.reset();
		tt.normalize(true);
		tt.year(yy);
		tt.dayOfYear(doy);
		tt.hour(hh);
		tt.minute(mm);
		tt.second(ss);
		tt.microSecond((unsigned long)(sec*1000));
		ut=tt.value().value;
		return true;
	}
}
// ************************************************************************
