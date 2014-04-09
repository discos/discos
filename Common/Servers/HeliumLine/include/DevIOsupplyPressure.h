/*
 * DevIOsupplyPressure.h
 *
 *  Created on: Mar 21, 2014
 *      Author: spoppi
 */

#ifndef DEVIOSUPPLYPRESSURE_H_
#define DEVIOSUPPLYPRESSURE_H_
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
class DevIOsupplyPressure : public DevIO<CORBA::Double>
{
public:

	/**
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a the command line socket. This object must be already initialized and configured.
	*/
	DevIOsupplyPressure(CSecureArea<CommandLine>* Link) :  m_pLink(Link)
	{
		AUTO_TRACE("DevIOsupplyPressure::DevIOsupplyPressure()");
	}

	/**
	 * Destructor
	*/
	~DevIOsupplyPressure()
	{
		ACS_TRACE("DevIOsupplyPressure::~DevIOsupplyPressure()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIOsupplyPressure::initializeValue()");
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
			line->getSupplyPressure(m_val);
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOsupplyPressure::read()");
			dummy.setPropertyName("supplyPressure");
			dummy.setReason("Property could not be read");
			//_IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy,LM_DEBUG);
			throw dummy;
		} 	catch (GPIBException& ex)
		{
			 _EXCPT(ReceiversErrors::LocalOscillatorErrorExImpl,dummy,"DevIOsupplyPressure::read()");
			 dummy.log(LM_DEBUG);
			 ACS_LOG(LM_FULL_INFO,"DevIOsupplyPressure::read()",(LM_DEBUG,"DevIOsupplyPressure::read() %s",ex.what()));

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


#endif /* DEVIOSUPPLYPRESSURE_H_ */
