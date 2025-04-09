/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __SUBSYSTEMVSTATUSDEVIO__H
#define __SUBSYSTEMVSTATUSDEVIO__H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciDevIO.h>
#include <IRA>
#include <ComponentErrors.h>
#include <LogFilter.h>
#include <bitset>

#define INIT_IDX 0
#define TRACK_IDX 1



using namespace IRA;

template <class T> class SubsystemVStatusDevIO : public DevIO<T>
{
public:
    enum TLinkedProperty {
        VERBOSE_STATUS          /*!< the devio will be used to read the minor servo status */
    };


    SubsystemVStatusDevIO(
            TLinkedProperty Property,
            VerboseStatusFlags *vstatus_flags,
            DDWORD guardInterval=1000000
            ) : m_Property(Property), 
                m_logGuard(guardInterval),
                m_vstatus_flags(vstatus_flags)
    {
        switch (m_Property) {
            case VERBOSE_STATUS : {
                m_PropertyName=CString("Verbose Status");
                break;
            }
        }

        CString trace("SubsystemVStatusDevIO::SubsystemVStatusDevIO() ");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
    }

    ~SubsystemVStatusDevIO()
    {
        CString trace("SubsystemVStatusDevIO::~SubsystemVStatusDevIO() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
    }

    bool initializeValue()
    {       
        CString trace("SubsystemVStatusDevIO::initializeValue() ");
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
        CString trace("SubsystemVStatusDevIO::read() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
        try {
            switch (m_Property) {
                case VERBOSE_STATUS : {
                    try {
                        bitset<2> status_bset(0);
                        status_bset.set(INIT_IDX, *(m_vstatus_flags->is_initialized));
                        status_bset.set(TRACK_IDX, 0);
                        m_value = (T)status_bset.to_ulong();
                        break;
                    }
                    catch(...) {
                        ACS_SHORT_LOG((LM_ERROR, "Some problem in SubsystemVStatusDevIO::read()"));
                    }
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
        timestamp = getTimeStamp();  // Complition time
        return m_value;
    }

    // It writes values into controller. Unused because the properties are read-only.
    void write(const T &value, ACS::Time &timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("SubsystemVStatusDevIO::write() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
        try {
            switch (m_Property) {
                case VERBOSE_STATUS : {
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
        timestamp = getTimeStamp();  // Complition time
    }
    
private:

    TLinkedProperty m_Property;
    CString m_PropertyName;
    CLogGuard m_logGuard;
    VerboseStatusFlags *m_vstatus_flags;
    T m_value;

};

#endif 
