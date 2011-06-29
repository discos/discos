#ifndef _DEVIOS_H
#define _DEVIOS_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DevIOs.h,v 1.4 2011-01-14 18:07:52 a.orlati Exp $									           */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/11/2010     Creation                                         */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciDevIO.h>
#include <IRA>
#include "CommonData.h"
#include <AntennaErrors.h>
#include <ComponentErrors.h>


/**
 * This template class is derived from DevIO and it is used by the almost all attributes of the component SRTMount. 
 * The associeted property can be selected at construction time and cannot be changed anymore.
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
		AZIMUTH_TRACKERROR,	    /*!< the devio will be used to read the azimuth tracking error from the ACU */		
		ELEVATION_TRACKERROR,	/*!< the devio will be used to read the elevation tracking error from the ACU */	
		AZIMUTH_MODE,		    /*!< the devio will be used to read the azimuth mode */
		ELEVATION_MODE,         /*!< the devio will be used to read the elevation mode */
		TIME,                   /*!< the devio will be used to read the current ACU time */
		STATUSLINESTATUS,       /*!< the devio will be used to read the status of the status socket */
		CONTROLLINESTATUS,      /*!< the devio will be used to read the status of the status socket */
		PROGRAM_TRACK_POS,		/*!< the devio will be used to read the number of free stack slots to store time tagged program track positions */
		SECTION,				/*!< the devio will be used to read what section of the travel range the antenna is located in */
		AZIMUTH_RATE,			/*!< the devio will be used to read the azimuth rate property*/
		ELEVATION_RATE,         /*!< the devio will be used to read the elevation rate property */
		AZIMUTH_OFF,	        /*!< the devio will be used to read the azimuth offset property */
		ELEVATION_OFF,	        /*!< the devio will be used to read the elevation offset property */
		DELTA_TIME,  	        /*!< the devio will be used to read the delta time property */
		COMMANDED_AZ,           /*!< the devio will be used to read the commanded azimuth property */
		COMMANDED_EL,           /*!< the devio will be used to read the commanded elevation property */
		COMMANDED_AZ_RATE,      /*!< the devio will be used to read the commandedAzimtuhRate property */
		COMMANDED_EL_RATE,      /*!< the devio will be used to read the commandedElevationRate property */
		COMMANDED_AZ_OFFSET,    /*!< the devio will be used to read the commandedAzimuthOffset property */
		COMMANDED_EL_OFFSET,    /*!< the devio will be used to read the commandedElevationOffset property */
		CABLEWRAP_POSITION,     /*!< the devio will be used to read the cableWrapPosition property */
		CABLEWRAP_TRACKERROR,        /*!< the devio will be used to read the cableWrapError property */
		CABLEWRAP_RATE,          /*!< the devio will be used to read the cableWrapRate property */
		GENERAL_STATUS,          /*!< the devio will be used to read the generalStatus property */
		AZIMUTH_AXIS_STATUS,     /*!< the devio will be used to read the elevationStatus property */
		ELEVATION_AXIS_STATUS,   /*!< the devio will be used to read the elevationStatus property */
		MOUNT_STATUS,			 /*!< the devio will be used to read the mountStatus property */
	};
	
	/** 
	 * Constructor
	 * @param data pointer to a SecureArea that proctects a component common data area. This object must be already initialized and configured.
	 * @param property Indicates which property will make use of this DEVIO.
	*/
	ACUDevIO(IRA::CSecureArea<CCommonData>* data,TLinkedProperty property) :  m_pdata(data), m_property(property)
	{
		switch (m_property) {
			case AZIMUTH : {
				m_propertyName=CString("Azimuth");
				break;
			}
			case ELEVATION : {
				m_propertyName=CString("Elevation");
				break;
			}
			case AZIMUTH_ERROR : {
				m_propertyName=CString("Azimuth Error");
				break;
			}
			case ELEVATION_ERROR : {
				m_propertyName=CString("Elevation Error");
				break;
			}
			case AZIMUTH_TRACKERROR : {
				m_propertyName=CString("Azimuth Tracking Error");
				break;
			}
			case ELEVATION_TRACKERROR : {
				m_propertyName=CString("Elevation Tracking Error");
				break;
			}
    		case AZIMUTH_MODE: {
    			m_propertyName=CString("Azimuth Mode");
				break;
			}			
			case ELEVATION_MODE: {
				m_propertyName=CString("Elevation Mode");
				break;
			}			
			case TIME: {
				m_propertyName=CString("Time");
				break;
			}			
			case STATUSLINESTATUS: {
				m_propertyName=CString("Status Line Status");
				break;
			}
			case CONTROLLINESTATUS: {
				m_propertyName=CString("Control Line Status");
				break;
			}							
			case PROGRAM_TRACK_POS: {
				m_propertyName=CString("Free Program Track Positions");
				break;
			}
			case SECTION: {
				m_propertyName=CString("Section");
				break;				
			}	
			case AZIMUTH_RATE : {
				m_propertyName=CString("Azimuth Rate");
				break;				
			}
			case ELEVATION_RATE : {
				m_propertyName=CString("Elevation Rate");
				break;								
			}
			case ELEVATION_OFF : {
				m_propertyName=CString("Elevation Offset");
				break;								
			}
			case AZIMUTH_OFF : {
				m_propertyName=CString("Azimuth Offset");
				break;								
			}
			case DELTA_TIME : {
				m_propertyName=CString("Delta Time");
				break;								
			}
			case COMMANDED_AZ : {
				m_propertyName=CString("Commanded Azimuth");
				break;								
			}
			case COMMANDED_EL : {
				m_propertyName=CString("Commanded Elevation");
				break;								
			}
			case COMMANDED_AZ_RATE: {
				m_propertyName=CString("Commanded Azimuth Rate");
				break;
			}	
			case COMMANDED_EL_RATE: {
				m_propertyName=CString("Commanded Elevation Rate");
				break;
			}	
			case COMMANDED_AZ_OFFSET: {
				m_propertyName=CString("Commanded Azimuth Offset");
				break;
			}	
			case COMMANDED_EL_OFFSET: {
				m_propertyName=CString("Commanded Elevation Offset");
				break;
			}	
			case CABLEWRAP_POSITION: {
				m_propertyName=CString("Cable Wrap Position");
				break;
			}	
			case CABLEWRAP_TRACKERROR: {
				m_propertyName=CString("Cable Wrap Error");
				break;
			}	
			case CABLEWRAP_RATE: {
				m_propertyName=CString("Cable Wrap Rate");
				break;
			}	
			case GENERAL_STATUS: {
				m_propertyName=CString("General Status");
				break;
			}	
			case AZIMUTH_AXIS_STATUS: {
				m_propertyName=CString("Azimuth Status");
				break;
			}	
			case ELEVATION_AXIS_STATUS: {
				m_propertyName=CString("Elevation Status");
				break;
			}
			case MOUNT_STATUS: {
				m_propertyName=CString("Mount Status");
				break;
			}	
		}
	}

	/**
	 * Destructor
	*/ 
	~ACUDevIO()
	{
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
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
		if (m_pdata) {
			// get the ACU Socket.......
			CSecAreaResourceWrapper<CCommonData> data=m_pdata->Get();
			try {
				switch (m_property) {
					case AZIMUTH : {
						m_value=(T)data->azimuthStatus()->actualPosition();
						break;
					}
					case ELEVATION : {
						m_value=(T)data->elevationStatus()->actualPosition();
						break;
					}
					case AZIMUTH_ERROR : {
						double az,el;
						ACS::Time tm;
						data->getAntennaErrors(az,el,tm);
						m_value=(T)az;
						break;
					}
					case ELEVATION_ERROR : {
						double az,el;
						ACS::Time tm;
						data->getAntennaErrors(az,el,tm);
						m_value=(T)el;
						break;
					}					
					case AZIMUTH_TRACKERROR : {
						m_value=(T)data->azimuthStatus()->positionError();
						break;
					}
					case ELEVATION_TRACKERROR : {
						m_value=(T)data->elevationStatus()->positionError();
						break;
					}
					case AZIMUTH_MODE : {
						Antenna::TCommonModes azMode,elMode;
						data->getActualMode(azMode,elMode);
						m_value=(T)azMode;
						break;
					}
					case ELEVATION_MODE : {
						Antenna::TCommonModes azMode,elMode;
						data->getActualMode(azMode,elMode);
						m_value=(T)elMode;						
						break;
					}
					case TIME : {
						m_value=(T)data->pointingStatus()->actualTime();
						break;
					}
					case STATUSLINESTATUS : {
						m_value=(T)data->getStatusLineState();
						break;
					}
					case CONTROLLINESTATUS : {
						m_value=(T)data->getControlLineState();
						break;
					}					
					case PROGRAM_TRACK_POS: {
						m_value=(T)data->pointingStatus()->pTTLength();
						break;
					}
					case SECTION: {
						m_value=(T)data->pointingStatus()->azimuthSector();
						break;
					}
					case AZIMUTH_RATE: {
						m_value=(T)data->azimuthStatus()->actualVelocity();
						break;				
					}
					case ELEVATION_RATE: {
						m_value=(T)data->elevationStatus()->actualVelocity();
						break;								
					}
					case AZIMUTH_OFF: {
						m_value=(T)data->azimuthStatus()->positionOffset();
						break;
					}
					case ELEVATION_OFF: {
						m_value=(T)data->elevationStatus()->positionOffset();
						break;
					}
					case DELTA_TIME: {
						m_value=(T)data->pointingStatus()->actualTimeOffset();
						break;
					}
					case COMMANDED_AZ: {
						m_value=(T)data->azimuthStatus()->commandedPosition();
						break;
					}
					case COMMANDED_EL: {
						m_value=(T)data->elevationStatus()->commandedPosition();
						break;
					}
					case COMMANDED_AZ_RATE: {
						m_value=(T)data->azimuthStatus()->commandedVelocity();
						break;
					}	
					case COMMANDED_EL_RATE: {
						m_value=(T)data->elevationStatus()->commandedVelocity();
						break;
					}	
					case COMMANDED_AZ_OFFSET: {
						double az,el;
						data->getCommandedOffsets(az,el);
						m_value=(T)az;
						break;
					}	
					case COMMANDED_EL_OFFSET: {
						double az,el;
						data->getCommandedOffsets(az,el);
						m_value=(T)el;
						break;
					}	
					case CABLEWRAP_POSITION: {
						m_value=(T)data->cableWrapStatus()->actualPosition();
						break;
					}	
					case CABLEWRAP_TRACKERROR: {
						m_value=(T)data->cableWrapStatus()->positionError();
						break;
					}	
					case CABLEWRAP_RATE: {
						m_value=(T)data->cableWrapStatus()->actualVelocity();
						break;
					}
					case GENERAL_STATUS: {
						m_value=(T)data->getStatusWord();
						break;
					}	
					case AZIMUTH_AXIS_STATUS: {
						m_value=(T)data->getAzimuthStatusWord();
						break;
					}	
					case ELEVATION_AXIS_STATUS: {
						m_value=(T)data->getElevationStatusWord();
						break;
					}	
					case MOUNT_STATUS: {
						m_value=(T)data->getMountStatus();
						break;
					}									
				}
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"ACUDevIO::read()");
				dummy.setPropertyName((const char *)m_propertyName);
				dummy.setReason("Property could not be read");
				throw dummy;
			}
		} 
		timestamp=getTimeStamp();  //complition time
		return m_value;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/ 
	void write(const T& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}
	
