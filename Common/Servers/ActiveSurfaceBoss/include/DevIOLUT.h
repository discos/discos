#ifndef _ACTIVESURFACEBOSSIMPLDEVIOLUT_H_
#define _ACTIVESURFACEBOSSIMPLDEVIOLUT_H_

#include <baciDevIO.h>
#include <IRA>

using namespace baci;

class ActiveSurfaceBossImplDevIOLUT: public virtual DevIO<ACE_CString>
{
public:

	ActiveSurfaceBossImplDevIOLUT(ActiveSurfaceBossImpl& impl): m_impl(impl)
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOLUT::ActiveSurfaceBossImplDevIOLUT()");
	}

	~ActiveSurfaceBossImplDevIOLUT()
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOLUT::~ActiveSurfaceBossImplDevIOLUT()");
	}

	bool initializeValue()
    {
		return false;
	}

	ACE_CString read(ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
		AUTO_TRACE("ActiveSurfaceBossImplDevIOLUT::read()");
		timestamp = getTimeStamp();
		return m_impl.m_LUT.c_str();
    }

    void write(const CORBA::Long& value, ACS::Time& timestamp) throw (ACSErr::ACSbaseExImpl)
    {
        AUTO_TRACE("ActiveSurfaceBossImplDevIOLUT::write()");
	}

private:
	const ActiveSurfaceBossImpl& m_impl;
};

#endif /*DEVIOLUT_H_*/
