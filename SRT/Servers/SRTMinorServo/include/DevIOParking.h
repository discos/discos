#ifndef DEVIOPARKING_H_
#define DEVIOPARKING_H_


#include <baciDevIO.h>

using namespace baci;

class DevIOParking: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOParking(MSBossConfiguration* conf): m_configuration(conf) {
		AUTO_TRACE("DevIOParking::DevIOParking()");
	}
	
	~DevIOParking() {
		AUTO_TRACE("DevIOParking::~DevIOParking()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOParking::read()");
		timestamp=getTimeStamp();
		if (m_configuration->isParking()) {
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOParking::write()");
	}
    
private:
	MSBossConfiguration *m_configuration;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
