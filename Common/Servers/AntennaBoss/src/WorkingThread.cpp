#include "WorkingThread.h"
#include <LogFilter.h>

#define STIFFNESS 4

_IRA_LOGFILTER_IMPORT;

CWorkingThread::CWorkingThread(const ACE_CString& name,IRA::CSecureArea<CBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CWorkingThread::CWorkingThread()");
	IRA::CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	m_gap=resource->m_config->getGapTime();
	m_slice.microSecond(m_gap);
	m_maxPoints=resource->m_config->getMaxPointNumber();
	m_minPoints=resource->m_config->getMinPointNumber();
	m_sleepTime=resource->m_config->getWorkingThreadTime()/10;  // wants to know the number of microseconds
}

CWorkingThread::~CWorkingThread()
{
	AUTO_TRACE("CWorkingThread::~CWorkingThread()");
}

void CWorkingThread::onStart()
{
	AUTO_TRACE("CWorkingThread::onStart()");
}

void CWorkingThread::onStop()
{
	 AUTO_TRACE("CWorkingThread::onStop()");
}
 
void CWorkingThread::runLoop()
{
	 IRA::CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	 TIMEVALUE currentTime,now;
	 WORD elem,threshold,i;
	 WORD ramp=(m_maxPoints+m_minPoints)/(STIFFNESS+1);
	 WORD dataPoints=(m_sleepTime/m_gap); // points that the antenna is supposed to "consume" between two executions of the thread;
	 
	 IRA::CIRATools::timeCopy(currentTime,resource->m_rawCoordinates.getLastTime());
	 CIRATools::getTime(now);
	 resource->m_rawCoordinates.purge(now);
	 elem=resource->m_rawCoordinates.elements(); //gets the points that should have been loaded into the ACU.
	 currentTime.normalize(true);    // this should take care of the midnight problem
	 if (elem<m_minPoints) dataPoints+=(m_minPoints-elem);  // if the level is lower than the minimum, take it up again.
	 threshold=ramp+m_minPoints;  //first threshold
	 dataPoints+=(STIFFNESS/2);  //adds more points according to the stiffness
	 while (elem>threshold) {
		 dataPoints--;
		 threshold+=ramp;
	 }
	 // now loads the points if they do not go beyong the limits
	 if (elem+dataPoints<m_maxPoints) {
		 for (i=0;i<dataPoints;i++) {
			 currentTime+=m_slice.value();
			try {
				resource->loadTrackingPoint(currentTime,false);
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,impl,E,"CWorkingThread::runLoop()");
				impl.setReason("Cannot load the tracking point");
				_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
			}
		 }
	 }
}
 // END
