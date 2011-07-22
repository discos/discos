#ifndef _DEVIOSTATUS_H_
#define _DEVIOSTATUS_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DevIOStatus.h,v 1.1 2011-03-14 14:15:07 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  	21/11/2008     Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the status property of the TotalPower
 * component. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
class DevIOStatus : public DevIO<ACS::pattern>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOStatus(CSecureArea<CCommandLine>* Link) :  m_pLink(Link)
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
	 * @throw ComponentErrors::PropertyError
	 * @param timestamp epoch when the operation completes
	*/ 
	ACS::pattern read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		// get the CommandLine .......
		CSecAreaResourceWrapper<CCommandLine> line=m_pLink->Get();
		try {
			DWORD ptrn;
			line->getBackendStatus(ptrn);
			m_val=(ACS::pattern)ptrn;
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOStatus::read()");
			dummy.setPropertyName("status");
			dummy.setReason("Property could not be read");
			//_IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy,LM_DEBUG);
			throw dummy;
		} 	
		timestamp=getTimeStamp();  //complition time
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
	CSecureArea<CCommandLine>* m_pLink;
	ACS::pattern m_val;
	//CLogGuard m_logGuard;
};


#endif /*_DEVIOSTATUS_H_*/
