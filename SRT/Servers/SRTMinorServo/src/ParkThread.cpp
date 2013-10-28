/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "ParkThread.h"
#include <pthread.h>
#include "MSParameters.h"
#include "MinorServoBossImpl.h"
#include "utils.h"
#include <bitset>


ParkThread::ParkThread(
        const ACE_CString& name, 
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_configuration(configuration)
{
    AUTO_TRACE("ParkThread::ParkThread()");
    m_positioning.clear();
}

ParkThread::~ParkThread() { AUTO_TRACE("ParkThread::~ParkThread()"); }

void ParkThread::onStart() { 
    AUTO_TRACE("ParkThread::onStart()"); 

    m_positioning.clear();
}

void ParkThread::onStop() { AUTO_TRACE("ParkThread::onStop()"); }

void ParkThread::run()
{
    AUTO_TRACE("ParkThread::run()");
    vector<string> pass_check; 

    // Put the servos in the position elevation independent and disable the axes 
    unsigned long counter = 0;
    vector<string> toMove(m_configuration->m_servosToMove);

    // Positioning
    counter = 0;
    while(true) {
        unsigned short num_of_on_target = 0;
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
                            ACS::doubleSeq target_pos = m_configuration->getPosition(comp_name, getTimeStamp());

                            if(act_pos->length() != target_pos.length()) {
                                ACS_SHORT_LOG((LM_ERROR, ("ParkThread: lenghts of target and act pos do not match")));
                                m_configuration->m_isParking = false;
                                if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                                    pass_check.push_back(comp_name);
                                    ++num_of_on_target;
                                }
                                continue; // Try with the other servos
                            }
                            bool on_target = true;
                            // Compute the difference between actual and target positions
                            for(size_t i=0; i<target_pos.length(); i++) { 
                                if(fabs(target_pos[i] - (*act_pos)[i]) > component_ref->getTrackingDelta())
                                    on_target = false;
                            }
                            if(on_target) {
                                ++num_of_on_target;
                                pass_check.push_back(comp_name);
                                continue;
                            }
                            if(component_ref->isReady()) {
                                if(std::find(m_positioning.begin(), m_positioning.end(), comp_name) != m_positioning.end()) {
                                    ACS_SHORT_LOG((LM_INFO, ("ParkThread: the " + comp_name + " is moving.").c_str()));
                                    continue; // The servo is moving
                                }
                                else {
                                    component_ref->setPosition(target_pos, 0);
                                    m_positioning.push_back(comp_name);
                                    m_configuration->m_isConfigured = false;
                                    continue;
                                }
                            }
                        }
                    }
                }
                catch(...) {
                    ACS_SHORT_LOG((LM_WARNING, ("ParkThread: error positioning "  + comp_name).c_str()));
                    if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                        pass_check.push_back(comp_name);
                        ++num_of_on_target;
                    }
                    continue;
                }
            }
            else {
                ACS_SHORT_LOG((LM_WARNING, ("ParkThread::run(): cannot get the component reference.")));
                if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                    pass_check.push_back(comp_name);
                    ++num_of_on_target;
                }
                continue;
            }
        }
        if(toMove.size() == num_of_on_target)
            break;

        ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
        sleep_ret = ACS::ThreadBase::sleep(PARK_ITER_SLEEP_TIME); // 2 seconds
        counter += PARK_ITER_SLEEP_TIME / 2;
        if(sleep_ret != SLEEP_OK || counter > PARK_MAX_ACTION_TIME) {
            ACS_SHORT_LOG((LM_WARNING, ("ParkThread::run(): PARK_MAX_ACTION_TIME reached.")));
            m_configuration->m_isParking = false;
            return;
        }
    }
    
    // Wait till the position is stable 
    ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
    sleep_ret = ACS::ThreadBase::sleep(PARK_ITER_SLEEP_TIME); // 2 seconds
    if(sleep_ret != SLEEP_OK || counter > PARK_MAX_ACTION_TIME) {
        m_configuration->m_isParking = false;
        return;
    }

    for(vector<string>::iterator iter = toMove.begin(); iter != toMove.end(); iter++) {
        string comp_name = *iter;
        MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
        if((m_configuration->m_component_refs).count(comp_name)) {
            component_ref = (m_configuration->m_component_refs)[comp_name];
            // Disabling the minor servos
            try {
                if(!CORBA::is_nil(component_ref)) {
                   if(component_ref->isReady()) {
                       component_ref->disable(0);
                       m_configuration->m_isConfigured = false;
                   }
                }
            }
            catch(...) {
                ACS_SHORT_LOG((LM_ERROR, ("ParkThread: cannot disable "  + comp_name).c_str()));
            }
        }
        else {
            ACS_SHORT_LOG((LM_ERROR, ("ParkThread: cannot disable "  + comp_name).c_str()));
        }
    }

    /* Park the components
    else { 
        // Park the components
        unsigned long counter = 0;
        vector<string> toPark;
        toPark.push_back(string("SRP"));
        toPark.push_back(string("PFP"));
        toPark.push_back(string("GFR"));
        toPark.push_back(string("M3R"));
        while(true) {
            unsigned short num_of_parked_comp = 0;
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
                                ACS_SHORT_LOG((LM_INFO, (string("ParkThread: the ") + comp_name + string(" is parking.")).c_str()));
                                continue;
                            }
                            if(component_ref->isStarting()) {
                                ACS_SHORT_LOG((LM_INFO, (string("ParkThread: the ") + comp_name + string(" is starting.")).c_str()));
                                continue;
                            }
                            else if(component_ref->isParked()) {
                                ++num_of_parked_comp;
                                pass_check.push_back(comp_name);
                                ACS_SHORT_LOG((LM_INFO, (string("ParkThread: the ") + comp_name + string(" is parked.")).c_str()));
                                continue;
                            }
                            else if(component_ref->isReadyToSetup()) {
                                component_ref->setup(0);
                                continue;
                            }
                            else if(component_ref->isDisabledFromOtherDC()) {
                                continue;
                            }
                            else if(status_bset.test(STATUS_FAILURE)) {
                                ACS_SHORT_LOG((LM_ERROR, ("ParkThread: failure of "  + comp_name).c_str()));
                                if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                                    pass_check.push_back(comp_name);
                                    ++num_of_parked_comp;
                                }
                                continue;
                            }
                            else if(component_ref->isInEmergencyStop()){
                                ACS_SHORT_LOG((LM_ERROR, ("ParkThread: emergency stop of "  + comp_name).c_str()));
                                if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                                    pass_check.push_back(comp_name);
                                    ++num_of_parked_comp;
                                }
                                continue;
                            }
                            else if(!component_ref->isReady()) {
                                ACS_SHORT_LOG((LM_ERROR, ("ParkThread: not ready "  + comp_name).c_str()));
                                if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                                    pass_check.push_back(comp_name);
                                    ++num_of_parked_comp;
                                }
                                continue;
                            }
                            else {
                                component_ref->stow(0);
                                m_configuration->m_isConfigured = false;
                                continue;
                            }
                        }
                        else {
                            ACS_SHORT_LOG((LM_ERROR, ("ParkThread: error parking "  + comp_name).c_str()));
                            if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                                pass_check.push_back(comp_name);
                                ++num_of_parked_comp;
                            }
                            continue;
                        }
                    }
                    catch(...) {
                        ACS_SHORT_LOG((LM_ERROR, ("ParkThread: error parking "  + comp_name).c_str()));
                        if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                            pass_check.push_back(comp_name);
                            ++num_of_parked_comp;
                        }
                        continue;
                    }
                }
                else {
                    ACS_SHORT_LOG((LM_ERROR, ("ParkThread::run(): error retrieving " + comp_name).c_str()));
                    if(find(pass_check.begin(), pass_check.end(), comp_name) == pass_check.end()) {
                        pass_check.push_back(comp_name);
                        ++num_of_parked_comp;
                    }
                }
            }
            if(toPark.size() == num_of_parked_comp)
                break;

            // Verificare i tempi complessivi
            ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
            sleep_ret = ACS::ThreadBase::sleep(PARK_ITER_SLEEP_TIME); // 2 seconds
            counter += PARK_ITER_SLEEP_TIME / 2;
            if(sleep_ret != SLEEP_OK || counter > MAX_PARK_TIME) {
                ACS_SHORT_LOG((LM_WARNING, ("SetupThread::run(): MAX_PARK_TIME reached.")));
                m_configuration->m_isParking = false;
                return;
            }
        }
    } 
    */

    // Park DONE
    m_configuration->m_isParking = false;

    ACS_SHORT_LOG((LM_INFO, ("ParkThread::run(): setup done.")));
    return;
}

