#ifndef _DEVIOMODE8BIT_H_
#define _DEVIOMODE8BIT_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DevIOMode8bit.h,v 1.2 2010/07/09 13:30:23 bliliana Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                           				     when            What                                              */
/* Liliana Branciforti(bliliana@arcetri.astro.it) 04/08/2009 		Creation								*/


#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the busy property  of the TotalPower
 * component. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
class DevIOMode8bit : public DevIO<Management::TBoolean>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOMode8bit(CSecureArea<CCommandLine>* Link) :  m_pLink(Link)
	{		
		AUTO_TRACE("DevIOMode8bit::DevIOMode8bit()");		
	}

	/**
	 * Destructor
	*/ 
	~DevIOMode8bit()
	{
		ACS_TRACE("DevIOMode8bit::~DevIOMode8bit()");		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOMode8bit::initializaValue()");		
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
			if (line->getModo8bit()) {
				m_val=Management::MNG_TRUE;
			}
			else {
				m_val=Management::MNG_FALSE;
			}
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOMode8bit::read()");
			dummy.setPropertyName("mode8bit");
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


#endif /*_DEVIOMODE8BIT_H_*/
