//$Id: LogFilter.cpp,v 1.9 2011-03-14 16:19:32 a.orlati Exp $

#include "LogFilter.h"
#include "IRATools.h"
#include <ace/UUID.h>
#include <assert.h>

#define LOGFILTER_THREAD_NAME "LogFilterWorkerThread"

using namespace IRA;

void CLogGuard::log(Logging::Logger::LoggerSmartPtr& logger,const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fn,
	const DDWORD& ln,const CString& rtn,const unsigned flg)
{
	TIMEVALUE now;
	DDWORD elapsed;
	if (m_firstTime) {
		m_firstTime=false;
		CIRATools::getTime(m_time);
		LoggingProxy::Flags(flg);
		logger->log(prior,(const char *)msg,(const char *)fn,ln,(const char *)rtn);
		return;
	}
	CIRATools::getTime(now);
	elapsed=CIRATools::timeDifference(m_time,now);
	if (elapsed>=m_interval) {
		CLogWrapper tmp(logger,prior,msg,fn,ln,rtn,flg);	
		logLog(tmp,m_time,now,m_counter);
		m_counter=0;
		CIRATools::timeCopy(m_time,now);
	}
	else {
		m_counter++;
	}
}

void CLogGuard::log(ACSErr::ErrorTraceHelper& et,const CString& fn,const DDWORD& ln,const ACE_Log_Priority& priority)
{
	TIMEVALUE now;
	DDWORD elapsed;
	if (m_firstTime) {
		m_firstTime=false;
		CIRATools::getTime(m_time);
		et.log(priority);
		return;
	}
	CIRATools::getTime(now);
	elapsed=CIRATools::timeDifference(m_time,now);
	if (elapsed>=m_interval) {
		CErrorWrapper tmp(et.getErrorTrace(),priority);
		logError(tmp,m_time,now,m_counter);
		m_counter=0;
		CIRATools::timeCopy(m_time,now);
	}
	else {
		m_counter++;
	}	
}

bool CLogGuard::check()
{
	DDWORD elapsed;
	TIMEVALUE now;
	if (m_firstTime) {
		m_firstTime=false;
		return true;
	}
	CIRATools::getTime(now);
	elapsed=CIRATools::timeDifference(m_time,now);
	return (elapsed>=m_interval);
}


void CLogGuard::logLog(const CLogWrapper& log,TIMEVALUE& startTime,TIMEVALUE& stopTime,const DWORD& counter) const
{
	CString ff,ll;
	const char *ffc,*llc;
	LoggingProxy::Flags(log.flags);
	ff.Format("%02d:%02d:%02d.%03d",startTime.hour(),startTime.minute(),startTime.second(),(long)(startTime.microSecond()/1000));
	ll.Format("%02d:%02d:%02d.%03d",stopTime.hour(),stopTime.minute(),stopTime.second(),(long)(stopTime.microSecond()/1000));
	ffc=(const char *)ff;
	llc=(const char *)ll;
	LoggingProxy::AddData("RepeatCounter","%d times",counter);
	LoggingProxy::AddData("Period","%s to %s",ffc,llc);
	log.logger->log(log.priority,(const char *)log.message,(const char *)log.fileName,log.lineNumber,(const char *)log.routine);	
}

void CLogGuard::changeInterval(const DDWORD& interval)
{
	if (m_firstTime) {
		m_interval=interval;
	}
}

void CLogGuard::logError(CErrorWrapper& err,TIMEVALUE& startTime,TIMEVALUE& stopTime,const DWORD& counter) const
{
	CString ff,ll;
	const char *ffc,*llc;	
	TIMEVALUE tm;	
	ff.Format("%02d:%02d:%02d.%03d",startTime.hour(),startTime.minute(),startTime.second(),(long)(startTime.microSecond()/1000));
	ll.Format("%02d:%02d:%02d.%03d",stopTime.hour(),stopTime.minute(),stopTime.second(),(long)(stopTime.microSecond()/1000));
	ffc=(const char *)ff;
	llc=(const char *)ll;
	CIRATools::getTime(tm);
	err.top();
	err.setTimeStamp(tm.value().value);
	LoggingProxy::AddData("RepeatCounter","%d times",counter);
	LoggingProxy::AddData("Period","%s to %s",ffc,llc);
	LoggingProxy::AddData("ErrorTraceSize","%d",err.getDepth());		
	err.log(err.getLogPriority());	
}

