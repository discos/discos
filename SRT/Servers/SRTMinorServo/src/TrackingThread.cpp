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
        m_antennaMount = Antenna::Mount::_nil();
        m_antennaMount = (m_configuration->m_services)->getComponent<Antenna::Mount>("ANTENNA/Mount");
        if(CORBA::is_nil(m_antennaMount)) {
            ACS_SHORT_LOG((LM_WARNING, "TrackingThread: _nil reference of Mount component"));
            m_configuration->m_isElevationTracking = false;
        }
    }
    catch (maciErrType::CannotGetComponentExImpl& ex) {
        m_configuration->m_isElevationTracking = false;
        ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the AntennaMount component"));
    }
}

TrackingThread::~TrackingThread() { AUTO_TRACE("TrackingThread::~TrackingThread()"); }

void TrackingThread::onStart() { 
    AUTO_TRACE("TrackingThread::onStart()"); 
    m_configuration->m_isElevationTrackingEn = true;
}

void TrackingThread::onStop() { 
    AUTO_TRACE("TrackingThread::onStop()"); 
    m_configuration->m_isElevationTracking = false;
    m_configuration->m_isElevationTrackingEn = false;
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
        vector<string> dynamics_comp = m_configuration->getDynamicComponents();
        for(size_t i=0; i<dynamics_comp.size(); i++) {
            string comp_name(dynamics_comp[i]);
            // Get the elevation
            try {
                if(CORBA::is_nil(m_antennaMount))
                    m_antennaMount = (m_configuration->m_services)->getComponent<Antenna::Mount>("ANTENNA/Mount");

                TIMEVALUE now;
                double elevation = 45.0;
                IRA::CIRATools::getTime(now);

                try {
                    if(!CORBA::is_nil(m_antennaMount)) {
                        // Get the reference to the  actEl double property
                        ACS::ROdouble_var actEl = m_antennaMount->commandedElevation();
                        if (actEl.ptr() != ACS::ROdouble::_nil()) {
                            // Get the current value of the property synchronously
                            ACSErr::Completion_var completion;
                            elevation = actEl->get_sync(completion.out());
                        }
                        else {
                            ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the antenna elevation."));
                            m_configuration->m_isElevationTracking = false;
                        }
                    }
                }
                catch (CORBA::SystemException& ex) {
                    ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the antenna elevation."));
                    m_configuration->m_isElevationTracking = false;
                }

                // elevation = elevation * DR2D; // From radians to decimal (be sure we want to use decimal...)
                elevation = (elevation > MIN_ELEVATION) ? elevation : 45.0; // Decimal?

                // Set the servo position
                MinorServo::WPServo_var component_ref;
                if((m_configuration->m_component_refs).count(comp_name)) {
                    component_ref = (m_configuration->m_component_refs)[comp_name];
                    if(!CORBA::is_nil(component_ref)) {
                        // Set a doubleSeq from a string of positions
                        ACS::doubleSeq positions = m_configuration->getPosition(comp_name, elevation);
                        // Set a minor servo position if the doubleSeq is not empty
                        if(positions.length()) {
                            component_ref->setPosition(positions, NOW);
                        }
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
                ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the AntennaMount component"));
            }
        }
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "An error is occurred in TrackingThread"));
        m_configuration->m_isElevationTracking = false;
    }
}

