// $Id: Schedule.cpp,v 1.17 2011-03-28 10:17:02 a.orlati Exp $

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
//#define OUTPUTFILE "OUTPUTFILE:"
#define SCANLIST "SCANLIST:"
#define PROCLIST "PROCEDURELIST:"
#define BACKENDLIST "BACKENDLIST:"
#define MODE "MODE:"
#define SCANID "SCANID:"
#define INITPROC "INITPROC:"

#define SIDEREAL "SIDEREAL" 
#define SUN "SUN"                 
#define MOON "MOON"              
#define SATELLITE "SATELLITE"    
#define SOLARSYTEMBODY "SOLARSYTEMBODY"
#define OTF "OTF"
#define LSTMODE "LST"
#define SEQMODE "SEQ"

#define FRAMEEQ "EQ"
#define FRAMEHOR "HOR"
#define FRAMEGAL "GAL"
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

bool CParser::parse(CBaseSchedule* unit,DWORD& line)
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
			if (!unit->parseLine(IRA::CString(inLine),line)) {
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
	if (!m_parser->open(m_fileName)) {
		m_lastError.Format("Cannot open file %s",(const char *)m_fileName);
		return false;
	}
	if (!m_parser->parse(this,m_lines)) {
		m_lastError.Format("Parsing error at line %u of file %s",m_lines,(const char *)m_fileName);
		return false;
	}
	m_parser->close();
	if (check) {
		DWORD line;
		if (!checkConsistency(line)) {
			m_lastError.Format("File %s has inconsistencies at line %u",
					(const char*)m_fileName,line);
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

bool CBackendList::parseLine(const IRA::CString& line,const DWORD& lnNumber)
{
	IRA::CString workLine=line;
	workLine.RTrim();
	workLine.LTrim();
	if (m_started) { // if the procedure is started
		if (workLine.Find(PROCEDURE_STOP)>=0) { // if the end breaket is found...close the procedure parsing
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
			return false;
		}
		if (!IRA::CIRATools::getNextToken(workLine,start,PROCEDURE_START,instance)) {  // get the backend name
			return false;
		}
		if (workLine[workLine.GetLength()-1]!=PROCEDURE_START) {
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

bool CBackendList::checkConsistency(DWORD& line)
{
	 TIterator i,p;
	 for (i=m_backend.begin();i<m_backend.end();i++) { //check for duplicated proc names
		 for (p=i+1;p<m_backend.end();p++) {
			 if ((*i)->procName==(*p)->procName) {
				 line=(*p)->line;
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
	m_lastError.Format("Unknown backend procedure %s",(const char *)name);
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
	if (m_currentRecord!=NULL) delete m_currentRecord;
	m_procedure.clear();		
}

bool CProcedureList::parseLine(const IRA::CString& line,const DWORD& lnNumber)
{
	IRA::CString workLine=line;
	workLine.RTrim();
	workLine.LTrim();
	IRA::CString proc;
	if (m_started) { // if the procedure is started
		if (workLine.Find(PROCEDURE_STOP)>=0) { // if the end breaket is found...close the procedure parsing
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
			return false;
		}
		if (workLine[workLine.GetLength()-1]!=PROCEDURE_START) { // check the presence of open bracket
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

bool CProcedureList::checkConsistency(DWORD& line)
{	
	 TIterator i,p;
	 for (i=m_procedure.begin();i<m_procedure.end();i++) { //check for duplicated proc names
		 for (p=i+1;p<m_procedure.end();p++) {
			 if ((*i)->procName==(*p)->procName) {
				 line=(*p)->line;
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
	m_lastError.Format("Unknown procedure %s",(const char *)conf);
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
			Antenna::TTrackingParameters * tmp=static_cast<Antenna::TTrackingParameters * >((*p)->scanPar);
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
	m_lastError.Format("Scan number %u is not known",id);
	return false;
}

bool CScanList::getScan(const DWORD&id,Antenna::TGeneratorType& type,void *&scan,IRA::CString& target)
{
	TIterator i;
	for (i=m_schedule.begin();i<m_schedule.end();i++) {
		if ((*i)->id==id) {
			target=(*i)->target;
			type=(*i)->type;
			scan=(*i)->scanPar;
			return true;
		}
	}
	m_lastError.Format("Scan number %u is not known",id);
	return false;
}

bool CScanList::getScan(const DWORD& id,TRecord& rec)
{
	return getScan(id,rec.type,rec.scanPar,rec.target);
}

 bool CScanList::checkConsistency(DWORD& line)
 {	
	 TIterator i,p;
	 for (i=m_schedule.begin();i<m_schedule.end();i++) {
		 for (p=i+1;p<m_schedule.end();p++) {
			 if ((*i)->id==(*p)->id) {
				 line=(*p)->line;
				 return false;
			 }
		 }
	 }
	 return true;
 }

bool CScanList::parseLine(const IRA::CString& line,const DWORD& lnNumber)
{
	int start=0;
	IRA::CString ret;
	Antenna::TGeneratorType type;
	// get the second item.....
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) {
		return false;
	}
	//...the scan type
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) {
		return false;
	}
	ret.MakeUpper();
	if (!string2ScanType(ret,type)) {
		return false;
	}
	switch (type) {
		case Antenna::ANT_SIDEREAL: {
			DWORD id;
			IRA::CString sourceName;
			Antenna::TTrackingParameters *tmp=new Antenna::TTrackingParameters;
			if (!parseSidereal(line,tmp,id,sourceName)) {
				if (tmp) delete tmp;
				return false;
			}
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->scanPar=(void *) tmp;
			rec->target=sourceName;
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Antenna::ANT_SUN: {
			break;
		}
		case Antenna::ANT_MOON: {
			DWORD id;
			Antenna::TTrackingParameters *tmp=new Antenna::TTrackingParameters;
			if (!parseMoon(line,tmp,id)) {
				if (tmp) delete tmp;
				return false;
			}
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->scanPar=(void *) tmp;
			rec->target="";
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		case Antenna::ANT_SATELLITE: {
			break;
		}
		case Antenna::ANT_SOLARSYTEMBODY: {
			break;
		}
		case Antenna::ANT_OTF: {
			DWORD id;
			Antenna::TTrackingParameters *tmp=new Antenna::TTrackingParameters; 
			if (!parseOTF(line,tmp,id)) {
				if (tmp) delete tmp;
				return false;
			}
			TRecord *rec=new TRecord;
			rec->id=id;
			rec->type=type;
			rec->scanPar=(void *) tmp;
			rec->target="";
			rec->line=lnNumber;
			m_schedule.push_back(rec);
			break;
		}
		default: {
			return false;
		}
	}
	return true;
}

bool CScanList::parseMoon(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id)
{
	char type[32],offFrame[32],lonOff[32],latOff[32];
	long out;
	//double lonOff,latOff;
	out=sscanf((const char *)val,"%u\t%s\t%s\t%s\t%s",&id,type,offFrame,lonOff,latOff);
	if ((out!=2) && (out!=5)) {
		return false;
	}
	scan->type=Antenna::ANT_MOON;
	scan->paramNumber=0;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	if (out==5) {
		if (strcmp(offFrame,OFFFRAMEEQ)==0) {
			scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) return false; //ra
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) return false;  //dec 
		}
		else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
			scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) return false;  //azimuth...since they are offsets negative values are valid
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) return false; //elevation 
		}
		else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
			scan->offsetFrame=Antenna::ANT_GALACTIC;
			if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) return false;  //longitude
			if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) return false; //latitude
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

bool CScanList::parseSidereal(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id,IRA::CString& sourceName)
{
	int start=0;
	IRA::CString token;
	
	// get the second item.....
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATOR,token)) {  // id
		return false;
	}
	id=token.ToLong();
	if (id==0) {
		return false;
	}
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATOR,token)) {  // type
		return false;
	}
	scan->type=Antenna::ANT_SIDEREAL;   //already know it is a sidereal
	if (!IRA::CIRATools::getNextToken(val,start,SEPARATOR,token)) {  // name
		return false;
	}
	sourceName=token;
	bool frameOpen=false;
	bool offFrameOpen=false;
	long counter=0;
	long paramCounter=0;
	scan->latitudeOffset=scan->longitudeOffset=0.0;
	scan->applyOffsets=false;
	scan->paramNumber=0;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	while (IRA::CIRATools::getNextToken(val,start,SEPARATOR,token)) {  //get the next token...it represents the frame in which the coordinares are expressed
		if (token==FRAMEEQ) {
			if (frameOpen || offFrameOpen) return false;  // if the frame has allready been expressed, raise an error;
			scan->frame=Antenna::ANT_EQUATORIAL;
			frameOpen=true;
		}
		else if (token==FRAMEGAL) {
			if (frameOpen || offFrameOpen) return false;  // if the frame has allready been expressed, raise an error;
			scan->frame=Antenna::ANT_GALACTIC;
			frameOpen=true;
		}
		else if (token==FRAMEHOR) {
			if (frameOpen || offFrameOpen) return false;  // if the frame has allready been expressed, raise an error;
			scan->frame=Antenna::ANT_HORIZONTAL;
			frameOpen=true;
		}
		else if (token==OFFFRAMEEQ) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) return false;
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}
			if (offFrameOpen || frameOpen) return false; // if the offsets frame has already been expressed, raise an error;
			scan->offsetFrame=Antenna::ANT_EQUATORIAL;
			scan->applyOffsets=true;
			offFrameOpen=true;
		}
		else if (token==OFFFRAMEGAL) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) return false;
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}			
			if (offFrameOpen || frameOpen) return false; // if the offsets frame has allready been expressed, raise an error;
			scan->offsetFrame=Antenna::ANT_GALACTIC;
			scan->applyOffsets=true;			
			offFrameOpen=true;
		}
		else if (token==OFFFRAMEHOR) {
			if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
				if (counter<3) return false;
				frameOpen=false;
				scan->paramNumber=paramCounter;
				counter=0;
			}			
			if (offFrameOpen || frameOpen) return false; // if the offsets frame has already been expressed, raise an error;
			scan->offsetFrame=Antenna::ANT_HORIZONTAL;
			scan->applyOffsets=true;			
			offFrameOpen=true;			
		}
		else {
			if (frameOpen) {
				if (counter==0) { //first two parameters are lon and lat
					if (scan->frame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::rightAscensionToRad(token,scan->parameters[paramCounter],true)) return false;
					}
					else if (scan->frame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::azimuthToRad(token,scan->parameters[paramCounter],true)) return false;
					}
					else if (scan->frame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::galLongitudeToRad(token,scan->parameters[paramCounter],true)) return false;
					}
					paramCounter++;
					counter++;
				}
				else if (counter==1) {
					if (scan->frame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::declinationToRad(token,scan->parameters[paramCounter],true)) return false;
					}
					else if (scan->frame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::elevationToRad(token,scan->parameters[paramCounter],true)) return false;
					}
					else if (scan->frame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::galLatitudeToRad(token,scan->parameters[paramCounter],true)) return false;
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
					double eqx=token.ToLong();
					counter++;
					if (eqx==1950) scan->equinox=Antenna::ANT_B1950;
					else if (eqx==2000) scan->equinox=Antenna::ANT_J2000;
					else if (eqx==-1) scan->equinox=Antenna::ANT_APPARENT;
					else {
						return false;
					}
				}
				else if ((counter<7) && (scan->frame==Antenna::ANT_EQUATORIAL)) { // if the frame is equatorial, up to 7 parameters are allowed
					scan->parameters[paramCounter]=token.ToDouble();
					paramCounter++;
					counter++;
				}
				else return false;
			}
			else if (offFrameOpen) {
				if (counter==0) {
					if (scan->offsetFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) return false;
					}
					else if (scan->offsetFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) return false;
					}
					else if (scan->offsetFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::offsetToRad(token,scan->longitudeOffset)) return false;
					}
					counter++;
				}
				else if (counter==1) {
					if (scan->offsetFrame==Antenna::ANT_EQUATORIAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) return false;
					}
					else if (scan->offsetFrame==Antenna::ANT_HORIZONTAL) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) return false;
					}
					else if (scan->offsetFrame==Antenna::ANT_GALACTIC) {
						if (!IRA::CIRATools::offsetToRad(token,scan->latitudeOffset)) return false;
					}					
					offFrameOpen=false;
					counter=0;
				}
				else return false;  // too many parameters in the offset specification
			}
			else return false; // there is a parameter without having specified the frame
		}
	}
	if (offFrameOpen) {
		return false;
	}
	if ((frameOpen) && (counter<2)) { 
		return false;
	}
	if (frameOpen && (scan->frame==Antenna::ANT_EQUATORIAL)) {
		if (counter<3) return false;
		frameOpen=false;
		scan->paramNumber=paramCounter;
		counter=0;
	}	
	if (offFrameOpen || frameOpen) return false; // if the frame and frame offsets are not specified completely, raise an error
	return true;	
}

