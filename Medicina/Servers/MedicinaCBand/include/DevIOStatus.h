#ifndef _DEVIOSTATUS_H_
#define _DEVIOSTATUS_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  	19/11/2008     Creation                                         */


#include <baciDevIO.h>
#include <IRA>

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the status property
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOStatus : public DevIO<ACS::pattern>
{
public:

	/**
	 * Constructor
	 * @param core pointer to the component core
	*/
	DevIOStatus(CComponentCore* core) :  m_pCore(core)
	{
		AUTO_TRACE("DevIOStatus::DevIOStatus()");
	}

	/**
	 * Destructor
	*/
	~DevIOStatus()
	{
		ACS_TRACE("DevIOStatus::~DevIOStatus()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIOStatus::DevIOStatus()");
		return false;
	}

	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	*/
	ACS::pattern read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		m_val=m_pCore->getStatusWord();
		timestamp=getTimeStamp();  //Completion time
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/
	void write(const ACS::pattern& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CComponentCore* m_pCore;
	ACS::pattern m_val;
};


#endif /*_DEVIOSTATUS_H_*/
