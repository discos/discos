#ifndef _ACTIVESURFACEBOSSIMPLDEVIOSTATUS_H_
#define _ACTIVESURFACEBOSSIMPLDEVIOSTATUS_H_

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

class ActiveSurfaceBossImplDevIOStatus: public virtual DevIO<Management::TSystemStatus>
{
public:

	ActiveSurfaceBossImplDevIOStatus(ActiveSurfaceBossImpl& impl): m_impl(impl)
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOStatus::ActiveSurfaceBossImplDevIOStatus()");
	}

	~ActiveSurfaceBossImplDevIOStatus()
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOStatus::~ActiveSurfaceBossImplDevIOStatus()");
	}

	bool initializeValue()
    {
		return false;
	}

	Management::TSystemStatus read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOStatus::read()");
		timestamp = getTimeStamp();
        return m_impl.m_status;
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        AUTO_TRACE("ActiveSurfaceBossImplDevIOStatus::write()");
	}

private:
	const ActiveSurfaceBossImpl& m_impl;
};

#endif /*DEVIOSTATUS_H_*/
