#include "NotoWeatherStationImpl.h"

CMeteoParamUpdaterThread::CMeteoParamUpdaterThread (const ACE_CString& name,
    NotoWeatherStationImpl* weatherStation,
    const ACS::TimeInterval& responseTime,
    const ACS::TimeInterval& sleepTime) :
    ACS::Thread(name)
{
    ACS_TRACE("CMeteoParamUpdaterThread::CMeteoParamUpdaterThread");
    loopCounter_m = 0;
    m_weatherstation_p = (NotoWeatherStationImpl*) weatherStation;
}

void CMeteoParamUpdaterThread::runLoop()
{
    AUTO_TRACE("CMeteoParamUpdaterThread::runLoop()");

    try
    {
        m_weatherstation_p->updateData();
    }
    catch(...)
    {
        ACS_LOG(LM_FULL_INFO, "CMeteoParamUpdaterThread::runLoop()", (LM_WARNING, "Could not update weather parameters"));
    }
}

void CMeteoParamUpdaterThread::onStart()
{
    AUTO_TRACE("CMeteoParamUpdaterThread::onStart()");
}

void CMeteoParamUpdaterThread::onStop()
{
    AUTO_TRACE("CMeteoParamUpdaterThread::onStop()");
}
