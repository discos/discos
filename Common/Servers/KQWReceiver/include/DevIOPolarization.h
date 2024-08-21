#ifndef _DEVIOPOLARIZATION_H_
#define _DEVIOPOLARIZATION_H_

/** *************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                      when           What                                         */
/* Andrea Orlati (aorlati@ira.inaf.it)      10/08/2011     Creation                                     */
/* Marco Buttu (mbuttu@oa-cagliari.inaf.it) 26/03/2013     LP configuration                             */

#include <baciDevIO.h>
#include <IRA>


/**
 * This class is derived from template DevIO and it is used by the polarization property  of the receiverBoss component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOPolarization : public DevIO<ACS::longSeq>
{
public:

    /**
     * Constructor
     * @param core pointer to the boss core
    */
    DevIOPolarization(CComponentCore* core) :  m_pCore(core)
    {
        AUTO_TRACE("DevIOPolarization::DevIOPolarization()");
    }

    /**
     * Destructor
    */
    ~DevIOPolarization()
    {
        ACS_TRACE("DevIOPolarization::~DevIOPolarization()");
    }

    /**
     * @return true to initialize the property with default value from CDB.
    */
    bool initializeValue()
    {
        AUTO_TRACE("DevIOLO::DevIOPolarization()");
        return false;
    }

    /**
     * Used to read the property value.
     * @param timestamp epoch when the operation completes
     * @throw ACSErr::ACSbaseExImpl
    */
    ACS::longSeq read(ACS::Time& timestamp)
    {
		m_pCore->getPolarizations(m_val);
		timestamp = getTimeStamp();  //Completion time
		return m_val;
    }

    /**
     * It writes values into controller. Unused because the properties are read-only.
     * @throw ACSErr::ACSbaseExImpl
    */
    void write(const ACS::longSeq& value, ACS::Time& timestamp)
    {
        timestamp=getTimeStamp();
        return;
    }

private:
    CComponentCore* m_pCore;
    ACS::longSeq m_val;
};

#endif
