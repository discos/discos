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
#include <MSBossConfiguration.h> // TODO: remove

#define MAX_DTIME 10000000 // 1 second

MSBossPublisher::MSBossPublisher(
        const ACE_CString& name, 
        MSBossConfiguration * configuration,
        const ACS::TimeInterval& responseTime,
        const ACS::TimeInterval& sleepTime,
        const bool del
        ) : ACS::Thread(name, responseTime, sleepTime, del), m_configuration(configuration), m_last_event(0.0L)
{
    AUTO_TRACE("MSBossPublisher::MSBossPublisher()");
}

MSBossPublisher::~MSBossPublisher() { AUTO_TRACE("MSBossPublisher::~MSBossPublisher()"); }

void MSBossPublisher::onStart() { 
    AUTO_TRACE("MSBossPublisher::onStart()"); 
}

void MSBossPublisher::onStop() { AUTO_TRACE("MSBossPublisher::onStop()"); }

void MSBossPublisher::runLoop()
{
    AUTO_TRACE("MSBossPublisher::runLoop()");

    bitset<VSTATUS_LENGTH> vstatus_bset(0);
    try {
        if(m_configuration->isStarting()) {
            vstatus_bset.set(VS_CONFIGURING);
        }
        else if(m_configuration->isParking()) {
            vstatus_bset.set(VS_PARKING);
        }
        else {
            vstatus_bset.set(VS_OK);
            vstatus_bset.set(VS_TRACKING);
            vstatus_bset.reset(VS_PARKED);
            vstatus_bset.reset(VS_WARNING);
            vstatus_bset.reset(VS_FAILURE);

            vector<string> toMove = m_configuration->getServosToMove();
            for(size_t i=0; i<toMove.size(); i++) {
                string comp_name(toMove[i]);
                // Set the name of component to move from a string
                MinorServo::WPServo_var component_ref = MinorServo::WPServo::_nil();

                if((m_configuration->m_component_refs).count(comp_name)) {
                    component_ref = (m_configuration->m_component_refs)[comp_name];
                    if(CORBA::is_nil(component_ref)) {
                        vstatus_bset.reset(VS_OK);
                        vstatus_bset.reset(VS_TRACKING);
                        vstatus_bset.reset(VS_PARKED);
                        vstatus_bset.set(VS_WARNING);
                        vstatus_bset.set(VS_FAILURE);
                        ACS_SHORT_LOG((LM_ERROR, "Error in MSBossPublisher: cannot get the components."));
                    }
                    else {
                        // OK
                        if(!m_configuration->isConfigured())
                            vstatus_bset.reset(VS_OK);
                        // TRACKING
                        if(!m_configuration->isConfigured() || 
                           !component_ref->isTracking() ||
                           (m_configuration->isScanActive() && !m_configuration->isScanning())) {
                                vstatus_bset.reset(VS_TRACKING);
                        }
                        // PARKED
                        if(!component_ref->isParked())
                            vstatus_bset.reset(VS_PARKED);
                        
                        // FAILURE and WARNING
                        // Get one of the component's BACI properties
                        ACS::ROpattern_var refStatus = component_ref->status();
                        if(refStatus.ptr() != ACS::ROpattern::_nil()) {
                            ACSErr::Completion_var completion;
                            //Just synchronously reading the value of refStatus
                            CORBA::Long status_val = refStatus->get_sync(completion.out());
                            bitset<STATUS_WIDTH> status_bset(status_val);
                            // string position = items.back(); // TODO: get the position?
                            // strip(position, coeffs_separator);
                            // If the minor servo should be parked but it doesn't
                            // if(status_bset.test(STATUS_WARNING) || (position == "park" && !component_ref->isParked()))
                            //     vstatus_bset.set(VS_WARNING);
                            // // If the minor servo is in failure and it is not parked 
                            if(status_bset.test(STATUS_FAILURE)) {
                                vstatus_bset.set(VS_FAILURE);
                            }
                        }

                        // If the tracking thread set the status as FAILURE
                        if(m_configuration->m_status == Management::MNG_FAILURE){
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

                // if(m_configuration->isStarting() || m_configuration->isParking())
                //     break;
            }
        }
        m_configuration->m_isTracking = vstatus_bset.test(VS_TRACKING) && (!vstatus_bset.test(VS_FAILURE));

        /* Begin Notification Channel Logic */
        static MinorServo::MinorServoDataBlock data = {0, false, false, false, false, Management::MNG_OK};
        TIMEVALUE now(0.0L);
        IRA::CIRATools::getTime(now);

        if(m_configuration->isStarting() || m_configuration->isParking() || vstatus_bset.test(VS_WARNING))
            ; // *m_configuration->status = Management::MNG_WARNING; // TODO: update the Boss status
        else if (vstatus_bset.test(VS_FAILURE))
            m_configuration->m_status = Management::MNG_FAILURE;
        else
            ; // *m_configuration->status = Management::MNG_OK; // TODO: update the Boss status


        Management::TSystemStatus current_status;
        if(vstatus_bset.test(VS_FAILURE)) {
            current_status = Management::MNG_FAILURE;
        }
        else if(vstatus_bset.test(VS_WARNING)) {
            current_status = Management::MNG_WARNING;
        }
        else {
            current_status = Management::MNG_OK;
        }

        bool publish_data = (CIRATools::timeDifference(m_last_event, now) >= MAX_DTIME) ? true : false;
        if(!publish_data) {
            publish_data = 
                data.status != current_status ||
                data.tracking != vstatus_bset.test(VS_TRACKING) || 
                data.parking != vstatus_bset.test(VS_PARKING) || 
                data.starting != vstatus_bset.test(VS_CONFIGURING) || 
                data.parked != vstatus_bset.test(VS_PARKED) 
                ? true : publish_data;
        }

        if(publish_data) {
            data.status = current_status;
            // data.tracking = vstatus_bset.test(VS_TRACKING) && (!vstatus_bset.test(VS_FAILURE));
            data.tracking = vstatus_bset.test(VS_TRACKING);
            data.parked = vstatus_bset.test(VS_PARKED); 
            data.parking = vstatus_bset.test(VS_PARKING); 
            data.starting = vstatus_bset.test(VS_CONFIGURING); 
            data.timeMark = (now.value()).value;
            try {
                if(m_configuration->m_nchannel != NULL)
                    (m_configuration->m_nchannel)->publishData<MinorServo::MinorServoDataBlock>(data);
                else
                    ACS_SHORT_LOG((LM_ERROR, "Error: cannot get the notification channel reference."));
            }
            catch (ComponentErrors::CORBAProblemEx& ex) {
                _ADD_BACKTRACE(ComponentErrors::NotificationChannelErrorExImpl, impl, ex, "MSBossPublisher()");
                throw impl;
            }
            IRA::CIRATools::timeCopy(m_last_event, now);
        }
        /* End Notification Channel Logic */
    }
    catch(...) {
        ACS_SHORT_LOG((LM_WARNING, "An error is occurred in MSBossPublisher"));
    }
}