bool CScanList::parseOTF(const IRA::CString& val,Antenna::TTrackingParameters *scan,DWORD& id)
{
	char coordFrame[32],geometry[32],subScanFrame[32],description[32],direction[32],type[32],offFrame[32],lon1[32],lon2[32],lat1[32],lat2[32],lonOff[32],latOff[32];
	double duration;
	long out;
	scan->latitudeOffset=scan->longitudeOffset=0.0;
	scan->applyOffsets=false;
	scan->type=Antenna::ANT_OTF;
	scan->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	out=sscanf((const char *)val,"%u\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lf\t%s\t%s\t%s",&id,type,lon1,lat1,lon2,
			lat2,coordFrame,subScanFrame,geometry,description,direction,&duration,offFrame,lonOff,latOff);
	if ((out!=12) && (out!=15)) {   //parameters are 12 for the OTF plus 3 (not mandatory) for the offsets...
		return false;
	}
	else {
		TIMEDIFFERENCE time;
		/*scan->otf.lon1*=DD2R;
		scan->otf.lat1*=DD2R;
		scan->otf.lon2*=DD2R;
		scan->otf.lat2*=DD2R;*/
		time.value((long double)duration);
		scan->otf.subScanDuration=time.value().value;
		if (strcmp(description,"SS")==0) scan->otf.description=Antenna::SUBSCAN_STARTSTOP;
		else if (strcmp(description,"CEN")==0) scan->otf.description=Antenna::SUBSCAN_CENTER;
		else {
			return false;
		}		
		if (strcmp(coordFrame,FRAMEEQ)==0) {
			scan->otf.coordFrame=Antenna::ANT_EQUATORIAL;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::rightAscensionToRad(lon1,scan->otf.lon1,true)) return false; //ra center ...need a complete check
				if (!IRA::CIRATools::declinationToRad(lat1,scan->otf.lat1,true)) return false;  //dec center ...need a complete check
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) return false; //ra span....no need of a check
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) return false;  //dec span....no need of a check
			}
			else {
				if (!IRA::CIRATools::rightAscensionToRad(lon1,scan->otf.lon1,true)) return false; //ra first point ...need a complete check
				if (!IRA::CIRATools::declinationToRad(lat1,scan->otf.lat1,true)) return false;  //dec first point ...need a complete check
				if (!IRA::CIRATools::rightAscensionToRad(lon2,scan->otf.lon2,true)) return false; //ra second point ...need a complete check
				if (!IRA::CIRATools::declinationToRad(lat2,scan->otf.lat2,true)) return false;  //dec second point ...need a complete check				
			}
		}
		else if (strcmp(coordFrame,FRAMEHOR)==0) {
			scan->otf.coordFrame=Antenna::ANT_HORIZONTAL;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::azimuthToRad(lon1,scan->otf.lon1,true)) return false; //az center ...need a complete check
				if (!IRA::CIRATools::elevationToRad(lat1,scan->otf.lat1,true)) return false;  //el center ...need a complete check
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) return false; //az span....no need of a check
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) return false;  //el span....no need of a check
			}
			else {
				if (!IRA::CIRATools::azimuthToRad(lon1,scan->otf.lon1,true)) return false; //az first point ...need a complete check
				if (!IRA::CIRATools::elevationToRad(lat1,scan->otf.lat1,true)) return false;  //el first point ...need a complete check
				if (!IRA::CIRATools::azimuthToRad(lon2,scan->otf.lon2,true)) return false; //az second point ...need a complete check
				if (!IRA::CIRATools::elevationToRad(lat2,scan->otf.lat2,true)) return false;  //el second point ...need a complete check				
			}			
		}
		else if (strcmp(coordFrame,FRAMEGAL)==0) {
			scan->otf.coordFrame=Antenna::ANT_GALACTIC;
			if (scan->otf.description==Antenna::SUBSCAN_CENTER) {
				if (!IRA::CIRATools::galLongitudeToRad(lon1,scan->otf.lon1,true)) return false; //lon center ...need a complete check
				if (!IRA::CIRATools::galLatitudeToRad(lat1,scan->otf.lat1,true)) return false;  //lat center ...need a complete check
				if (!IRA::CIRATools::offsetToRad(lon2,scan->otf.lon2)) return false; //lon span....no need of a check
				if (!IRA::CIRATools::offsetToRad(lat2,scan->otf.lat2)) return false;  //lat span....no need of a check
			}
			else {
				if (!IRA::CIRATools::galLongitudeToRad(lon1,scan->otf.lon1,true)) return false; //lon first point ...need a complete check
				if (!IRA::CIRATools::galLatitudeToRad(lat1,scan->otf.lat1,true)) return false;  //lat first point ...need a complete check
				if (!IRA::CIRATools::galLongitudeToRad(lon2,scan->otf.lon2,true)) return false; //lon second point ...need a complete check
				if (!IRA::CIRATools::galLatitudeToRad(lat2,scan->otf.lat2,true)) return false;  //lat second point ...need a complete check				
			}			
		}
		else {
			return false;
		}
		if (strcmp(subScanFrame,FRAMEEQ)==0) scan->otf.subScanFrame=Antenna::ANT_EQUATORIAL;
		else if (strcmp(subScanFrame,FRAMEHOR)==0) scan->otf.subScanFrame=Antenna::ANT_HORIZONTAL;
		else if (strcmp(subScanFrame,FRAMEGAL)==0) scan->otf.subScanFrame=Antenna::ANT_GALACTIC;
		else {
			return false;
		}
		if (strcmp(geometry,"LON")==0) scan->otf.geometry=Antenna::SUBSCAN_CONSTLON;
		else if (strcmp(geometry,"LAT")==0) scan->otf.geometry=Antenna::SUBSCAN_CONSTLAT;
		else if (strcmp(geometry,"GC")==0) scan->otf.geometry=Antenna::SUBSCAN_GREATCIRCLE;
		else {
			return false;
		}
		if (strcmp(direction,"INC")==0) scan->otf.direction=Antenna::SUBSCAN_INCREASE;
		else if (strcmp(direction,"DEC")==0) scan->otf.direction=Antenna::SUBSCAN_DECREASE;
		else {
			return false;
		}
		if (out==15) {
			if (strcmp(offFrame,OFFFRAMEEQ)==0) {
				scan->offsetFrame=Antenna::ANT_EQUATORIAL;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) return false; //ra
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) return false;  //dec 
			}
			else if (strcmp(offFrame,OFFFRAMEHOR)==0) {
				scan->offsetFrame=Antenna::ANT_HORIZONTAL;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) return false;  //azimuth...since they are offsets negative values are valid
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) return false; //elevation 
			}
			else if (strcmp(offFrame,OFFFRAMEGAL)==0) {
				scan->offsetFrame=Antenna::ANT_GALACTIC;
				if (!IRA::CIRATools::offsetToRad(lonOff,scan->longitudeOffset)) return false;  //longitude
				if (!IRA::CIRATools::offsetToRad(latOff,scan->latitudeOffset)) return false; //latitude
			}
			else {
				return false;
			}
			scan->applyOffsets=true;
		}
		return true;
	}
}	

