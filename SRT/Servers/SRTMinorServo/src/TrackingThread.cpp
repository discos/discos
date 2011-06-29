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

TrackingThread::TrackingThread(
        const ACE_CString& name, 
        MSThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_params(&params)
{
    AUTO_TRACE("TrackingThread::TrackingThread()");
}

TrackingThread::~TrackingThread() { AUTO_TRACE("TrackingThread::~TrackingThread()"); }

void TrackingThread::onStart() { AUTO_TRACE("TrackingThread::onStart()"); }

void TrackingThread::onStop() { AUTO_TRACE("TrackingThread::onStop()"); }

void TrackingThread::runLoop()
{
    AUTO_TRACE("TrackingThread::runLoop()");

    TIMEVALUE now;
    IRA::CIRATools::getTime(now);

    try {
        for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
            // Split the action in items.
            vector<string> items = split(*iter, items_separator);
            // Set the name of component to move from a string
            string comp_name = get_component_name(items.front());
            MinorServo::WPServo_var component_ref;
            if((*m_params->tracking_list).count(comp_name))
                if((*m_params->tracking_list)[comp_name] && (*m_params->component_refs).count(comp_name)) {
                    component_ref = (*m_params->component_refs)[comp_name];
                    if(!CORBA::is_nil(component_ref)) {
                        // Set a doubleSeq from a string of positions
                        ACS::doubleSeq positions = get_positions(comp_name, items.back(), m_params);
                        // Set a minor servo position if the doubleSeq is not empty
                        if(positions.length() && !CORBA::is_nil(component_ref) && *m_params->actual_conf == m_params->commanded_conf)
                            component_ref->setPosition(positions, NOW);
                    }
                    else {
                        ACS_SHORT_LOG((LM_WARNING, ("TrackingThread: _nil reference for " + comp_name).c_str()));
                    }
                }
        }
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "An error is occurred in TrackingThread"));
    }
}

