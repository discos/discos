#ifndef _DEVIO_STATUS_H_
#define _DEVIO_STATUS_H_


#include <baciDevIO.h>
#include <IRA>
#include "ManagmentDefinitionsC.h"
#include "MedMinorServoStatus.hpp"

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the  status property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class DevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
	
public:
	
	DevIOStatus(MedMinorServoStatus *status): m_status(status) { 
		AUTO_TRACE("SubsystemStatusDevIO::SubsystemStatusDevIO()");
	}
	
	~DevIOStatus() {
		AUTO_TRACE("SubsystemStatusDevIO::~SubsystemStatusDevIO()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TSystemStatus  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("SubsystemStatusDevIO::read()");
		timestamp = getTimeStamp();
        return m_status->status;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
        AUTO_TRACE("SubsystemStatusDevIO::write()");
    }

private:
    MedMinorServoStatus *m_status;
};

#endif
