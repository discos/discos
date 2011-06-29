#ifndef DEVIOSCANNUMBER_H_
#define DEVIOSCANNUMBER_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: DevIOScanNumber.h,v 1.2 2009-01-16 17:44:57 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/12/2008      Creation                                                  */

#include <baciDevIO.h>

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the scanNumber  property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOScanNumber: public virtual DevIO<CORBA::Long>
{
public:
	
	DevIOScanNumber(CCore* core): m_core(core) { 
		AUTO_TRACE("DevIOScanNumber::DevIOScanNumber()");
	}
	
	~DevIOScanNumber() {
		AUTO_TRACE("DevIOScanNumber::~DevIOScanNumber()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOScanNumber::read()");
		DWORD  scanNumber=m_core->getScanNumber();
		timestamp=getTimeStamp();
		m_val=(CORBA::Long) scanNumber;
		return m_val;
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOScanNumber::write()");
	}
    
private:
	CCore *m_core;
	CORBA::Long m_val;
};



#endif /*DEVIOSCANNUMBER_H_*/
