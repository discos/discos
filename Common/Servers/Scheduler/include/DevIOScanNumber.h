#ifndef DEVIOSCANNUMBER_H_
#define DEVIOSCANNUMBER_H_

/** ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/12/2008      Creation                                                  */

#include <baciDevIO.h>

/**
 * This  class is derived from the template DevIO. It is used by the by the scanNumber  property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOScanNumber: public virtual DevIO<CORBA::Long>
{
public:
	
	enum TSelector {
		SCANID,
		SUBSCANID
	};

	DevIOScanNumber(CCore* core,const TSelector& ss): m_core(core), m_selector(ss) {
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
		DWORD  scanID,subScanID;
		m_core->getCurrentIdentifiers(scanID,subScanID);
		if (m_selector==SCANID) {
			m_val=(CORBA::Long)scanID;
		}
		else {
			m_val=(CORBA::Long)subScanID;
		}
		timestamp=getTimeStamp();
		return m_val;
    }
	
    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOScanNumber::write()");
	}
    
private:
	CCore *m_core;
	TSelector m_selector;
	CORBA::Long m_val;
};



#endif /*DEVIOSCANNUMBER_H_*/
