#ifndef _DEVIODATAY_H_
#define _DEVIODATAY_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DevIODataY.h,v 1.1 2011-01-04 09:40:59 c.migoni Exp $            */
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
 * This  class is derived from the template DevIO. It is used by the dataY property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * @remarks Last compiled under ACS 7.0.2<br>
 * */

class DevIODataY: public virtual DevIO<CORBA::Double>
{
	
public:
	
	DevIODataY(CalibrationTool_private::CDataCollection* data): m_data(data) { 
		AUTO_TRACE("DevIODataY::DevIODataY()");
	}
	
	~DevIODataY() {
		AUTO_TRACE("DevIODataY::~DevIODataY()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Double read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		//CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> resource=m_data->Get();
		AUTO_TRACE("DevIODataY::read()");
		timestamp=getTimeStamp();
		return m_data->getDataY();
    }
	
    void write(const CORBA::Double& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIODataY::write()");
	}
    
private:
    CalibrationTool_private::CDataCollection *m_data;
};

};
#endif /*DEVIODATAY_H_*/
