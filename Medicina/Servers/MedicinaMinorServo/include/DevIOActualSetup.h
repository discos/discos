#ifndef _DEVIOACTUALSETUP_H_
#define _DEVIOACTUALSETUP_H_

#include <baciDevIO.h>
#include <IRA>

#include "MedMinorServoParameters.hpp"

using namespace baci;

/**
 * This  class is derived from the template DevIO. It is used by the by the mode  property.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia<br>
 */
class DevIOActualSetup: public virtual DevIO<ACE_CString>
{
public:

	DevIOActualSetup(MedMinorServoParameters *conf): m_configuration(conf) {
		AUTO_TRACE("DevIOActualSetup::DevIOActualSetup()");
	}

	~DevIOActualSetup() {
		AUTO_TRACE("DevIOActualSetup::~DevIOActualSetup()");
	}

	bool initializeValue(){
		return false;
	}

	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
		AUTO_TRACE("DevIOActualSetup::read()");
		timestamp=getTimeStamp();
        if(m_configuration){
		    return (m_configuration->get_name()).c_str();
        }else{
            return "unconfigured";
        }
    }

    void write(const ACE_CString& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOActualSetup::write()");
	}

private:
	MedMinorServoParameters *m_configuration;
};


#endif
