#ifndef DEVIOAMPLITUDE_H_
#define DEVIOAMPLITUDE_H_

/* **************************************************************************************************** */
/* INAF OA Cagliari                                                                     */
/* $Id: DevIOamplitude.h,v 1.0 2011-10-20 14:15:07 s.poppi Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  	20/10/2011     Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"

using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the systemTemperature  property  of the TotalPower
 * component. 
 * @author <a href=mailto:spoppi@oa-cagliari.inaf.it>Sergio Poppi</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
*/
class DevIOamplitude : public DevIO<CORBA::Double>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOamplitude(CSecureArea<CommandLine>* Link) :  m_pLink(Link)
	{		
		AUTO_TRACE("DevIOamplitude::DevIOamplitude()");
	}

	/**
	 * Destructor
	*/ 
	~DevIOamplitude()
	{
		ACS_TRACE("DevIOamplitude::~DevIOamplitude()");
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOamplitude::initializeValue()");
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @throw ComponentErrors::PropertyError
	 * @param timestamp epoch when the operation completes
	*/ 
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		// get the CommandLine .......
		CSecAreaResourceWrapper<CommandLine> line=m_pLink->Get();
		try {
			line->getPower(m_val);
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOamplitude::read()");
			dummy.setPropertyName("amplitude");
			dummy.setReason("Property could not be read");
			//_IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy,LM_DEBUG);
			throw dummy;
		} 	catch (GPIBException& ex)
		{
			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,dummy,"DevIOamplitude::read()");
			 dummy.log(LM_DEBUG);
			 ACS_LOG(LM_FULL_INFO,"DevIOamplitude::read()",(LM_DEBUG,"LocalOscillatorImpl::initialize() %s",ex.what()));

		}


		timestamp=getTimeStamp();  //complition time
		return m_val;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/ 
	void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}
	
private:
	CSecureArea<CommandLine>* m_pLink;
	CORBA::Double m_val;
	//CLogGuard m_logGuard;
};



#endif /*DEVIOAMPLITUDE_H_*/
