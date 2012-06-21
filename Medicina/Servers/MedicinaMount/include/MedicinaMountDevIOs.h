#ifndef _DEVIOS_H
#define _DEVIOS_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  26/01/2005     Creation                                         */
/* Andrea Orlati(aorlati@ira.inaf.it)  16/06/2005     Ported from ACS 4.02 to ACS 4.1                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  19/09/2005     Added StatusDevIO                                */
/* Andrea Orlati(aorlati@ira.inaf.it)  03/11/2005	  Introduced one DevIo in place of all the other   */
/* Andrea Orlati(aorlati@ira.inaf.it)  10/11/2006	  Added the capability to read the servo system status   */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/11/2006	  Added the properties azimuthRate and elevationRate  */
/* Andrea Orlati(aorlati@ira.inaf.it)  09/09/2007	  Added a log guard to prevent log flooding in the read method  */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciDevIO.h>
#include <IRA>
#include "MedicinaMountSocket.h"
#include <AntennaErrors.h>
#include <ComponentErrors.h>
#include <LogFilter.h>

/**
 * This template class is derived from DevIO and it is used by the azimuth, elevation, azimuth error and elevation error and so on properties 
 * of the Medicina ACU component. The associeted property can be selected at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br> 
*/
template <class T> class ACUDevIO : public DevIO<T>
{
public:
	/**
	 * List of all possible event in case of the asynchronous mechanism is used. </a>
	*/
	enum TLinkedProperty {
		AZIMUTH,                /*!< the devio will be used to read the azimuth from the ACU */
		ELEVATION,		        /*!< the devio will be used to read the elevation from the ACU */
		AZIMUTH_ERROR,	        /*!< the devio will be used to read the azimuth error from the ACU */
		ELEVATION_ERROR,        /*!< the devio will be used to read the elevation error from the ACU */
		AZIMUTH_MODE,		    /*!< the devio will be used to read the azimuth mode */
		ELEVATION_MODE,         /*!< the devio will be used to read the elevation mode */
		TIME,                   /*!< the devio will be used to read the current ACU time */
		STATUS,				    /*!< the devio will be used to read the ACU status */
		AZIMUTH_SERVO_STATUS,   /*!< the devio will be used to read the status of the azimuth servo system */
		ELEVATION_SERVO_STATUS, /*!< the devio will be used to read the status of the elevation servo system */
		SERVO_SYSTEM_STATUS,	/*|< the devio will be used to read the servo system status */
		PROGRAM_TRACK_POS,		/*!< the devio will be used to read the number of free stack slots to store time tagged program track positions */
		SECTION,				/*!< the devio will be used to read what section of the travel range the antenna is located in */
		AZIMUTH_RATE,			/*!< the devio will be used to read the azimuth rate property*/
		ELEVATION_RATE,         /*!< the devio will be used to read the elevation rate property */
		AZIMUTH_OFF,	        /*!< the devio will be used to read the azimuth offset property */
		ELEVATION_OFF,	        /*!< the devio will be used to read the elevation offset property */
		DELTA_TIME,  	        /*!< the devio will be used to read the delta time property */
		COMMANDED_AZ,           /*!< the devio will be used to read the commanded azimuth property */
		COMMANDED_EL,           /*!< the devio will be used to read the commanded elevation property */
		MOUNTSTATUS             /*!< the devio will be used to read the mountstatus property */
	};
	
	/** 
	 * Constructor
	 * @param Link pointer to a SecureArea that proctects a Medicina ACU socket. This object must be already initialized and configured.
	 * @param Property Indicates which property will make use of this DEVIO.
	*/
	ACUDevIO(CSecureArea<CMedicinaMountSocket>*& Link,TLinkedProperty Property,DDWORD guardInterval=1000000) : 
	  m_pLink(Link), m_Property(Property), m_logGuard(guardInterval)
	{
		switch (m_Property) {
			case AZIMUTH : {
				m_PropertyName=CString("Azimuth");
				break;
			}
			case ELEVATION : {
				m_PropertyName=CString("Elevation");
				break;
			}
			case AZIMUTH_ERROR : {
				m_PropertyName=CString("Azimuth Error");
				break;
			}
			case ELEVATION_ERROR : {
				m_PropertyName=CString("Elevation Error");
				break;
			}
    		case AZIMUTH_MODE: {
				m_PropertyName=CString("Azimuth Mode");
				break;
			}			
			case ELEVATION_MODE: {
				m_PropertyName=CString("Elevation Mode");
				break;
			}			
			case TIME: {
				m_PropertyName=CString("Time");
				break;
			}			
			case STATUS: {
				m_PropertyName=CString("Status");
				break;
			}			
			case AZIMUTH_SERVO_STATUS: {
				m_PropertyName=CString("Azimuth Servo Status");
				break;
			}			
			case ELEVATION_SERVO_STATUS: {
				m_PropertyName=CString("Elevation Servo Status");
				break;
			}	
			case SERVO_SYSTEM_STATUS: {
				m_PropertyName=CString("Servo System Status");
				break;
			}	
			case PROGRAM_TRACK_POS: {
				m_PropertyName=CString("Free Program Track Positions");
				break;
			}
			case SECTION: {
				m_PropertyName=CString("Section");
				break;				
			}	
			case AZIMUTH_RATE : {
				m_PropertyName=CString("Azimuth Rate");
				break;				
			}
			case ELEVATION_RATE : {
				m_PropertyName=CString("Elevation Rate");
				break;								
			}
			case ELEVATION_OFF : {
				m_PropertyName=CString("Elevation Offset");
				break;								
			}
			case AZIMUTH_OFF : {
				m_PropertyName=CString("Azimuth Offset");
				break;								
			}
			case DELTA_TIME : {
				m_PropertyName=CString("Delta Time");
				break;								
			}
			case COMMANDED_AZ : {
				m_PropertyName=CString("Commanded Azimuth");
				break;								
			}
			case COMMANDED_EL : {
				m_PropertyName=CString("Commanded Elevation");
				break;								
			}
			case MOUNTSTATUS: {
				m_PropertyName=CString("Mount Status");
				break;
			}		
		}
		CString trace("ACUDevIO::ACUDevIO() ");
		trace+=m_PropertyName;
		
		AUTO_TRACE((const char*)trace);		
	}

	/**
	 * Destructor
	*/ 
	~ACUDevIO()
	{
		CString trace("ACUDevIO::~ACUDevIO() ");
		trace+=m_PropertyName;
		ACS_TRACE((const char*)trace);		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		CString trace("ACUDevIO::initializeValue() ");
		trace+=m_PropertyName;
		AUTO_TRACE((const char*)trace);		
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @throw ComponentErrors::PropertyError
	 * 	     @arg \c ComponentErrors::Timeout</a>
	 * 	     @arg \c AntennaErrors::Connection</a>	 
	 * 	     @arg \c ComponentErrors::SocketError
	 * @param timestamp epoch when the operation completes
	*/ 
	T read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		CString trace("ACUDevIO::read() ");
		trace+=m_PropertyName;
		ACS_TRACE((const char*)trace);
		if (m_pLink) {
			// get the ACU Socket.......
			CSecAreaResourceWrapper<CMedicinaMountSocket> Socket=m_pLink->Get();
			try {
				switch (m_Property) {
					case AZIMUTH : {
						m_Value=(T)Socket->getAzimuth();
						break;
					}
					case ELEVATION : {
						m_Value=(T)Socket->getElevation();
						break;
					}
					case AZIMUTH_ERROR : {
						m_Value=(T)Socket->getAzimuthError();
						break;
					}
					case ELEVATION_ERROR : {
						m_Value=(T)Socket->getElevationError(); 
						break;
					}
					case AZIMUTH_MODE : {
						m_Value=(T)CMedicinaMountSocket::acuModes2IDL(Socket->getAzimuthMode());
						break;
					}
					case ELEVATION_MODE : {
						m_Value=(T)CMedicinaMountSocket::acuModes2IDL(Socket->getElevationMode());
						break;
					}
					case TIME : {
						m_Value=(T)Socket->getTime();
						break;
					}
					case STATUS : {
						m_Value=(T)Socket->getStatus();
						break;
					}
					case AZIMUTH_SERVO_STATUS : {
						m_Value=(T)Socket->getAzimuthServoStatus();
						break;
					}
					case ELEVATION_SERVO_STATUS : {
						m_Value=(T)Socket->getElevationServoStatus();
						break;
					}
					case SERVO_SYSTEM_STATUS : {
						m_Value=(T)Socket->getServoSystemStatus();
						break;
					}
					case PROGRAM_TRACK_POS: {
						m_Value=(T)Socket->getFreeStackPositions();
						break;
					}
					case SECTION: {
						m_Value=(T)CMedicinaMountSocket::acuSection2IDL(Socket->getSection());
						break;
					}
					case AZIMUTH_RATE: {
						m_Value=(T)Socket->getAzimuthRate();
						break;				
					}
					case ELEVATION_RATE: {
						m_Value=(T)Socket->getElevationRate();
						break;								
					}
					case AZIMUTH_OFF: {
						m_Value=(T)Socket->getAzimuthOffset();
						break;
					}
					case ELEVATION_OFF: {
						m_Value=(T)Socket->getElevationOffset();
						break;
					}
					case DELTA_TIME: {
						m_Value=(T)Socket->getDeltaTime();
						break;
					}
					case COMMANDED_AZ: {
						CACUInterface::TAxeModes azMode;
						azMode=Socket->getAzimuthMode();
						if (azMode==CACUInterface::PROGRAMTRACK)
							m_Value=(T)Socket->getCommandedAzimuth(true);
						else 
							m_Value=(T)Socket->getCommandedAzimuth(false);					
						break;
					}
					case COMMANDED_EL: {
						CACUInterface::TAxeModes elMode;
						elMode=Socket->getElevationMode();
						if (elMode==CACUInterface::PROGRAMTRACK)
							m_Value=(T)Socket->getCommandedElevation(true);
						else 
							m_Value=(T)Socket->getCommandedElevation(false);					
						break;
					}
					case MOUNTSTATUS: {
						m_Value=(T)Socket->getMountStatus();
						break;
					}
				}
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"ACUDevIO::read()");
				dummy.setPropertyName((const char *)m_PropertyName);
				dummy.setReason("Property could not be read");
				_IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy,LM_DEBUG);
				throw dummy;
			}
		} 
		timestamp=getTimeStamp();  //complition time
		return m_Value;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/ 
	void write(const T& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		CString trace("ACUDevIO::write() ");
		trace+=m_PropertyName;
		AUTO_TRACE((const char*)trace);		
		timestamp=getTimeStamp();
		return;
	}
	
private:
	CSecureArea<CMedicinaMountSocket>*& m_pLink;
	T m_Value;
	TLinkedProperty m_Property;
	CString m_PropertyName;
	CLogGuard m_logGuard;
};

#endif /*!_H*/
