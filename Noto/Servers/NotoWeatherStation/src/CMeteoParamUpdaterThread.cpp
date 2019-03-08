#include "NotoWeatherStationImpl.h"

CMeteoParamUpdaterThread::CMeteoParamUpdaterThread (const ACE_CString& name,
                        MeteoSocket  *  socketConnectionWeatherStation,
                        const ACS::TimeInterval& responseTime,
                        const ACS::TimeInterval& sleepTime) :
                        ACS::Thread(name)

                {
                        ACS_TRACE("CMeteoParamUpdaterThread::CMeteoParamUpdaterThread");
                        loopCounter_m = 0;
                        m_socket = (MeteoSocket  *) socketConnectionWeatherStation;
                }



void
CMeteoParamUpdaterThread::runLoop()
{
        /*Weather::parameters wdata;
        double wind;*/
        TIMEVALUE now;
        IRA::CIRATools::getTime(now);
        try{
            m_socket->updateParam();
        }catch(ComponentErrors::SocketErrorExImpl &ex){
            ACS_LOG(LM_FULL_INFO, "CMeteoParamUpdaterThread::runLoop()", 
                       (LM_WARNING, "Could not update meteo parameters"));
        }
}

void
CMeteoParamUpdaterThread::onStop()
{
 

        AUTO_TRACE("CMeteoParamUpdaterThread::onStop()");

}
void
CMeteoParamUpdaterThread::onStart()
{


        AUTO_TRACE("WindCheckerThread::onStart()");

}
