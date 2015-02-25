#ifndef DEVIOASCONFIGURATION_H_
#define DEVIOASCONFIGURATION_H_


#include <baciDevIO.h>

#include "MedMinorServoStatus.hpp"

using namespace baci;

class DevIOASConfiguration: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOASConfiguration(MedMinorServoStatus *status): m_status(status) {
		AUTO_TRACE("DevIOASConfiguration::DevIOASConfiguration()");
	}
	
	~DevIOASConfiguration() {
		AUTO_TRACE("DevIOASConfiguration::~DevIOASConfiguration()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOASConfiguration::read()");
		timestamp=getTimeStamp();
		if (m_status->as_configuration) {
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOASConfiguration::write()");
	}
    
private:
	MedMinorServoStatus *m_status;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
