// $Id: Schedule.cpp,v 1.19 2011-06-21 16:39:52 a.orlati Exp $

#include "Schedule.h"
#include <slamac.h>

using namespace std;
using namespace Schedule;

#define LINE_SIZE 256
#define COMMENT_CHAR '#'
#define SEPARATOR '\t'
#define PROCEDURE_SEPARATOR ':'
#define PROCEDURE_START '{'
#define PROCEDURE_STOP '}'
#define PROJECT "PROJECT:"
#define OBSERVER "OBSERVER:"
#define SCANLAYOUT "SCANLAYOUT:"
//#define OUTPUTFILE "OUTPUTFILE:"
#define SCANLIST "SCANLIST:"
#define PROCLIST "PROCEDURELIST:"
#define BACKENDLIST "BACKENDLIST:"
#define MODE "MODE:"
#define SCANTAG "SCANTAG:"
#define INITPROC "INITPROC:"
#define SCAN_START "SC:"

#define SIDEREAL "SIDEREAL" 
#define SUN "SUN"                 
#define MOON "MOON"              
#define SATELLITE "SATELLITE"    
#define SOLARSYTEMBODY "SOLARSYTEMBODY"
#define OTF "OTF"
#define OTFC "OTFC"
#define FOCUS "FOCUS"
#define LSTMODE "LST"
#define SEQMODE "SEQ"

#define OFFFRAMEEQ "-EQOFFS"
#define OFFFRAMEHOR "-HOROFFS"
#define OFFFRAMEGAL "-GALOFFS"

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
		if ((inLine[0]!=COMMENT_CHAR) && (inLine[0]!=0) && (strlen(inLine)!=0)) {
			if (!unit->parseLine(IRA::CString(inLine),line,errorMsg)) {
				return false;
			}
		}
	}
	return true;
}

//CBaseSchedule

CBaseSchedule::CBaseSchedule(const IRA::CString& path,const IRA::CString& fileName) : m_lines(0), m_fileName(""), m_filePath(path), m_lastError("") 
{
	m_parser=new CParser;
	m_fileName=m_filePath+fileName;
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
	TRecord *tmp=new TRecord;
	// add the NULL procedure.........in order to allow for the nil
	tmp->procName=_SCHED_NULLTARGET;
	tmp->proc.clear();
	m_procedure.push_back(tmp);
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
	IRA::CString proc;
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
		if (!IRA::CIRATools::getNextToken(workLine,start,PROCEDURE_START,proc)) {  // get the procedure name
			errorMsg="name of the procedure missing or incorrect";
			return false;
		}
		if (workLine[workLine.GetLength()-1]!=PROCEDURE_START) { // check the presence of open bracket
			errorMsg="could not find the procedure start symbol (open bracket)";
			return false;
		}	
		m_currentRecord=new TRecord;
		m_started=true;
		proc.RTrim();
		m_currentRecord->line=lnNumber;
		m_currentRecord->procName=proc;
	}
	return true;	
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

bool CProcedureList::getProcedure(const IRA::CString& conf,std::vector<IRA::CString>& proc)
{	
	TIterator p;
	for(p=m_procedure.begin();p<m_procedure.end();p++) {
		if ((*p)->procName==conf) {
			proc=(*p)->proc;
			return true;
		}
	}
	m_lastError.Format("Unknown procedure %s",(const char *)conf);
	return false;
}

bool CProcedureList::getProcedure(const IRA::CString& conf,ACS::stringSeq& proc)
{
	TIterator p;
	for(p=m_procedure.begin();p<m_procedure.end();p++) {
		if ((*p)->procName==conf) {
			WORD procLen=(*p)->proc.size();
			proc.length(procLen);
			for (WORD s=0;s<procLen;s++) {
				proc[s]=(*p)->proc[s];
			}
			return true;
		}
	}
	m_lastError.Format("Unknown procedure %s",(const char *)conf);
	return false;	
}

bool CProcedureList::checkProcedure(const IRA::CString& conf)
{
	TIterator i;	
	for (i=m_procedure.begin();i<m_procedure.end();i++) {
		if ((*i)->procName==conf) {
			return true;
		}
	}
	return false;	
}
//*****************************************************************************************************************************************************

// CScanList

CScanList::CScanList(const IRA::CString& path,const IRA::CString& fileName): CBaseSchedule(path,fileName)
{
	m_schedule.clear();
}
	
CScanList::~CScanList()
{	
	TIterator p;
	for(p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)!=NULL) {
			Antenna::TTrackingParameters * tmp=static_cast<Antenna::TTrackingParameters * >((*p)->primaryParameters);
			if (tmp) delete tmp;
			tmp=static_cast<Antenna::TTrackingParameters * >((*p)->secondaryParameters);
			if (tmp) delete tmp;
			delete (*p);
		}
	}
	m_schedule.clear();
}

