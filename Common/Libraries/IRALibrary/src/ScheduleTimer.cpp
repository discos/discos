// $Id: ScheduleTimer.cpp,v 1.4 2010-04-30 10:08:45 a.orlati Exp $

#include "ScheduleTimer.h"
#include "String.h"
#include "Definitions.h"
#include "IRATools.h"

using namespace IRA;

CScheduleTimer::EventHandler::EventHandler(TCallBack callBack,bool oneShot,CScheduleTimer *timer):
	m_callBack(callBack),m_oneShot(oneShot),m_timer(timer)
{
}

CScheduleTimer::EventHandler::~EventHandler()
{
}

int CScheduleTimer::EventHandler::handle_timeout(const ACE_Time_Value& time,const void* param)
{
	acstime::Epoch e=TimeUtil::ace2epoch(time);
	// invoke the callback
	m_callBack(e.value,param);
	if (m_oneShot) {
		m_timer->clearEvent(m_id);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////

CScheduleTimer::CScheduleTimer(): m_timerQueue(NULL), m_active(false)
{
	m_events.clear();
}

CScheduleTimer::~CScheduleTimer()
{
	TEventsIterator i;
	if (m_active) {
		m_timerQueue->deactivate();
		m_timerQueue->wait();
		delete m_timerQueue;
	}
	for(i=m_events.begin();i<m_events.end();i++) {
		delete (*i)->handler;
		delete (*i);
	}
	m_events.clear();
}

bool CScheduleTimer::init()
{
	try {
		m_timerQueue=new TThread_Timer_Queue();
	}
	catch (...) {
		return false;
	}
	if(m_timerQueue->activate()==-1) {
		return false;
	}
	m_active=true;
	return true;
}

bool CScheduleTimer::cancel(const ACS::Time& time)
{
	// locate the event
	TEventsIterator i;
	for(i=m_events.begin();i<m_events.end();i++) {
		if ((*i)->time==time) { 
			if (m_timerQueue->cancel((*i)->id)!=0) { //remove the event from the queue
				delete (*i)->handler;
				delete (*i);
				m_events.erase(i);
				return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
}

bool CScheduleTimer::cancel(const unsigned& pos)
{
	long id;
	if (pos>=m_events.size()) {
		return false;
	}
	id=m_events[pos]->id;
	if (m_timerQueue->cancel(id)!=0) { //remove the event from the queue
		delete m_events[pos]->handler;
		delete m_events[pos];
		m_events.erase(m_events.begin()+pos);
		return true;
	}
	else {
		return false;
	}
}

bool CScheduleTimer::cancelAll()
{
	bool ok=true;
	TEventsIterator i;
	for(i=m_events.begin();i<m_events.end();i++) { 
		if (m_timerQueue->cancel((*i)->id)!=0) { //remove the event from the queue
		}
		else {
			ok=false;
		}
		delete (*i)->handler;
		delete (*i);
	}
	m_events.clear();
	return ok;	
}

bool CScheduleTimer::getNextEvent(unsigned &index,ACS::Time& time,ACS::TimeInterval &interval,const void *&parameter)
{
	if  (index>=m_events.size()) {
		return false;
	}
	parameter=m_events[index]->parameter;
	interval=m_events[index]->interval;
	time=m_events[index]->time;
	index++;
	return true;
}

bool CScheduleTimer::schedule(TCallBack callBack,const ACS::Time& timeMark,const ACS::TimeInterval& interval,const void* param,TCleanupFunction cleanup)
{
	long id;
	ACS::Time time=timeMark;
	acstime::Epoch epoch;
	acstime::Duration duration;
	ACE_Time_Value start,period;
	bool oneShot;
	EventHandler *handler;
	TRecord *record;
	if ((time==0) && (interval==0)) { 
		return false; // the time mark must be given
	}
	else if (time==0) {
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		time=now.value().value+interval;
	}
	oneShot=(interval==0);
	try {
		handler=new EventHandler(callBack,oneShot,this);
		record=new TRecord;
	}
	catch (...) {
		return false;
	}
	epoch.value=time;
	duration.value=interval;
	start=TimeUtil::epoch2ace(epoch);
	period=TimeUtil::duration2ace(duration);
	id=m_timerQueue->schedule(handler,param,start,period);
	if (id==-1) {
		delete handler;
		delete record;
		if (cleanup) cleanup(param);
		return false;
	}
	handler->setIdentifier(id);
	// save the event information
	record->time=time;
	record->interval=interval;
	record->handler=handler;
	record->id=id;
	record->cleanupFunction=cleanup;
	record->parameter=param;
	m_events.push_back(record);
	return true;
}

bool CScheduleTimer::clearEvent(const long& id)
{
	TEventsIterator i;
	for(i=m_events.begin();i<m_events.end();i++) {
		if ((*i)->id==id) {
			if ((*i)->cleanupFunction) (*i)->cleanupFunction((*i)->parameter);
			delete (*i)->handler;
			delete (*i);
			m_events.erase(i);
			return true;
		}
	}
	return false;
}
