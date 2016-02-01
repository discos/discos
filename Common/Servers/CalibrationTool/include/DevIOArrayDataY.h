#ifndef _DEVIOARRAYDATAY_H_
#define _DEVIOARRAYDATAY_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DevIOArrayDataY.h,v 1.1 2011-01-04 09:40:59 c.migoni Exp $          */
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
 * This  class is derived from the template DevIO. It is used by the arrayDataY property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * @remarks Last compiled under ACS 7.0.2<br>
 * */

class DevIOArrayDataY: public virtual DevIO<ACS::doubleSeq>
{
	
public:
	
	DevIOArrayDataY(CalibrationTool_private::CDataCollection* data): m_data(data) { 
		AUTO_TRACE("DevIOArrayDataY::DevIOArrayDataY()");
	}
	
	~DevIOArrayDataY() {
		AUTO_TRACE("DevIOArrayDataY::~DevIOArrayDataY()");
	}
	
	bool initializeValue(){
		return false;
	}
	
    ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		//CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> resource=m_data->Get();
		AUTO_TRACE("DevIOArrayDataY::read()");
		timestamp=getTimeStamp();
		return m_data->getArrayDataY();
    }
	
    void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOArrayDataY::write()");
	}
    
private:
   CalibrationTool_private::CDataCollection *m_data;
};

};
#endif /*DEVIOARRAYDATAY_H_*/