bool CScanList::checkScan(const DWORD& id)
{
	TIterator i;	
	for (i=m_schedule.begin();i<m_schedule.end();i++) {
		if ((*i)->id==id) {
			return true;
		}
	}
	return false;
}

bool CScanList::getScan(const DWORD&id,Management::TScanTypes& type,void *&prim,void *& sec/*,IRA::CString& target*/)
{
	TIterator i;
	for (i=m_schedule.begin();i<m_schedule.end();i++) {
		if ((*i)->id==id) {
			//target=(*i)->target;
			type=(*i)->type;
			prim=(*i)->primaryParameters;
			sec=(*i)->secondaryParameters;
			return true;
		}
	}
	m_lastError.Format("Scan number %u is not known",id);
	return false;
}

bool CScanList::getScan(const DWORD& id,TRecord& rec)
{
	return getScan(id,rec.type,rec.primaryParameters,rec.secondaryParameters/*,rec.target*/);
}

 bool CScanList::checkConsistency(DWORD& line,IRA::CString& errMsg)
 {	
	 TIterator i,p;
	 for (i=m_schedule.begin();i<m_schedule.end();i++) {
		 for (p=i+1;p<m_schedule.end();p++) {
			 if ((*i)->id==(*p)->id) {
				 line=(*p)->line;
				 errMsg.Format("scan %d is duplicated",(*i)->id);
				 return false;
			 }
		 }
	 }
	 return true;
 }

bool CScanList::parseLine(const IRA::CString& line,const DWORD& lnNumber,IRA::CString& errMsg)
{
	int start=0;
	IRA::CString ret;
	Management::TScanTypes type;
	// get the second item.....
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) {
		errMsg="format error";
		return false;
	}
	//...the scan type
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) {
		errMsg="could not read scan type";
		return false;
	}
	ret.MakeUpper();
	if (!string2ScanType(ret,type)) {
		errMsg="unknown scan type";
		return false;
	}
	switch (type) {
		case Management::MNG_SIDEREAL: {
			DWORD id;
			IRA::CString sourceName;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters;
			if (!parseSidereal(line,prim,id,errMsg)) {
				if (prim) delete prim;
				return false; // errMsg already set by previous call
			}
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			resetTrackingParameters(sec);
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec;
			//rec->target=sourceName;
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_SUN: {
			break;
		}
		case Management::MNG_MOON: {
			DWORD id;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters;
			if (!parseMoon(line,prim,id,errMsg)) {
				if (prim) delete prim;
				return false; // errMsg already set by previous call
			}
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			resetTrackingParameters(sec);
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec;			
			//rec->target="";
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_OTFC: {
			DWORD id;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters;
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			if (!parseOTFC(line,prim,sec,id,errMsg)) {
				if (prim) delete prim;
				if (dec) delete sec;
				return false; // errMsg already set by previous call
			}			
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec; 
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Management::MNG_SATELLITE: {
			break;
		}
		case Management::MNG_SOLARSYTEMBODY: {
			break;
		}
		case Management::MNG_OTF: {
			DWORD id;
			Antenna::TTrackingParameters *prim=new Antenna::TTrackingParameters; 
			if (!parseOTF(line,prim,id,errMsg)) {
				if (prim) delete prim;
				return false; // errMsg already set by previous call
			}
			Antenna::TTrackingParameters *sec=new Antenna::TTrackingParameters;
			resetTrackingParameters(sec);			
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->primaryParameters=(void *)prim;
			rec->secondaryParameters=(void *)sec; 
			//rec->target="";
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		default: {
			errMsg="unknown scan type";
			return false;
		}
	}
	return true;
}

bool CScanList::parseMoon(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg)
{
	char type[32],offFrame[32],lonOff[32],latOff[32];
	long out;
	//double lonOff,latOff;
	out=sscanf((const char *)val,"%u\t%s\t%s\t%s\t%s",&id,type,offFrame,lonOff,latOff);
	if ((out!=2) && (out!=5)) {
		errMsg="invalid moon scan definition";
		return false;
	}
	scan->targetName=CORBA::string_dup("Moon");
	scan->type=Antenna::ANT_MOON;
	scan->paramNumber=0;
	scan->secondary=false;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	if (out==5) {
		if (strcmp(offFrame,OFFFRAMEEQ)==0) {
			scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid equatorial longitude offset";
				return false; //ra
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid equatorial latitude offset";
				return false;  //dec
			}
		}
		else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
			scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid horizontal longitude offset";
				return false;  //azimuth...since they are offsets negative values are valid
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid horizontal latitude offset";
				return false; //elevation
			}
		}
		else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
			scan->offsetFrame=Antenna::ANT_GALACTIC;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
				errMsg="invalid galactic longitude offset";
				return false;  //longitude
			}
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
				errMsg="invalid galactic latitude offset";
				return false; //latitude
			}
		}
		else {
			return false;
		}
		scan->applyOffsets=true;
	}
	else {
		scan->latitudeOffset=0.0;
		scan->longitudeOffset=0.0;
		scan->applyOffsets=false;
	}
	return true;	
}

