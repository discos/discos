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
    m_antennaBoss = Antenna::AntennaBoss::_nil();
    try {
        m_antennaBoss = m_services->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
        if(CORBA::is_nil(m_antennaBoss))
        {
            ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::init():_nil reference of AntennaBoss component"));
            m_status->elevation_tracking = false;
        }
    }catch(maciErrType::CannotGetComponentExImpl& ex)
    {
        m_status->elevation_tracking = false;
        ACS_SHORT_LOG((LM_WARNING, "MSBossConfiguration::init(): cannot get the AntennaBoss component"));
    }
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
            if(!CORBA::is_nil(m_antennaBoss))
            {
                m_antennaBoss->getRawCoordinates(IRA::CIRATools::getACSTime(), azimuth, elevation);
                elevation = elevation * DR2D;
                offset_position = m_offset->getOffsetPosition();
                correct_position = (*m_parameters)->get_position(elevation);
                m_control->set_position(correct_position + offset_position);
                CUSTOM_LOG(LM_FULL_INFO, "MinorServo::MSBossTracker::runLoop",
                          (LM_DEBUG, "Correcting position"));
            }
            //TODO: warning? error?
        }catch(...){
        }
    }
}

