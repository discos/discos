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

TrackingThread::TrackingThread(
        const ACE_CString& name, 
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
            ) : ACS::Thread(name, responseTime, sleepTime, del), m_configuration(configuration)
    {
        AUTO_TRACE("TrackingThread::TrackingThread()");
        m_ready_error = false;
    }

    TrackingThread::~TrackingThread() { AUTO_TRACE("TrackingThread::~TrackingThread()"); }

    void TrackingThread::onStart() { 
        AUTO_TRACE("TrackingThread::onStart()"); 
    }

    void TrackingThread::onStop() { 
        AUTO_TRACE("TrackingThread::onStop()"); 
        m_configuration->m_isElevationTracking = false;
    }

    void TrackingThread::runLoop()
    {
        AUTO_TRACE("TrackingThread::runLoop()");

        vector<string>::iterator start = (m_configuration->getDynamicComponents()).begin();
        vector<string>::iterator end = (m_configuration->getDynamicComponents()).end();
        m_configuration->m_isElevationTracking = true;

        try {
            vector<string> dynamics_comp = m_configuration->getDynamicComponents();
            for(size_t i=0; i<dynamics_comp.size(); i++) {
                string comp_name(dynamics_comp[i]);
                    // Set the servo position
                    MinorServo::WPServo_var component_ref;
                    if((m_configuration->m_component_refs).count(comp_name)) {
                        component_ref = (m_configuration->m_component_refs)[comp_name];
                        if(!CORBA::is_nil(component_ref)) {
                            // Set a doubleSeq from a string of positions
                            ACS::Time t = getTimeStamp() + ELEVATION_FUTURE_TIME;
                            ACS::doubleSeq positions = m_configuration->getPosition(comp_name, t);
                        // Set a minor servo position if the doubleSeq is not empty
                        if(positions.length()) {
                            if(component_ref->isReady()) {
                                component_ref->setPosition(positions, NOW);
                                m_ready_error = false;
                                m_configuration->m_status = Management::MNG_OK;
                            }
                            else {
                                if(component_ref->isStarting() || component_ref->isParked() || component_ref->isParking()) {
                                    m_ready_error = false;
                                    m_configuration->m_status = Management::MNG_OK;
                                }
                                else {
                                    m_configuration->m_status = Management::MNG_FAILURE;
                                    if(!m_ready_error) {
                                        string msg(comp_name + " in failure.");
                                        ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                                        m_configuration->m_isElevationTracking = false;
                                        m_ready_error = true;
                                    }
                                }
                            }
                        }
                        else {
                            ACS_SHORT_LOG((LM_WARNING, "TrackingThread: cannot get the position to set."));
                            m_configuration->m_isElevationTracking = false;
                        }
                    }
                    else {
                        ACS_SHORT_LOG((LM_WARNING, ("TrackingThread: _nil reference for " + comp_name).c_str()));
                        m_configuration->m_isElevationTracking = false;
                    }
                }
        }
    }
    catch (ACSErr::ACSbaseExImpl& E) {
        ACS_SHORT_LOG((LM_WARNING, "Unexpected error in TrackingThread"));
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "An error is occurred in TrackingThread"));
        m_configuration->m_isElevationTracking = false;
    }
}

