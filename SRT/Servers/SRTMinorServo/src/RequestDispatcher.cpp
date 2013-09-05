/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "RequestDispatcher.h"
#include <pthread.h>
#include <time.h>


RequestDispatcher::RequestDispatcher(
        const ACE_CString& name, 
        ThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime
        ) : ACS::Thread(name, responseTime, sleepTime), m_params(&params)
{
    AUTO_TRACE("RequestDispatcher::RequestDispatcher()");
}

RequestDispatcher::~RequestDispatcher() { 
    AUTO_TRACE("RequestDispatcher::~RequestDispatcher()"); 
}

void RequestDispatcher::onStart() { AUTO_TRACE("RequestDispatcher::onStart()"); }

void RequestDispatcher::onStop() { AUTO_TRACE("RequestDispatcher::onStop()"); }

void RequestDispatcher::runLoop()
{
    // A smarter policy than round robin could be to iterate only on active servos.
    try {
        pthread_mutex_lock(m_params->scheduler_mutex); 
        map<int, WPServoTalker *>::iterator map_it = (m_params->map_of_talkers_ptr)->begin();
        map<int, WPServoTalker *>::iterator map_end = (m_params->map_of_talkers_ptr)->end();
        while(map_it != map_end) {
            if(!(((map_it->second)->m_requests)->Get())->empty()) {
                CSecAreaResourceWrapper<vector<string> > requests = ((map_it->second)->m_requests)->Get();
                (m_params->socket_ptr)->sendBuffer(*(requests->begin()));
                requests->erase(requests->begin());
                requests.Release();
            }
            ++map_it;
        }
        pthread_mutex_unlock(m_params->scheduler_mutex); 
    }
    catch(...) {
        pthread_mutex_unlock(m_params->scheduler_mutex); 
        ACS_SHORT_LOG((LM_ERROR, "Unexpected error in RequestDispatcher"));
    }
}
 
