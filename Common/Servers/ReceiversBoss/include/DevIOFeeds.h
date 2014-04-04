#ifndef _DEVIOFEEDS_H_
#define _DEVIOFEEDS_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 06/08/2010     Creation                                         */


#include <baciDevIO.h>
#include <IRA>

/**
 * This class is derived from template DevIO and it is used by the feeds property  of the receiverBoss component. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
class DevIOFeeds : public DevIO<CORBA::Long>
{
public:

	/** 
	 * Constructor
	 * @param core pointer to the boss core
	*/
	DevIOFeeds(CRecvBossCore* core) :  m_pCore(core)
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
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	*/ 
	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		m_pCore->getFeeds(m_val);
		timestamp=getTimeStamp();  //complition time
		return (CORBA::Long)m_val;
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
	CRecvBossCore* m_pCore;
	long m_val;
};

#endif
