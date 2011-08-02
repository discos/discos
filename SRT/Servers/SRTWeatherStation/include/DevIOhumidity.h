#ifndef DevIOHumidity_H_
#define DevIOHumidity_H_

/* **************************************************************************************************** */
/*INAF - OACA                                                                      */
/*  					           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Sergio  Poppi(spoppi@oa-cagliari.inaf.it)  	02/01/2009     Creation                                         */


#include <baciDevIO.h>
#include <IRA>
#include <map>
#include "SRTWeatherSocket.h"
#include "WeatherStationData.h"
using namespace IRA;

/**
 * This class is derived from template DevIO and it is used by the temperature  property  of the MeteoStation component
 * @author <a href=mailto:spoppi@oa-cagliari.inaf.it>Sergio Poppi</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
class DevIOHumidity : public DevIO<CORBA::Double>
{
public:
	
 	/** 
	 * Constructor
	 * @param Socket pointer to a SecureArea that proctects a the  socket. This object must be already initialized and configured.
	*/
	DevIOHumidity(CSecureArea<SRTWeatherSocket>* socket):m_socket(socket) 
	{		
 		m_initparser=false;
		AUTO_TRACE("DevIOHumidity::DevIOHumidity()");		
	}

	/**
	 * Destructor
	*/ 
	~DevIOHumidity()
	{
		ACS_TRACE("DevIOHumidity::~DevIOHumidity()");		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		AUTO_TRACE("DevIOHumidity::initializeValue()");		
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @throw ComponentErrors::PropertyError
	 * @param timestamp epoch when the operation completes
	*/ 
	CORBA::Double  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		// get the CommandLine .......
		try {
			CError err;
			CString rdata="";
			CSecAreaResourceWrapper<SRTWeatherSocket> sock=m_socket->Get();
			m_val=sock->getHumidity();
			return m_val;


		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"DevIOHumidity::read()");
			dummy.setPropertyName("systemTemperature");
			dummy.setReason("Property could not be read");
			//_IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy,LM_DEBUG);
			throw dummy;
		} 				
		timestamp=getTimeStamp();  //complition time
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
	CSecureArea<SRTWeatherSocket>* m_socket;
	WeatherStationData m_wsdata; 

	CORBA::Double m_val;
	bool m_initparser;
 };



#endif /*DevIOHumidity_H_*/
