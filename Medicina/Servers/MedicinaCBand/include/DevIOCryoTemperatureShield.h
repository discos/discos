#ifndef _DEVIOCRYOCOOLHEADWIN_H_
#define _DEVIOCRYOCOOLHEADWIN_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 19/08/2011     Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include "Commons.h"

/**
 * This class is derived from template DevIO and it is used by the cryoTemparatureCoolHeadWindow  property of the  component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOCryoTemperatureShield : public DevIO<CORBA::Double>
{
public:

	/**
	 * Constructor
	 * @param core pointer to the boss core
	*/
	DevIOCryoTemperatureShield(CComponentCore* core) :  m_pCore(core)
	{
		AUTO_TRACE("DevIOCryoTemperatureShield::DevIOCryoTemperatureShield()");
	}

	/**
	 * Destructor
	*/
	~DevIOCryoTemperatureShield()
	{
		ACS_TRACE("DevIOCryoTemperatureShield::~DevIOCryoTemperatureShield()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIOCryoTemperatureShield::initializeValue()");
		return true; // initialize with the default in order to avoid the alarm system when the component start and the value has not been read at least once
	}

	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	*/
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
      BoardValue result = m_pCore->getShieldTemperature();
      m_val = result.temperature;
      timestamp = result.timestamp;
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/
	void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CComponentCore* m_pCore;
	double  m_val;
};

#endif