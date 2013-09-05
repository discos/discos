#ifndef _DEVIOARRAYDATAX_H_
#define _DEVIOARRAYDATAX_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DevIOArrayDataX.h,v 1.1 2011-01-04 09:40:59 c.migoni Exp $          */
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
 * This  class is derived from the template DevIO. It is used by the arrayDataX property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * @remarks Last compiled under ACS 7.0.2<br>
 * */

class DevIOArrayDataX: public virtual DevIO<ACS::doubleSeq>
{
	
public:
	
	DevIOArrayDataX(CSecureArea<CalibrationTool_private::CDataCollection>* data): m_data(data) { 
		AUTO_TRACE("DevIOArrayDataX::DevIOArrayDataX()");
	}
	
	~DevIOArrayDataX() {
		AUTO_TRACE("DevIOArrayDataX::~DevIOArrayDataX()");
	}
	
	bool initializeValue(){
		return false;
	}
	
    ACS::doubleSeq read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> resource=m_data->Get();
		AUTO_TRACE("DevIOArrayDataX::read()");
		timestamp=getTimeStamp();
		return resource->getArrayDataX();
    }
	
    void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOArrayDataX::write()");
	}
    
private:
    CSecureArea<CalibrationTool_private::CDataCollection> *m_data;
};

};
#endif /*DEVIOARRAYDATAX_H_*/
