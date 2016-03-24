#ifndef DEVIOTRACKING_H_
#define DEVIOTRACKING_H_

#include <baciDevIO.h>

#include "MedMinorServoControl.hpp"

using namespace baci;

class DevIOTracking: public virtual DevIO<Management::TBoolean>
{
public:
	
	DevIOTracking(MedMinorServoStatus * status,
                  MedMinorServoControl_sp control): 
                  m_status(status),
                  m_control(control) {
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
        if(m_status->scan_active)
            if(m_status->scanning)
                m_val=Management::MNG_TRUE;
            else
                m_val=Management::MNG_FALSE;
        else
            if(m_control)
                if (m_control->is_tracking()) {
                    m_val=Management::MNG_TRUE;
                }
                else {
                    m_val = Management::MNG_FALSE;
                }
            else
                m_val = Management::MNG_FALSE;
		return m_val;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOTracking::write()");
	}
    
private:
    MedMinorServoStatus * m_status;
	MedMinorServoControl_sp m_control;
	Management::TBoolean m_val;
};







#endif /*DEVIOTRACKING_H_*/
