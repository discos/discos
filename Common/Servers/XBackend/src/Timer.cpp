 // $Id: Timer.cpp,v 1.4 2010/06/21 11:04:51 bliliana Exp $

#include "Timer.h"

using namespace IRA;

Timer::Timer(const ACE_CString& name,TimerParameter *par,
  const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime): ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("Timer::Timer()");
	v_interval=par->interval;
	TimerId=getThreadID();
	par->TimerId=TimerId;
	m_sender=par->sender;
    tp=par->tp;
    event=false;
    ok=false;
}

Timer::~Timer()
{ 
	AUTO_TRACE("Timer::~Timer()");
}

void Timer::Start()
{
	AUTO_TRACE("Timer::Start()");
	ok=true;  
//	resume(); 
	event=false;
	timeStart=getTime();
	ACS_DEBUG("Timer::Start()","Timer thread starts");

}
	
void Timer::Stop()
{
	AUTO_TRACE("Timer::Stop()");
	ok=false;  
//	suspend();
	event=false;
	ACS_DEBUG("Timer::Stop()","Timer thread stops");
}

void Timer::runLoop()
{
	ACS::Time time;
	if(ok==true){
		do{
			time=getTime();
		}while(time<(timeStart+v_interval));
		if(time>=(timeStart+v_interval)){
			timeStart=getTime();
			if(tp){ 
				ACS_DEBUG_PARAM("Timer::runLoop()"," GetDataTP %i ",TimerId);
			    event=true;
			}
			else{ 
				ACS_DEBUG_PARAM("Timer::runLoop()"," GetData %i ",TimerId);
				event=true;
			}
		}		
	}
}
