#ifndef _ACTIVESURFACEBOSSIMPLDEVIOPROFILE_H_
#define _ACTIVESURFACEBOSSIMPLDEVIOPROFILE_H_

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

class ActiveSurfaceBossImplDevIOProfile: public virtual DevIO<ActiveSurface::TASProfile>
{
public:

	ActiveSurfaceBossImplDevIOProfile(ActiveSurfaceBossImpl& impl): m_impl(impl)
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOProfile::ActiveSurfaceBossImplDevIOProfile()");
	}

	~ActiveSurfaceBossImplDevIOProfile()
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOProfile::~ActiveSurfaceBossImplDevIOProfile()");
	}

	bool initializeValue()
    {
		return false;
	}

	ActiveSurface::TASProfile read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOProfile::read()");
		timestamp = getTimeStamp();
        return m_impl.m_profile;
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        AUTO_TRACE("ActiveSurfaceBossImplDevIOProfile::write()");
	}

private:
	const ActiveSurfaceBossImpl& m_impl;
};

#endif /*DEVIOPROFILE_H_*/
