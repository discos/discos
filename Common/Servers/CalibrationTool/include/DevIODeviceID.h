#ifndef _DEVIODEVICEID_H_
#define _DEVIODEVICEID_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DevIODeviceID.h,v 1.1 2011-01-04 09:40:59 c.migoni Exp $            */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

namespace CalibrationTool_private {

/**
 * This  class is derived from the template DevIO. It is used by the deviceID property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * @remarks Last compiled under ACS 7.0.2<br>
 * */

class DevIODeviceID: public virtual DevIO<CORBA::Long>
{
	
public:
	
	DevIODeviceID(CSecureArea<CalibrationTool_private::CDataCollection>* data): m_data(data) { 
		AUTO_TRACE("DevIODeviceID::DevIODeviceID()");
	}
	
	~DevIODeviceID() {
		AUTO_TRACE("DevIODeviceID::~DevIODeviceID()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> resource=m_data->Get();
		AUTO_TRACE("DevIODeviceID::read()");
		timestamp=getTimeStamp();
		return resource->getDevice();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIODeviceID::write()");
	}
    
private:
    CSecureArea<CalibrationTool_private::CDataCollection> *m_data;
};

};
#endif /*DEVIODEVICEID_H_*/
