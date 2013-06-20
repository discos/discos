/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include <ManagmentDefinitionsS.h>
#include <ComponentErrors.h>
#include <bitset>
#include "MSBossPublisher.h"
#include <pthread.h>
#include <time.h>
#include "MSParameters.h"
#include "MinorServoBossImpl.h"
#include "utils.h"

#define MAX_DTIME 1000000

MSBossPublisher::MSBossPublisher(
        const ACE_CString& name, 
        MSThreadParameters& params,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_params(&params)
{
    AUTO_TRACE("MSBossPublisher::MSBossPublisher()");
}

MSBossPublisher::~MSBossPublisher() { AUTO_TRACE("MSBossPublisher::~MSBossPublisher()"); }

void MSBossPublisher::onStart() { AUTO_TRACE("MSBossPublisher::onStart()"); }

void MSBossPublisher::onStop() { AUTO_TRACE("MSBossPublisher::onStop()"); }

void MSBossPublisher::runLoop()
{
    AUTO_TRACE("MSBossPublisher::runLoop()");

    bitset<VSTATUS_LENGTH> vstatus_bset(0);
    try {
        if(m_params->is_setup_locked || m_params->is_parking_locked) {
            // CONFIGURING
            if(m_params->is_setup_locked)
                vstatus_bset.set(VS_CONFIGURING);
            // PARKING
            if(m_params->is_parking_locked)
                vstatus_bset.set(VS_PARKING);
        }
        else {
            vstatus_bset.set(VS_OK);
            vstatus_bset.set(VS_TRACKING);
            vstatus_bset.set(VS_PARKED);
            vstatus_bset.reset(VS_WARNING);
            vstatus_bset.reset(VS_FAILURE);
            for(vector<string>::iterator iter = (m_params->actions).begin(); iter != (m_params->actions).end(); iter++) {
                // Split the action in items.
                vector<string> items = split(*iter, items_separator);
                // Set the name of component to move from a string
                string comp_name = get_component_name(items.front());
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();

                if((*m_params->component_refs).count(comp_name)) {
                    component_ref = (*m_params->component_refs)[comp_name];
                    if(CORBA::is_nil(component_ref)) {
                        vstatus_bset.reset(VS_OK);
                        vstatus_bset.reset(VS_TRACKING);
                        vstatus_bset.reset(VS_PARKED);
                        vstatus_bset.set(VS_WARNING);
                        vstatus_bset.set(VS_FAILURE);
                    }
                    else {
                        // OK
                        if(!m_params->is_initialized || !component_ref->isReady())
                            vstatus_bset.reset(VS_OK);
                        // TRACKING
                        if(!m_params->is_initialized || !component_ref->isTracking())
                            vstatus_bset.reset(VS_TRACKING);
                        // PARKED
                        if(!m_params->is_initialized || !component_ref->isParked())
                            vstatus_bset.reset(VS_PARKED);
                        
                        // FAILURE and WARNING
                        // Get one of the component's BACI properties
                        ACS::ROpattern_var refStatus = component_ref->status();
                        if(refStatus.ptr() != ACS::ROpattern::_nil()) {
                            ACSErr::Completion_var completion;
                            //Just synchronously reading the value of refStatus
                            CORBA::Long status_val = refStatus->get_sync(completion.out());
                            bitset<STATUS_WIDTH> status_bset(status_val);
                            string position = items.back();
                            strip(position, coeffs_separator);
                            // If the minor servo should be parked but it doesn't
                            if(status_bset.test(STATUS_WARNING) || (position == "park" && !component_ref->isParked()))
                                vstatus_bset.set(VS_WARNING);
                            // If the minor servo is in failure and it is not parked 
                            if(status_bset.test(STATUS_FAILURE) && position != "park")
                                vstatus_bset.set(VS_FAILURE);
                        }
                    }
                }
                else {
                    vstatus_bset.reset(VS_OK);
                    vstatus_bset.reset(VS_TRACKING);
                    vstatus_bset.reset(VS_PARKED);
                    vstatus_bset.set(VS_WARNING);
                    vstatus_bset.set(VS_FAILURE);
                }

                if(m_params->is_setup_locked || m_params->is_parking_locked)
                    break;
            }
        }

        /* Begin Notification Channel Logic */
        static TIMEVALUE last_event(0.0L);
        static MinorServo::MinorServoDataBlock data = {0, false, false, false, false, Management::MNG_OK};
        TIMEVALUE now(0.0L);
        IRA::CIRATools::getTime(now);

        if(m_params->is_setup_locked || m_params->is_parking_locked || vstatus_bset.test(VS_WARNING))
            *m_params->status = Management::MNG_WARNING;
        else if (vstatus_bset.test(VS_FAILURE))
            *m_params->status = Management::MNG_FAILURE;
        else
            *m_params->status = Management::MNG_OK;

        bool publish_data = (CIRATools::timeDifference(last_event, now) >= MAX_DTIME) ? true : false;
        publish_data = 
            data.status != *m_params->status ||
            data.tracking != vstatus_bset.test(VS_TRACKING) || 
            data.parking != vstatus_bset.test(VS_PARKING) || 
            data.starting != vstatus_bset.test(VS_CONFIGURING) || 
            data.parked != vstatus_bset.test(VS_PARKED) ? true : publish_data;

        if(publish_data) {
            data.status = *m_params->status;
            data.tracking = vstatus_bset.test(VS_TRACKING); 
            data.parked = vstatus_bset.test(VS_PARKED); 
            data.parking = vstatus_bset.test(VS_PARKING); 
            data.starting = vstatus_bset.test(VS_CONFIGURING); 
            data.timeMark = (now.value()).value;
            try {
                (m_params->nchannel)->publishData<MinorServo::MinorServoDataBlock>(data);
            }
            catch (ComponentErrors::CORBAProblemEx& ex) {
                _ADD_BACKTRACE(ComponentErrors::NotificationChannelErrorExImpl, impl, ex, "MSBossPublisher()");
                throw impl;
            }
            IRA::CIRATools::timeCopy(last_event, now);
        }
        /* End Notification Channel Logic */
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "An error is occurred in MSBossPublisher"));
    }
}

