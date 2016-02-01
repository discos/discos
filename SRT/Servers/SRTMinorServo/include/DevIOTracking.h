#ifndef DEVIOTRACKING_H_
#define DEVIOTRACKING_H_


#include <baciDevIO.h>

using namespace baci;

class DevIOTracking: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOTracking(MSBossConfiguration* conf): m_configuration(conf) {
		AUTO_TRACE("DevIOTracking::DevIOTracking()");
	}
	
	~DevIOTracking() {
		AUTO_TRACE("DevIOTracking::~DevIOTracking()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOTracking::read()");
		timestamp=getTimeStamp();
		if (m_configuration->isConfigured() && m_configuration->isTracking()) {
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOTracking::write()");
	}
    
private:
	MSBossConfiguration *m_configuration;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
