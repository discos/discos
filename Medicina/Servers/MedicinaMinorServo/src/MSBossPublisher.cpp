/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include <time.h>
#include "MSBossPublisher.h"

#define MAX_DTIME 10000000 // 1 second

PublisherThreadParameters::PublisherThreadParameters(MedMinorServoStatus *status,
                                                    nc::SimpleSupplier *nc,
                                                    MedMinorServoControl_sp control) :
                                                    m_status(status),
                                                    m_nc(nc),
                                                    m_control(control)
{}

PublisherThreadParameters::~PublisherThreadParameters()
{}

MSBossPublisher::MSBossPublisher(
        const ACE_CString& name, 
        PublisherThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), 
            m_status(params.m_status), 
            m_nc(params.m_nc),
            m_control(params.m_control),
            m_old_tracking(false),
            m_last_event(0.0L)
{
    AUTO_TRACE("MSBossPublisher::MSBossPublisher()");
}

MSBossPublisher::~MSBossPublisher() { AUTO_TRACE("MSBossPublisher::~MSBossPublisher()"); }

void MSBossPublisher::onStart() { 
    AUTO_TRACE("MSBossPublisher::onStart()"); 
}

void MSBossPublisher::onStop() { AUTO_TRACE("MSBossPublisher::onStop()"); }

void MSBossPublisher::runLoop()
{
    //AUTO_TRACE("MSBossPublisher::runLoop()");
    static MinorServo::MinorServoDataBlock data = {0, false, false, false, false, Management::MNG_OK};
    TIMEVALUE now(0.0L);
    IRA::CIRATools::getTime(now);
    bool tracking = m_control->is_tracking();
    try{
    if((m_old_status.parked != m_status->parked)||
       (m_old_status.parking != m_status->parking)||
       (m_old_status.starting != m_status->starting)||
       (m_old_status.status != m_status->status)||
       (m_old_tracking != tracking)){
        data.tracking = tracking;
        data.parked = m_status->parked;
        data.parking = m_status->parking;
        data.starting = m_status->starting;
        data.status = m_status->status;
        data.timeMark = (now.value()).value;
        try {
            if(m_nc != NULL)
            {
                m_nc->publishData<MinorServo::MinorServoDataBlock>(data);
                CUSTOM_LOG(LM_FULL_INFO, 
                           "MinorServo::MSBossPublisher::runLoop",
                           (LM_DEBUG, "Publish minor servo status"));
            }else
            {
                ACS_SHORT_LOG((LM_ERROR, "Error: cannot get the notification channel reference."));
            }
        }
        catch (ComponentErrors::CORBAProblemEx& ex) {
            _ADD_BACKTRACE(ComponentErrors::NotificationChannelErrorExImpl, impl, ex, "MSBossPublisher()");
            throw impl;
        }
        IRA::CIRATools::timeCopy(m_last_event, now);
        m_old_status = *m_status;
        m_old_tracking = tracking;
    }//endif
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "An error is occurred in MSBossPublisher"));
    }
}

