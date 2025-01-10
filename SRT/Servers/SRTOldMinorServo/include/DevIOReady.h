#ifndef DEVIOREADY_H_
#define DEVIOREADY_H_


#include <baciDevIO.h>

using namespace baci;

class DevIOReady: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOReady(MSBossConfiguration* conf): m_configuration(conf) {
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
		if (m_configuration->isConfigured()) {
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
	MSBossConfiguration *m_configuration;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