bool CScanList::string2ScanType(const IRA::CString& val,Antenna::TGeneratorType& type)
{
	if (val==SIDEREAL) {
		type=Antenna::ANT_SIDEREAL;
		return true;
	}
	else if (val==SUN) {
		type=Antenna::ANT_SUN;
		return true;
	}
	else if (val==MOON) {
		type=Antenna::ANT_MOON;
		return true;
	}
	else if (val==SATELLITE) {
		type=Antenna::ANT_SATELLITE;
		return true;
	}
	else if (val==SOLARSYTEMBODY) {
		type=Antenna::ANT_SOLARSYTEMBODY;
		return true;
	}
	else if (val==OTF) {
		type=Antenna::ANT_OTF;
		return true;
	}
	else {
		return false;
	}
	return true;
}

// CSchedule

CSchedule::CSchedule(const IRA::CString& path,const IRA::CString& fileName) : CBaseSchedule(path,fileName),m_projectName(""),m_observer(""),
	m_scanList(""),m_configList(""),m_backendList(""),m_scanListUnit(NULL),m_preScanUnit(NULL),m_postScanUnit(NULL),m_backendListUnit(NULL),
	m_mode(LST),m_repetitions(0),m_scanId(-1),m_modeDone(false),m_initProc(_SCHED_NULLTARGET)
{
	m_schedule.clear();
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
}

