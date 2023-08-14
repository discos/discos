#ifndef _DEVIOCRYOLNA_H_
#define _DEVIOCRYOLNA_H_

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
 * This class is derived from template DevIO and it is used by the cryoTemparatureLNA  property of the  component.
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOCryoTemperatureLNA : public DevIO<CORBA::Double>
{
public:

	/**
	 * Constructor
	 * @param core pointer to the boss core
	*/
	DevIOCryoTemperatureLNA(CComponentCore* core) :  m_pCore(core)
	{
		AUTO_TRACE("DevIOCryoTemperatureLNA::DevIOCryoTemperatureLNA()");
	}

	/**
	 * Destructor
	*/
	~DevIOCryoTemperatureLNA()
	{
		ACS_TRACE("DevIOCryoTemperatureLNA::~DevIOCryoTemperatureLNA()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIOCryoTemperatureLNA::initializeValue()");
		return true; // initialize with the default in order to avoid the alarm system when the component start and the value has not been read at least once
	}

	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	 * @throw ACSErr::ACSbaseExImpl
	*/
	CORBA::Double read(ACS::Time& timestamp)
	{
        CConfiguration::BoardValue result = m_pCore->getCryoLNA();
        m_val = result.temperature;
        timestamp = result.timestamp;
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	 * @throw ACSErr::ACSbaseExImpl
	*/
	void write(const CORBA::Double& value, ACS::Time& timestamp)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CComponentCore* m_pCore;
	double  m_val;
};

#endif