private:
	IRA::CSecureArea<CCommonData>* m_pdata;
	T m_value;
	TLinkedProperty m_property;
	IRA::CString m_propertyName;
};

template<> class ACUDevIO<ACS::longSeq> : public DevIO<ACS::longSeq>
{
public:
	/**
	 * List of all possible event in case of the asynchronous mechanism is used. </a>
	*/
	enum TLinkedProperty {
		MOTORS_STATUS			/*!< the devio will be used to read the motorsStatus property */
	};
	
	/** 
	 * Constructor
	 * @param data pointer to a SecureArea that proctects a component common data area. This object must be already initialized and configured.
	 * @param property Indicates which property will make use of this DEVIO.
	*/
	ACUDevIO(IRA::CSecureArea<CCommonData>* data,TLinkedProperty property) :  m_pdata(data), m_property(property)
	{
		switch (m_property) {
			case MOTORS_STATUS: {
				m_propertyName=CString("Motors Status");
				break;
			}				
		}
	}

	/**
	 * Destructor
	*/ 
	~ACUDevIO()
	{
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
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
	ACS::longSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		if (m_pdata) {
			// get the ACU Socket.......
			CSecAreaResourceWrapper<CCommonData> data=m_pdata->Get();
			try {
				switch (m_property) {
					case MOTORS_STATUS: {
						data->getMotorsStatus(m_value);
						break;
					}									
				}
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"ACUDevIO::read()");
				dummy.setPropertyName((const char *)m_propertyName);
				dummy.setReason("Property could not be read");
				throw dummy;
			}
		} 
		timestamp=getTimeStamp();  //complition time
		return m_value;
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
	IRA::CSecureArea<CCommonData>* m_pdata;
	ACS::longSeq m_value;
	TLinkedProperty m_property;
	IRA::CString m_propertyName;
};