bool CSchedule::readAll(bool check)
{
	if (!CBaseSchedule::readAll(false)) { //the check is posticipated at the time of the other file are opened
		return false;
	}
	if (isComplete()) {
		m_scanListUnit=new CScanList(m_filePath,m_scanList);
		if (!m_scanListUnit->readAll(check)) {
			m_lastError.Format("Error in scan list: %s",(const char *)m_scanListUnit->getLastError());
			return false;
		}
		m_postScanUnit=new CProcedureList(m_filePath,m_configList);
		m_preScanUnit=new CProcedureList(m_filePath,m_configList);	
		if (!m_preScanUnit->readAll(check)) {
			m_lastError.Format("Error in procedures list: %s",(const char *)m_preScanUnit->getLastError());
			return false;
		}
		if (!m_postScanUnit->readAll(check)) {
			m_lastError.Format("Error in procedures list: %s",(const char *)m_postScanUnit->getLastError());
			return false;
		}
		m_backendListUnit=new CBackendList(m_filePath,m_backendList);
		if (!m_backendListUnit->readAll(check)) {
			m_lastError.Format("Error in backend list: %s",(const char *)m_backendListUnit->getLastError());
			return false;
		}
		// check that the initialization procedure has been defined
		if (!m_postScanUnit->checkProcedure(m_initProc)) {
			m_lastError.Format("The init procedure has not been defined!");
			return false;
		}
	}
	else {
		m_lastError.Format("One or more schedule sections are missing in file : %s",(const char *)m_fileName);
		return false;
	}
	if (check) {
		DWORD line;
		if (!checkConsistency(line)) {
			m_lastError.Format("File %s has inconsistencies at line %u",
					(const char*)m_fileName,line);
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

bool CSchedule::getLine(const DWORD& line,unsigned long long& time,double& duration,DWORD& scan,IRA::CString& pre,bool& preBlocking,IRA::CString& post,bool& postBlocking,IRA::CString& bckProc,
		IRA::CString& wrtInstance,IRA::CString& suffix)
{
	if ((line>0)&&(line-1<=m_schedule.size())) {
		duration=m_schedule[line-1]->duration;
		scan=m_schedule[line-1]->scan;
		pre=m_schedule[line-1]->preScan;
		post=m_schedule[line-1]->postScan;
		bckProc=m_schedule[line-1]->backendProc;
		wrtInstance=m_schedule[line-1]->writerInstance;
		suffix=m_schedule[line-1]->suffix;
		preBlocking=m_schedule[line-1]->preScanBlocking;
		postBlocking=m_schedule[line-1]->postScanBlocking;
		if (m_mode==LST) {
			time=m_schedule[line-1]->lst;
		}
		else time=m_schedule[line-1]->ut;
		return true;
	}
	else {
		m_lastError.Format("Line number %u is not present",line);
		return false;
	}	
}

bool CSchedule::getScan_SEQ(DWORD& id,double& duration,DWORD& scan,IRA::CString& pre,bool& preBlocking,IRA::CString& post,bool& postBlocking,
		IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix)
{
	if (m_mode!=SEQ) {
		m_lastError.Format("The current schedule is not a sequence");
		return false;
	}
	if (m_schedule.size()==0) {
		m_lastError.Format("The current schedule is empty");
		return false;
	}
	TIterator p;
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)->id>id) {
			duration=(*p)->duration;
			scan=(*p)->scan;
			pre=(*p)->preScan;
			post=(*p)->postScan;
			preBlocking=(*p)->preScanBlocking;
			postBlocking=(*p)->postScanBlocking;
			bckProc=(*p)->backendProc;
			wrtInstance=(*p)->writerInstance;
			suffix=(*p)->suffix;
			id=(*p)->id;
			return true;
		}
	}	
	p=m_schedule.begin();
	scan=(*p)->scan;
	duration=(*p)->duration;
	pre=(*p)->preScan;
	post=(*p)->postScan;
	preBlocking=(*p)->preScanBlocking;
	postBlocking=(*p)->postScanBlocking;	
	bckProc=(*p)->backendProc;
	wrtInstance=(*p)->writerInstance;	
	suffix=(*p)->suffix;
	id=(*p)->id;	
	return true;
}

