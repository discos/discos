#ifndef _DEVIOFEEDS_H_
#define _DEVIOFEEDS_H_

/** **************************************************************************************************** */
/*                                                                                                       */
/* Who                                        when           What                                        */
/* Marco Buttu (mbuttu@oa-cagliari.inaf.it)   27/05/2013    Creation                                     */


#include <baciDevIO.h>
#include <IRA>

/**
 * This class is derived from template DevIO and it is used by the feeds  property of the  component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIOFeeds : public DevIO<CORBA::Long>
{
public:

	/**
	 * Constructor
	 * @param core pointer to the component core
	*/
	DevIOFeeds(CComponentCore* core) :  m_pCore(core)
	{
		AUTO_TRACE("DevIOFeeds::DevIOFeeds()");
	}

	/**
	 * Destructor
	*/
	~DevIOFeeds()
	{
		ACS_TRACE("DevIOFeeds::~DevIOFeeds()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIOFeeds::DevIOFeeds()");
		return true; // initialize with the default in order to avoid the alarm system when the component start and the value has not been read at least once
	}

	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	*/
	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		m_val=m_pCore->getFeeds();
		timestamp=getTimeStamp();  //Completion time
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/
	void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}

private:
	CComponentCore* m_pCore;
	long  m_val;
};

#endif
