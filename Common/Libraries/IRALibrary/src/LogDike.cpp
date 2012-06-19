#include "LogDike.h"

using namespace IRA;

CLogDike::CLogDike(Logging::Logger::LoggerSmartPtr logger,const long& cacheTime,const long& expireTime) : m_logger(logger)
{
	m_events.clear();
	m_expireTime=expireTime*10; // transform micro seconds to 100 nano
	m_cacheTime=cacheTime*10; // transform micro seconds to 100 nano
	//m_logger=logger;
	m_loggerReady=true;
}

CLogDike::~CLogDike()
{
	destroy();
}

CLogDike::CEvent *CLogDike::findEvent(const CString& fileName,const DDWORD& lineNumber)
{
	TIterator i;
	CEvent *event;
	i=m_events.begin();
	while (i<m_events.end()) {
		event=*i;
		if (event->match(fileName,lineNumber)) {
			return event;
		}
		i++;
	}
	return NULL;
}

void CLogDike::addEvent(CEvent* const event)
{
	m_events.push_back(event);
}

void CLogDike::flush()
{
	CEvent *event;
	for(TIterator i=m_events.begin();i<m_events.end();i++) {
		event=(*i);
		if (event->getCounter()>0) {
			event->logMe(m_logger,true);
		}
	}
}

void CLogDike::destroy()
{
	flush();
	for(unsigned i=0;i<m_events.size();i++) {
		delete m_events[i];
	}
	m_events.clear();
}

void CLogDike::openDike()
{
	CEvent *event;
	bool logit,removeit;
	for(TIterator i=m_events.begin();i<m_events.end();i++) {
		event=(*i);
		event->check(m_expireTime,m_cacheTime,logit,removeit); // this will also update the event tracking data in case of logit=true
		if (logit) {
			event->logMe(m_logger,true);
			event->reset();
		}
		if (removeit) {
			delete event;
			i=m_events.erase(i);
		}
	}
}

void CLogDike::log(Logging::Logger::LoggerSmartPtr& logger,const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fn, const DDWORD& ln,const CString& rtn,const unsigned flg)
{
	CEvent* event;
	if ((event=findEvent(fn,ln))!=NULL) {
		event->updateEvent();
	}
	else {
		// the first time the log is submitted
		CLogWrapper* log=new CLogWrapper(prior,msg,fn,ln,rtn,flg);
		log->logMe(logger,false);
		addEvent(log);
	}
}

void CLogDike::log(const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fn,const DDWORD& ln,const CString& rtn,const unsigned flg)
{
	CEvent* event;
	if ((event=findEvent(fn,ln))!=NULL) {
		event->updateEvent();
	}
	else {
		// the first time the log is submitted
		CLogWrapper* log=new CLogWrapper(prior,msg,fn,ln,rtn,flg);
		log->logMe(m_logger,false);
		addEvent(log);
	}
}


void CLogDike::log(ACSErr::ErrorTraceHelper& et,const CString& fn,const DDWORD& ln,const ACE_Log_Priority& priority)
{
	CEvent* event;
	if ((event=findEvent(fn,ln))!=NULL) {
		event->updateEvent();
	}
	else {
		ACSErr::ErrorTrace* ptr=new ACSErr::ErrorTrace(et.getErrorTrace()); // the memory will freed by ErrorWrapper
		CErrorWrapper* err=new CErrorWrapper(ptr,priority,fn,ln);
		err->logMe(m_logger,false);
		addEvent(err);
	}
}

/******************** CEvent ****************************************/

CLogDike::CEvent::CEvent(const TEventType& type,const IRA::CString& file,const DDWORD& line) : m_type(type), m_lineNumber(line), m_file(file)
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	m_lastEpoch=m_firstEpoch=now.value().value;
	m_counter=0;
}

CLogDike::CEvent::~CEvent()
{
}

bool CLogDike::CEvent::match(const IRA::CString& file,const DDWORD& line) const
{
	return (  (m_file==file) && (m_lineNumber==line) );
}

void CLogDike::CEvent::check(const ACS::TimeInterval& expireTime,const ACS::TimeInterval& cacheTime,bool& logit, bool& removeit) const
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	ACS::TimeInterval delta;
	logit=removeit=false;
	if (m_counter==0) {
		delta=now.value().value-m_lastEpoch;
		if (delta>expireTime) {
			removeit=true;
		}
	}
	else { // m_counter>0
		delta=now.value().value-m_firstEpoch;
		if (delta>cacheTime) {
			logit=true;
		}
	}
}

void CLogDike::CEvent::updateEvent()
{
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	m_counter++;
	m_lastEpoch=now.value().value;
}

void CLogDike::CEvent::reset()
{
	m_counter=0;
	m_firstEpoch=m_lastEpoch;
}

/******************** CErrorWrapper ****************************************/

CLogDike::CErrorWrapper::CErrorWrapper(ACSErr::ErrorTrace* et,const ACE_Log_Priority& pr,const CString& fName, const DDWORD& lNumber): ACSErr::ErrorTraceHelper(*et),
		CEvent(CEvent::ERROREVENT,fName,lNumber), m_logPriority(pr),m_trace(et)
{
}

CLogDike::CErrorWrapper::~CErrorWrapper() {
	if (m_trace) delete m_trace;
}

void CLogDike::CErrorWrapper::logMe(Logging::Logger::LoggerSmartPtr& logger,bool addExtraInfo)
{
	IRA::CString epoch1,epoch2;
	addData(CUSTOM_LOGGING_DATA_NAME,CUSTOM_LOGGING_DATA_VALUE); // this will respect our internal logging conventions
	if (addExtraInfo) {
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		top();
		while (getNext()!=NULL) {
			setTimeStamp(now.value().value);
		}
		top();
		setTimeStamp(now.value().value);
		LoggingProxy::AddData("RepeatCounter","%d times",getCounter());
		IRA::CIRATools::timeToStr(getFirstEpoch(),epoch1);
		IRA::CIRATools::timeToStr(getLastEpoch(),epoch2);
		LoggingProxy::AddData("Period","%s to %s",(const char *)epoch1,(const char *)epoch2);
	}
	log(m_logPriority);
}

/******************** CLogWrapper ****************************************/

CLogDike::CLogWrapper::CLogWrapper(const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fName, const DDWORD& lNumber,const CString& rtn,
		const unsigned& flg): CEvent(CEvent::LOGEVENT,fName,lNumber), m_priority(prior), m_message(msg) ,m_fileName(fName),m_lineNumber(lNumber), m_routine(rtn), m_flags(flg)
{
}


CLogDike::CLogWrapper::~CLogWrapper()
{
}

void CLogDike::CLogWrapper::logMe(Logging::Logger::LoggerSmartPtr& logger,bool addExtraInfo)
{
	IRA::CString epoch1,epoch2;
	LoggingProxy::Flags(m_flags);
	if (addExtraInfo) {
		LoggingProxy::AddData("RepeatCounter","%d times",getCounter());
		IRA::CIRATools::timeToStr(getFirstEpoch(),epoch1);
		IRA::CIRATools::timeToStr(getLastEpoch(),epoch2);
		LoggingProxy::AddData("Period","%s to %s",(const char *)epoch1,(const char *)epoch2);
	}
	LoggingProxy::AddData(CUSTOM_LOGGING_DATA_NAME, CUSTOM_LOGGING_DATA_VALUE);  // this will respect our internal logging conventions
	logger->log(m_priority,(const char *)m_message,(const char *)m_fileName,m_lineNumber,(const char *)m_routine);
}

