#ifndef _DEVIOMOTIONINFO_H_
#define _DEVIOMOTIONINFO_H_

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

class DevIOMotionInfo: public virtual DevIO<ACE_CString>
{
public:

	DevIOMotionInfo(MSBossConfiguration* conf): m_configuration(conf) {
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
		return (m_configuration->motionInfo()).c_str();
    }

    void write(const ACE_CString& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl) {
    	AUTO_TRACE("DevIOMotionInfo::write()");
	}

private:
	MSBossConfiguration *m_configuration;
};


#endif