void CLogGuard::CErrorWrapper::log(ACE_Log_Priority priorty)
{
	ACSErr::ErrorTrace* c=m_errorTracePtr;
	char uuidBuf[40];
	ACE_Utils::UUID* uuid=ACE_Utils::UUID_GENERATOR::instance()->generate_UUID();
	//use this with ACS version prior than ACS 8.0
	//ACE_Utils::UUID* uuid=ACE_Utils::UUID_GENERATOR::instance()->generateUUID();
	snprintf(uuidBuf,40,"%s",uuid->to_string()->c_str());
	delete uuid;
	if (!m_depth) return;
	ACSErr::ErrorTraceHelper::log(c,m_depth,uuidBuf,priorty);
}

/******************** LogFilter ***********************************************/
CLogFilter::CDataRecord::CDataRecord(const DWORD& line,const CString& file,void * const data,const TLogType& type): 
  lineNumber(line), fileName(file), logPointer(data), logType(type)
{
	updateTimer();
	reset();
}

CLogFilter::CDataRecord::~CDataRecord()
{
	if (logType==LOG) {
		CLogWrapper *log=static_cast<CLogWrapper *>(logPointer);
		delete log;
	}
	else if (logType==ERROR) {
		CErrorWrapper *log=static_cast<CErrorWrapper *>(logPointer);
		delete log;
	}
}

void CLogFilter::CDataRecord::updateTimer()
{
	CIRATools::getTime(lastTime);	
}

void CLogFilter::CDataRecord::reset()
{
	counter=0;
	CIRATools::timeCopy(firstTime,lastTime);
}

//*******************************************************************************************************************************

CLogFilter::CLogFilter() : CLogGuard(0), m_cache(0), m_expire(0), m_active(false)
{
	m_logs.clear();
	m_pThread=NULL;
	m_destroyed=false;
	m_pThread=m_threadManager.create(LOGFILTER_THREAD_NAME,(void *)worker,static_cast<void *>(this));
	assert(m_pThread);	
}

CLogFilter::CLogFilter(DDWORD cacheTime,DDWORD expirationTime) : CLogGuard(0),  m_cache(0), m_expire(0), m_active(false)
{
	m_logs.clear();
	m_pThread=NULL;
	m_pThread=m_threadManager.create(LOGFILTER_THREAD_NAME,(void *)worker,static_cast<void *>(this));
	assert(m_pThread);
	m_destroyed=false;
	activate(cacheTime,expirationTime);
}
	
CLogFilter::~CLogFilter()
{
	destroy();
}

void CLogFilter::destroy()
{
	if (!m_destroyed) {
		if (m_pThread) {
			m_pThread->stop();
			while (m_pThread->isAlive()) {
				CIRATools::Wait(0,50000);
			}
			m_threadManager.terminate(LOGFILTER_THREAD_NAME);	
		}
		baci::ThreadSyncGuard guard(&m_localMutex);
		for(unsigned i=0;i<m_logs.size();i++) {
			delete m_logs[i];
		}
		m_logs.clear();
		m_destroyed=true;
	}
}

void CLogFilter::activate(DDWORD cacheTime,DDWORD expirationTime)
{
	m_cache=cacheTime;
	m_expire=expirationTime;
	assert(m_expire>=m_cache);
	m_active=(m_expire!=0);
	if (m_active) m_pThread->resume();	
}

void CLogFilter::flush()
{
	baci::ThreadSyncGuard guard(&m_localMutex);
	CDataRecord *rec;	
	for(TIterator i=m_logs.begin();i<m_logs.end();i++) {
		rec=(*i);
		if (rec->getCounter()>0) {
			logRecord(i);
		}
	}
}

