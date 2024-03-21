#ifndef DEVIOASCONFIGURATION_H_
#define DEVIOASCONFIGURATION_H_


#include <baciDevIO.h>

using namespace baci;

class DevIOASConfiguration: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOASConfiguration(MSBossConfiguration* conf): m_configuration(conf) {
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
		if (m_configuration->isASConfiguration()) {
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
	MSBossConfiguration *m_configuration;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
