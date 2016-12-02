#include "NotoMinorServoBossWatchingThread.h"

CNotoMinorServoBossWatchingThread::CNotoMinorServoBossWatchingThread(const ACE_CString& name,IRA::CSecureArea<CNotoMinorServoBossCore>  *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime), m_core(param)
{
	AUTO_TRACE("CNotoMinorServoBossWatchingThread::CNotoMinorServoBossWatchingThread()");
}

CNotoMinorServoBossWatchingThread::~CNotoMinorServoBossWatchingThread()
{
	AUTO_TRACE("CNotoMinorServoBossWatchingThread::~CNotoMinorServoBossWatchingThread()");
}

void CNotoMinorServoBossWatchingThread::onStart()
{
	AUTO_TRACE("CNotoMinorServoBossWatchingThread::onStart()");
}

void CNotoMinorServoBossWatchingThread::onStop()
{
	 AUTO_TRACE("CNotoMinorServoBossWatchingThread::onStop()");
}

void CNotoMinorServoBossWatchingThread::runLoop()
{
    IRA::CSecAreaResourceWrapper<CNotoMinorServoBossCore> resource=m_core->Get();

    try {
        resource->watchingMinorServoStatus();
    }
    catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    }
}
