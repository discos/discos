/*
 * DevIOreturnPressure.h
 *
 *  Created on: Mar 21, 2014
 *      Author: spoppi
 */

#ifndef DEVIORETURNPRESSURE_H_
#define DEVIORETURNPRESSURE_H_

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
class DevIOreturnPressure : public DevIO<CORBA::Double>
{
public:

	/**
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOreturnPressure(CSecureArea<CommandLine>* Link) :  m_pLink(Link)
	{
		AUTO_TRACE("DevIOreturnPressure::DevIOreturnPressure()");
	}

	/**
	 * Destructor
	*/
	~DevIOreturnPressure()
	{
		ACS_TRACE("DevIOreturnPressure::~DevIOreturnPressure()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIOreturnPressure::initializeValue()");
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
			line->getReturnPressure(m_val);
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOreturnPressure::read()");
			dummy.setPropertyName("returnPressure");
			dummy.setReason("Property could not be read");
			//_IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy,LM_DEBUG);
			throw dummy;
		} 	catch (GPIBException& ex)
		{
			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,dummy,"DevIOreturnPressure::read()");
			 dummy.log(LM_DEBUG);
			 ACS_LOG(LM_FULL_INFO,"DevIOreturnPressure::read()",(LM_DEBUG,"DevIOreturnPressure::read() %s",ex.what()));

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





#endif /* DEVIORETURNPRESSURE_H_ */
