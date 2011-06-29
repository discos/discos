#ifndef _DEVIOPOLARIZATION_H_
#define _DEVIOPOLARIZATION_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DevIOPolarization.h,v 1.1 2010-08-06 13:46:20 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 06/08/2010     Creation                                         */


#include <baciDevIO.h>
#include <IRA>

using namespace IRA;

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
	DevIOPolarization(CRecvBossCore* core) :  m_pCore(core)
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
	*/ 
	ACS::longSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		m_pCore->getPolarization(m_val);
		timestamp=getTimeStamp();  //complition time
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/ 
	void write(const ACS::longSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}
	
private:
	CRecvBossCore* m_pCore;
	ACS::longSeq m_val;
};

#endif 