void CLogFilter::log(Logging::Logger::LoggerSmartPtr& logger,const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fn,
	   const DDWORD& ln,const CString& rtn,const unsigned flg)
{
	baci::ThreadSyncGuard guard(&m_localMutex);
	CDataRecord* rec;
	if ((rec=findRecord(fn,ln))!=NULL) {
		rec->updateTimer();
		rec->incCounter();
	}
	else {
		// the first time the log is submitted
		LoggingProxy::Flags(flg);
		logger->log(prior,(const char *)msg,(const char *)fn,ln,(const char *)rtn);
		if (m_active) {
			CLogWrapper* log=new CLogWrapper(logger,prior,msg,fn,ln,rtn,flg);
			rec=new CDataRecord(ln,fn,static_cast<void *>(log),CDataRecord::LOG);
			addRecord(rec);
		}
	}
}

void CLogFilter::log(ACSErr::ErrorTraceHelper& et,const CString& fn,const DDWORD& ln,const ACE_Log_Priority& priority)
{
	baci::ThreadSyncGuard guard(&m_localMutex);
	CDataRecord* rec;
	if ((rec=findRecord(fn,ln))!=NULL) {
		rec->updateTimer();
		rec->incCounter();
	}
	else {
		// the first time the exception is submitted
		et.log(priority);
		if (m_active) {
			ACSErr::ErrorTrace *tmp=new ACSErr::ErrorTrace(et.getErrorTrace());
			CErrorWrapper* log=new CErrorWrapper(tmp,priority);   //tmp will be deleted by the wrapper
			rec=new CDataRecord(ln,fn,static_cast<void *>(log),CDataRecord::ERROR);
			addRecord(rec);
		}
	}
}

CLogFilter::CDataRecord *CLogFilter::findRecord(const CString& fileName,const DDWORD& lineNumber) const
{
	CDataRecord *tmp;
	for(unsigned i=0;i<m_logs.size();i++) {
		tmp=m_logs[i];
		if ((tmp->fileName==fileName) && (tmp->lineNumber==lineNumber)) {
			return tmp;
		}
	}
	return NULL;
}

void CLogFilter::addRecord(CDataRecord* const record)
{
	m_logs.push_back(record);
} 

void CLogFilter::logRecord(const TIterator& pos)
{
	CDataRecord *rec=(*pos);
	if (rec->logType==CDataRecord::LOG) {
		CLogWrapper *log=static_cast<CLogWrapper *>(rec->logPointer);
		logLog(*log,rec->firstTime,rec->lastTime,rec->counter);
	}
	else if (rec->logType==CDataRecord::ERROR) {
		CErrorWrapper *ex=static_cast<CErrorWrapper *>(rec->logPointer);
		logError(*ex,rec->firstTime,rec->lastTime,rec->counter);		
	}
	rec->reset();
}

void CLogFilter::processLogs()
{
	baci::ThreadSyncGuard guard(&m_localMutex);
	TIMEVALUE now;
	std::vector<CDataRecord *>::iterator i;
	CDataRecord *log;
	DDWORD elapsed;
	CIRATools::getTime(now);
	for(i=m_logs.begin();i<m_logs.end();i++) {
		log=*i;
		if (log->getCounter()==0) {
			elapsed=CIRATools::timeDifference(log->lastTime,now);
			if (elapsed>m_expire) {
				delete (*i);
				i=m_logs.erase(i);	
			}
		}
		else {  // counter>0
			elapsed=CIRATools::timeDifference(log->firstTime,now);
			if (elapsed>m_cache) {
				logRecord(i);	
			}			
		}
	}
}

void CLogFilter::worker(void *threadParam)
{
	CLogFilter *logFilter;
	ACS::ThreadBaseParameter *param=static_cast<ACS::ThreadBaseParameter *>(threadParam);
	ACS::ThreadBase *myself=param->getThreadBase();
	// Init the thread
	if (ACS::ThreadBase::InitThread!=0)  {
		ACS::ThreadBase::InitThread(LOGFILTER_THREAD_NAME);
	}
	logFilter=(CLogFilter *)param->getParameter();
	while(myself->check()==true) {
		if(!(myself->isSuspended())) {
			logFilter->processLogs();	
		}
		myself->sleep();
	}
	myself->setStopped();
	if (ACS::ThreadBase::DoneThread!=0) {
		ACS::ThreadBase::DoneThread();
	}
	//delete param;
}