bool CScanList::parseSidereal(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg)
{
	int start=0;
	IRA::CString token;
	Antenna::TCoordinateFrame frame;
	// get the second item.....
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATOR,token)) {  // id
		errMsg="could read scan identifier";
		return false;
	}
	id=token.ToLong();
	if (id==0) {
		errMsg="scan identifier cannot be zero";
		return false;
	}
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATOR,token)) {  // type
		errMsg="cannot read scan type";
		return false;
	}
	scan->type=Antenna::ANT_SIDEREAL;   //already know it is a sidereal
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATOR,token)) {  // name
		errMsg="cannot read source name";
		return false;
	}
	scan->targetName=CORBA::string_dup((const char *)token);
	bool frameOpen=false;
	bool offFrameOpen=false;
	long counter=0;
	long paramCounter=0;
	scan->latitudeOffset=scan->longitudeOffset=0.0;
	scan->applyOffsets=false;
	scan->paramNumber=0;
	scan->secondary=false;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	frame=Antenna::ANT_EQUATORIAL;
	while (IRA::CIRATools::getNextToken(val,start,SEPARATOR,token)) {  //get the next token...it represents the frame in which the coordinates are expressed
		bool ok=IRA::CIRATools::strToCoordinateFrame(token,frame);
		if ((frame==Antenna::ANT_EQUATORIAL) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong equatorial format";
				return false;  // if the frame has already been expressed, raise an error;
			}
			scan->frame=Antenna::ANT_EQUATORIAL;
			frameOpen=true;
		}
		else if ((frame==Antenna::ANT_GALACTIC) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong galactic format";
				return false;  // if the frame has allready been expressed, raise an error;
			}
			scan->frame=Antenna::ANT_GALACTIC;
			frameOpen=true;
		}
		else if ((frame==Antenna::ANT_HORIZONTAL) && (ok)) {
			if (frameOpen || offFrameOpen) {
				errMsg="wrong horizontal format";
				return false;  // if the frame has allready been expressed, raise an error;
			}
			scan->frame=Antenna::ANT_HORIZONTAL;
			frameOpen=true;
		}
		else if (token==OFFFRAMEEQ) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong equatorial offsets format";
					return false;
				}
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			scan->applyOffsets=true;
			offFrameOpen=true;
		}
		else if (token==OFFFRAMEGAL) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong galactic offsets format";
					return false;
				}
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}			
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			scan->offsetFrame=Antenna::ANT_GALACTIC;
			scan->applyOffsets=true;			
			offFrameOpen=true;
		}
		else if (token==OFFFRAMEHOR) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) {
					errMsg="wrong horizontal offsets format";
					return false;
				}
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}			
			if (offFrameOpen || frameOpen) {
				errMsg="wrong offsets format";
				return false; // if the offsets frame has already been expressed, raise an error;
			}
			scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			scan->applyOffsets=true;			
			offFrameOpen=true;			
		}
		else {
			if (frameOpen) {
				if (counter==0) { //first two parameters are lon and lat
					if (scan->frame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::rightAscensionToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="right ascension format error";
							return false;
						}
					}
					else if (scan->frame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::azimuthToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="azimuth format error";
							return false;
						}
					}
					else if (scan->frame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::galLongitudeToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="galactic longitude format error";
							return false;
						}
					}
					paramCounter++;
					counter++;
				}
				else if (counter==1) {
					if (scan->frame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::declinationToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="declination format error";
							return false;
						}
					}
					else if (scan->frame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::elevationToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="elevation format error";
							return false;
						}
					}
					else if (scan->frame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::galLatitudeToRad(token,scan->parameters[paramCounter],true)) {
							errMsg="galactic latitude format error";
							return false;
						}
					}										
					if ((scan->frame==Antenna::ANT_HORIZONTAL) || (scan->frame==Antenna::ANT_GALACTIC)) {
						frameOpen=false;
						scan->paramNumber=2;
						counter=0;
					}
					else {
						counter++;
						paramCounter++;
					}
				}
				else if ((counter==2) && (scan->frame==Antenna::ANT_EQUATORIAL)) {  // this should be the equinox, but only for equatorial frame
					if (!IRA::CIRATools::strToEquinox(token,scan->equinox)) {
						errMsg="invalid equinox";
						return false;
					}
					counter++;
				}
				else if ((counter<7) && (scan->frame==Antenna::ANT_EQUATORIAL)) { // if the frame is equatorial, up to 7 parameters are allowed
					scan->parameters[paramCounter]=token.ToDouble();
					paramCounter++;
					counter++;
				}
				else {
					errMsg="invalid sidereal scan definition";
					return false;
				}
			}
			else if (offFrameOpen) {
				if (counter==0) {
					if (scan->offsetFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) {
							errMsg="equatorial longitude offset format error";
							return false;
						}
					}
					else if (scan->offsetFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) {
							errMsg="horizontal longitude offset format error";
							return false;
						}
					}
					else if (scan->offsetFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) {
							errMsg="galactic longitude offset format error";
							return false;
						}
					}
					counter++;
				}
				else if (counter==1) {
					if (scan->offsetFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) {
							errMsg="equatorial latitude offset format error";
							return false;
						}
					}
					else if (scan->offsetFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) {
							errMsg="horizontal latitude offset format error";
							return false;
						}
					}
					else if (scan->offsetFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) {
							errMsg="galactic latitude offset format error";
							return false;
						}
					}					
					offFrameOpen=false;
					counter=0;
				}
				else {
					errMsg="too many parameters for offsets definition";
					return false;  // too many parameters in the offset specification
				}
			}
			else {
				errMsg="frame is missing";
				return false; // there is a parameter without having specified the frame
			}
		}
	}
	if (offFrameOpen) {
		errMsg="invalid sidereal scan definition";
		return false;
	}
	if ((frameOpen) && (counter<2)) { 
		errMsg="invalid offsets definition";
		return false;
	}
	if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
		if (counter<3) {
			errMsg="invalid equatorial coordinates definition";
			return false;
		}
		frameOpen=false;
		scan->paramNumber=paramCounter;
		counter=0;
	}	
	if (offFrameOpen || frameOpen) {
		errMsg="invalid sidereal scan definition";
		return false; // if the frame and frame offsets are not specified completely, raise an error
	}
	return true;	
}

