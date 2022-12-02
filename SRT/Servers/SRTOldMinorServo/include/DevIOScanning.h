#ifndef DEVIOSCANNING_H_
#define DEVIOSCANNING_H_


#include <baciDevIO.h>

using namespace baci;

class DevIOScanning: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOScanning(MSBossConfiguration* conf): m_configuration(conf) {
		AUTO_TRACE("DevIOScanning::DevIOScanning()");
	}
	
	~DevIOScanning() {
		AUTO_TRACE("DevIOScanning::~DevIOScanning()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOScanning::read()");
		timestamp=getTimeStamp();
		if (m_configuration->isScanning()) {
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOScanning::write()");
	}
    
private:
	MSBossConfiguration *m_configuration;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
