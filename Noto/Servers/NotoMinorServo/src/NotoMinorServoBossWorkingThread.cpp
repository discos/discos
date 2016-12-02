#include "NotoMinorServoBossWorkingThread.h"

CNotoMinorServoBossWorkingThread::CNotoMinorServoBossWorkingThread(const ACE_CString& name,IRA::CSecureArea<CNotoMinorServoBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CNotoMinorServoBossWorkingThread::CNotoMinorServoBossWorkingThread()");
}

CNotoMinorServoBossWorkingThread::~CNotoMinorServoBossWorkingThread()
{
	AUTO_TRACE("CNotoMinorServoBossWorkingThread::~CNotoMinorServoBossWorkingThread()");
}

void CNotoMinorServoBossWorkingThread::onStart()
{
	AUTO_TRACE("CNotoMinorServoBossWorkingThread::onStart()");
}

void CNotoMinorServoBossWorkingThread::onStop()
{
	 AUTO_TRACE("CNotoMinorServoBossWorkingThread::onStop()");
}

void CNotoMinorServoBossWorkingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();

    try {
        resource->workingMinorServo();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