bool CScanList::parseOTFC(const IRA::CString& val,Antenna::TTrackingParameters *scan,Antenna::TTrackingParameters *secScan,DWORD& id,IRA::CString& errMsg)
{
	TRecord rec;
	char coordFrame[32],geometry[32],subScanFrame[32],direction[32],type[32],offFrame[32],span[32],lonOff[32],latOff[32];
	DWORD scanCenter;
	double duration;
	Antenna::TCoordinateFrame frame;
	long out;
	scan->targetName=CORBA::string_dup("");
	scan->paramNumber=0;
	scan->latitudeOffset=scan->longitudeOffset=0.0;
	scan->applyOffsets=false;
	scan->type=Antenna::ANT_OTF;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now	
	scan->secondary=true;
	out=sscanf((const char *)val,"%u\t%s\t%u\t%s\t%s\t%s\t%s\t%s\t%lf\t%s\t%s\t%s\t",&id,type,&scanCenter,span,coordFrame,subScanFrame,geometry,direction,&duration,offFrame,lonOff,latOff);
	if ((out!=9) && (out!=12)) {   //parameters are 9 for the OTF plus 3 (not mandatory) for the offsets...
		errMsg="invalid on the fly scan definition";
		return false;
	}
	else {
		//Description
		scan->otf.description=Antenna::SUBSCAN_CENTER;
		scan->otf.lon1=scan->otf.lat1=0.0;
		//duration
		TIMEDIFFERENCE time;
		time.value((long double)duration);
		scan->otf.subScanDuration=time.value().value;
		//coordFrame
		if (!IRA::CIRATools::strToCoordinateFrame(coordFrame,frame)) {
			errMsg="the frame is not known";
			return false;
		}
		if (frame==Antenna::ANT_HORIZONTAL) {
			errMsg="horizontal frame is not supported as coordinate frame";
			return false;
		}
		scan->otf.coordFrame=frame;
		//direction
		if (strcmp(direction,"INC")==0) scan->otf.direction=Antenna::SUBSCAN_INCREASE;
		else if (strcmp(direction,"DEC")==0) scan->otf.direction=Antenna::SUBSCAN_DECREASE;
		else {
			errMsg="invalid on the fly scan direction";
			return false;
		}
		//geometry
		if (strcmp(geometry,"LON")==0) scan->otf.geometry=Antenna::SUBSCAN_CONSTLON;
		else if (strcmp(geometry,"LAT")==0) scan->otf.geometry=Antenna::SUBSCAN_CONSTLAT; //GC not allowed
		else {
			errMsg="invalid on the fly scan geometry";
			return false;
		}
		//subScan Frame
		if (!IRA::CIRATools::strToCoordinateFrame(subScanFrame,frame)) {
			errMsg="invalid on the fly scan frame";
			return false;
		}
		scan->otf.subScanFrame=frame;
		//span
		if (scan->otf.geometry==Antenna::SUBSCAN_CONSTLON) {
			if (!IRA::CIRATools::offsetToRad(span,scan->otf.lat2)) {
				errMsg="invalid on the fly scan latitude span";
				return false; //az span....no need of a check
			}
			scan->otf.lon2=0.0;
		}
		else {
			if (!IRA::CIRATools::offsetToRad(span,scan->otf.lon2)) {
				errMsg="invalid on the fly scan longitude span";
				return false; //el span....no need of a check
			}
			scan->otf.lat2=0.0;
		}
		if (out==12) {
			if (strcmp(offFrame,OFFFRAMEEQ)==0) {
				scan->offsetFrame=Antenna::ANT_EQUATORIAL;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid right ascension offset";
					return false; //ra
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid declination offset";
					return false;  //dec
				}
			}
			else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
				scan->offsetFrame=Antenna::ANT_HORIZONTAL;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid azimuth offset";
					return false;  //azimuth...since they are offsets negative values are valid
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid elevation offset";
					return false;   //elevation
				}
			}
			else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
				scan->offsetFrame=Antenna::ANT_GALACTIC;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid galactic longitude offset";
					return false;  //longitude
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid galactic latitude offset";
					return false; //latitude
				}
			}
			else {
				errMsg="scan offset frame is unknown";
				return false;
			}
			scan->applyOffsets=true;
		}
	}
	if (!getScan(scanCenter,rec)) {
		errMsg=getLastError();
		return false; // the secondary scan does not exist
	}
	Antenna::TTrackingParameters *tmp=static_cast<Antenna::TTrackingParameters *>(rec.primaryParameters);
	secScan->targetName=CORBA::string_dup(tmp->targetName);
	secScan->type=tmp->type;
	for (long k=0;k<Antenna::ANTENNA_TRACKING_PARAMETER_NUMBER;k++) secScan->parameters[k]=tmp->parameters[k];
	secScan->paramNumber=tmp->paramNumber;
	secScan->frame=tmp->frame;
	secScan->equinox=tmp->equinox;
	secScan->longitudeOffset=0.0;
	secScan->latitudeOffset=0.0;
	secScan->applyOffsets=false;
	secScan->section=tmp->section;
	secScan->secondary=false;
	secScan->otf.lon1=tmp->otf.lon1;
	secScan->otf.lat1=tmp->otf.lat1;
	secScan->otf.lon2=tmp->otf.lon2;
	secScan->otf.lat2=tmp->otf.lat2;
	secScan->otf.coordFrame=tmp->otf.coordFrame;
	secScan->otf.geometry=tmp->otf.geometry;
	secScan->otf.subScanFrame=tmp->otf.subScanFrame;
	secScan->otf.description=tmp->otf.description;
	secScan->otf.direction=tmp->otf.direction;
	secScan->otf.subScanDuration=tmp->otf.subScanDuration;
	return true;
}

