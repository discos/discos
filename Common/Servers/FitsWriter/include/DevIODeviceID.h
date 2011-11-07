#ifndef _DEVIODEVICEID_H_
#define _DEVIODEVICEID_H_

/** **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  03/11/2011     Creation                                         */


#include <baciDevIO.h>


namespace FitsWriter_private {

/**
 * This class is derived from template DevIO and it is used by the deviceID property  of the FitsWriter component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
*/
class DevIODeviceID : public DevIO<CORBA::Long>
{
public:

	/**
	 * Constructor
	 */
	DevIODeviceID(CSecureArea<FitsWriter_private::CDataCollection>*data) :  m_data(data)
	{
		AUTO_TRACE("DevIODeviceID::DevIODeviceID()");
	}

	/**
	 * Destructor
	*/
	~DevIODeviceID()
	{
		ACS_TRACE("DevIODeviceID::~DevIODeviceID()");
	}

	/**
	 * @return true to initialize the property with default value from CDB.
	*/
	bool initializeValue()
	{
		AUTO_TRACE("DevIODeviceID::initializaValue()");
		return false;
	}

	/**
	 * Used to read the property value.
	 * @param timestamp epoch when the operation completes
	*/
	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		AUTO_TRACE("DevIODeviceID::read()");
		CSecAreaResourceWrapper<FitsWriter_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		m_val=data->getDeviceID();
		return m_val;
	}

	/**
	 * It writes values into controller. Unused because the properties are read-only.
	*/
	void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
    	AUTO_TRACE("DevIODeviceID::write()");
		timestamp=getTimeStamp();
	}

private:
	CSecureArea<FitsWriter_private::CDataCollection> *m_data;
	CORBA::Long m_val;
};

};


#endif /*_DEVIODEVICEID_H_*/
