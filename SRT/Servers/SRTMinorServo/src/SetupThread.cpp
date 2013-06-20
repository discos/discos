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
#include <bitset>


SetupThread::SetupThread(
        const ACE_CString& name, 
        MSThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_params(&params)
{
    AUTO_TRACE("SetupThread::SetupThread()");
    m_validActions = false;
    m_toPark.clear();
    m_toSet.clear();
    m_positioning.clear();
}

SetupThread::~SetupThread() { AUTO_TRACE("SetupThread::~SetupThread()"); }

void SetupThread::onStart() { 
    AUTO_TRACE("SetupThread::onStart()"); 

    m_toPark.clear();
    m_toSet.clear();
    m_positioning.clear();
    m_validActions = true;
    *(m_params->status) = Management::MNG_OK;
    // Get the actions
    for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
        // Split the action in items.
        vector<string> items = split(*iter, items_separator);
        // Get the name of component 
        string comp_name = get_component_name(items.front());
        // Get the action
        string action(items.back());
        strip(action);
        if(startswith(action, "none") || startswith(action, "NONE")) {
            continue;
        }
        else if(startswith(action, "park") || startswith(action, "PARK")) {
            m_toPark.push_back(comp_name);
        }
        else
            // Set a doubleSeq from a string of positions
            try {
                ACS::doubleSeq positions = get_positions(comp_name, action, m_params);
                m_toSet[comp_name] = positions;
            }
            catch(...) {
                m_validActions=false;
                return;
            }
    }
}

void SetupThread::onStop() { AUTO_TRACE("SetupThread::onStop()"); }