bool CScanList::parseOTF(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& errMsg)
{
	char coordFrame[32],geometry[32],subScanFrame[32],description[32],direction[32],type[32],offFrame[32],lon1[32],lon2[32],lat1[32],lat2[32],lonOff[32],latOff[32];
	char targetName[32];
	double duration;
	Antenna::TCoordinateFrame frame;
	long out;
	//scan->targetName="";
	scan->secondary=false;
	scan->latitudeOffset=scan->longitudeOffset=0.0;
	scan->applyOffsets=false;
	scan->type=Antenna::ANT_OTF;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	out=sscanf((const char *)val,"%u\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lf\t%s\t%s\t%s",&id,type,targetName,lon1,lat1,lon2,
			lat2,coordFrame,subScanFrame,geometry,description,direction,&duration,offFrame,lonOff,latOff);
	if ((out!=13) && (out!=16)) {   //parameters are 12 for the OTF plus 3 (not mandatory) for the offsets...
		errMsg="invalid on the fly scan definition";
		return false;
	}
	else {
		TIMEDIFFERENCE time;
		time.value((long double)duration);
		scan->otf.subScanDuration=time.value().value;
		scan->targetName=CORBA::string_dup(targetName);
		if (strcmp(description,"SS")==0) scan->otf.description=Antenna::SUBSCAN_STARTSTOP;
		else if (strcmp(description,"CEN")==0) scan->otf.description=Antenna::SUBSCAN_CENTER;
		else {
			errMsg="invalid on the fly scan description";
			return false;
		}		
		if (!IRA::CIRATools::strToCoordinateFrame(coordFrame,frame)) {
			errMsg="the coordinate frame is not known";
			return false;
		}
		if (frame==Antenna::ANT_EQUATORIAL) {
			scan->otf.coordFrame=Antenna::ANT_EQUATORIAL;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::rightAscensionToRad(lon1,scan->otf.lon1,true)) { //ra center ...need a complete check
					errMsg="central point right ascension format error";
					return false;
				}
				if (!IRA::CIRATools::declinationToRad(lat1,scan->otf.lat1,true)) { //dec center ...need a complete check
					errMsg="central point declination format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) { //ra span....no need of a check
					errMsg="right ascension span format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) { //dec span....no need of a check
					errMsg="declination span format error";
					return false;
				}
			}
			else {
				if (!IRA::CIRATools::rightAscensionToRad(lon1,scan->otf.lon1,true)) { //ra first point ...need a complete check
					errMsg="start point right ascension format error";
					return false;
				}
				if (!IRA::CIRATools::declinationToRad(lat1,scan->otf.lat1,true)) { //dec first point ...need a complete check
					errMsg="start point declination format error";
					return false;
				}
				if (!IRA::CIRATools::rightAscensionToRad(lon2,scan->otf.lon2,true)) {  //ra second point ...need a complete check
					errMsg="end point right ascension format error";
					return false;
				}
				if (!IRA::CIRATools::declinationToRad(lat2,scan->otf.lat2,true)) { //dec second point ...need a complete check
					errMsg="end point declination format error";
					return false;
				}
			}
		}
		else if (frame==Antenna::ANT_HORIZONTAL) {
			scan->otf.coordFrame=Antenna::ANT_HORIZONTAL;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::azimuthToRad(lon1,scan->otf.lon1,true)) { //az center ...need a complete check
					errMsg="central point azimuth format error";
					return false;
				}
				if (!IRA::CIRATools::elevationToRad(lat1,scan->otf.lat1,true)) {  //el center ...need a complete check
					errMsg="central point elevation format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) { //az span....no need of a check
					errMsg="azimuth span format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) { //el span....no need of a check
					errMsg="elevation span format error";
					return false;
				}
			}
			else {
				if (!IRA::CIRATools::azimuthToRad(lon1,scan->otf.lon1,true)) { //az first point ...need a complete check
					errMsg="start point azimuth format error";
					return false;
				}
				if (!IRA::CIRATools::elevationToRad(lat1,scan->otf.lat1,true)) { //el first point ...need a complete check
					errMsg="start point elevation format error";
					return false;
				}
				if (!IRA::CIRATools::azimuthToRad(lon2,scan->otf.lon2,true)) { //az second point ...need a complete check
					errMsg="end point azimuth format error";
					return false;
				}
				if (!IRA::CIRATools::elevationToRad(lat2,scan->otf.lat2,true)) { //el second point ...need a complete check
					errMsg="end point elevation format error";
					return false;
				}
			}			
		}
		else if (frame==Antenna::ANT_GALACTIC) {
			scan->otf.coordFrame=Antenna::ANT_GALACTIC;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::galLongitudeToRad(lon1,scan->otf.lon1,true)) { //lon center ...need a complete check
					errMsg="central point galactic longitude format error";
					return false;
				}
				if (!IRA::CIRATools::galLatitudeToRad(lat1,scan->otf.lat1,true)) {  //lat center ...need a complete check
					errMsg="central point galactic latitude format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) { //lon span....no need of a check
					errMsg="galactic longitude span format error";
					return false;
				}
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) {  //lat span....no need of a check
					errMsg="galactic latitude span format error";
					return false;
				}
			}
			else {
				if (!IRA::CIRATools::galLongitudeToRad(lon1,scan->otf.lon1,true)) { //lon first point ...need a complete check
					errMsg="start point galactic longitude format error";
					return false;
				}
				if (!IRA::CIRATools::galLatitudeToRad(lat1,scan->otf.lat1,true)) { //lat first point ...need a complete check
					errMsg="start point galactic latitude format error";
					return false;
				}
				if (!IRA::CIRATools::galLongitudeToRad(lon2,scan->otf.lon2,true)) {  //lon second point ...need a complete check
					errMsg="end point galactic longitude format error";
					return false;
				}
				if (!IRA::CIRATools::galLatitudeToRad(lat2,scan->otf.lat2,true)) {  //lat second point ...need a complete check
					errMsg="end point galactic latitude format error";
					return false;
				}
			}			
		}
		if (!IRA::CIRATools::strToCoordinateFrame(subScanFrame,frame)) {
			errMsg="the scan frame is not known";
			return false;
		}
		scan->otf.subScanFrame=frame;
		if (strcmp(geometry,"LON")==0) scan->otf.geometry=Antenna::SUBSCAN_CONSTLON;
		else if (strcmp(geometry,"LAT")==0) scan->otf.geometry=Antenna::SUBSCAN_CONSTLAT;
		else if (strcmp(geometry,"GC")==0) scan->otf.geometry=Antenna::SUBSCAN_GREATCIRCLE;
		else {
			errMsg="invalid on the fly scan geometry";
			return false;
		}
		if (strcmp(direction,"INC")==0) scan->otf.direction=Antenna::SUBSCAN_INCREASE;
		else if (strcmp(direction,"DEC")==0) scan->otf.direction=Antenna::SUBSCAN_DECREASE;
		else {
			errMsg="invalid on the fly scan direction";
			return false;
		}
		if (out==15) {
			if (strcmp(offFrame,OFFFRAMEEQ)==0) {
				scan->offsetFrame=Antenna::ANT_EQUATORIAL;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid right ascension offset";
					return false; //ra
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid declination offset";
					return false;  //dec
				}
			}
			else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
				scan->offsetFrame=Antenna::ANT_HORIZONTAL;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid azimuth offset";
					return false;  //azimuth...since they are offsets negative values are valid
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid elevation offset";
					return false; //elevation
				}
			}
			else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
				scan->offsetFrame=Antenna::ANT_GALACTIC;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) {
					errMsg="invalid galactic longitude offset";
					return false;  //longitude
				}
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) {
					errMsg="invalid galactic latitude offset";
					return false; //latitude
				}
			}
			else {
				errMsg="scan offset frame is unknown";
				return false;
			}
			scan->applyOffsets=true;
		}
		return true;
	}
}

