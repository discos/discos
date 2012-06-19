#include "String.h"
#include "Definitions.h"
#include "IRATools.h"
#include "CustomLoggerUtils.h"
#include "LogDike.h"
#include <acserr.h>
#include "ClientErrors.h"
#include <acsThread.h>
#include <maciSimpleClient.h>

using namespace IRA;

class CWork : public ACS::Thread
{
public:

	CWork(const ACE_CString& name, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime)  : ACS::Thread(name,responseTime,sleepTime),  m_dike(_IRA_LOGDIKE_GETLOGGER)  {}

    ~CWork() { }

    virtual void onStart() {}

     virtual void onStop() {
    	 m_dike.flush();
     }

     virtual void runLoop() {
    	 _IRA_LOGDIKE_CHECK(m_dike);
    	 _IRA_LOGDIKE_LOG(m_dike,LM_NOTICE,"CWork::runLoop()","FILTERED_LOG");
    	 CUSTOM_LOG(LM_FULL_INFO,"CWork::runLoop()",(LM_NOTICE,"NOT_FILTERED_LOG"));
     }
private:
     CLogDike m_dike;
};

class CTask : public ACS::Thread
{
public:

	CTask(const ACE_CString& name, const ACS::TimeInterval& responseTime=ThreadBase::defaultResponseTime,
			const ACS::TimeInterval& sleepTime=ThreadBase::defaultSleepTime)  : ACS::Thread(name,responseTime,sleepTime),  m_dike(_IRA_LOGDIKE_GETLOGGER,10000000,20000000)  {}

    ~CTask() { }

    virtual void onStart() {}

     virtual void onStop() {
    	 m_dike.flush();
     }

     virtual void runLoop() {
    	 _IRA_LOGDIKE_CHECK(m_dike);
    	 _EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"::Main()");
    	 impl.setComponentName("Dummy");
    	 _ADD_EXTRA(impl, CUSTOM_LOGGING_DATA_NAME, CUSTOM_LOGGING_DATA_VALUE);
    	 _ADD_BACKTRACE(ClientErrors::CORBAProblemExImpl,ex,impl,"::Main()");
    	 _IRA_LOGDIKE_EXCEPTION(m_dike,ex,LM_ERROR);
    	 _COMPL(ClientErrors::CouldntAccessPropertyExImpl,com,"::Main()");
    	 _IRA_LOGDIKE_EXCEPTION(m_dike,com,LM_ALERT);
     }
private:
     CLogDike m_dike;
};


int main(int argc, char *argv[])
{
	CWork * work=NULL;
	CTask * task=NULL;
	char ch;
	maci::SimpleClient client;
	ACS::ThreadManager mng;

	CUSTOM_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"MANAGER_LOGGING"));
	try {
		if (client.init(argc,argv)==0) {
			CUSTOM_LOG(LM_FULL_INFO,"::Main()",(LM_CRITICAL,"COULD_NOT_CONNECT"));
			return -1;
		}
		else {
			if (client.login()==0) {
				CUSTOM_LOG(LM_FULL_INFO,"::Main()",(LM_CRITICAL,"COULD_NOT_CONNECT"));
				return -1;
			}
		}
	}
	catch(...) {
		CUSTOM_LOG(LM_FULL_INFO,"::Main()",(LM_CRITICAL,"COULD_NOT_CONNECT"));
		return-1;
	}
	CUSTOM_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"::DONE"));
	try {
		work=mng.create<CWork>("WORKTHREAD");
		task=mng.create<CTask>("TASKTHREAD");
	}
	catch (...) {
		 CUSTOM_LOG(LM_FULL_INFO,":main()",(LM_CRITICAL,"THREAD_ERROR"));
	}
	CUSTOM_LOG(LM_FULL_INFO,":main()",(LM_NOTICE,"START_RUNNING"));
	work->setSleepTime(5000000); // half a second
	task->setSleepTime(7500000); // 3/4 second
	work->resume();
	task->resume();
	scanf("%c",&ch);
	if (work!=NULL) {
		mng.destroy(work);
	}
	if (task!=NULL) {
		mng.destroy(task);
	}
	client.logout();
}
