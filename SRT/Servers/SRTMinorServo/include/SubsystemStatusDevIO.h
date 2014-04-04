#ifndef _SUBSYSTEM_STATUS_DEVIO_H_
#define _SUBSYSTEM_STATUS_DEVIO_H_


#include <baciDevIO.h>
#include <IRA>
#include "ManagmentDefinitionsC.h"

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the  status property. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */ 
class SubsystemStatusDevIO: public virtual DevIO<Management::TSystemStatus>
{
	
public:
	
	SubsystemStatusDevIO(Management::TSystemStatus *status): m_status(status) { 
		AUTO_TRACE("SubsystemStatusDevIO::SubsystemStatusDevIO()");
	}
	
	~SubsystemStatusDevIO() {
		AUTO_TRACE("SubsystemStatusDevIO::~SubsystemStatusDevIO()");
	}
	
	bool initializeValue() {
		return false;
	}
	
	Management::TSystemStatus  read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("SubsystemStatusDevIO::read()");
		timestamp = getTimeStamp();
        return *m_status;
    }
	
    void write(const ACE_CString &value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
        AUTO_TRACE("SubsystemStatusDevIO::write()");
    }

private:
    Management::TSystemStatus *m_status;
    
};

#endif
