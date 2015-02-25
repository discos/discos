#ifndef DEVIOSTARTING_H_
#define DEVIOSTARTING_H_


#include <baciDevIO.h>

#include "MedMinorServoStatus.hpp"

using namespace baci;

class DevIOStarting: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOStarting(MedMinorServoStatus *status): m_status(status) {
		AUTO_TRACE("DevIOStarting::DevIOStarting()");
	}
	
	~DevIOStarting() {
		AUTO_TRACE("DevIOStarting::~DevIOStarting()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOStarting::read()");
		timestamp=getTimeStamp();
		if (m_status->starting) {
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOStarting::write()");
	}
    
private:
	MedMinorServoStatus *m_status;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
