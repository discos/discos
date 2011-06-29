/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/
#ifndef __RECSINGLEVALUE_H__
#define __RECSINGLEVALUE_H__

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciDevIO.h>
#include <IRA>
#include <ComponentErrors.h>
#include <LogFilter.h>
#include "ReceiverSocket.h"
#include "SRTKBandMFParameters.h"

using namespace IRA;

template <class T> class RecSingleValueDevIO : public DevIO<T>
{
public:
    enum TLinkedProperty {
        VACUUM,      
        LOW_TEMPERATURE,      
        HIGH_TEMPERATURE,      
        FEEDS,
        IFS,
        STATUS
    };
    
    RecSingleValueDevIO(
            CSecureArea<ReceiverSocket>  *link, 
            TLinkedProperty property,
            const RCDBParameters *cdb_ptr,
            DDWORD guard_interval=1000000
            ) : m_link(link), m_property(property), m_log_guard(guard_interval), m_cdb_ptr(cdb_ptr) 
    {
        switch (m_property) {
            case VACUUM : {
                m_property_name = CString("Vacuum");
                break;
            }
            case LOW_TEMPERATURE : {
                m_property_name = CString("Low Temperature");
                break;
            }
            case HIGH_TEMPERATURE : {
                m_property_name = CString("High Temperature");
                break;
            }
            case FEEDS : {
                m_property_name = CString("Number of Feeds");
                break;
            }
            case IFS : {
                m_property_name = CString("Intermediate Frequencies");
                break;
            }
            case STATUS : {
                m_property_name = CString("Receiver Status");
                break;
            }
        }

        CString trace("RecSingleValueDevIO::RecSingleValueDevIO()");
        trace += m_property_name;
        AUTO_TRACE((const char*)trace);     
    }

    ~RecSingleValueDevIO()
    {
        CString trace("RecSingleValueDevIO::~RecSingleValueDevIO()");
        trace += m_property_name;
        ACS_TRACE((const char*)trace);      
    }

    bool initializeValue()
    {       
        CString trace("RecSingleValueDevIO::initializeValue()");
        trace += m_property_name;
        AUTO_TRACE((const char*)trace);     
        return false;
    }
    
    T read(ACS::Time &timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        CString trace("RecSingleValueDevIO::read() ");
        trace += m_property_name;
        ACS_TRACE((const char*)trace);      
        CSecAreaResourceWrapper <ReceiverSocket> thsafe_link = m_link->Get();
        try {
            switch (m_property) {
                case VACUUM : {
                    m_value = (T)thsafe_link->vacuum();
                    break;
                }
                case LOW_TEMPERATURE: {
                    double dummy(0);
                    m_value = (T)dummy;
                    break;
                }
                case HIGH_TEMPERATURE: {
                    double dummy(1);
                    m_value = (T)dummy;
                    break;
                }
                case FEEDS: {
                    m_value = (T)m_cdb_ptr->NUMBER_OF_FEEDS;
                    break;
                }
                case IFS: {
                    long dummy(1);
                    m_value = (T)dummy;
                    break;
                }
                case STATUS: {
                    unsigned long dummy(1);
                    m_value = (T)dummy;
                    break;
                }
            }
        }
        catch (ACSErr::ACSbaseExImpl& E) {
            _ADD_BACKTRACE(
                    ComponentErrors::PropertyErrorExImpl, 
                    dummy, 
                    E, 
                    "RecSingleValueDevIO::read()"
                    );
            dummy.setPropertyName((const char *)m_property_name);
            dummy.setReason("Property could not be read");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_log_guard,dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
        return m_value;
    }

    // It writes values into controller. Unused because the properties are read-only.
    // void write(const T& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    void write(const double &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        CString trace("RecSingleValueDevIO::read() ");
        trace += m_property_name;
        ACS_TRACE((const char*)trace);      
        CSecAreaResourceWrapper <ReceiverSocket> thsafe_link = m_link->Get();
        try {
            switch (m_property) {
                case VACUUM : {
                    break;
                }
                default : {
                     break;
                 }
            }
        }
        catch (ACSErr::ACSbaseExImpl& E) {
            _ADD_BACKTRACE(
                    ComponentErrors::PropertyErrorExImpl, 
                    dummy, 
                    E, 
                    "RecSingleValueDevIO::write()"
                    );
            dummy.setPropertyName((const char *)m_property_name);
            dummy.setReason("Property could not be write");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_log_guard,dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
    }
    
private:
    CSecureArea<ReceiverSocket>* m_link;
    T m_value;
    TLinkedProperty m_property;
    CString m_property_name;
    CLogGuard m_log_guard;
    const RCDBParameters *m_cdb_ptr;
};

#endif 
