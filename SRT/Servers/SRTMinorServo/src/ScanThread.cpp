/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "ScanThread.h"
#include <pthread.h>
#include <time.h>
#include "MinorServoBossImpl.h"
#include <MSBossConfiguration.h> // TODO: remove

ScanThread::ScanThread(
        const ACE_CString& name, 
        MSThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_params(&params)
{
    AUTO_TRACE("ScanThread::ScanThread()");
}

ScanThread::~ScanThread() { AUTO_TRACE("ScanThread::~ScanThread()"); }

void ScanThread::onStart() { AUTO_TRACE("ScanThread::onStart()"); }

void ScanThread::onStop() { AUTO_TRACE("ScanThread::onStop()"); }

void ScanThread::runLoop()
{

    ACS_SHORT_LOG((LM_ERROR, "ScanThread: runLoop()"));
    TIMEVALUE now(0.0L);
    IRA::CIRATools::getTime(now);
    TIMEVALUE stime(m_params->starting_scan_time);
    ACS::Time sleep_time = CIRATools::timeSubtract(stime, now) * 10 > SCAN_SHIFT_TIME ? \
                           CIRATools::timeSubtract(stime, now) * 10 - SCAN_SHIFT_TIME: 0;
    ACS_SHORT_LOG((LM_WARNING, "ScanThread, startingTime %llu", stime.value().value));
    ACS_SHORT_LOG((LM_WARNING, "ScanThread, sleep_time %llu", sleep_time));
    ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
    if(sleep_time > 0) {
        ACS_SHORT_LOG((LM_WARNING, "ScanThread: sleep_time > 0"));
        sleep_ret = ACS::ThreadBase::sleep(sleep_time);
        if(sleep_ret != SLEEP_OK) {
            ACS_SHORT_LOG((LM_WARNING, "ScanThread: sleep_ret != SLEEP_OK"));
            pthread_mutex_unlock(m_params->setup_mutex); 
            *m_params->is_scanning_ptr = false;
            return;
        }
    }
    ACS_SHORT_LOG((LM_WARNING, "ScanThread: after sleep"));

    try {
        for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
            // Split the action in items.
            vector<string> items = split(*iter, items_separator);
            // Set the name of component to move from a string
            string comp_name = get_component_name(items.front());
            if(comp_name != (m_params->scan_data).comp_name)
                continue;
            MinorServo::WPServo_var component_ref;
            if((*m_params->component_refs).count(comp_name)) {
                component_ref = (*m_params->component_refs)[comp_name];
                if(!CORBA::is_nil(component_ref)) {
                    // Set a doubleSeq from a string of positions
                    ACS::doubleSeq positions = get_positions(comp_name, items.back(), m_params);
                    while(true) {
                        if(positions.length() && !CORBA::is_nil(component_ref) && *m_params->actual_conf == m_params->commanded_conf) {
                            if(!(*((m_params->scan_data).scan_pos)).empty()) {
                                if((m_params->scan_data).axis_id < positions.length()) {
                                    TIMEVALUE next_point(((*((m_params->scan_data).scan_pos)).front()).time);
                                    IRA::CIRATools::getTime(now);
                                    if(CIRATools::timeSubtract(next_point, now) * 10 < 0) {
                                        (*((m_params->scan_data).scan_pos)).erase((*((m_params->scan_data).scan_pos)).begin());
                                        continue;
                                    }
                                    else if(CIRATools::timeSubtract(next_point, now) * 10 < SCAN_SHIFT_TIME / 10) {
                                        positions[(m_params->scan_data).axis_id] += \
                                            ((*((m_params->scan_data).scan_pos)).front()).delta_pos;
                                        // Test if the position is out of range (out of limits, positive or negative)
                                        component_ref->setPosition(positions, ((*((m_params->scan_data).scan_pos)).front()).time);
                                        (*((m_params->scan_data).scan_pos)).erase((*((m_params->scan_data).scan_pos)).begin());
                                        break;
                                    }
                                }
                                else {
                                    *m_params->is_scanning_ptr = false;
                                    ACS_SHORT_LOG((LM_ERROR, "ScanThread: axis %d does not exist", (m_params->scan_data).axis_id));
                                    pthread_mutex_unlock(m_params->setup_mutex); 
                                    m_params->scan_thread_ptr = NULL;
                                    suspend();
                                    break;
                                }
                            }
                            else {
                                *m_params->is_scanning_ptr = false;
                                pthread_mutex_unlock(m_params->setup_mutex); 
                                ACS_SHORT_LOG((LM_ERROR, "ScanThread: Suspending...."));
                                m_params->scan_thread_ptr = NULL;
                                suspend();
                                break;
                            }
                        }
                    }
                }
                else {
                    ACS_SHORT_LOG((LM_WARNING, ("ScanThread: _nil reference for " + comp_name).c_str()));
                    pthread_mutex_unlock(m_params->setup_mutex); 
                    *m_params->is_scanning_ptr = false;
                }
            }
        }
        pthread_mutex_unlock(m_params->setup_mutex); 
        *m_params->is_scanning_ptr = false;
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "An error is occurred in ScanThread"));
        pthread_mutex_unlock(m_params->setup_mutex); 
        *m_params->is_scanning_ptr = false;
    }
}

