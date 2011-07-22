#ifndef _DEVIOINITIALFREQUENCY_H_
#define _DEVIOINITIALFREQUENCY_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DevIOInitialFrequency.h,v 1.1 2011-02-23 09:35:01 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 22/02/2011     Creation                                         */


#include <baciDevIO.h>
#include <IRA>

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the initialFrequency property of the receiverBoss component. 
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
	DevIOInitialFrequency(CRecvBossCore* core) :  m_pCore(core)
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
		m_pCore->getInitialFrequency(m_val);
		timestamp=getTimeStamp();  //complition time
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
	CRecvBossCore* m_pCore;
	ACS::doubleSeq m_val;
};

#endif 