void CScanList::resetTrackingParameters(Antenna::TTrackingParameters *scan)
{
	scan->secondary=false;
	scan->applyOffsets=false;
	scan->type=Antenna::ANT_NONE;
	scan->paramNumber=0;
}

bool CScanList::string2ScanType(const IRA::CString& val,Management::TScanTypes& type)
{
	if (val==SIDEREAL) {
		type=Management::MNG_SIDEREAL;
		return true;
	}
	else if (val==SUN) {
		type=Management::MNG_SUN;
		return true;
	}
	else if (val==MOON) {
		type=Management::MNG_MOON;
		return true;
	}
	else if (val==SATELLITE) {
		type=Management::MNG_SATELLITE;
		return true;
	}
	else if (val==SOLARSYTEMBODY) {
		type=Management::MNG_SOLARSYTEMBODY;
		return true;
	}
	else if (val==OTF) {
		type=Management::MNG_OTF;
		return true;
	}
	else if (val==OTFC) {
		type=Management::MNG_OTFC;
		return true;
	}
	else if (val==FOCUS) {
		type=Management::MNG_FOCUS;
		return true;
	}
	else {
		return false;
	}
	return true;
}

// CSchedule

CSchedule::CSchedule(const IRA::CString& path,const IRA::CString& fileName) : CBaseSchedule(path,fileName),m_projectName(""),m_observer(""),
	m_scanList(""),m_configList(""),m_backendList(""),m_layoutFile(""),m_scanListUnit(NULL),m_preScanUnit(NULL),m_postScanUnit(NULL),m_backendListUnit(NULL),m_layoutListUnit(NULL),
	m_mode(LST),m_repetitions(0),m_scanTag(-1),m_modeDone(false),m_initProc(_SCHED_NULLTARGET)
{
	m_schedule.clear();
	m_currentScanDef.valid=false;
	m_currentScanDef.line=m_currentScanDef.id=0;
	m_currentScanDef.backendProc=m_currentScanDef.layout=m_currentScanDef.writerInstance=m_currentScanDef.suffix="";
}