void SetupThread::run()
{
    AUTO_TRACE("SetupThread::run()");
    if(!m_validActions) {
        m_params->is_initialized = false;
        *(m_params->status) = Management::MNG_FAILURE;
        pthread_mutex_unlock(m_params->setup_mutex); 
        m_params->is_setup_locked = false;
        ACS_SHORT_LOG((LM_ERROR, ("SetupThread::onStart(): error getting the configuration from the CDB.")));
        return;
    }

    // Park the components
    unsigned long counter = 0;
    while(true) {
        unsigned short num_of_parked_comp = 0;
        for(vector<string>::iterator iter = m_toPark.begin(); iter != m_toPark.end(); iter++) {
            MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
            if((*m_params->component_refs).count(*iter)) {
                component_ref = (*m_params->component_refs)[*iter];
                // Park the minor servo
                try {
                    if(!CORBA::is_nil(component_ref)) {
                        bitset<STATUS_WIDTH> status_bset(component_ref->getStatus());
                        if(component_ref->isParking()) {
                            ACS_SHORT_LOG((LM_INFO, (string("SetupThread: the ") + *iter + string(" is parking.")).c_str()));
                            continue;
                        }
                        if(component_ref->isStarting()) {
                            ACS_SHORT_LOG((LM_INFO, (string("SetupThread: the ") + *iter + string(" is starting.")).c_str()));
                            continue;
                        }
                        else if(component_ref->isParked()) {
                            ++num_of_parked_comp;
                            ACS_SHORT_LOG((LM_INFO, (string("SetupThread: the ") + *iter + string(" is parked.")).c_str()));
                            continue;
                        }
                        else if(component_ref->isReadyToSetup()) {
                            component_ref->setup(0);
                            continue;
                        }
                        else if(component_ref->isDisabledFromOtherDC()) {
                            string msg(*iter + " disabled from other DC.");
                            ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                            *(m_params->status) = Management::MNG_FAILURE;
                            pthread_mutex_unlock(m_params->setup_mutex); 
                            m_params->is_setup_locked = false;
                            return;
                        }
                        else if(status_bset.test(STATUS_FAILURE)) {
                            string msg(*iter + " in failure.");
                            ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                            *(m_params->status) = Management::MNG_FAILURE;
                            pthread_mutex_unlock(m_params->setup_mutex); 
                            m_params->is_setup_locked = false;
                            return;
                        }
                        else if(component_ref->isInEmergencyStop()){
                            string msg(*iter + " in emergency stop.");
                            ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                            *(m_params->status) = Management::MNG_FAILURE;
                            pthread_mutex_unlock(m_params->setup_mutex); 
                            m_params->is_setup_locked = false;
                            return;
                        }
                        else if(!component_ref->isReady()) {
                            string msg(*iter + " not ready.");
                            ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                            *(m_params->status) = Management::MNG_FAILURE;
                            pthread_mutex_unlock(m_params->setup_mutex); 
                            m_params->is_setup_locked = false;
                            return;
                        }
                        else {
                            component_ref->stow(0);
                            continue;
                        }
                    }
                    else {
                        ACS_SHORT_LOG((LM_ERROR, ("SetupThread: cannot get the component "  + (*iter)).c_str()));
                        *(m_params->status) = Management::MNG_FAILURE;
                        pthread_mutex_unlock(m_params->setup_mutex); 
                        m_params->is_setup_locked = false;
                        return;
                    }
                }
                catch(...) {
                    ACS_SHORT_LOG((LM_ERROR, ("SetupThread: error parking "  + (*iter)).c_str()));
                    *(m_params->status) = Management::MNG_FAILURE;
                    pthread_mutex_unlock(m_params->setup_mutex); 
                    m_params->is_setup_locked = false;
                    return;
                }
            }
            else {
                ACS_SHORT_LOG((LM_ERROR, ("SetupThread::run(): error performing the setup")));
                *(m_params->status) = Management::MNG_FAILURE;
                pthread_mutex_unlock(m_params->setup_mutex); 
                m_params->is_setup_locked = false;
                return;
            }
        }
        if(m_toPark.size() == num_of_parked_comp)
            break;

        ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME); // 2 seconds
        counter += ITER_SLEEP_TIME / 2;
        if(sleep_ret != SLEEP_OK || counter > MAX_ACTION_TIME) {
            ACS_SHORT_LOG((LM_WARNING, ("SetupThread::run(): MAX_ACTION_TIME reached.")));
            pthread_mutex_unlock(m_params->setup_mutex); 
            m_params->is_setup_locked = false;
            m_params->is_initialized = false;
            return;
        }
    }

    // Wait till the PFP or SRP get ready
    ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
    sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME * 3); // 6 seconds
    if(sleep_ret != SLEEP_OK || counter > MAX_ACTION_TIME) {
        pthread_mutex_unlock(m_params->setup_mutex); 
        m_params->is_setup_locked = false;
        m_params->is_initialized = false;
        return;
    }

    // Positioning
    counter = 0;
    while(true) {
        unsigned short num_of_on_target = 0;
        for(map<string, ACS::doubleSeq>::iterator iter = m_toSet.begin(); iter != m_toSet.end(); iter++) {
            MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
            if((*m_params->component_refs).count(iter->first)) {
                component_ref = (*m_params->component_refs)[iter->first];
                // Positioning the minor servo
                try {
                    if(!CORBA::is_nil(component_ref)) {
                        // Compute the position difference
                        ACS::ROdoubleSeq_var refActPos = component_ref->actPos();
                        if (refActPos.ptr() != ACS::ROdoubleSeq::_nil()) {
                            ACSErr::Completion_var completion;
                            ACS::doubleSeq * act_pos = refActPos->get_sync(completion.out());
                            if(act_pos->length() != (iter->second).length()) {
                                ACS_SHORT_LOG((LM_ERROR, ("SetupThread: lenghts of target and act pos do not match")));
                                pthread_mutex_unlock(m_params->setup_mutex); 
                                m_params->is_setup_locked = false;
                                m_params->is_initialized = false;
                                return;
                            }
                            bool on_target = true;
                            // Compute the difference between actual and target positions
                            for(size_t i=0; i<(iter->second).length(); i++) { 
                                if(fabs((iter->second)[i] - (*act_pos)[i]) > component_ref->getTrackingDelta())
                                    on_target = false;
                            }
                            if(on_target) {
                                ++num_of_on_target;
                                continue;
                            }
                            else {
                                bitset<STATUS_WIDTH> status_bset(component_ref->getStatus());
                                if(component_ref->isParking()) {
                                    ACS_SHORT_LOG((LM_INFO, ("SetupThread: the " + iter->first + " is parking.").c_str()));
                                    continue;
                                }
                                if(component_ref->isStarting()) {
                                    ACS_SHORT_LOG((LM_INFO, ("SetupThread: the " + iter->first + " is starting.").c_str()));
                                    continue;
                                }
                                else if(component_ref->isReadyToSetup()) {
                                    component_ref->setup(0);
                                    continue;
                                }
                                else if(component_ref->isDisabledFromOtherDC()) {
                                    string msg(iter->first + " disabled from other DC.");
                                    ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                                    *(m_params->status) = Management::MNG_FAILURE;
                                    pthread_mutex_unlock(m_params->setup_mutex); 
                                    m_params->is_setup_locked = false;
                                    return;
                                }
                                else if(status_bset.test(STATUS_FAILURE)) {
                                    string msg(iter->first + " in failure.");
                                    ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                                    *(m_params->status) = Management::MNG_FAILURE;
                                    pthread_mutex_unlock(m_params->setup_mutex); 
                                    m_params->is_setup_locked = false;
                                    return;
                                }
                                else if(component_ref->isInEmergencyStop()){
                                    string msg(iter->first + " in emergency stop.");
                                    ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                                    *(m_params->status) = Management::MNG_FAILURE;
                                    pthread_mutex_unlock(m_params->setup_mutex); 
                                    m_params->is_setup_locked = false;
                                    return;
                                }
                                else if(!component_ref->isReady()) {
                                    string msg(iter->first + " not ready.");
                                    ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                                    *(m_params->status) = Management::MNG_FAILURE;
                                    pthread_mutex_unlock(m_params->setup_mutex); 
                                    m_params->is_setup_locked = false;
                                    return;
                                }
                                else {
                                    if(std::find(m_positioning.begin(), m_positioning.end(), iter->first) != m_positioning.end()) {
                                        ACS_SHORT_LOG((LM_INFO, ("SetupThread: the " + iter->first + " is moving.").c_str()));
                                        continue; // The servo is moving
                                    }
                                    else {
                                        component_ref->setPosition(iter->second, 0);
                                        m_positioning.push_back(iter->first);
                                        continue;
                                    }
                                }
                            }
                        }
                        else {
                            ACS_SHORT_LOG((LM_ERROR, (string("SetupThread: cannot get the act_pos of ")  + iter->first).c_str()));
                            pthread_mutex_unlock(m_params->setup_mutex); 
                            m_params->is_setup_locked = false;
                            m_params->is_initialized = false;
                            return;
                        }
                    }
                    else {
                        ACS_SHORT_LOG((LM_ERROR, ("SetupThread: cannot get the component "  + iter->first).c_str()));
                        *(m_params->status) = Management::MNG_FAILURE;
                        pthread_mutex_unlock(m_params->setup_mutex); 
                        m_params->is_setup_locked = false;
                        return;
                    }
                }
                catch(...) {
                    ACS_SHORT_LOG((LM_ERROR, ("SetupThread: error positioning "  + iter->first).c_str()));
                    *(m_params->status) = Management::MNG_FAILURE;
                    pthread_mutex_unlock(m_params->setup_mutex); 
                    m_params->is_setup_locked = false;
                    return;
                }
            }
            else {
                ACS_SHORT_LOG((LM_ERROR, ("SetupThread::run(): error performing the setup")));
                *(m_params->status) = Management::MNG_FAILURE;
                pthread_mutex_unlock(m_params->setup_mutex); 
                m_params->is_setup_locked = false;
                return;
            }
        }
        if(m_toSet.size() == num_of_on_target)
            break;

        ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME); // 2 second
        counter += ITER_SLEEP_TIME / 2;
        if(sleep_ret != SLEEP_OK || counter > MAX_ACTION_TIME) {
            ACS_SHORT_LOG((LM_WARNING, ("SetupThread::run(): MAX_ACTION_TIME reached.")));
            pthread_mutex_unlock(m_params->setup_mutex); 
            m_params->is_setup_locked = false;
            m_params->is_initialized = false;
            return;
        }
    }

    // Setup DONE
    *m_params->actual_conf = m_params->commanded_conf;
    m_params->is_initialized = true;
    if(m_params->tracking_thread_ptr != NULL)
        (m_params->tracking_thread_ptr)->resume();

    pthread_mutex_unlock(m_params->setup_mutex); 
    m_params->is_setup_locked = false;
    ACS_SHORT_LOG((LM_INFO, ("SetupThread::run(): setup done.")));
    return;
}

