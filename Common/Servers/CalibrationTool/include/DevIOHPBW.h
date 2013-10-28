#ifndef _DEVIOHPBW_H_
#define _DEVIOHPBW_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DevIOHPBW.h,v 1.1 2011-01-18 10:48:33 c.migoni Exp $                */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  14/01/2011      Creation              */

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

namespace CalibrationTool_private {

/**
 * This  class is derived from the template DevIO. It is used by the HPBW property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * @remarks Last compiled under ACS 7.0.2<br>
 * */

class DevIOHPBW: public virtual DevIO<CORBA::Double>
{
	
public:
	
	DevIOHPBW(CalibrationTool_private::CDataCollection* data): m_data(data) { 
		AUTO_TRACE("DevIOHPBW::DevIOHPBW()");
	}
	
	~DevIOHPBW() {
		AUTO_TRACE("DevIOHPBW::~DevIOHPBW()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		//CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> resource=m_data->Get();
		AUTO_TRACE("DevIOHPBW::read()");
		timestamp=getTimeStamp();
		return m_data->getHPBW();
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOHPBW::write()");
	}
    
private:
    CalibrationTool_private::CDataCollection *m_data;
};

};
#endif /*DEVIOHPBW_H_*/
