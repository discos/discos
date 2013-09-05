/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Andrea Orlati <orlati@ira.inaf.it>
 *  Marco Buttu <mbuttu@oa-cagliari.inaf.it>
\*******************************************************************************/

#ifndef _KBANDDEVIOENVTEMPERATURE_H_
#define _KBANDDEVIOENVTEMPERATURE_H_

#include <baciDevIO.h>
#include <IRA>

/**
 * This class is derived from template DevIO and it is used by the environmentTemperatureproperty of the  component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>.
*/
class DevIOEnvTemperature : public DevIO<CORBA::Double>
{
public:


    /**
     * Constructor
     * @param core pointer to the boss core
     */
    DevIOEnvTemperature(CComponentCore* core) : m_pCore(core) { AUTO_TRACE("DevIOEnvTemperature::DevIOEnvTemperature()"); }


    ~DevIOEnvTemperature() { ACS_TRACE("DevIOEnvTemperature::~DevIOEnvTemperature()"); }


    /**
     * @return true to initialize the property with default value from CDB.
     */
    bool initializeValue()
    {
        AUTO_TRACE("DevIOEnvTemperature::initializeValue()");
        return true; // initialize with the default in order to avoid the alarm system when the component starts
                     // and the value has not been read at least once
    }


    /**
     * Used to read the property value.
     * @param timestamp epoch when the operation completes
     */
    CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        m_val = m_pCore->getVertexTemperature();
        timestamp = getTimeStamp();  //Completion time
        return m_val;
    }


    /**
     * It writes values into controller. Unused because the properties are read-only.
    */
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        timestamp = getTimeStamp();
        return;
    }

private:
    CComponentCore* m_pCore;
    double  m_val;
};

#endif