template <> class ACUDevIO<ACS::doubleSeq> : public DevIO<ACS::doubleSeq>
{
public:
	/**
	 * List of all possible event in case of the asynchronous mechanism is used. </a>
	*/
	enum TLinkedProperty {
		MOTORS_POSITION,		/*!< the devio will be used to read the motorsPosition property */
		MOTORS_SPEED,			/*!< the devio will be used to read the motorsSpeed property */
		MOTORS_TORQUE,			/*!< the devio will be used to read the motorsTorque property */
		MOTORS_UTILIZATION,      /*!< the devio will be used to read the motorsUtilization property */
	};
	
	/** 
	 * Constructor
	 * @param data pointer to a SecureArea that proctects a component common data area. This object must be already initialized and configured.
	 * @param property Indicates which property will make use of this DEVIO.
	*/
	ACUDevIO(IRA::CSecureArea<CCommonData>* data,TLinkedProperty property) :  m_pdata(data), m_property(property)
	{
		switch (m_property) {
			case MOTORS_POSITION: {
				m_propertyName=CString("Motors Position");
				break;
			}	
			case MOTORS_SPEED: {
				m_propertyName=CString("Motors Speed");
				break;
			}	
			case MOTORS_TORQUE: {
				m_propertyName=CString("Motprs Torque");
				break;
			}	
			case MOTORS_UTILIZATION: {
				m_propertyName=CString("Motors Utilization");
				break;
			}	
		}
	}

	/**
	 * Destructor
	*/ 
	~ACUDevIO()
	{
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
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
	ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		if (m_pdata) {
			// get the ACU Socket.......
			CSecAreaResourceWrapper<CCommonData> data=m_pdata->Get();
			try {
				switch (m_property) {
					case MOTORS_POSITION: {
						data->getMotorsPosition(m_value);
						break;
					}	
					case MOTORS_SPEED: {
						data->getMotorsSpeed(m_value);
						break;
					}	
					case MOTORS_TORQUE: {
						data->getMotorsTorque(m_value);
						break;
					}	
					case MOTORS_UTILIZATION: {
						data->getMotorsUtilization(m_value);
						break;
					}	
				}
			}
			catch (ACSErr::ACSbaseExImpl& E) {
				_ADD_BACKTRACE(ComponentErrors::PropertyErrorExImpl,dummy,E,"ACUDevIO::read()");
				dummy.setPropertyName((const char *)m_propertyName);
				dummy.setReason("Property could not be read");
				throw dummy;
			}
		} 
		timestamp=getTimeStamp();  //complition time
		return m_value;
	}
	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/ 
	void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		timestamp=getTimeStamp();
		return;
	}
	
private:
	IRA::CSecureArea<CCommonData>* m_pdata;
	ACS::doubleSeq m_value;
	TLinkedProperty m_property;
	IRA::CString m_propertyName;
};




#endif /*!_H*/

