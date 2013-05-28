#ifndef _DEVIOINITIALFREQUENCY_H_
#define _DEVIOINITIALFREQUENCY_H_

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
 * This class is derived from template DevIO and it is used by the initialFrequency  property of the  component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOInitialFrequency : public DevIO<ACS::doubleSeq>
{
public:

    /**
     * Constructor
     * @param core pointer to the boss core
    */
    DevIOInitialFrequency(CComponentCore* core) :  m_pCore(core)
    {
        AUTO_TRACE("DevIOInitialFrequency::DevIOInitialFrequency()");
    }

    /**
     * Destructor
    */
    ~DevIOInitialFrequency()
    {
        ACS_TRACE("DevIOInitialFrequency::~DevIOInitialFrequency()");
    }

    /**
     * @return true to initialize the property with default value from CDB.
    */
    bool initializeValue()
    {
        AUTO_TRACE("DevIOInitialFrequency::DevIOInitialFrequency()");
        return false;
    }

    /**
     * Used to read the property value.
     * @param timestamp epoch when the operation completes
    */
    ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        ACS::doubleSeq lbandValue, pbandValue;
        lbandValue.length(m_pCore->getIFs());
        pbandValue.length(m_pCore->getIFs());
        m_pCore->getLBandStartFrequency(lbandValue);
        m_pCore->getPBandStartFrequency(pbandValue);

        IRA::CString actualMode(m_pCore->getActualMode());
        if(actualMode.Right() == "X") { // P band
            m_val.length(pbandValue.length());
            for(size_t i=0; i<m_val.length(); i++)
                m_val[i] = pbandValue[i];
        }
        else if(actualMode.Left() == "X") { // L band
            m_val.length(lbandValue.length());
            for(size_t i=0; i<m_val.length(); i++)
                m_val[i] = lbandValue[i];
        }
        else { // Dual feed
            m_val.length(pbandValue.length() + lbandValue.length());
            // Set the first two values (IF0 and IF1)
            for(size_t i=0; i<pbandValue.length(); i++)
                m_val[i] = pbandValue[i];
            // Set the IF2 and IF3 values
            for(size_t i=2, j=0; i<m_val.length(); i++, j++)
                m_val[i] = lbandValue[j];
        }

        timestamp = getTimeStamp();  //Completion time
        return m_val;

    }
    /**
     * It writes values into controller. Unused because the properties are read-only.
    */
    void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        timestamp=getTimeStamp();
        return;
    }

private:
    CComponentCore* m_pCore;
    ACS::doubleSeq m_val;
};

#endif
