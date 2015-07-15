#ifndef _DEVIOMOTIONINFO_H_
#define _DEVIOMOTIONINFO_H_

#include <baciDevIO.h>
#include <IRA>

#include "MedMinorServoParameters.hpp"

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the mode  property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOMotionInfo: public virtual DevIO<ACE_CString>
{
public:

	DevIOMotionInfo(MedMinorServoStatus *status): m_status(status) {
		AUTO_TRACE("DevIOMotionInfo::DevIOMotionInfo()");
	}

	~DevIOMotionInfo() {
		AUTO_TRACE("DevIOMotionInfo::~DevIOMotionInfo()");
	}

	bool initializeValue(){
		return false;
	}

	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOMotionInfo::read()");
		timestamp=getTimeStamp();
        if(m_status){
		    return (m_status->getStatusString()).c_str();
        }else{
            return "unconfigured";
        }
    }

    void write(const ACE_CString& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOActualSetup::write()");
	}

private:
	MedMinorServoStatus *m_status;
};


#endif
