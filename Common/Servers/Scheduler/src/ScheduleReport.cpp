#include "ScheduleReport.h"

using namespace IRA;

CScheduleReport::CScheduleReport(const IRA::CString logPath,const IRA::CString& reportPath,
		const IRA::CString& backupPath,const IRA::CString& lock): m_lastError(""),m_logFilePath(logPath),m_reportPath(reportPath),
		m_backupPath(backupPath), m_lockFile(lock),m_scheduleBaseName(""),m_scheduleBasePath(""),
		m_scheduleExtension(""), m_currentPostfix(""),m_active(false),m_reportEnabled(false),
		m_lockFileEnabled(false), m_scheduleProvided(false)
{
	if ((m_reportPath!="") && (m_backupPath!="")) m_reportEnabled=true;
	if (m_lockFile!="") m_lockFileEnabled=true;
	m_auxFiles.clear();
	m_scanPaths.clear();
}

CScheduleReport::CScheduleReport() :m_lastError(""),m_logFilePath(""),m_reportPath(""),
		m_backupPath(""), m_lockFile(""),m_scheduleBaseName(""),m_scheduleBasePath(""),
		m_scheduleExtension(""), m_currentPostfix(""),m_active(false),m_reportEnabled(false),
		m_lockFileEnabled(false), m_scheduleProvided(false)
{
	m_auxFiles.clear();
	m_scanPaths.clear();
}

CScheduleReport::~CScheduleReport()
{
	m_active=false;
	m_scheduleProvided=false;
	m_lockFileEnabled=false;
	m_reportEnabled=false;
	m_auxFiles.clear();
	m_scanPaths.clear();
}

void CScheduleReport::addScheduleName(const IRA::CString& fullName)
{
	generatePostfix();
	if (CIRATools::extractFileName(fullName,m_scheduleBasePath,m_scheduleBaseName,m_scheduleExtension)) {
		m_scheduleProvided=true;
		m_auxFiles.push_back(fullName);
	}
	else {
		m_scheduleBasePath=m_scheduleBaseName=m_scheduleExtension="";
		m_scheduleProvided=false;
	}
}

void CScheduleReport::addAuxScheduleFile(const IRA::CString& fullName)
{
	if (m_scheduleProvided) {
		m_auxFiles.push_back(fullName);
	}
}

bool CScheduleReport::activate()
{
	bool ret=true;
	if (!createLock()) {  // create the lock, if not configured the method will not fail
		m_lastError="the lock file could not be created";
		ret=false;
	}
	if (m_reportEnabled) {  // check if the reporting is configured
		if (!m_scheduleProvided) { // if this case a schedule must be provided!
			m_lastError="no schedule has been provided";
			ret=false;
		}
		else { //make a copy of the schedule
			ret=backupSchedule();
		}
	}
	m_active=true; // this is always set because I want to keep going even if the report fails.
	return ret;
}

IRA::CString CScheduleReport::getLogFileName() const
{
	return m_scheduleBaseName+"_"+m_currentPostfix+".log";
}

IRA::CString CScheduleReport::getSchedule() const
{
		return m_scheduleBaseName+"_"+m_currentPostfix;
}

IRA::CString CScheduleReport::getReportFileName() const
{
	return m_reportPath+m_scheduleBaseName+"_"+m_currentPostfix+".rep";
}

IRA::CString CScheduleReport::getBackupFolder() const
{
	return m_backupPath+m_scheduleBaseName+"_"+m_currentPostfix;
}

void CScheduleReport::addScanPath(const IRA::CString& scan)
{
	CScheduleReport::TInternalListIter it;
	IRA::CString copyStr;	
	copyStr=scan;
	removeTrailingSlash(copyStr);
	if (m_reportEnabled) {
		for (it=m_scanPaths.begin();it!=m_scanPaths.end();++it) {
			if (copyStr==(*it)) return;
		}
		m_scanPaths.push_front(copyStr);
	}
}

