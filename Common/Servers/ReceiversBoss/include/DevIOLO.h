#ifndef _DEVIOLO_H_
#define _DEVIOLO_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 22/07/2010     Creation                                         */


#include <baciDevIO.h>
#include <IRA>

/**
 * This class is derived from template DevIO and it is used by the Local Oscillator property  of the receiverBoss component. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
class DevIOLO : public DevIO<ACS::doubleSeq>
{
public:

	/** 
	 * Constructor
	 * @param core pointer to the boss core
	*/
	DevIOLO(CRecvBossCore* core) :  m_pCore(core)
	{		
		AUTO_TRACE("DevIOLO::DevIOLO()");		
	}

	/**
	 * Destructor
	*/ 
	~DevIOLO()
	{
		ACS_TRACE("DevIOLO::~DevIOLO()");		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOLO::DevIOLO()");		
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	*/ 
	ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		m_pCore->getLO(m_val);
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

