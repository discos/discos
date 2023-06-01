#ifndef _DEVIOATTENUATION_H_
#define _DEVIOATTENUATION_H_

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
 * This class is derived from template DevIO and it is used by the attenuation  property  of the SRTMistral
 * component. 
 * @author <a href=mailto:carlo.migoni@inaf.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br> 
*/
class DevIOAttenuation : public DevIO<ACS::doubleSeq>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOAttenuation(CSecureArea<CCommandLine>* Link) :  m_pLink(Link)
	{		
		AUTO_TRACE("DevIOAttenuation::DevIOAttenuation()");		
	}

	/**
	 * Destructor
	*/ 
	~DevIOAttenuation()
	{
		ACS_TRACE("DevIOAttenuation::~DevIOAttenuation()");		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOAttenuation::initializeValue()");		
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @throw ComponentErrors::PropertyError
	 * @param timestamp epoch when the operation completes
	*/ 
	ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		// get the CommandLine .......
		CSecAreaResourceWrapper<CCommandLine> line=m_pLink->Get();
		try {
			//line->getAttenuationAttr(m_val);
			line->getAttenuation(m_val);
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOAttenuation::read()");
			dummy.setPropertyName("attenuation");
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
	void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}
	
private:
	CSecureArea<CCommandLine>* m_pLink;
	ACS::doubleSeq m_val;
	//CLogGuard m_logGuard;
};





#endif /*DEVIOATTENUATION_H_*/
