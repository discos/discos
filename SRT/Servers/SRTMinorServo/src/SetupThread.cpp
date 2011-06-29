/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "SetupThread.h"
#include <pthread.h>
#include "MSParameters.h"
#include "MinorServoBossImpl.h"
#include "utils.h"


SetupThread::SetupThread(
        const ACE_CString& name, 
        MSThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_params(&params)
{
    AUTO_TRACE("SetupThread::SetupThread()");
}

SetupThread::~SetupThread() { AUTO_TRACE("SetupThread::~SetupThread()"); }

void SetupThread::onStart() { AUTO_TRACE("SetupThread::onStart()"); }

void SetupThread::onStop() { AUTO_TRACE("SetupThread::onStop()"); }

void SetupThread::run()
{
    AUTO_TRACE("SetupThread::run()");
    try {

        for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
            // Split the action in items.
            vector<string> items = split(*iter, items_separator);
            // Set the name of component to move from a string
            MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
            string comp_name = get_component_name(items.front());
            if((*m_params->component_refs).count(comp_name)) {
                component_ref = (*m_params->component_refs)[comp_name];
                // Park the minor servo
                try {
                    if(!CORBA::is_nil(component_ref))
                        component_ref->stow(NOW);
                }
                catch(...) {
                    ACS_SHORT_LOG((LM_WARNING, ("SetupThread: error parking " + comp_name).c_str()));
                }
            }
            ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
            sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME); // 1 second
            if(sleep_ret != SLEEP_OK) {
                pthread_mutex_unlock(m_params->setup_mutex); 
                m_params->is_initialized = false;
                return;
            }
        }
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "Exception in SetupThread"));
    }

    try {
        unsigned int counter = 0;
        while(counter < MAX_PARK_TIME) {
            bool all_parked = true;
            counter += INCR_SLEEP_TIME;
            ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
            sleep_ret = ACS::ThreadBase::sleep(INCR_SLEEP_TIME);
            if(sleep_ret != SLEEP_OK) {
                pthread_mutex_unlock(m_params->setup_mutex); 
                m_params->is_initialized = false;
                return;
            }

            for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
                // Split the action in items.
                vector<string> items = split(*iter, items_separator);
                // Set the name of component to move from a string
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
                string comp_name = get_component_name(items.front());
                if((*m_params->component_refs).count(comp_name)) {
                    component_ref = (*m_params->component_refs)[comp_name];
                    // If the MinorServo should be parked
                    if(!CORBA::is_nil(component_ref) && !component_ref->isParked())
                        all_parked = false;
                }
            }

            if(all_parked)
                break;
        }
    }
    catch(...) {
        ACS_SHORT_LOG((LM_ERROR, "Exception in SetupThread"));
    }

    try {
        for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
            // Split the action in items.
            vector<string> items = split(*iter, items_separator);
            // Set the name of component to move from a string
            MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
            string comp_name = get_component_name(items.front());
            // Set a doubleSeq from a string of positions
            ACS::doubleSeq positions = get_positions(comp_name, items.back(), m_params);

            if(positions.length() && (*m_params->component_refs).count(comp_name)) {
                component_ref = (*m_params->component_refs)[comp_name];
                // Set a minor servo position if the doubleSeq is not empty
                if(!CORBA::is_nil(component_ref))
                    component_ref->setup(NOW);
            }
            ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
            sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME);
            if(sleep_ret != SLEEP_OK) {
                pthread_mutex_unlock(m_params->setup_mutex); 
                m_params->is_initialized = false;
                return;
            }
        }
    }
    catch(...) {
        ACS_SHORT_LOG((LM_ERROR, "Exception in SetupThread"));
    }

    bool is_initialized = false;
    try {
        unsigned int counter = 0;
        is_initialized = true;
        while(counter < MAX_SETUP_TIME) {
            counter += INCR_SLEEP_TIME;
            ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
            sleep_ret = ACS::ThreadBase::sleep(INCR_SLEEP_TIME);
            if(sleep_ret != SLEEP_OK) {
                pthread_mutex_unlock(m_params->setup_mutex); 
                is_initialized = false;
                return;
            }
            for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
                // Split the action in items.
                vector<string> items = split(*iter, items_separator);
                // Set the name of component to move from a string
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
                string comp_name = get_component_name(items.front());
                // Set a doubleSeq from a string of positions
                ACS::doubleSeq positions = get_positions(comp_name, items.back(), m_params);

                // Set a minor servo position if the MinorServo is Ready
                if(positions.length() && (*m_params->component_refs).count(comp_name)) {
                    component_ref = (*m_params->component_refs)[comp_name];
                    // Check if the MinorServo is configured
                    if(CORBA::is_nil(component_ref) || !component_ref->isReady()) {
                        is_initialized = false;
                        break;
                    }
                }
            }
        }
    }
    catch(...) {
        is_initialized = false;
        ACS_SHORT_LOG((LM_ERROR, "Exception in SetupThread"));
    }

    try {
        if(is_initialized) {
            for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
                // Split the action in items.
                vector<string> items = split(*iter, items_separator);
                // Set the name of component to move from a string
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
                string comp_name = get_component_name(items.front());
                // Set a doubleSeq from a string of positions
                ACS::doubleSeq positions = get_positions(comp_name, items.back(), m_params);

                if(positions.length() && (*m_params->component_refs).count(comp_name)) {
                    component_ref = (*m_params->component_refs)[comp_name];
                    // Set a minor servo position if the doubleSeq is not empty
                    if(!CORBA::is_nil(component_ref))
                        component_ref->setPosition(positions, NOW);
                }
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME);
                if(sleep_ret != SLEEP_OK) {
                    pthread_mutex_unlock(m_params->setup_mutex); 
                    m_params->is_initialized = false;
                    return;
                }
            }
            // Notify the successfully of setup
            *m_params->actual_conf = m_params->commanded_conf;
            m_params->is_initialized = true;
            if(m_params->tracking_thread_ptr != NULL)
                (m_params->tracking_thread_ptr)->resume();
        }
        m_params->is_setup_locked = false;
        pthread_mutex_unlock(m_params->setup_mutex); 
    }
    catch(...) {
        m_params->is_setup_locked = false;
        pthread_mutex_unlock(m_params->setup_mutex); 
        ACS_SHORT_LOG((LM_ERROR, "Exception in SetupThread"));
    }
}

