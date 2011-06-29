/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: ObservatoryDevIOs.h,v 1.2 2009-03-20 15:42:11 a.orlati Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 08/02/2006     Creation                                           */


#ifndef _DEVIOS_H
#define _DEVIOS_H

#include <baciDevIO.h>
#include <IRA>
#include <SecureArea.h>
#include <Site.h>
#include <DateTime.h>
#include <slamac.h>

/**
 * This template class is derived from DevIO and it is used by the properties of the Observatory component. The associeted 
 * property can be selected at construction time and cannot be changed anymore.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
template <class T> class ObsDevIO : public DevIO<T>
{
public:
	/**
	 * List of all possible properties that can be associated to the component. </a>
	*/
	enum TLinkedProperty {
		UNIVERSALTIME,		/*!< the devio will be used to read the current time */
		JULIANDAY,			/*!< the devio will be used to read the current time as a julian day */
		GAST,	    		/*!< the devio will be used to read the current GST */
		LST,				/*!< the devio will be used to read the current Local Sidereal Time */
		GEOMODEL,			/*!< the devio will be used to read the current model of the geoid */
		LONGITUDE,			/*!< the devio will be used to read the site longitude */
		LATITUDE,			/*!< the devio will be used to read the site latitude */
		HEIGHT,				/*!< the devio will be used to read the site heigth over the sea level */
		XGEOD,				/*!< the devio will be used to read the X geodetic coordinate of the site */
		YGEOD,				/*!< the devio will be used to read the Y geodetic coordinate of the site */
		ZGEOD,				/*!< the devio will be used to read the Z geodetic coordinate of the site */
		XPOLAR,				/*!< the devio will be used to read the X polar motion */
		YPOLAR,			    /*!< the devio will be used to read the Y polar motion */
		DUT1                /*!< the devio will be used to read the delta UT1 */
	};
		
	/** 
	 * Constructor
	 * @param property indicates which property will make use of this DEVIO.
	 * @param site extra parameter passed to the DEVIO, that's the site information object 
	*/
	ObsDevIO(TLinkedProperty property,void *data) : m_Property(property)
	{
		switch (m_Property) {
			case UNIVERSALTIME : {
				m_PropertyName=CString("Current Time");
				break;
			}
			case JULIANDAY : {
				m_PropertyName=CString("Julian Day");
				break;
			}
			case GAST : {
				m_PropertyName=CString("Apparent GST");
				break;
			}
			case LST : {
				m_PropertyName=CString("Local Sidereal Time");
				break;
			}			
			case GEOMODEL : {
				m_PropertyName=CString("Geodetic Model");
				break;
			}
			case LONGITUDE : {
				m_PropertyName=CString("Site Longitude");
				break;
			}
			case LATITUDE : {
				m_PropertyName=CString("Site Latitude");
				break;
			}
			case HEIGHT : {
				m_PropertyName=CString("Site Height");
				break;
			}
			case XGEOD : {
				m_PropertyName=CString("X geodetic coordinate");
				break;
			}
			case YGEOD : {
				m_PropertyName=CString("Y geodetic coordinate");
				break;
			}
			case ZGEOD : {
				m_PropertyName=CString("Z geodetic coordinate");
				break;
			}
			case YPOLAR : {
				m_PropertyName=CString("Y polar motion angle");
				break;
			}
			case XPOLAR : {
				m_PropertyName=CString("X polar motion angle");
				break;
			}				
			case DUT1 : {
				m_PropertyName=CString("Delta UT1");
				break;
			}			
		}
		IRA::CString trace("ObsDevIO::ObsDevIO() ");
		trace+=m_PropertyName;
		m_data=static_cast<CSecureArea<T_DevDataBlock>* >(data);
		AUTO_TRACE((const char*)trace);
	}

	/**
	 * Destructor
	*/ 
	~ObsDevIO()
	{
		IRA::CString trace("ObsDevIO::~ObsDevIO() ");
		trace+=m_PropertyName;
		AUTO_TRACE((const char*)trace);		
	}

	/** 
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{		
		IRA::CString trace("ObsDevIO::initializeValue() ");
		trace+=m_PropertyName;
		AUTO_TRACE((const char*)trace);	
		return false;
	}
	
	/**
	 * Used to read the property value.
	 * @param timestamp epoch of the complitions of the operation
	*/ 
	T read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		IRA::CString trace("ACUDevIO::read() ");
		trace+=m_PropertyName;
		AUTO_TRACE((const char*)trace);		
		// get the ACU Socket.......
		switch (m_Property) {
			case UNIVERSALTIME : {
				TIMEVALUE now;
				CDateTime clock;
				clock.setCurrentDateTime();
				clock.getDateTime(now);
				m_Value=(T)now.value().value;
				break;
			}
			case JULIANDAY : {
				TIMEVALUE now;
				IRA::CIRATools::getTime(now);
				CDateTime clock(now);
				m_Value=(T)clock.getJD();
				break;				
			}
			case GAST : {
				TIMEDIFFERENCE now;
				CDateTime clock;
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
				clock.setDut1(data->dut1);
				clock.setCurrentDateTime();
				// get the CDateTime object that contains the Greenwich sidereal time
				CDateTime nGST=clock.GST((data->site));
				// get the corrisponding TIMEVALUE 
				nGST.getDateTime(now);
				m_Value=(T)now.value().value;
				break;
			}
			case LST : {
				TIMEDIFFERENCE now;
				CDateTime clock;
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
				clock.setDut1(data->dut1);
				clock.setCurrentDateTime();
				// get the CDateTime object that contains the Greenwich sidereal time
				CDateTime LST=clock.LST((data->site));
				LST.getDateTime(now);
				m_Value=(T)now.value().value;								
				break;
			}			
			case GEOMODEL : {				
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
				m_Value=(T)ObservatoryImpl::GeodeticModel2IDL((data->site).getEllipsoid());
				break;				
			}
			case LONGITUDE : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
				m_Value=(T)(data->site.getLongitude()*DR2D);
				break;
			}
			case LATITUDE : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();				
				m_Value=(T)(data->site.getLatitude()*DR2D);				
				break;
			}
			case HEIGHT : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();				
				m_Value=(T)(data->site.getHeight());				
				break;
			}
			case XGEOD : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();				
				m_Value=(T)(data->site.getXPos());							
				break;
			}
			case YGEOD : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();				
				m_Value=(T)(data->site.getYPos());							
				break;
			}
			case ZGEOD : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();				
				m_Value=(T)(data->site.getZPos());							
				break;
			}
			case YPOLAR : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
				m_Value=(T)(data->site.getYPoleMotion());	
				break;
			}
			case XPOLAR : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
				m_Value=(T)(data->site.getXPoleMotion());					
				break;
			}
			case DUT1 : {
				CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
				m_Value=(T)data->dut1;
				break;
			}														
		}
		timestamp=getTimeStamp();  //completion time
		return m_Value;
	}
	
	/**
	 * It writes values into controller. An implementation is given only for LATITUDE,LONGITUDE and HEIGHT.
	 * Even if these properties are read-only, they need to be initialized with the CDB default value.
	*/ 
	void write(const T& value,ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		IRA::CString trace("ObsDevIO::write() ");
		trace+=m_PropertyName;
		AUTO_TRACE((const char*)trace);
		if (m_Property==LONGITUDE) { 
			CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
			m_Value=value;  
			data->site.setLongitude(m_Value*DD2R);
		}
		else if (m_Property==LATITUDE) {
			CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
			m_Value=value;				
			data->site.setLatitude(m_Value*DD2R);				
		}
		else if (m_Property==HEIGHT) {
			CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();				
			m_Value=value;
			data->site.setHeight(m_Value);				
		} 
		else if (m_Property==GEOMODEL) {
			CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();				
			m_Value=value;
			data->site.setEllipsoid(ObservatoryImpl::IDL2GeodeticModel((Antenna::TGeodeticModel)value));		
		}	
		else if (m_Property==DUT1) {
			CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();				
			m_Value=value;
			data->dut1=m_Value;			
		}
		else if (m_Property==XPOLAR) {
			CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
			m_Value=value;
			data->site.setXPoleMotion(m_Value);
		}
		else if (m_Property==YPOLAR) {
			CSecAreaResourceWrapper<T_DevDataBlock> data=m_data->Get();
			m_Value=value;
			data->site.setYPoleMotion(m_Value);
		}	
		timestamp=getTimeStamp();
		return;
	}
	
private:
	T m_Value;
	TLinkedProperty m_Property;
	CString m_PropertyName;
	CSecureArea<T_DevDataBlock>* m_data;
};

#endif /*!_H*/
