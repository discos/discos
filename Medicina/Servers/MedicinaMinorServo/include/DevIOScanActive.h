#ifndef DEVIOSCANACTIVE_H_
#define DEVIOSCANACTIVE_H_


#include <baciDevIO.h>

#include "MedMinorServoStatus.hpp"

using namespace baci;

class DevIOScanActive: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOScanActive(MedMinorServoStatus *status): m_status(status) {
		AUTO_TRACE("DevIOScanActive::DevIOScanActive()");
	}
	
	~DevIOScanActive() {
		AUTO_TRACE("DevIOScanActive::~DevIOScanActive()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOScanActive::read()");
		timestamp=getTimeStamp();
		if (m_status->scan_active) {
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOScanActive::write()");
	}
    
private:
	MedMinorServoStatus *m_status;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
