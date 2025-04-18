#ifndef _DEVIOINITIALFREQUENCY_H_
#define _DEVIOINITIALFREQUENCY_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(andrea.orlati@inaf.it) 10/08/2023      Creation                                         */


#include <baciDevIO.h>
#include <IRA>

/**
 * This class is derived from template DevIO and it is used by the initialFrequency  property of the  component.
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>,
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
	 * @throw ACSErr::ACSbaseExImpl
	*/
	ACS::doubleSeq read(ACS::Time& timestamp)
	{
		m_pCore->getStartFrequency(m_val);
		timestamp=getTimeStamp();  //Completion time
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	 * @throw ACSErr::ACSbaseExImpl
	*/
	void write(const ACS::doubleSeq& value, ACS::Time& timestamp)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CComponentCore* m_pCore;
	ACS::doubleSeq m_val;
};

#endif
