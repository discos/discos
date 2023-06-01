#ifndef _DEVIOBUSY_H_
#define _DEVIOBUSY_H_

/* ************************************************************************************* */
/* OAC - Osservatorio Astronomico di Cagliari                                            */
/* $Id: Configuration.h,v 1.2 2011-05-12 14:14:31 c.migoni Exp $                         */
/*                                                                                       */
/* This code is under GNU General Public Licence (GPL).                                  */
/*                                                                                       */
/* Who                                        When               What                    */
/* Carlo Migoni (carlo.migoni@inaf.it)        09/03/2023         Creation                */


#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the busy property  of the SRTMistral
 * component. 
 * @author <a href=mailto:carlo.migoni@inaf.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br> 
*/
class DevIOBusy : public DevIO<Management::TBoolean>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOBusy(CSecureArea<CCommandLine>* Link) :  m_pLink(Link)
	{		
		AUTO_TRACE("DevIOBusy::DevIOBusy()");		
	}

	/**
	 * Destructor
	*/ 
	~DevIOBusy()
	{
		ACS_TRACE("DevIOBusy::~DevIOBusy()");		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOBusy::initializaValue()");		
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @throw ComponentErrors::PropertyError
	 * @param timestamp epoch when the operation completes
	*/ 
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		// get the CommandLine .......
		CSecAreaResourceWrapper<CCommandLine> line=m_pLink->Get();
		try {
			if (line->getIsBusy()) {
				m_val=Management::MNG_TRUE;
			}
			else {
				m_val=Management::MNG_FALSE;
			}
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOBusy::read()");
			dummy.setPropertyName("busy");
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
	void write(const Management::TBoolean& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}
	
private:
	CSecureArea<CCommandLine>* m_pLink;
	Management::TBoolean m_val;
	//CLogGuard m_logGuard;
};


#endif /*_DEVIOBUSY_H_*/
