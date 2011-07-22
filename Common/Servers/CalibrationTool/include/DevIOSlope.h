#ifndef _DEVIOSLOPE_H_
#define _DEVIOSLOPE_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DevIOSlope.h,v 1.1 2011-01-18 10:48:33 c.migoni Exp $              */
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
 * This  class is derived from the template DevIO. It is used by the Slope property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * @remarks Last compiled under ACS 7.0.2<br>
 * */

class DevIOSlope: public virtual DevIO<CORBA::Double>
{
	
public:
	
	DevIOSlope(CSecureArea<CalibrationTool_private::CDataCollection>* data): m_data(data) { 
		AUTO_TRACE("DevIOSlope::DevIOSlope()");
	}
	
	~DevIOSlope() {
		AUTO_TRACE("DevIOSlope::~DevIOSlope()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> resource=m_data->Get();
		AUTO_TRACE("DevIOSlope::read()");
		timestamp=getTimeStamp();
		return resource->getSlope();
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOSlope::write()");
	}
    
private:
    CSecureArea<CalibrationTool_private::CDataCollection> *m_data;
};

};
#endif /*DEVIOSLOPE_H_*/
