/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __WPSTATUSDEVIO__H
#define __WPSTATUSDEVIO__H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciDevIO.h>
#include <IRA>
#include <ComponentErrors.h>
#include <LogFilter.h>

#define WPSTATUS_STIME_FACTOR 100


using namespace IRA;

template <class T> class WPStatusDevIO : public DevIO<T>
{
public:
    enum TLinkedProperty {
        STATUS,      
        APP_STATE,  
        APP_STATUS,    
        CAB_STATE    
    };


    WPStatusDevIO(
            WPServoTalker *talk, 
            TLinkedProperty Property,
            CDBParameters *cdb_ptr,
            ExpireTime *expire_ptr,
            DDWORD guardInterval=1000000
            ) : m_wpServoTalk(talk), 
                m_Property(Property), 
                m_logGuard(guardInterval),
                m_cdb_ptr(cdb_ptr),
                m_expire_ptr(expire_ptr)
    {
        switch (m_Property) {
            case STATUS : {
                m_PropertyName=CString("Status");
                break;
            }
            case APP_STATE : {
                m_PropertyName=CString("Application State");
                break;
            }
            case APP_STATUS : {
                m_PropertyName=CString("Application Status");
                break;
            }
            case CAB_STATE : {
                m_PropertyName=CString("Drive Cabinet State");
                break;
            }
        }

        CString trace("WPStatusDevIO::WPStatusDevIO() ");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
    }

    ~WPStatusDevIO()
    {
        CString trace("WPStatusDevIO::~WPStatusDevIO() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
    }

    bool initializeValue()
    {       
        CString trace("WPStatusDevIO::initializeValue() ");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
        return false;
    }
    
    /**
     * Used to read the property value.
     * @param timestamp epoch when the operation completes
     * @throw ComponentErrors::PropertyErrorExImpl
    */ 
    T read(ACS::Time &timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("WPStatusDevIO::read() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
        try {
            switch (m_Property) {
                case STATUS : {
                    m_value = (T)m_expire_ptr->status[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }
                case APP_STATE : {
                    m_value = (T)m_expire_ptr->appState[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }
                case APP_STATUS : {
                    m_value = (T)m_expire_ptr->appStatus[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }
                case CAB_STATE : {
                    m_value = (T)m_expire_ptr->cabState[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                default: 
                    break;
            }
        }
        catch (ACSErr::ACSbaseExImpl& E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "PdoubleSeqDevIO::read()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be read");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        catch (MinorServoErrors::CommunicationErrorEx E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "PdoubleSeqDevIO::read()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Answer not found in m_responses");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
        return m_value;
    }

    // It writes values into controller. Unused because the properties are read-only.
    void write(const T &value, ACS::Time &timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("WPStatusDevIO::write() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
        try {
            switch (m_Property) {
                case STATUS : {
                    break;
                }

                default: 
                    break;
            }
        }
        catch (ACSErr::ACSbaseExImpl& E) {
            _ADD_BACKTRACE(
                    ComponentErrors::PropertyErrorExImpl, 
                    dummy, 
                    E, 
                    "PdoubleSeqDevIO::write()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be write");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard, dummy, LM_DEBUG);
            throw dummy;
        }               
        catch (MinorServoErrors::PositioningErrorEx E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "PdoubleSeqDevIO::write()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be write");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        catch (MinorServoErrors::CommunicationErrorEx E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "PdoubleSeqDevIO::write()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be write: answer not found");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
    }
    
private:

    WPServoTalker *m_wpServoTalk;

    TLinkedProperty m_Property;
    CString m_PropertyName;
    CLogGuard m_logGuard;
    CDBParameters *m_cdb_ptr;
    ExpireTime *m_expire_ptr;
    T m_value;

    bool isValueUpToDate(void) {

        timeval now;
        double actual_time, diff;
        gettimeofday(&now, NULL);
        actual_time = now.tv_sec + now.tv_usec / 1000000.0;

        switch (m_Property) {

            case STATUS: {
                diff = actual_time - m_expire_ptr->timeLastStatus[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }

            default: return false;

        }

        return diff < m_cdb_ptr->EXPIRE_TIME * WPSTATUS_STIME_FACTOR;
    }

};

#endif 
