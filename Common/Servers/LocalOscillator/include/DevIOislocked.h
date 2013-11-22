#ifndef DEVIOLOCKED_H_
#define DEVIOLOCKED_H_

/* **************************************************************************************************** */
/* INAF OA Cagliari                                                                     */
/* $Id: DevIOfrequency.h,v 1.0 2011-10-20 14:15:07 s.poppi Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  	20/10/2011     Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include "CommandLine.h"
#include <cmath>
using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the systemTemperature  property  of the TotalPower
 * component. 
 * @author <a href=mailto:spoppi@oa-cagliari.inaf.it>Sergio Poppi</a>,
 * Osservatorio Astronomico di Cagliari, Italia<br>
*/

class  LocalOscillatorImpl;

class DevIOislocked : public DevIO<CORBA::Long>
{
public:

	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOislocked(CSecureArea<CommandLine>* Link, LocalOscillatorImpl* impl ) :  m_pLink(Link),m_impl(impl)
	{		
		AUTO_TRACE("DevIOislocked::DevIOislocked()");
	}

	/**
	 * Destructor
	*/ 
	~DevIOislocked()
	{
		ACS_TRACE("DevIOislocked::~DevIOislocked()");
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOislocked::initializeValue()");
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
		CSecAreaResourceWrapper<CommandLine> line=m_pLink->Get();
		try {
			double expected_power,expected_freq,read_power,read_frequency;
			line->getFreq(read_frequency);
			line->getPower(read_power);
			m_impl->getStoredValue(expected_power,expected_freq);

			if (fabs(expected_freq - read_frequency) <= 0.1)
 	 		{
				m_val=1;
	 			cout << "expected freq: " <<expected_freq << "Read Freq:" << read_frequency  << "flag" <<m_val<< endl;

 	 		} else
 	 		{

 	 			ACS::Time timestamp;
	 			m_val=0;
	 			cout << "expected freq: " <<expected_freq << "Read Freq:" << read_frequency  << "flag" <<m_val<< endl;

 	 			ACS_LOG(LM_FULL_INFO,"LocalOscillatorImpl::get()",(LM_WARNING,"LocalOscillatorImpl expected frequency (%f) does not matched with read (%f)",expected_freq,read_frequency));
 	 		}


		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOislocked::read()");
			dummy.setPropertyName("islocked");
			dummy.setReason("Property could not be read");
			//_IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy,LM_DEBUG);
			throw dummy;
		} 	catch (GPIBException& ex)
		{
			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,dummy,"DevIOislocked::read()");
			 dummy.log(LM_DEBUG);
			 ACS_LOG(LM_FULL_INFO,"DevIOislocked::read()",(LM_DEBUG,"DevIOislocked::DevIOislocked() %s",ex.what()));

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
	CORBA::Long m_val;
	LocalOscillatorImpl* m_impl;

};



#endif /*DEVIOLOCKED_H_*/
