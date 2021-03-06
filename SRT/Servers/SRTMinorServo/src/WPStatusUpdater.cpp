/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include "WPStatusUpdater.h"
#include "libCom.h"
#include <pthread.h>
#include <time.h>
#include <bitset>
#include "MSParameters.h"

#define COUNTER_LIMIT 10000

WPStatusUpdater::WPStatusUpdater(
        const ACE_CString& name, 
        ThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime
        ) : ACS::Thread(name, responseTime, sleepTime), m_params(&params)
{
    AUTO_TRACE("WPStatusUpdater::WPStatusUpdater()");
    m_counter = 0;
    for(size_t i=0; i<NUMBER_OF_SERVOS; i++)
        m_clean_counter.push_back(false);
}

WPStatusUpdater::~WPStatusUpdater() { 
    AUTO_TRACE("WPStatusUpdater::~WPStatusUpdater()"); 
}

void WPStatusUpdater::onStart() { AUTO_TRACE("WPStatusUpdater::onStart()"); }

void WPStatusUpdater::onStop() { AUTO_TRACE("WPStatusUpdater::onStop()"); }

void WPStatusUpdater::runLoop()
{
    // unsigned long status;
    ACS::Time timestamp;
    bool is_tracking = false;
    unsigned long address=0;
    // A smarter policy than round robin should be to iterate only on active servos.
    try {
        pthread_mutex_lock(m_params->status_mutex); 
        if(!(m_params->map_of_talkers_ptr)->empty()) {
            // The servo address
            address = m_counter % NUMBER_OF_SERVOS;

            if((m_params->map_of_talkers_ptr)->count(address) && (*m_params->status_thread_en).count(address)
                    && (*m_params->status_thread_en)[address]) 
            {
                /* TODO: REMOVE the following lines ---------*/
                // TIMEVALUE now(0.0L);
                // IRA::CIRATools::getTime(now);
                // IRA::CString outString;
                // IRA::CIRATools::timeToStr(now.value().value, outString);
                // cout << "Begin cicle: " << outString << endl;
                /* TODO: REMOVE the lines above -------------*/

                StatusParameters status_par;
                ((*m_params->map_of_talkers_ptr)[address])->getStatus(status_par, timestamp);
                bitset<STATUS_WIDTH> status_bset((m_params->expire_time)->status[address]);

                // The position retrieved by getStatus is alredy converted to virtual
                ACS::doubleSeq act_pos = (status_par.actual_pos).position;
                ACS::Time act_pos_time = (status_par.actual_pos).exe_time;
                ACS::Time diff = abs_diff(act_pos_time, getTimeStamp());
                PositionItem act_pos_item;
                act_pos_item.position = act_pos;
                act_pos_item.exe_time = act_pos_time;
                (act_pos_item.offsets).user = ((status_par.actual_pos).offsets).user;
                (act_pos_item.offsets).system = ((status_par.actual_pos).offsets).system;

                // Actual position list (the history)
                CSecAreaResourceWrapper<map<int, vector< PositionItem> > > apl_secure_requests = (m_params->act_pos_list)->Get();
                if((*apl_secure_requests).count(address)) {
                    long diff = ((*apl_secure_requests)[address]).size() - MAX_HISTORY_SIZE;
                    if(diff > 0) {
                        vector<PositionItem>::iterator ibegin = ((*apl_secure_requests)[address]).begin();
                        ((*apl_secure_requests)[address]).erase(ibegin, ibegin + diff);
                    }

                    if(((*apl_secure_requests)[address]).size() > MAX_HISTORY_SIZE) {
                        ACS_SHORT_LOG((LM_ERROR, "In WPStatusUpdater: reached the MAX_HISTORY_SIZE"));
                    }
                    else {
                        ((*apl_secure_requests)[address]).push_back(act_pos_item);
                    }
                }
                else {
                    ((*apl_secure_requests)[address]).insert(
                        ((*apl_secure_requests)[address]).begin(), act_pos_item);
                }

                apl_secure_requests.Release();

                // Update the actual elongation (it is just the actual position for the GFR, PFP and M3R)
                ACS::doubleSeq act_elongation = (status_par.actual_elongation).position;
                for(size_t i=0; i != act_elongation.length(); i++) {
                    ((m_params->expire_time)->actElongation[address])[i] = act_elongation[i]; 
                }

                /**
                 * The value of application state (appstate) reports the machine state condition of the application.
                 * The possible values are:
                 *
                 *   0. Application Initialization
                 *   1. Application Startup
                 *   2. Application Local
                 *   3. Application Remote Manual
                 *   4. Application Remote Auto 
                 */
                (m_params->expire_time)->appState[address] = status_par.appState;

                /** 
                 *  The appStatus reports the status of Minor Servo application in the MSCU. 
                 *  The returned long value come from a 16 bits field with the following meaning:
                 *
                 *  @arg \c 0 Link: 1 means there is a link
                 *  @arg \c 1 Init: 1 means the system is initialized
                 *  @arg \c 2 Remote: 1 means the remote control is enabled
                 *  @arg \c 3 Calibrated: 1 means the system is calibrated
                 *  @arg \c 4 Ready: 1 means the slaves are ready
                 *  @arg \c 5 Automatic: 1 means the slaves mode is automatic
                 *  @arg \c 6 Dummy: Dummy value 1
                 *  @arg \c 7 Dummy: Dummy value 1
                 *  @arg \c 8 Power: 1 means the minor servo is power on
                 *  @arg \c 9 Enable: 1 means the minor servo is enable to moving
                 *  @arg \c 10 Fault: 1 means there is not a fault
                 *  @arg \c 11 Message: 1 means there is not a message
                 *  @arg \c 12 Trip: 1 means there is not a trip
                 *  @arg \c 13 Warning: 1 means there is not a warning
                 *  @arg \c 14 Quick Stop: 1 means there is not a quick stop enabled
                 *  @arg \c 15 On target: 1 means the minor servo is on target
                 */
                (m_params->expire_time)->appStatus[address] = status_par.appStatus;

                /** 
                 *  The cabState reports the machine state condition of the drive cabinet.
                 *  The possible values are:
                 *  
                 *    0. Drive Cabinet Ok
                 *    1. Drive Cabinet Startup
                 *    2. Drive Cabinet Block Removed
                 *    3. Drive Cabinet Disabled from AIF-IN
                 *    4. Drive Cabinet Supply fail
                 *    5. Drive Cabinet disabled from other cabinet
                 *    6. Drive Cabinet emergency stop or emergency limit removed
                 *    7. Drive Cabinet block condition active
                 */
                (m_params->expire_time)->cabState[address] = status_par.cabState;

                bitset<32> app_status_bset(status_par.appStatus);

                // Set the READY bit of the status pattern
                if(status_par.appState == APP_REMOTE_AUTO && status_par.cabState == CAB_OK && !((*m_params->stow_state)[address]))
                    status_bset.set(STATUS_READY);
                else
                    status_bset.reset(STATUS_READY);

                // Set the SETUP bit of the status pattern
                if(status_par.cabState == CAB_STARTUP)
                    status_bset.set(STATUS_SETUP);
                else
                    status_bset.reset(STATUS_SETUP);
                
                // Set the PARKED bit of the status pattern
                if(status_par.cabState == CAB_DISABLED_FROM_OTHER_CAB) { 
                        status_bset.set(STATUS_PARKED);
                    }
                else 
                    if(status_par.cabState == CAB_DISABLED_FROM_AIF_IN || status_par.cabState == CAB_BLOCK_REMOVED) { 
                            // Compute the difference of actual pos and park pos
                            for(size_t i=0; i<((*m_params->park_positions)[address]).size(); i++) { 
                                if(!(fabs(((*m_params->park_positions)[address])[i] - act_pos[i]) < (m_params->tracking_delta)[address])) {
                                    status_bset.reset(STATUS_PARKED);
                                    break;
                                }
                                status_bset.set(STATUS_PARKED);
                            }
                        }
                    else
                        status_bset.reset(STATUS_PARKED);
                
                // Set the WARNING bit of the status pattern
                if(!app_status_bset.test(ASTATUS_WARNING) || !app_status_bset.test(ASTATUS_MESSAGE))
                    status_bset.set(STATUS_WARNING);
                else
                    status_bset.reset(STATUS_WARNING);
                
                // Set the FAILURE bit of the status pattern
                if(
                        !app_status_bset.test(ASTATUS_FAULT) || 
                        !app_status_bset.test(ASTATUS_TRIP)  ||
                        status_par.cabState == CAB_SUPPLY_FAIL   ||
                        status_par.cabState == CAB_BLOCK_ACTIVE
                )
                    status_bset.set(STATUS_FAILURE);
                else
                    status_bset.reset(STATUS_FAILURE);

                if((*m_params->positioning_error)[address]) {
                    status_bset.set(STATUS_FAILURE);
                }
                else {
                    status_bset.reset(STATUS_FAILURE);
                }

                if(diff > MAX_TIME_DIFF) {
                    ACS_SHORT_LOG((
                        LM_WARNING, 
                        "In WPStatusUpdater: abs(actual_time - actual_pos_time) = %llu",
                        static_cast<unsigned long long>(diff)
                    ));
                }
                else {
                    CSecAreaResourceWrapper<map<int, vector< PositionItem> > > lst_secure_requests = (m_params->cmd_pos_list)->Get();
                    if((*lst_secure_requests).count(address)) {
                        try {
                            vector<PositionItem>::size_type idx = findPositionIndex(lst_secure_requests, act_pos_time, address, true);
                            // Updating of the commanded position property
                            // The commanded position is updated adding to the position in the list of PositionItems
                            // The system offset is unknown by the user
                            (m_params->expire_time)->cmdPos[address] = (((*lst_secure_requests)[address])[idx]).position;

                            if(act_pos.length() != ((m_params->expire_time)->cmdPos[address]).length()) {
                                ACS_SHORT_LOG((
                                    LM_ERROR,
                                    "@%llu: Wrong number of axes in the actual position.",
                                    static_cast<unsigned long long>(getTimeStamp())
                                ));
                            } 
                            else { 
                                // Updating of actual position property
                                for(size_t i=0; i != act_pos.length(); i++) {
                                        ((m_params->expire_time)->virtualActElongation[address])[i] = act_pos[i];
                                        ((m_params->expire_time)->actPos[address])[i] = \
                                            act_pos[i] - ((((*lst_secure_requests)[address])[idx]).offsets).system[i];
                                        ((m_params->expire_time)->plainActPos[address])[i] =
                                            act_pos[i] - ((((*lst_secure_requests)[address])[idx]).offsets).system[i] - \
                                                         ((((*lst_secure_requests)[address])[idx]).offsets).user[i];
                                }

                                for(unsigned int i = 0; i < act_pos.length(); i++)
                                    ((m_params->expire_time)->posDiff[address])[i] = \
                                        ((m_params->expire_time)->cmdPos[address])[i] - \
                                        ((m_params->expire_time)->actPos[address])[i] + \
                                        (((((*lst_secure_requests)[address])[idx]).offsets).user)[i];

                                is_tracking = true;
                                for(unsigned int i = 0; i < act_pos.length(); i++)
                                    if(fabs(((m_params->expire_time)->posDiff[address])[i]) > (m_params->tracking_delta)[address]) {
                                        is_tracking = false;
                                        break;
                                    }
                            }
                            m_clean_counter[address] = false;
                        }
                        catch(PosNotFoundEx) {
                            if(m_clean_counter[address]) {
                                ACS_SHORT_LOG((
                                    LM_WARNING,
                                    "In WPStatusUpdater: cmd position at @%llu not found!",
                                    static_cast<unsigned long long>(act_pos_time)
                                ));
                                m_clean_counter[address] = false;
                            }
                            else
                                m_clean_counter[address] = true;
                        }
                    }
                    lst_secure_requests.Release();
                }
                // Set the TRACKING bit of status pattern
                if(is_tracking)
                    status_bset.set(STATUS_TRACKING);
                else
                    status_bset.reset(STATUS_TRACKING);
                
                // Update the status pattern
                (m_params->expire_time)->status[address] = status_bset.to_ulong();

                /* TODO: REMOVE the following lines ---------*/
                // IRA::CIRATools::getTime(now);
                // IRA::CIRATools::timeToStr(now.value().value, outString);
                // cout << "End cicle: " << outString << endl;
                /* TODO: REMOVE the lines above -------------*/
            }  
        }
        pthread_mutex_unlock(m_params->status_mutex); 
        m_counter = m_counter == COUNTER_LIMIT ? 0 : ++m_counter;
    }
    catch (const std::exception &e) {
        pthread_mutex_unlock(m_params->status_mutex); 
        ACS_SHORT_LOG((
            LM_ERROR,
            "@%llu: unexpected error updating the status.",
            static_cast<unsigned long long>(timestamp)
        ));
        ACS_SHORT_LOG((LM_ERROR, e.what()));
    }
    catch (...) {
        pthread_mutex_unlock(m_params->status_mutex); 
        ACS_SHORT_LOG((
            LM_ERROR,
            "@%llu: unexpected error updating the status.",
            static_cast<unsigned long long>(timestamp)
        ));
    }
}
 
