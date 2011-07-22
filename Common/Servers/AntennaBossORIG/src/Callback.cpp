// $Id: Callback.cpp,v 1.3 2010-10-14 12:23:36 a.orlati Exp $

#include "Callback.h"
#include <ComponentErrors.h>
#include "BossCore.h"

void CCallback::working (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
}

void CCallback::done (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
	ACSErr::CompletionImpl comp(c);
	if (comp.isErrorFree()) {
		if (m_operation==STOW) {
			ACS_LOG(LM_FULL_INFO,"CCallback::done()",(LM_NOTICE,"AntennaBoss::ANTENNA_STOWED"));			
		}
		else if (m_operation==SETUP) {
			ACS_LOG(LM_FULL_INFO,"CCallback::done()",(LM_NOTICE,"AntennaBoss::ANTENNA_UNSTOWED"));
			try {
				m_core->setProgramTrack();
			}
			catch (ACSErr::ACSbaseExImpl& ex) {
				_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,ex,"CCallback::done()");
				impl.setReason("Could not change axis mode");
				impl.log();
			}
		}
		else if (m_operation==UNSTOW) {
			ACS_LOG(LM_FULL_INFO,"CCallback::done()",(LM_NOTICE,"AntennaBoss::ANTENNA_UNSTOWED"));
		}
	}
	else {
		ComponentErrors::OperationErrorExImpl ex(comp,__FILE__,__LINE__,"CCallback::done()");
		if (m_operation==STOW) ex.setReason("Antenna cannot be stowed");
		else if (m_operation==UNSTOW) ex.setReason("Antenna cannot be unstowed");
		m_core->changeBossStatus(Management::MNG_WARNING);
		ex.log();
	}
}