bool CSchedule::getScan_SEQ(TRecord& rec)
{
	return getScan_SEQ(rec.id,rec.duration,rec.scan,rec.preScan,rec.preScanBlocking,rec.postScan,rec.postScanBlocking,rec.backendProc,rec.writerInstance,rec.suffix);
}

bool CSchedule::getScan_LST(ACS::TimeInterval& lst,DWORD& id,double& duration,DWORD& scan,
		IRA::CString& pre,bool& preBlocking,IRA::CString& post,bool& postBlocking,IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix)
{
	if (m_mode!=LST) {
		m_lastError.Format("The current schedule is not LST based");
		return false;
	}
	TIterator p;
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)->id>id) {
			if ((*p)->lst>lst) {
				lst=(*p)->lst;
				id=(*p)->id;
				scan=(*p)->scan;
				duration=(*p)->duration;
				pre=(*p)->preScan;
				post=(*p)->postScan;
				preBlocking=(*p)->preScanBlocking;
				postBlocking=(*p)->postScanBlocking;
				bckProc=(*p)->backendProc;
				wrtInstance=(*p)->writerInstance;
				suffix=(*p)->suffix;
				return true;
			}
		}
	}
	//that means that no scheduled lst are later than the given one...than take the first one which will be the next day.
	p=m_schedule.begin();
	// This is to bypass the following problem in the scheduler:
	// Coming from the last scan of the schedule, if the next (which is the first of the schedule) scan is late.....the scheduler will call again this function
	// again and again until the current lst is elepsed...since this function in this case returns the first scan of the schedule
	if ( (*p)->id==id ) {
		p++;
	}
	lst=(*p)->lst;
	id=(*p)->id;
	scan=(*p)->scan;
	duration=(*p)->duration;
	pre=(*p)->preScan;
	post=(*p)->postScan;
	preBlocking=(*p)->preScanBlocking;
	postBlocking=(*p)->postScanBlocking;	
	bckProc=(*p)->backendProc;
	wrtInstance=(*p)->writerInstance;	
	suffix=(*p)->suffix;
	return true;
}

