#ifndef _DEVIOBINS_H_
#define _DEVIOBINS_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DevIOBins.h,v 1.1 2011-03-14 15:16:22 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  	19/11/2008     Creation                                         */

#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the bins property  of the TotalPower
 * component. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
class DevIOBins : public DevIO<ACS::longSeq>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOBins(CCommandLine* Link) :  m_pLink(Link)
	{		
		AUTO_TRACE("DevIOBins::DevIOBins()");		
	}

	/**
	 * Destructor
	*/ 
	~DevIOBins()
	{
		ACS_TRACE("DevIOBins::~DevIOBins()");		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOBins::DevIOBins()");		
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
		try {
			m_pLink->getBins(m_val);
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOBins::read()");
			dummy.setPropertyName("Bins");
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
	CCommandLine* m_pLink;
	ACS::longSeq m_val;
	//CLogGuard m_logGuard;
};



#endif /*_DEVIOBINS_H_*/
