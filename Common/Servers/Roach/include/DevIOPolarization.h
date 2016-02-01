#ifndef _DEVIOPOLARIZATION_H_
#define _DEVIOPOLARIZATION_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DevIOPolarization.h,v 1.1 2011-03-14 14:15:07 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  	18/11/2008     Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the polarization property  of the TotalPower
 * component. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
class DevIOPolarization : public DevIO<ACS::longSeq>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOPolarization(CSecureArea<CCommandLine>* Link) :  m_pLink(Link)
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
		AUTO_TRACE("DevIOPolarization::DevIOPolarization()");		
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @throw ComponentErrors::PropertyErrorr
	 * @param timestamp epoch when the operation completes
	*/ 
	ACS::longSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		// get the CommandLine .......
		CSecAreaResourceWrapper<CCommandLine> line=m_pLink->Get();
		try {
			line->getPolarization(m_val);
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOPolarization::read()");
			dummy.setPropertyName("polarization");
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


#endif /*_DEVIOPOLARIZATION_H_*/
