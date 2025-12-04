#ifndef DEVIOELEVATIONTRACK_H_ 
#define DEVIOELEVATIONTRACK_H_ 

#include <baciDevIO.h>

using namespace baci;

class DevIOElevationTrack: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOElevationTrack(MSBossConfiguration* conf): m_configuration(conf) {
		AUTO_TRACE("DevIOElevationTrack::DevIOElevationTrack()");
	}
	
	~DevIOElevationTrack() {
		AUTO_TRACE("DevIOElevationTrack::~DevIOElevationTrack()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOElevationTrack::read()");
		timestamp=getTimeStamp();
		if(m_configuration->isElevationTrackingEn()) {
			m_val=Management::MNG_TRUE;
		}
		else {
			m_val=Management::MNG_FALSE;
		}
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOElevationTrack::write()");
	}
    
private:
	MSBossConfiguration *m_configuration;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