bool CSchedule::getScan_LST(TRecord& rec)
{
	return getScan_LST(rec.lst,rec.id,rec.duration,rec.scan,rec.preScan,rec.preScanBlocking,rec.postScan,rec.postScanBlocking,rec.backendProc,rec.writerInstance,rec.suffix);
}

/*bool CSchedule::getLine_UT(ACS::Time& ut,DWORD& id,double& duration,DWORD& scan,
		IRA::CString& pre,IRA::CString& post,IRA::CString& bckProc,IRA::CString& wrtInstance,IRA::CString& suffix)
{
	if (m_mode!=UT) {
		m_lastError.Format("The current schedule is not UT based");
		return false;
	}
	TIterator p;
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)->id>id) {
			if ((*p)->ut>ut) {
				ut=(*p)->ut;
				id=(*p)->id;
				scan=(*p)->scan;
				duration=(*p)->duration;
				pre=(*p)->preScan;
				post=(*p)->postScan;
				bckProc=(*p)->backendProc;
				wrtInstance=(*p)->writerInstance;
				suffix=(*p)->suffix;
				return true;
			}
		}
	}
	m_lastError.Format("No next line in the schedule");
	return false;
}*/

bool CSchedule::checkConsistency(DWORD& line)
{
	//ACS::Time ut=0;
	ACS::TimeInterval lst=0;
	TIterator p;
	//DWORD count=1;
	DWORD id=0;
	for (p=m_schedule.begin();p<m_schedule.end();p++) {
		if ((*p)->id<=id) {
			line=(*p)->line;
			return false;
		}
		else {
			id=(*p)->id;
		}
		if (m_mode==LST) { //check that all the time stamps are increasing....
			if ((*p)->lst<lst) {
				line=(*p)->line;
				return false;
			}
			else {
				lst=(*p)->lst;
			}
		}
		/*else if (m_mode==UT) {
			if ((*p)->ut<ut) {
				line=(*p)->line;
				return false;
			}
			else {
				ut=(*p)->ut;
			}			
		}*/
		if (!m_scanListUnit->checkScan((*p)->scan) ) {
			line=(*p)->line;
			return false;
		}
		if (!m_preScanUnit->checkProcedure((*p)->preScan)) {
			line=(*p)->line;
			return false;
		}
		if (!m_postScanUnit->checkProcedure((*p)->postScan)) {
			line=(*p)->line;
			return false;
		}
		if (!m_backendListUnit->checkBackend((*p)->backendProc)) {
			line=(*p)->line;
			return false;
		}
		//count++;
	}
	return true;
}

