#ifndef _ACTIVESURFACEBOSSIMPLDEVIOTRACKING_H_
#define _ACTIVESURFACEBOSSIMPLDEVIOTRACKING_H_

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

class ActiveSurfaceBossImplDevIOTracking: public virtual DevIO<Management::TBoolean>
{
public:

	ActiveSurfaceBossImplDevIOTracking(ActiveSurfaceBossImpl& impl): m_impl(impl)
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOTracking::ActiveSurfaceBossImplDevIOTracking()");
	}

	~ActiveSurfaceBossImplDevIOTracking()
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOTracking::~ActiveSurfaceBossImplDevIOTracking()");
	}

	bool initializeValue()
    {
		return false;
	}

	Management::TBoolean read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOTracking::read()");
		timestamp = getTimeStamp();
        return m_impl.m_tracking;
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        AUTO_TRACE("ActiveSurfaceBossImplDevIOTracking::write()");
	}

private:
	const ActiveSurfaceBossImpl& m_impl;
};

#endif /*DEVIOTRACKING_H_*/
