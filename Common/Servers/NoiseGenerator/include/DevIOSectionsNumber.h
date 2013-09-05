#ifndef _DEVIOSECTIONSNUMBER_H_
#define _DEVIOSECTIONSNUMBER_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DevIOSectionsNumber.h,v 1.1 2011-03-14 15:16:22 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  	31/12/2009     Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the sectionsNumber property  of the TotalPower
 * component. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
class DevIOSectionsNumber : public DevIO<CORBA::Long>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that protects the command line socket. This object must be already initialized and configured.
	*/
	DevIOSectionsNumber(CCommandLine* Link) :  m_pLink(Link)
	{		
		AUTO_TRACE("DevIOSectionsNumber::DevIOSectionsNumber()");		
	}

	/**
	 * Destructor
	*/ 
	~DevIOSectionsNumber()
	{
		ACS_TRACE("DevIOSectionsNumber::~DevIOSectionsNumber()");		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOSectionsNumber::DevIOSectionsNumber()");		
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @throw ComponentErrors::PropertyError
	 * @param timestamp epoch when the operation completes
	*/ 
	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		// get the CommandLine .......
		try {
			long val;
			m_pLink->getSectionsNumber(val);
			m_val=(CORBA::Long)val;
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOSectionsNumber::read()");
			dummy.setPropertyName("sectionsNumber");
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
	void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}
	
private:
	CCommandLine* m_pLink;
	CORBA::Long m_val;
	//CLogGuard m_logGuard;
};


#endif /*_DEVIOINPUTSNUMBER_H_*/
