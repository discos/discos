#ifndef DEVIOREADY_H_
#define DEVIOREADY_H_

#include <baciDevIO.h>

#include "MedMinorServoStatus.hpp"

using namespace baci;

class DevIOReady: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOReady(MedMinorServoStatus* status): m_status(status) {
		AUTO_TRACE("DevIOReady::DevIOReady()");
	}
	
	~DevIOReady() {
		AUTO_TRACE("DevIOReady::~DevIOReady()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOReady::read()");
		timestamp=getTimeStamp();
		if (m_status->ready){
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOReady::write()");
	}
    
private:
	MedMinorServoStatus *m_status;
	Management::TBoolean m_val;
};

#endif /*DEVIOREADY_H_*/
