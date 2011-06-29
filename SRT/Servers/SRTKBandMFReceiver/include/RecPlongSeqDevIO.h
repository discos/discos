/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __RECPLONGSEQDEVIO__H
#define __RECPLONGSEQDEVIO__H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciDevIO.h>
#include <IRA>
#include <ComponentErrors.h>
#include <LogFilter.h>
#include "SRTKBandMFParameters.h"
#include <MinorServoErrors.h>
#include "ReceiverSocket.h"

using namespace IRA;

class RecPlongSeqDevIO : public DevIO<ACS::longSeq>
{
public:
    enum TLinkedProperty {

        POLARIZATION                /*!< the devio will be used to read the  Local Oscillator property */
    };
    
    RecPlongSeqDevIO(
            CSecureArea<ReceiverSocket> *link, 
            TLinkedProperty Property,
            DDWORD guardInterval=1000000
            ) : m_link(link),
                m_Property(Property), 
                m_logGuard(guardInterval)
    {

        switch (m_Property) {

            case POLARIZATION: {
                m_PropertyName=CString("Polarization");
                break;
            }
        }

        CString trace("RecPlongSeqDevIO::RecPlongSeqDevIO()");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
    }

    ~RecPlongSeqDevIO()
    {
        CString trace("RecPlongSeqDevIO::~RecPlongSeqDevIO()");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
    }

    bool initializeValue()
    {       
        CString trace("RecPlongSeqDevIO::initializeValue()");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
        return false;
    }

    
    /**
     * Used to read the property value.
     * 
     * @param timestamp epoch when the operation completes
     * @throw ComponentErrors::PropertyErrorExImpl
     */ 
    ACS::longSeq read(ACS::Time &timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("RecPlongSeqDevIO::read()");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      

        try {
            switch (m_Property) {

                case POLARIZATION: {
                    ACS::longSeq dummy;
                    dummy.length(0);
                    m_value = dummy;
                    break;
                }
            }
        }
        catch (ACSErr::ACSbaseExImpl& E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "RecPlongSeqDevIO::read()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be read");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
        return m_value;
    }

    /**
     * Used to write the value into controller.
     * 
     * @throw ComponentErrors::PropertyErrorExImpl
    */ 
    void write(const ACS::longSeq& value, ACS::Time& timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("RecPlongSeqDevIO::write()");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
        try {
            switch (m_Property) {
 
                default: 
                    break;
            }
        }
        catch (ACSErr::ACSbaseExImpl &E) {
            _ADD_BACKTRACE(
                    ComponentErrors::PropertyErrorExImpl, 
                    dummy, 
                    E, 
                    "RecPlongSeqDevIO::write()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be write");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard, dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
    }
    
private:
    CSecureArea<ReceiverSocket>* m_link;
    TLinkedProperty m_Property;
    CString m_PropertyName;
    CLogGuard m_logGuard;
    ExpireTime *m_expire_ptr;
    ACS::longSeq m_value;;

};

#endif 
