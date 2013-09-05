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
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_configuration(configuration)
{
    AUTO_TRACE("ScanThread::ScanThread()");
}

ScanThread::~ScanThread() { AUTO_TRACE("ScanThread::~ScanThread()"); }

void ScanThread::onStart() { AUTO_TRACE("ScanThread::onStart()"); }

void ScanThread::onStop() {AUTO_TRACE("ScanThread::onStop()");}

void ScanThread::run()
{
    ACS_SHORT_LOG((LM_ERROR, "ScanThread::run()"));
    string comp_name = (m_configuration->m_scan).comp_name;
    MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();
    if((m_configuration->m_component_refs).count(comp_name)) {
        component_ref = (m_configuration->m_component_refs)[comp_name];
    }
    else {
        ACS_SHORT_LOG((LM_ERROR, ("ScanThread::run(): Cannot get the component reference.")));
        m_configuration->m_isScanning = false;
        return;
    }

    TIMEVALUE ttime((m_configuration->m_scan).total_time);
    TIMEVALUE stime((m_configuration->m_scan).starting_time);
    TIMEVALUE dtime((m_configuration->m_scan).delta_time);
    TIMEVALUE final_time((m_configuration->m_scan).starting_time + (m_configuration->m_scan).total_time);
    double range = (m_configuration->m_scan).range;
    size_t axis = (m_configuration->m_scan).axis_index;
    ACS::doubleSeq_var act_pos = (m_configuration->m_scan).actPos;

    const ACS::Time MSCU_SAMPLING_TIME = 1000000; // 100ms
    const size_t MSCU_BUFF_SIZE = 100; // 100 points
    const size_t EQUIVALENT_BUFF_SIZE = size_t(MSCU_BUFF_SIZE * MSCU_SAMPLING_TIME / dtime.value().value);
    // ACS::Time real_exe_time = getTimeStamp();
    //
    // Compute the positions
    double N = static_cast<double>((ttime.value()).value) / dtime.value().value;
    double delta = range / N;               
    vector<ACS::doubleSeq> positions;
    vector<ACS::Time> exe_times;
    ACS::doubleSeq pos; // The position to set
    pos.length(act_pos->length());           
    // The first position is the actual one for all the axes except for the one to scan, whose is (act_pos - range/2)
    for(size_t i=0; i<pos.length(); i++) {
        if(i==axis)                             
            pos[i] = act_pos[i] - range/2;      
        else                                    
            pos[i] = act_pos[i];                
    }
    size_t idx = 0;
    try {
        component_ref->setPosition(pos, 0); // Go to the starting position
        double starting_pos = pos[axis]; // Axis value
        for(size_t i=0; i<N; i++) {
            pos[axis] = starting_pos + delta * i;
            ACS::Time exe_time = stime.value().value + dtime.value().value * i;
            positions.push_back(pos);
            exe_times.push_back(exe_time);
        }
        pos[axis] = starting_pos + range;
        positions.push_back(pos);
        exe_times.push_back(stime.value().value + ttime.value().value);
        if(positions.size() != exe_times.size()) {
            m_configuration->m_isScanning = false;
            ACS_SHORT_LOG((LM_ERROR, ("ScanThread::run(): unexpected error: positions.size() != exe_times.size().")));
            return;
        }

        if(positions.size() <= EQUIVALENT_BUFF_SIZE) { // Send all the positions
            for(; idx<positions.size(); idx++) {
                component_ref->setPosition(positions[idx], exe_times[idx]);
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(10000); // Wait 1 ms
                if(sleep_ret != SLEEP_OK) {
                    m_configuration->m_isScanning = false;
                    return;
                }
            }
        }
        else { // Send (EQUIVALENT_BUFF_SIZE / 2) positions only
            for(; idx<(EQUIVALENT_BUFF_SIZE/2); idx++) {
                component_ref->setPosition(positions[idx], exe_times[idx]);
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(10000); // Wait 1 ms
                if(sleep_ret != SLEEP_OK) {
                    m_configuration->m_isScanning = false;
                    return;
                }
            }
            // Wait until the starting time
            TIMEVALUE now(0.0L);
            IRA::CIRATools::getTime(now);
            while(now.value().value < stime.value().value) {
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(100000); // Wait 10 ms
                if(sleep_ret != SLEEP_OK) {
                    m_configuration->m_isScanning = false;
                    return;
                }
                IRA::CIRATools::getTime(now);
            }

            ACS::Time lastTime = getTimeStamp();
            while(idx < positions.size()) {
                size_t freeEquivalentPoints = (size_t)((getTimeStamp() - lastTime) / dtime.value().value);
                if(freeEquivalentPoints > 0) {
                    size_t remainingPoints = positions.size() - (idx + 1);
                    if(remainingPoints <= freeEquivalentPoints) {
                        // Send all the remaining points
                        for(; idx<positions.size(); idx++) {
                            component_ref->setPosition(positions[idx], exe_times[idx]);
                        }
                    }
                    else {
                        for(size_t j=0; j<freeEquivalentPoints; j++, idx++) {
                            component_ref->setPosition(positions[idx], exe_times[idx]);
                        }
                    }
                    lastTime = getTimeStamp();
                }
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(200000); // Wait 20 ms
                if(sleep_ret != SLEEP_OK) {
                    m_configuration->m_isScanning = false;
                    return;
                }
            }
        }
    }
    catch(...) { // Position not allowed
        m_configuration->m_isScanning = false;
        ACS_SHORT_LOG((LM_ERROR, ("ScanThread::run():position not allowed!.")));
        return;
    }

    m_configuration->m_isScanning = false;
    ACS_SHORT_LOG((LM_INFO, ("ScanThread::run(): scanning done.")));
    // A new setup?
    return;
}

