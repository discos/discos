/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "TrackingThread.h"
#include <pthread.h>
#include <time.h>
#include "MSParameters.h"
#include "MinorServoBossImpl.h"
#include "utils.h"
#include "slamac.h"

const double MIN_ELEVATION = 5; // In decimals

TrackingThread::TrackingThread(
        const ACE_CString& name, 
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_configuration(configuration)
{
    AUTO_TRACE("TrackingThread::TrackingThread()");
    
    try {
        m_antennaBoss = Antenna::AntennaBoss::_nil();
        m_antennaBoss = (m_configuration->m_services)->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");
        if(CORBA::is_nil(m_antennaBoss)) {
            ACS_SHORT_LOG((LM_WARNING, "TrackingThread: _nil reference of AntennaBoss component"));
            m_configuration->m_isElevationTracking = false;
        }
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        m_configuration->m_isElevationTracking = false;
        ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the AntennaBoss component"));
    }
}

TrackingThread::~TrackingThread() { AUTO_TRACE("TrackingThread::~TrackingThread()"); }

void TrackingThread::onStart() { 
    AUTO_TRACE("TrackingThread::onStart()"); 
    m_configuration->m_isTrackingEn = true;
}

void TrackingThread::onStop() { 
    AUTO_TRACE("TrackingThread::onStop()"); 
    m_configuration->m_isElevationTracking = false;
    m_configuration->m_isTrackingEn = false;
}

void TrackingThread::runLoop()
{
    AUTO_TRACE("TrackingThread::runLoop()");

    TIMEVALUE now;
    IRA::CIRATools::getTime(now);
    vector<string>::iterator start = (m_configuration->getDynamicComponents()).begin();
    vector<string>::iterator end = (m_configuration->getDynamicComponents()).end();
    m_configuration->m_isElevationTracking = true;

    try {
        for(vector<string>::iterator iter = start; iter != end; iter++) {
            string comp_name = *iter;
            // Get the elevation
            try {
                if(CORBA::is_nil(m_antennaBoss))
                    m_antennaBoss = (m_configuration->m_services)->getComponent<Antenna::AntennaBoss>("ANTENNA/Boss");

                TIMEVALUE now;
                double azimuth = 0.0;
                double elevation = 0.0;
                IRA::CIRATools::getTime(now);

                try {
                    if(!CORBA::is_nil(m_antennaBoss))
                        m_antennaBoss->getRawCoordinates(now.value().value, azimuth, elevation);
                }
                catch (CORBA::SystemException& ex) {
                    ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the antenna elevation."));
                    m_configuration->m_isElevationTracking = false;
                }

                elevation = elevation * DR2D; // From radians to decimal (be sure we want to use decimal...)
                elevation = (elevation > MIN_ELEVATION) ? elevation : 0.0; // Decimal?

                // Set the servo position
                MinorServo::WPServo_var component_ref;
                if((m_configuration->m_component_refs).count(comp_name)) {
                    component_ref = (m_configuration->m_component_refs)[comp_name];
                    if(!CORBA::is_nil(component_ref)) {
                        // Set a doubleSeq from a string of positions
                        ACS::doubleSeq positions = m_configuration->getPosition(comp_name, elevation);
                        // Set a minor servo position if the doubleSeq is not empty
                        if(positions.length())
                            component_ref->setPosition(positions, NOW);
                        else {
                            ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the antenna elevation."));
                            m_configuration->m_isElevationTracking = false;
                        }
                    }
                    else {
                        ACS_SHORT_LOG((LM_WARNING, ("TrackingThread: _nil reference for " + comp_name).c_str()));
                        m_configuration->m_isElevationTracking = false;
                    }
                }
            }
            catch (maciErrType::CannotGetComponentExImpl& ex) {
                m_configuration->m_isElevationTracking = false;
                ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the AntennaBoss component"));
            }
        }
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "An error is occurred in TrackingThread"));
        m_configuration->m_isElevationTracking = false;
    }
}

