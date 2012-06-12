// $Id: RefractionImpl.cpp,v 1.10 2009-01-30 07:29:47 c.migoni Exp $

#include <new>
#include <RefractionImpl.h>

static char *rcsId="@(#) $Id: RefractionImpl.cpp,v 1.10 2009-01-30 07:29:47 c.migoni Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

RefractionImpl::RefractionImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices) : 
	ACSComponentImpl(CompName,containerServices),
    m_core(NULL)
{	
	AUTO_TRACE("RefractionImpl::RefractionImpl()");
}

RefractionImpl::~RefractionImpl()
{
	AUTO_TRACE("RefractionImpl::~RefractionImpl()");
}

void RefractionImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("RefractionImpl::initialize()");
    CRefractionCore *boss;
    ACS_LOG(LM_FULL_INFO,"RefractionImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		boss=(CRefractionCore *)new CRefractionCore(getContainerServices(),this);
		m_core=new IRA::CSecureArea<CRefractionCore>(boss);
    }
    catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"RefractionImpl::initialize()");
		throw dummy;
	}
    boss->initialize(getContainerServices());
    try {
		m_workingThread=getContainerServices()->getThreadManager()->create<CRefractionWorkingThread,CSecureArea<CRefractionCore> *>
		  ("REFRACTIONBOSSWORKER",m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"Refraction::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"RefractionImpl::initialize()");
	}
    ACS_LOG(LM_FULL_INFO, "RefractionImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void RefractionImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("RefractionImpl::execute()");
	CSecAreaResourceWrapper<CRefractionCore>  core=m_core->Get();
    try {
		core->execute();
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"Refraction::execute()");
		throw _dummy;
	}
	//starts the threads....
	m_workingThread->resume();
	/* added in order to avoid to overflood the meteo component with too many requests*/ 
	m_workingThread->setSleepTime(80000000);  //8 seconds of sllep time
	ACS_LOG(LM_FULL_INFO,"RefractionImpl::execute()",(LM_INFO,"Refraction::COMPSTATE_OPERATIONAL"));
}

void RefractionImpl::cleanUp()
{
	AUTO_TRACE("RefractionImpl::cleanUp()");
    CSecAreaResourceWrapper<CRefractionCore>  core=m_core->Get();
    core->byebye();
    if (m_workingThread!=NULL) m_workingThread->suspend();
    getContainerServices()->getThreadManager()->destroy(m_workingThread);
    ACS_LOG(LM_FULL_INFO,"RefractionImpl::cleanUp()",(LM_INFO,"Refraction::THREADS_TERMINATED"));
	core->cleanUp();
	ACS_LOG(LM_FULL_INFO,"RefractionImpl::cleanUp()",(LM_INFO,"Refraction::BOSS_CORE_FREED"));
	ACSComponentImpl::cleanUp();
}

void RefractionImpl::aboutToAbort()
{
	AUTO_TRACE("RefractionImpl::aboutToAbort()");
    if (m_workingThread!=NULL) m_workingThread->suspend();
    getContainerServices()->getThreadManager()->destroy(m_workingThread);
	CSecAreaResourceWrapper<CRefractionCore>  core=m_core->Get(); 
	core->cleanUp();
}

void RefractionImpl::getCorrection (CORBA::Double obsZenithDistance, CORBA::Double_out corZenithDistance) throw (CORBA::SystemException)
{
	AUTO_TRACE("RefractionImpl::getCorrection()");

    CSecAreaResourceWrapper<CRefractionCore> resource=m_core->Get();

	resource->getCorrection(obsZenithDistance, &corZenithDistance);	
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(RefractionImpl)

/*___oOo___*/
