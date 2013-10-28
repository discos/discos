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
    pos.length(((m_configuration->m_scan).centralPos).length());
    // The first position is the actual one for all the axes except for the one to scan, whose is (act_pos - range/2)
    bool reverse = false;
    // Distance between the actual position and the left point of the scan
    double dist_from_left = fabs((m_configuration->m_scan).actPos[axis] - ((m_configuration->m_scan).centralPos[axis] - range/2));
    // Distance between the actual position and the right point of the scan
    double dist_from_right = fabs((m_configuration->m_scan).actPos[axis] - ((m_configuration->m_scan).centralPos[axis] + range/2));
    // Distance between the actual position and the central point of the scan
    double dist_from_central = fabs((m_configuration->m_scan).actPos[axis] - (m_configuration->m_scan).centralPos[axis] );
    if(dist_from_central > delta && dist_from_right < dist_from_left) {
        reverse = true;
    }

    for(size_t i=0; i<pos.length(); i++) {
        if(i==axis) {
            pos[i] = reverse ? 
                     (m_configuration->m_scan).plainCentralPos[i] + range/2 : 
                     (m_configuration->m_scan).plainCentralPos[i] - range/2;      
        }
        else {
            pos[i] = (m_configuration->m_scan).plainCentralPos[i];                
        }
    }
    // Update the positions of the axes not involved in the scan
    if((m_configuration->m_scan).wasElevationTrackingEn)
        updatePos(comp_name, pos, (m_configuration->m_scan).starting_time, axis);

    size_t idx = 0;
    try {
        component_ref->setPosition(pos, 0); // Go to the starting position
        double starting_pos = pos[axis]; // Axis value
        // Wait until the (starting_time - 0.5 seconds), in order to have the future antenna elevations
        TIMEVALUE now(0.0L);
        IRA::CIRATools::getTime(now);
        while(now.value().value > (stime.value().value - 5000000)) {
            ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
            sleep_ret = ACS::ThreadBase::sleep(10000); // Wait 1 ms
            if(sleep_ret != SLEEP_OK) {
                m_configuration->m_isScanning = false;
                return;
            }
            IRA::CIRATools::getTime(now);
        }

        for(size_t i=0; i<N; i++) {
            pos[axis] = reverse ? starting_pos - delta * i : starting_pos + delta * i;
            ACS::Time exe_time = stime.value().value + dtime.value().value * i;
            // Update the positions of the axes not involved in the scan
            if((m_configuration->m_scan).wasElevationTrackingEn)
                updatePos(comp_name, pos, exe_time, axis);
            positions.push_back(pos);
            exe_times.push_back(exe_time);
        }
        pos[axis] = reverse ? starting_pos - range : starting_pos + range;
        positions.push_back(pos);
        exe_times.push_back(stime.value().value + ttime.value().value);
        if(positions.size() != exe_times.size()) {
            m_configuration->m_isScanning = false;
            ACS_SHORT_LOG((LM_ERROR, ("ScanThread::run(): unexpected error: positions.size() != exe_times.size().")));
            return;
        }

        if(positions.size() <= EQUIVALENT_BUFF_SIZE) { // Send all the positions
            for(; idx<positions.size(); idx++) {
                ACS::doubleSeq pos = positions[idx];
                ACS::Time exe_time = exe_times[idx];
                // Update the positions of the axes not involved in the scan
                if((m_configuration->m_scan).wasElevationTrackingEn)
                    updatePos(comp_name, pos, exe_time, axis);
                component_ref->setPosition(pos, exe_time);
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(10000); // Wait 1 ms
                if(sleep_ret != SLEEP_OK) {
                    m_configuration->m_isScanning = false;
                    return;
                }
            }
        }
        else { // Send (EQUIVALENT_BUFF_SIZE / 4) positions only
            for(; idx<(EQUIVALENT_BUFF_SIZE / 4); idx++) {
                ACS::doubleSeq pos = positions[idx];
                ACS::Time exe_time = exe_times[idx];
                // Update the positions of the axes not involved in the scan
                if((m_configuration->m_scan).wasElevationTrackingEn)
                    updatePos(comp_name, pos, exe_time, axis);
                component_ref->setPosition(pos, exe_time);
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(1000); // Wait 100 us
                if(sleep_ret != SLEEP_OK) {
                    m_configuration->m_isScanning = false;
                    return;
                }
            }
            // Wait until the starting time
            IRA::CIRATools::getTime(now);
            while(now.value().value < stime.value().value) {
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(10000); // Wait 1 ms
                if(sleep_ret != SLEEP_OK) {
                    m_configuration->m_isScanning = false;
                    return;
                }
                IRA::CIRATools::getTime(now);
            }

            while(idx < positions.size()) {
                size_t consumed = (size_t)((getTimeStamp() - (m_configuration->m_scan).starting_time) / dtime.value().value);
                size_t freeEquivalentPoints = (idx + 1 - consumed);
                if(freeEquivalentPoints < EQUIVALENT_BUFF_SIZE / 4) {
                    size_t remainingPoints = positions.size() - (idx + 1);
                    if(remainingPoints <= freeEquivalentPoints) {
                        // Send all the remaining points
                        for(; idx<positions.size(); idx++) {
                            ACS::doubleSeq pos = positions[idx];
                            ACS::Time exe_time = exe_times[idx];
                            // Update the positions of the axes not involved in the scan
                            if((m_configuration->m_scan).wasElevationTrackingEn)
                                updatePos(comp_name, pos, exe_time, axis);
                            component_ref->setPosition(pos, exe_time);
                        }
                    }
                    else {
                        for(size_t j=0; j<freeEquivalentPoints; j++, idx++) {
                            ACS::doubleSeq pos = positions[idx];
                            ACS::Time exe_time = exe_times[idx];
                            // Update the positions of the axes not involved in the scan
                            if((m_configuration->m_scan).wasElevationTrackingEn)
                                updatePos(comp_name, pos, exe_time, axis);
                            component_ref->setPosition(pos, exe_time);
                        }
                    }
                }
                ACS::ThreadBase::SleepReturn sleep_ret = SLEEP_ERROR;
                sleep_ret = ACS::ThreadBase::sleep(1000); // Wait 100 us
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
    return;
}

void ScanThread::updatePos(string comp_name, ACS::doubleSeq & pos, ACS::Time exe_time, size_t exclude) {
    ACS::doubleSeq plainTargetPos = m_configuration->getPosition(comp_name, exe_time);
    if(pos.length() != plainTargetPos.length()) {
        m_configuration->m_isScanning = false;
        ACS_SHORT_LOG((LM_ERROR, ("ScanThread::updatePos(): unexpected error: plainTargetPos.length()() != pos.length().")));
        return;
    }
    for(size_t k=0; k<pos.length(); k++) {
        if(k != exclude) // Exclude the axis involved in the scan
            pos[k] = plainTargetPos[k];
    }
}


