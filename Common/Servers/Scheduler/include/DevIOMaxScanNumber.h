#ifndef DEVIOMAXSCANNUMBER_H_
#define DEVIOMAXSCANNUMBER_H_

/****************************************************************************************************************/
/* This code is under GNU General Public Licence (GPL).                                                         */
/*                                                                                                              */
/* Who                                              When            What                                        */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)      16/04/2026      Creation                                    */

#include <baciDevIO.h>

/**
 * This class is derived from the template DevIO. It is used by the by the masScanNumber and maxSubScanNumber properties. 
 */ 
class DevIOMaxScanNumber: public virtual DevIO<CORBA::Long>
{
public:
	
	enum TSelector {
		SCANID,
		SUBSCANID
	};

	DevIOMaxScanNumber(CCore* core,const TSelector& ss): m_core(core), m_selector(ss) {
		AUTO_TRACE("DevIOMaxScanNumber::DevIOMaxScanNumber()");
	}
	
	~DevIOMaxScanNumber() {
		AUTO_TRACE("DevIOMaxScanNumber::~DevIOMaxScanNumber()");
	}
	
	bool initializeValue(){
		return false;
	}
	
	CORBA::Long read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOMaxScanNumber::read()");
		DWORD  scanID, subScanID;
		m_core->getMaxIdentifiers(scanID,subScanID);
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
    	AUTO_TRACE("DevIOMaxScanNumber::write()");
	}
    
private:
	CCore *m_core;
	TSelector m_selector;
	CORBA::Long m_val;
};



#endif /*DEVIOMAXSCANNUMBER_H_*/
