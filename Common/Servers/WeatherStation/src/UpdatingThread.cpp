#include "WeatherStationImpl.h"
#include <deque>
#include <numeric>

deque<float> deq;
float wind_avg;
float deq_sum;

CUpdatingThread::CUpdatingThread (const ACE_CString& name, WeatherStationImpl* weatherStation, const ACS::TimeInterval& responseTime, const ACS::TimeInterval& sleepTime) :
	ACS::Thread(name, responseTime, sleepTime),
    m_sleepTime(sleepTime)
{
    ACS_TRACE("CUpdatingThread::UpdatingThread");
    loopCounter_m = 0;
	m_weatherstation_p = (WeatherStationImpl*) weatherStation;
    m_threshold = m_weatherstation_p->m_threshold;
}

void CUpdatingThread::runLoop()
{
    AUTO_TRACE("UpdatingThread::runLoop()");

    m_weatherstation_p->updateData();
    Weather::parameters w_pars = m_weatherstation_p->getData();

    deq.push_back(w_pars.windspeed);
    ACS_LOG(LM_FULL_INFO,"UpdatingThread::runLoop()",(LM_DEBUG,"Deque size=%zu ", deq.size()));
    deq_sum  = accumulate(deq.begin(), deq.end(), 0.0);
    ACS_LOG(LM_FULL_INFO,"UpdatingThread::runLoop()",(LM_DEBUG,"Deque sum=%f ", deq_sum));
    
    if(deq.size() == 10){
    		wind_avg = deq_sum / deq.size();
    		ACS_LOG(LM_FULL_INFO,"UpdatingThread::runLoop()",(LM_DEBUG,"wind avg=%f ", wind_avg));
    		if(wind_avg > m_threshold){
       	 		m_weatherstation_p->parkAntenna();
       	 		ACS_LOG(LM_FULL_INFO,"UpdatingThread::runLoop()",(LM_WARNING,"WINDSPEED_AVG=%f ", wind_avg));
    			}
         
    		deq.pop_front();
   	 }
     
 /*   if(w_pars.windspeed > m_threshold) 
    {
        m_weatherstation_p->parkAntenna();
        ACS_LOG(LM_FULL_INFO,"UpdatingThread::runLoop()",(LM_WARNING,"WINDSPEED=%f ", w_pars.windspeed));
        std::cout<<"parking the Antenna"<<std::endl;
    }    */
    m_nextTime += m_sleepTime;
    setSleepTime(ACS::TimeInterval(std::max(long(0), long(m_nextTime - getTimeStamp()))));
}
    
void CUpdatingThread::onStart()
{
	AUTO_TRACE("UpdatingThread::onStart()");
    m_nextTime = getTimeStamp();
}

void CUpdatingThread::onStop()
{
	AUTO_TRACE("UpdatingThread::onStop()");
}