bool CScheduleReport::deactivate()
{
	bool ret=true;
	if (!writeReport()) {
		ret=false;
	}
	if (!removeLock()) {
		ret=false;
	}
	m_active=false;
	m_scheduleProvided=false;
	m_auxFiles.clear();
	m_scanPaths.clear();
	return ret;
}

void CScheduleReport::generatePostfix()
{
	TIMEVALUE currentUT;
 	IRA::CIRATools::getTime(currentUT); // get the current time
 	m_currentPostfix.Format("%04d%03d%02d%02d%02d",currentUT.year(),currentUT.dayOfYear(),currentUT.hour(),currentUT.minute(),
 			currentUT.second());
}

bool CScheduleReport::createLock()
{
	if (m_lockFileEnabled) {
		if (!CIRATools::fileExists(m_lockFile)) {
			if (!CIRATools::createEmptyFile(m_lockFile)) {
				m_lastError="the lock file could not be created";
				return false;
			}
			else {
				return true;
			}
		}
		else {
			return true;
		}
	}
	return true;
}

bool CScheduleReport::removeLock()
{
	if (m_lockFileEnabled) {
		if (CIRATools::fileExists(m_lockFile)) {
			if (!CIRATools::deleteFile(m_lockFile)) {
				m_lastError="the lock file could not be deleted";
				return false;
			}
			else {
				return true;
			}
		}
		else {
			return true;
		}
	}
	return true;
}

bool CScheduleReport::backupSchedule()
{
	IRA::CString source;
	IRA::CString dest;
	IRA::CString destFolder;
	CScheduleReport::TInternalListIter it;
	IRA::CString basePath,baseName,ext;
	destFolder=getBackupFolder();
	if (!CIRATools::directoryExists(destFolder)) { // is the folder does not exists....
		 if (!CIRATools::makeDirectory(destFolder)) {
			 m_lastError.Format("cannot create folder %s",(const char *)destFolder);
			 return false;
		 }
	}
	for (it=m_auxFiles.begin();it!=m_auxFiles.end();++it) {
		source=*it;
		if (!CIRATools::extractFileName(source,basePath,baseName,ext)) {
			 m_lastError.Format("error in preparing the copy of file %s",(const char *)source);
			 return false;
		}
		dest=destFolder+"/"+baseName+"."+ext;
		if (!CIRATools::copyFile(source,dest)) {
			 m_lastError.Format("error in copying the file %s into %s",(const char *)source,(const char *)dest);
			 return false;
		}
	}
	return true;
}

bool CScheduleReport::writeReport()
{
	IRA::CString dst;
	IRA::CString schedFolder,logFile;
	CScheduleReport::TInternalListIter it;
	schedFolder=m_backupPath+m_scheduleBaseName+"_"+m_currentPostfix;
	logFile=m_logFilePath+getLogFileName();
	if (!m_reportEnabled) return true; // if the reporting is not enables, just exit.
	if (!m_scheduleProvided) return true;
	dst=getReportFileName();
    std::ofstream out ((const char *)dst);
    if (out.fail()) {
    	m_lastError.Format("cannot open report file %s",(const char *)dst);
    	out.close();
    	return false;
    }
    if (m_scheduleProvided) {  // check if the schedule section has to be added
    	//out << "[Schedule]" << endl;
    	out <<  (const char *) schedFolder << endl;
    }
    //out << "[Log]" << endl;
    out <<  (const char *) logFile << endl;
    //out << "[Scans]" << endl;
    for (it=m_scanPaths.begin();it!=m_scanPaths.end();++it) {
    	out <<  (const char *) (*it) << endl;
   	}
    out.flush();
    out.close();
    return true;
}

void CScheduleReport::removeTrailingSlash(IRA::CString& path)
{
	if (path[path.GetLength()-1]=='/') {
		path=path.Left(path.GetLength()-1);
	}		 
}
