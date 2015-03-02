/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Bartolini
 *  E-mail:       bartolini@ira.inaf.it
\*******************************************************************************/

#include "MSBossTracker.hpp"

TrackerThreadParameters::TrackerThreadParameters(MedMinorServoStatus *status, 
                                                MedMinorServoControl_sp control,
                                                MedMinorServoParameters **params,
                                                MedMinorServoOffset *offset,
                                                maci::ContainerServices *services):
                                                m_status(status),
                                                m_control(control),
                                                m_parameters(params),
                                                m_offset(offset),
                                                m_services(services)
{}

TrackerThreadParameters::~TrackerThreadParameters()
{}

MSBossTracker::MSBossTracker(
        const ACE_CString& name, 
        TrackerThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), 
            m_status(params.m_status), 
            m_control(params.m_control),
            m_parameters(params.m_parameters),
            m_offset(params.m_offset)
{
    AUTO_TRACE("MSBossTracker::MSBossTracker()");
    m_antennaBoss.setContainerServices(m_services);
    m_antennaBoss.setComponentName("ANTENNA/BOSS");
}

MSBossTracker::~MSBossTracker() { AUTO_TRACE("MSBossTracker::~MSBossTracker()"); }

void MSBossTracker::onStart() { 
    AUTO_TRACE("MSBossTracker::onStart()"); 
}

void MSBossTracker::onStop() { AUTO_TRACE("MSBossTracker::onStop()"); }

void MSBossTracker::runLoop()
{
    double elevation = 45.0;
    double azimuth;
    MedMinorServoPosition offset_position, correct_position;
    if(m_status->elevation_tracking)
    {
        try {
            m_antennaBoss->getRawCoordinates(IRA::CIRATools::getACSTime(), azimuth, elevation);
        }catch(...){
            CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MSBossTracker::runLoop",
                      (LM_WARNING, "Cannot get antenna coordinates"));
            m_antennaBoss.setError();
        }
        if(!(m_antennaBoss.isError()))
        {
            try{
                elevation = elevation * DR2D;
                offset_position = m_offset->getOffsetPosition();
                correct_position = (*m_parameters)->get_position(elevation);
                m_control->set_position(correct_position + offset_position);
                CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MSBossTracker::runLoop",
                          (LM_DEBUG, "Correcting position"));
            }catch(...){
                CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MSBossTracker::runLoop",
                          (LM_WARNING, "Cannot command offset position"));
            }
        }
    }
}

