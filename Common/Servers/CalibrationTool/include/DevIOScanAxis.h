#ifndef DEVIOSCANAXIS_H_
#define DEVIOSCANAXIS_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DevIOScanAxis.h,v 1.1 2011-01-04 09:40:59 c.migoni Exp $            */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */

#include <baciDevIO.h>
#include <IRA>

namespace CalibrationTool_private {

/**
 * This  class is derived from the template DevIO. It is used by the scanAxis property. 
 *  @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * @remarks Last compiled under ACS 7.0.2<br>
 */ 

class DevIOScanAxis: public virtual DevIO<Management::TScanAxis>
{
	
public:
	
	DevIOScanAxis(CSecureArea<CalibrationTool_private::CDataCollection> * data): m_data(data) { 
		AUTO_TRACE("DevIOScanAxis::DevIOScanAxis()");
	}
	
	~DevIOScanAxis() {
		AUTO_TRACE("DevIOScanAxis::~DevIOScanAxis()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	Management::TScanAxis  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) 
	{
		AUTO_TRACE("DevIOScanAxis::read()");
		CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		return data->getScanAxis();
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOScanAxis::write()");
	}
    
private:
	CSecureArea<CalibrationTool_private::CDataCollection> *m_data;
};

};

#endif /*DEVIOSTATUS_H_*/