CSchedule::~CSchedule()
{
	TIterator p;
	for(p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)!=NULL) delete (*p);
	}
	if (m_scanListUnit) delete m_scanListUnit;
	if (m_postScanUnit) delete m_postScanUnit;
	if (m_preScanUnit) delete m_preScanUnit;
	if (m_backendListUnit) delete m_backendListUnit;
	if (m_layoutListUnit) delete m_layoutListUnit;
}

bool CSchedule::readAll(bool check)
{
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
		DWORD line;
		IRA::CString err;
		if (!checkConsistency(line,err)) {
			m_lastError.Format("File %s has inconsistencies at line %u, error message: %s",(const char*)m_fileName,line,(const char *)err);
			return false;
		}
	}
	return true;
}

bool CSchedule::getInitProc(IRA::CString& procName,ACS::stringSeq& proc)
{
	procName=m_initProc;
	if (m_initProc==_SCHED_NULLTARGET) {
		proc.length(0);
		return true;
	}
	if (!m_preScanUnit->getProcedure(m_initProc,proc)) {		
		m_lastError.Format("Unknown procedure %s",(const char *)m_initProc);
		return false;
	}
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

bool CSchedule::getSubScan_SEQ(DWORD& counter,DWORD& scanid,DWORD& subscanid,double& duration,DWORD& scan,IRA::CString& pre,bool& preBlocking,IRA::CString& post,bool& postBlocking,
		IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix,IRA::CString& layout,bool& rewind)
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
			bckProc=(*p)->backendProc;
			wrtInstance=(*p)->writerInstance;
			suffix=(*p)->suffix;
			layout=(*p)->layout;
			counter=(*p)->counter;
			rewind=false;
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
	bckProc=(*p)->backendProc;
	wrtInstance=(*p)->writerInstance;
	suffix=(*p)->suffix;
	layout=(*p)->layout;
	counter=(*p)->counter;
	rewind=true;
	return true;
}

