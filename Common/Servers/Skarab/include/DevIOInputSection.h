#ifndef DEVIOINPUTSECTION_H_
#define DEVIOINPUTSECTION_H_

/* ************************************************************************************************************ */
/* OAC Osservatorio Astronomico di Cagliari                                                                     */
/* $Id: SRTMistralImpl.h,v 1.1 2023-01-09 14:15:07 c.migoni Exp $						                        */
/*                                                                                                              */
/* This code is under GNU General Public Licence (GPL).                                                         */
/*                                                                                                              */
/* Who                                when            What                                                      */
/* Carlo Migoni(carlo.migoni@inaf.it) 16/03/2023      Creation                                                  */
/* Carlo Migoni(carlo.migoni@inaf.it)                                                                           */

#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the InputSection property  of the TotalPower
 * component. 
 * @author <a href=mailto:carlo.migoni@inaf.it>Carlo Migoni</a>,
 * Osservatorio Astronomico di Cagliari, Italia
*/
class DevIOInputSection : public DevIO<ACS::longSeq>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that protects the command line socket. This object must be already initialized and configured.
	*/
	DevIOInputSection(CSecureArea<CCommandLine>* Link) :  m_pLink(Link)
	{		
		AUTO_TRACE("DevIOInputSection::DevIOInputSection()");		
	}

	/**
	 * Destructor
	*/ 
	~DevIOInputSection()
	{
		ACS_TRACE("DevIOInputSection::~DevIOInputSection()");		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOInputSection::DevIOInputSection()");		
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @throw ComponentErrors::PropertyError
	 * @param timestamp epoch when the operation completes
	*/ 
	ACS::longSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		// get the CommandLine .......
		CSecAreaResourceWrapper<CCommandLine> line=m_pLink->Get();
		try {
			//line->getInputSectionAttr(m_val);
			line->getInputSection(m_val);
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOInputSection::read()");
			dummy.setPropertyName("inputSection");
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
	void write(const ACS::longSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}
	
private:
	CSecureArea<CCommandLine>* m_pLink;
	ACS::longSeq m_val;
	//CLogGuard m_logGuard;
};


#endif /*DEVIOINPUTSECTION_H_*/
