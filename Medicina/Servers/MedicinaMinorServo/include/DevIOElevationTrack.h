#ifndef DEVIOELEVATIONTRACK_H_ 
#define DEVIOELEVATIONTRACK_H_ 

#include <baciDevIO.h>

#include "MedMinorServoStatus.hpp"

using namespace baci;

class DevIOElevationTrack: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOElevationTrack(MedMinorServoStatus *status): m_status(status) {
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
		if(m_status->elevation_tracking) {
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
	MedMinorServoStatus *m_status;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
