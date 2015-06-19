/*******************************************************************************\
 *
 *  Author: Marco Buttu, mbuttu@oa-cagliari.inaf.it
 *  Last Modified: Mon Feb 23 10:17:53 CET 2009
 *   
\*******************************************************************************/

#ifndef __SENSORDEVIO__H
#define __SENSORDEVIO__H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciDevIO.h>
#include <IRA>
#include "sensorSocket.h"
#include <ComponentErrors.h>
#include <LogFilter.h>

using namespace IRA;

template <class T> class sensorDevIO : public DevIO<T>
{
public:
    enum TLinkedProperty {
        POSITION
    };
    
    sensorDevIO(
            CSecureArea<sensorSocket>* link, 
            TLinkedProperty Property,
            DDWORD guardInterval=1000000
            ) : m_sensorLink(link), m_Property(Property), m_logGuard(guardInterval)
    {
        switch (m_Property) {
            case POSITION : {
                m_PropertyName=CString("Sensor Position");
                break;
            }
            /*
            case SECOND_PROPERTY_NAME : {
                m_PropertyName=CString("Second Property Name");
                break;
            }
            */
        }

        CString trace("sensorDevIO::sensorDevIO() ");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
    }

    ~sensorDevIO()
    {
        CString trace("sensorDevIO::~sensorDevIO() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
    }

    bool initializeValue()
    {       
        CString trace("sensorDevIO::initializeValue() ");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
        return false;
    }
    
    /**
     * Used to read the property value.
     * @throw ComponentErrors::PropertyError
     *       @arg \c ComponentErrors::Timeout</a>
     *       @arg \c AntennaErrors::Connection</a>   
     *       @arg \c ComponentErrors::SocketError
     * @param timestamp epoch when the operation completes
    */ 
    T read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        CString trace("sensorDevIO::read() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
        CSecAreaResourceWrapper <sensorSocket> sensor_socket = m_sensorLink->Get();
        try {
            switch (m_Property) {
                case POSITION : {
                    m_Value = (T)sensor_socket->getPosition();
                    break;
                }
            }
        }
        catch (ACSErr::ACSbaseExImpl& E) {
            _ADD_BACKTRACE(
                    ComponentErrors::PropertyErrorExImpl, 
                    dummy, 
                    E, 
                    "sensorDevIO::read()"
                    );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be read");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
        return m_Value;
    }

    // It writes values into controller. Unused because the properties are read-only.
    void write(const T& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        CString trace("ACUDevIO::write() ");
        trace+=m_PropertyName;
        AUTO_TRACE((const char*)trace);     
        timestamp=getTimeStamp();
        return;
    }
    
private:
    CSecureArea<sensorSocket>* m_sensorLink;
    T m_Value;
    TLinkedProperty m_Property;
    CString m_PropertyName;
    CLogGuard m_logGuard;
};

#endif 
