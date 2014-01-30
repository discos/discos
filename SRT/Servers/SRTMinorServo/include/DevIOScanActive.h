#ifndef DEVIOSCANACTIVE_H_
#define DEVIOSCANACTIVE_H_


#include <baciDevIO.h>

using namespace baci;

class DevIOScanActive: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOScanActive(MSBossConfiguration* conf): m_configuration(conf) {
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
		if (m_configuration->isScanActive()) {
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
	MSBossConfiguration *m_configuration;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
