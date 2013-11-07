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
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_configuration(configuration)
{
    AUTO_TRACE("SetupThread::SetupThread()");
    m_positioning.clear();
}

SetupThread::~SetupThread() { AUTO_TRACE("SetupThread::~SetupThread()"); }

void SetupThread::onStart() { 
    AUTO_TRACE("SetupThread::onStart()"); 

    m_positioning.clear();
    m_configuration->m_status = Management::MNG_OK;
}

void SetupThread::onStop() { AUTO_TRACE("SetupThread::onStop()"); }

void SetupThread::run()
{
    AUTO_TRACE("SetupThread::run()");
    if(!m_configuration->isValidCDBConfiguration()) {
        ACS_SHORT_LOG((LM_ERROR, "Wrong CDB configuration."));
        m_configuration->m_status = Management::MNG_FAILURE;
        m_configuration->m_isStarting = false;
        m_configuration->m_isConfigured = true;
        return;
    }

    vector<string> park_check; 

    // Park the components
    unsigned long counter = 0;
    vector<string> toPark(m_configuration->m_servosToPark);
    vector<string> toMove(m_configuration->m_servosToMove);
    while(true) {
        for(vector<string>::iterator iter = toPark.begin(); iter != toPark.end(); iter++) {
            string comp_name = *iter;
            MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
            if((m_configuration->m_component_refs).count(comp_name)) {
                component_ref = (m_configuration->m_component_refs)[comp_name];
                // Park the minor servo
                try {
                    if(!CORBA::is_nil(component_ref)) {
                        bitset<STATUS_WIDTH> status_bset(component_ref->getStatus());
                        if(component_ref->isParking()) {
                            ACS_SHORT_LOG((LM_INFO, (string("SetupThread: the ") + comp_name + string(" is parking.")).c_str()));
                            continue;
                        }
                        if(component_ref->isStarting()) {
                            ACS_SHORT_LOG((LM_INFO, (string("SetupThread: the ") + comp_name + string(" is starting.")).c_str()));
                            continue;
                        }
                        else if(component_ref->isParked()) {
                            if(find(park_check.begin(), park_check.end(), comp_name) == park_check.end())
                                park_check.push_back(comp_name);
                            ACS_SHORT_LOG((LM_INFO, (string("SetupThread: the ") + comp_name + string(" is parked.")).c_str()));
                            continue;
                        }
                        else if(component_ref->isReadyToSetup()) {
                            component_ref->setup(0);
                            continue;
                        }
                        else if(component_ref->isDisabledFromOtherDC()) {
                            if(find(park_check.begin(), park_check.end(), comp_name) == park_check.end())
                                park_check.push_back(comp_name);
                            ACS_SHORT_LOG((LM_INFO, (string("SetupThread: the ") + comp_name + string(" is disabled from other DC.")).c_str()));
                            continue;
                        }
                        else if(status_bset.test(STATUS_FAILURE)) {
                            string msg(comp_name + " in failure.");
                            ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                            if(find(park_check.begin(), park_check.end(), comp_name) == park_check.end())
                                park_check.push_back(comp_name);
                            continue;
                        }
                        else if(component_ref->isInEmergencyStop()){
                            string msg(comp_name + " in emergency stop.");
                            ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                            m_configuration->m_status = Management::MNG_FAILURE;
                            m_configuration->m_isStarting = false;
                            m_configuration->m_isConfigured = false;
                            m_configuration->m_actualSetup = "unknown";
                            return;
                        }
                        else if(!component_ref->isReady()) {
                            // Set the targetPos
                            continue;
                        }
                        else {
                            component_ref->clearUserOffset(false);
                            component_ref->stow(0);
                            continue;
                        }
                    }
                    else {
                        ACS_SHORT_LOG((LM_ERROR, ("SetupThread: cannot get the component "  + comp_name).c_str()));
                        m_configuration->m_status = Management::MNG_FAILURE;
                        m_configuration->m_isStarting = false;
                        m_configuration->m_isConfigured = false;
                        m_configuration->m_actualSetup = "unknown";
                        return;
                    }
                }
                catch(...) {
                    ACS_SHORT_LOG((LM_ERROR, ("SetupThread: error parking "  + comp_name).c_str()));
                    m_configuration->m_status = Management::MNG_FAILURE;
                    m_configuration->m_isStarting = false;
                    m_configuration->m_isConfigured = false;
                    m_configuration->m_actualSetup = "unknown";
                    return;
                }
            }
            else {
                ACS_SHORT_LOG((LM_ERROR, ("SetupThread::run(): error performing the setup")));
                m_configuration->m_status = Management::MNG_FAILURE;
                m_configuration->m_isStarting = false;
                m_configuration->m_isConfigured = false;
                m_configuration->m_actualSetup = "unknown";
                return;
            }
        }
        if(toPark.size() == park_check.size())
            break;

        ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME); // 2 seconds
        counter += ITER_SLEEP_TIME / 2;
        if(sleep_ret != SLEEP_OK || counter > MAX_ACTION_TIME) {
            ACS_SHORT_LOG((LM_WARNING, ("SetupThread::run(): MAX_ACTION_TIME reached.")));
            m_configuration->m_isStarting = false;
            m_configuration->m_isConfigured = false;
            m_configuration->m_actualSetup = "unknown";
            return;
        }
    }

    // Wait till the PFP or SRP get ready
    ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
    sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME * 3); // 6 seconds
    if(sleep_ret != SLEEP_OK || counter > MAX_ACTION_TIME) {
        m_configuration->m_isStarting = false;
        m_configuration->m_isConfigured = false;
        m_configuration->m_actualSetup = "unknown";
        return;
    }

    // Positioning
    counter = 0;
    vector<string> on_target_check; 
    while(true) {
        for(vector<string>::iterator iter = toMove.begin(); iter != toMove.end(); iter++) {
            string comp_name = *iter;
            MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
            if((m_configuration->m_component_refs).count(comp_name)) {
                component_ref = (m_configuration->m_component_refs)[comp_name];
                // Positioning the minor servo
                try {
                    if(!CORBA::is_nil(component_ref)) {
                        // Compute the position difference
                        ACS::ROdoubleSeq_var refActPos = component_ref->actPos();
                        if (refActPos.ptr() != ACS::ROdoubleSeq::_nil()) {
                            ACSErr::Completion_var completion;
                            ACS::doubleSeq * act_pos = refActPos->get_sync(completion.out());
                            TIMEVALUE now;
                            IRA::CIRATools::getTime(now);
                            ACS::doubleSeq target_pos = m_configuration->getPosition(comp_name, getTimeStamp());

                            if(act_pos->length() != target_pos.length()) {
                                ACS_SHORT_LOG((LM_ERROR, ("SetupThread: lenghts of target and act pos do not match")));
                                m_configuration->m_isStarting = false;
                                m_configuration->m_isConfigured = false;
                                m_configuration->m_actualSetup = "unknown";
                                return;
                            }


                            bool on_target = true;
                            // Compute the difference between actual and target positions
                            for(size_t i=0; i<target_pos.length(); i++) { 
                                if(i < 3) {
                                    if(fabs(target_pos[i] - (*act_pos)[i]) > component_ref->getTrackingDelta()/5) {
                                        on_target = false;
                                    }
                                }
                                else {
                                    if(fabs(target_pos[i] - (*act_pos)[i]) > component_ref->getTrackingDelta()/1000) {
                                        on_target = false;
                                    }
                                }
                            }

                            if(on_target) {
                                if(find(on_target_check.begin(), on_target_check.end(), comp_name) == on_target_check.end()) {
                                    on_target_check.push_back(comp_name);
                                    component_ref->clearUserOffset(false);
                                    if(component_ref->isReadyToSetup())
                                        component_ref->setup(0);
                                }
                                continue;
                            }
                            else {
                                bitset<STATUS_WIDTH> status_bset(component_ref->getStatus());
                                if(component_ref->isParking()) {
                                    ACS_SHORT_LOG((LM_INFO, ("SetupThread: the " + comp_name + " is parking.").c_str()));
                                    continue;
                                }
                                if(component_ref->isStarting()) {
                                    ACS_SHORT_LOG((LM_INFO, ("SetupThread: the " + comp_name + " is starting.").c_str()));
                                    continue;
                                }
                                else if(component_ref->isReadyToSetup()) {
                                    component_ref->setup(0);
                                    continue;
                                }
                                else if(component_ref->isDisabledFromOtherDC()) {
                                    string msg(comp_name + " disabled from other DC.");
                                    ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                                    m_configuration->m_status = Management::MNG_FAILURE;
                                    m_configuration->m_isStarting = false;
                                    m_configuration->m_isConfigured = false;
                                    m_configuration->m_actualSetup = "unknown";
                                    return;
                                }
                                else if(status_bset.test(STATUS_FAILURE)) {
                                    string msg(comp_name + " in failure.");
                                    ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                                    m_configuration->m_status = Management::MNG_FAILURE;
                                    m_configuration->m_isStarting = false;
                                    m_configuration->m_isConfigured = false;
                                    m_configuration->m_actualSetup = "unknown";
                                    return;
                                }
                                else if(component_ref->isInEmergencyStop()){
                                    string msg(comp_name + " in emergency stop.");
                                    ACS_SHORT_LOG((LM_ERROR, msg.c_str()));
                                    m_configuration->m_status = Management::MNG_FAILURE;
                                    m_configuration->m_isStarting = false;
                                    m_configuration->m_isConfigured = false;
                                    m_configuration->m_actualSetup = "unknown";
                                    return;
                                }
                                else if(!component_ref->isReady()) {
                                    continue;
                                }
                                else {
                                    if(std::find(m_positioning.begin(), m_positioning.end(), comp_name) != m_positioning.end()) {
                                        ACS_SHORT_LOG((LM_INFO, ("SetupThread: the " + comp_name + " is moving.").c_str()));
                                        continue; // The servo is moving
                                    }
                                    else {
                                        ACS_SHORT_LOG((LM_INFO, ("SetupThread: positioning " + comp_name).c_str()));
                                        component_ref->clearUserOffset(false);
                                        component_ref->setPosition(target_pos, 0);
                                        m_positioning.push_back(comp_name);
                                        continue;
                                    }
                                }
                            }
                        }
                        else {
                            ACS_SHORT_LOG((LM_ERROR, (string("SetupThread: cannot get the act_pos of ")  + comp_name).c_str()));
                            m_configuration->m_isStarting = false;
                            m_configuration->m_isConfigured = false;
                            m_configuration->m_actualSetup = "unknown";
                            return;
                        }
                    }
                    else {
                        ACS_SHORT_LOG((LM_ERROR, ("SetupThread: cannot get the component "  + comp_name).c_str()));
                        m_configuration->m_status = Management::MNG_FAILURE;
                        m_configuration->m_isStarting = false;
                        m_configuration->m_isConfigured = false;
                        m_configuration->m_actualSetup = "unknown";
                        return;
                    }
                }
                catch(...) {
                    ACS_SHORT_LOG((LM_ERROR, ("SetupThread: error positioning "  + comp_name).c_str()));
                    m_configuration->m_status = Management::MNG_FAILURE;
                    m_configuration->m_isStarting = false;
                    m_configuration->m_isConfigured = false;
                    m_configuration->m_actualSetup = "unknown";
                    return;
                }
            }
            else {
                ACS_SHORT_LOG((LM_ERROR, ("SetupThread::run(): error performing the setup")));
                m_configuration->m_status = Management::MNG_FAILURE;
                m_configuration->m_isStarting = false;
                m_configuration->m_isConfigured = false;
                m_configuration->m_actualSetup = "unknown";
                return;
            }
        }
        if(toMove.size() == on_target_check.size())
            break;

        ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(ITER_SLEEP_TIME); // 2 seconds
        counter += ITER_SLEEP_TIME / 2;
        if(sleep_ret != SLEEP_OK || counter > MAX_ACTION_TIME) {
            ACS_SHORT_LOG((LM_WARNING, ("SetupThread::run(): MAX_ACTION_TIME reached.")));
            m_configuration->m_isStarting = false;
            m_configuration->m_isConfigured = false;
            m_configuration->m_actualSetup = "unknown";
            return;
        }
    }

    // Setup DONE
    m_configuration->m_isStarting = false;
    m_configuration->m_isConfigured = true;
    m_configuration->m_actualSetup = m_configuration->m_commandedSetup;

    ACS_SHORT_LOG((LM_INFO, ("SetupThread::run(): setup done.")));
    return;
}