bool CSchedule::parseLine(const IRA::CString& line,const DWORD& lnNumber)
{
	int start=0;
	IRA::CString ret;
	if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,ret)) {
		return false;
	}
	else {
		ret.MakeUpper();
		if (ret==PROJECT) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_projectName)) {
				return false;
			}
			else return true;
		}
		else if (ret==OBSERVER) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_observer)) {
				return false;
			}
			else return true;			
		}
		else if (ret==SCANLIST) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_scanList)) {
				return false;
			}
			else return true;
		}
		else if (ret==PROCLIST) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_configList)) {
				return false;
			}
			else return true;
		}
		else if (ret==BACKENDLIST) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_backendList)) {
				return false;
			}
			else return true;
		}
		else if (ret==MODE) {
			IRA::CString mode,rep,startTime;
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,mode)) {
				return false;
			}
			else {
				m_modeDone=true;
				if (mode==LSTMODE) {
					if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,rep)) {
						return false;
					}
					m_repetitions=rep.ToLong();
					m_mode=LST;
					if (m_repetitions!=0) return true;  // a zero value is not allowed
					else return false;
				}
				else if (mode==SEQMODE) {
					if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,startTime)) {
						m_mode=SEQ; //if it has not an extra argument the sequncial is pure
						return true;
					}
					else {
						//otherwise it is sequencial qith start time
						if (!parseLST(startTime,m_startLST)) return false;
						m_mode=TIMETAGGED;
						return true;
					}

				}
				else return false;
			}
		}
		else if (ret==SCANID) {
			IRA::CString scanId;
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,scanId)) {
				return false;
			}			
			else {
				m_scanId=scanId.ToLong();
			}
			return true;
		}
		else if (ret==INITPROC) {
			if (!IRA::CIRATools::getNextToken(line,start,SEPARATOR,m_initProc)) {
				return false;
			}
			else return true;
		}
		else { // check the schedule
			DWORD id,scan,prevId;
			double duration;
			char localLST[256],pre[256],post[256],backend[256],suffix[256];
			int start=0;
			IRA::CString storageCommand,backendProc,writerInstance;
			ACS::TimeInterval lst;
			//ACS::Time ut;
			TRecord *p;
			if (m_mode==LST) {
				if (sscanf((const char *)line,"%u\t%s\t%s\t%lf\t%u\t%s\t%s\t%s",&id,suffix,localLST,&duration,&scan,pre,post,backend)!=8) {
					return false;
				}
				else {
					if (!m_schedule.empty()) {
						prevId=m_schedule.back()->id;
						if (id<=prevId) {  //check that the line numeration is increasing
							return false;
						}
					}
					storageCommand=IRA::CString(backend);
					if (!IRA::CIRATools::getNextToken(storageCommand,start,PROCEDURE_SEPARATOR,backendProc)) {
						return false;
					}
					if (backendProc!=_SCHED_NULLTARGET) {
						if (!IRA::CIRATools::getNextToken(storageCommand,start,PROCEDURE_SEPARATOR,writerInstance)) {
							return false;
						}
					}
					else {
						writerInstance=_SCHED_NULLTARGET;
					}
					if (!parseLST(IRA::CString(localLST),lst)) return false;
					p=new TRecord;
					p->id=id;
					p->lst=lst;
					p->scan=scan;
					p->duration=duration;
					p->preScanBlocking=isSync(pre);
					p->preScan=IRA::CString(pre);
					p->postScanBlocking=isSync(post);
					p->postScan=IRA::CString(post);
					p->backendProc=backendProc;
					p->writerInstance=writerInstance;
					p->suffix=IRA::CString(suffix);
					p->line=lnNumber;
					m_schedule.push_back(p);
				}
			}
			/*else if (m_mode==UT) {
				if (sscanf((const char *)line,"%u\t%s\t%s\t%lf\t%u\t%s\t%s\t%s",&id,suffix,localLST,&duration,&scan,pre,post,backend)!=8) {
					return false;
				}
				else {
					if (!m_schedule.empty()) {
						prevId=m_schedule.back()->id;
						if (id<=prevId) {  //check that the line numeration is increasing
							return false;
						}
					}
					storageCommand=IRA::CString(backend);
					if (!IRA::CIRATools::getNextToken(storageCommand,start,PROCEDURE_SEPARATOR,backendProc)) {
						return false;
					}
					if (backendProc!=_SCHED_NULLTARGET) {
						if (!IRA::CIRATools::getNextToken(storageCommand,start,PROCEDURE_SEPARATOR,writerInstance)) {
							return false;
						}
					}
					if (!parseUT(IRA::CString(localLST),ut)) return false;
					p=new TRecord;
					p->id=id;
					p->ut=ut;
					p->scan=scan;
					p->duration=duration;
					p->preScanBlocking=isSync(pre);
					p->preScan=IRA::CString(pre);
					p->postScanBlocking=isSync(post);
					p->postScan=IRA::CString(post);					
					p->backendProc=backendProc;
					p->writerInstance=writerInstance;
					p->suffix=IRA::CString(suffix);
					p->line=lnNumber;
					m_schedule.push_back(p);
				}				
			}*/
			else if ((m_mode==SEQ) || (m_mode==TIMETAGGED)) {
				if (sscanf((const char *)line,"%u\t%s\t%lf\t%u\t%s\t%s\t%s",&id,suffix,&duration,&scan,pre,post,backend)!=7) {
					return false;
				}
				else {
					if (id!=m_schedule.size()+1) { //id numeration starts with 1
						return false;
					}
					storageCommand=IRA::CString(backend);
					if (!IRA::CIRATools::getNextToken(storageCommand,start,PROCEDURE_SEPARATOR,backendProc)) {
						return false;
					}
					if (backendProc!=_SCHED_NULLTARGET) {
						if (!IRA::CIRATools::getNextToken(storageCommand,start,PROCEDURE_SEPARATOR,writerInstance)) {
							return false;
						}
					}
					p=new TRecord;
					p->id=id;
					p->ut=0;  // this represents the indication that the scan must start as soon as possible
					p->scan=scan;
					p->duration=duration;
					p->preScanBlocking=isSync(pre);
					p->preScan=IRA::CString(pre);
					p->postScanBlocking=isSync(post);
					p->postScan=IRA::CString(post);
					p->backendProc=backendProc;
					p->writerInstance=writerInstance;
					p->suffix=IRA::CString(suffix);
					p->line=lnNumber;
					m_schedule.push_back(p);
				}					
			}
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
		m_lastError.Format("Some sections of the schedule are missing");
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
	
	/*long hh,mm,ss;
	double sec;
	if (sscanf((const char *)val,"%ld:%ld:%ld.%lf",&hh,&mm,&ss,&sec)!=4) {
		return false;
	}
	else {
		if (hh<0 || hh>23) return false;
		if (mm<0 || mm>59) return false;
		if (ss<0 || ss>59) return false;
		TIMEDIFFERENCE tt;
		tt.reset();
		tt.normalize(true);
		tt.hour(hh);
		tt.minute(mm);
		tt.second(ss);
		tt.microSecond((unsigned long)(sec*1000));
		lst=tt.value().value;
		return true;
	}*/
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
