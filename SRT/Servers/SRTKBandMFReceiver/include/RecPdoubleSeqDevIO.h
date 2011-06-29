/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __RECPDOUBLESEQDEVIO__H
#define __RECPDOUBLESEQDEVIO__H

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

class RecPdoubleSeqDevIO : public DevIO<ACS::doubleSeq>
{
public:
    enum TLinkedProperty {

        LO                /*!< the devio will be used to read the  Local Oscillator property */
    };
    
    RecPdoubleSeqDevIO(
            CSecureArea<ReceiverSocket> *link, 
            TLinkedProperty Property,
            DDWORD guardInterval=1000000
            ) : m_link(link),
                m_Property(Property), 
                m_logGuard(guardInterval)
    {

        switch (m_Property) {

            case LO: {
                m_PropertyName=CString("Local Oscillator");
                break;
            }
        }

        CString trace("RecPdoubleSeqDevIO::RecPdoubleSeqDevIO()");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
    }

    ~RecPdoubleSeqDevIO()
    {
        CString trace("RecPdoubleSeqDevIO::~RecPdoubleSeqDevIO()");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
    }

    bool initializeValue()
    {       
        CString trace("RecPdoubleSeqDevIO::initializeValue()");
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
    ACS::doubleSeq read(ACS::Time &timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("RecPdoubleSeqDevIO::read()");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      

        try {
            switch (m_Property) {

                case LO: {
                    ACS::doubleSeq dummy;
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
                "RecPdoubleSeqDevIO::read()"
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
    void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("RecPdoubleSeqDevIO::write()");
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
                    "RecPdoubleSeqDevIO::write()"
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
    ACS::doubleSeq m_value;;

};

#endif 
