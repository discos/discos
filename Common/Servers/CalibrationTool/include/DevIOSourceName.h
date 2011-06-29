#ifndef DEVIOSOURCENAME_H_
#define DEVIOSOURCENAME_H_

/* **************************************************************************/
/* OAC - Osservatorio Astronomico di Cagliari                               */
/* $Id: DevIOSourceName.h,v 1.1 2011-01-04 09:40:59 c.migoni Exp $            */
/*                                                                          */
/* This code is under GNU General Public Licence (GPL).                     */
/*                                                                          */
/* Who                                when            What                  */
/* Carlo Migoni (migoni@ca.astro.it)  23/11/2010      Creation              */

#include <baciDevIO.h>

namespace CalibrationTool_private {

/**
 * This  class is derived from the template DevIO. It is used by the by the fileName property. 
 * @author <a href=mailto:migoni@ca.astro.it>Carlo Migoni</a>,
 * @remarks Last compiled under ACS 7.0.2<br>
 */ 
class DevIOSourceName: public virtual DevIO<ACE_CString>
{
public:
	
	DevIOSourceName(CSecureArea<CalibrationTool_private::CDataCollection>*data): m_data(data) { 
		AUTO_TRACE("DevIOSourceName::DevIOSourceName()");
	}
	
	~DevIOSourceName() {
		AUTO_TRACE("DevIOSourceName::~DevIOSourceName()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
	{
		AUTO_TRACE("DevIOSourceName::read()");
		CSecAreaResourceWrapper<CalibrationTool_private::CDataCollection> data=m_data->Get();
		timestamp=getTimeStamp();
		m_val=(const char *)data->getSourceName();
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOSourceName::write()");
	}
    
private:
	CSecureArea<CalibrationTool_private::CDataCollection> *m_data;
	ACE_CString m_val;
};

};

#endif /*DEVIOSOURCENAME_H_*/
