#ifndef DEVIOLASTSCANNUMBER_H_
#define DEVIOLASTSCANNUMBER_H_

/****************************************************************************************************************/
/* This code is under GNU General Public Licence (GPL).                                                         */
/*                                                                                                              */
/* Who                                              When            What                                        */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)      16/04/2026      Creation                                    */

#include <baciDevIO.h>

/**
 * This class is derived from the template DevIO. It is used by the by the lastScanNumber and lastSubScanNumber properties. 
 */ 
class DevIOLastScanNumber: public virtual DevIO<CORBA::Long>
{
public:
	
	enum TSelector {
		SCANID,
		SUBSCANID
	};

	DevIOLastScanNumber(CCore* core,const TSelector& ss): m_core(core), m_selector(ss) {
		AUTO_TRACE("DevIOLastScanNumber::DevIOLastScanNumber()");
	}
	
	~DevIOLastScanNumber() {
		AUTO_TRACE("DevIOLastScanNumber::~DevIOLastScanNumber()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOLastScanNumber::read()");
		DWORD  scanID, subScanID;
		m_core->getLastIdentifiers(scanID,subScanID);
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



#endif /*DEVIOLASTSCANNUMBER_H_*/