bool CSchedule::getSubScan_SEQ(TRecord& rec)
{
	return getSubScan_SEQ(rec.counter,rec.scanid,rec.subscanid,rec.duration,rec.scan,rec.preScan,rec.preScanBlocking,rec.postScan,rec.postScanBlocking,rec.backendProc,rec.writerInstance,rec.suffix,
			rec.layout,rec.rewind);
}

bool CSchedule::getSubScan_LST(ACS::TimeInterval& lst,DWORD& counter,DWORD&scanid,DWORD& subscanid,double& duration,DWORD& scan,
		IRA::CString& pre,bool& preBlocking,IRA::CString& post,bool& postBlocking,IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix,IRA::CString& layout,bool& rewind)
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
				bckProc=(*p)->backendProc;
				wrtInstance=(*p)->writerInstance;
				suffix=(*p)->suffix;
				layout=(*p)->layout;
				rewind=false;
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
	bckProc=(*p)->backendProc;
	wrtInstance=(*p)->writerInstance;	
	suffix=(*p)->suffix;
	layout=(*p)->layout;
	rewind=true;
	return true;
}

bool CSchedule::getSubScan_LST(TRecord& rec)
{
	return getSubScan_LST(rec.lst,rec.counter,rec.scanid,rec.subscanid,rec.duration,rec.scan,rec.preScan,rec.preScanBlocking,rec.postScan,rec.postScanBlocking,rec.backendProc,rec.writerInstance,rec.suffix,
			rec.layout,rec.rewind);
}

bool CSchedule::checkConsistency(DWORD& line,IRA::CString& errMsg)
{
	ACS::TimeInterval lst=0;
	TIterator p,i;
	// check that the initialization procedure has been defined
	if (!m_postScanUnit->checkProcedure(m_initProc)) {
		line=0;
		errMsg.Format("schedule initialization procedure %s is not defined",(const char *)m_initProc);
		return false;
	}
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
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
		if (!m_preScanUnit->checkProcedure((*p)->preScan)) {
			line=(*p)->line;
			errMsg.Format("pre scan procedure %s is not defined",(const char *)(*p)->preScan);
			return false;
		}
		if (!m_postScanUnit->checkProcedure((*p)->postScan)) {
			line=(*p)->line;
			errMsg.Format("post scan procedure %s is not defined",(const char *)(*p)->postScan);
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
					errorMsg="unknow schedule mode";
					return false;
				}
			}
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
			else return true;
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
			p->postScanBlocking=isSync(post);
			p->postScan=IRA::CString(post);
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
			p->line=lnNumber;
			p->scanLine=m_currentScanDef.line;
			p->subscanid=subscanid;
			p->scanid=m_currentScanDef.id;
			p->counter=counter;
			p->ut=0;  // this represents the indication that the scan must start as soon as possible
			p->scan=scan;
			p->duration=duration;
			p->preScanBlocking=isSync(pre);
			p->preScan=IRA::CString(pre);
			p->postScanBlocking=isSync(post);
			p->postScan=IRA::CString(post);
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
