#include <new>
#include <baciDB.h>
#include <ComponentErrors.h>
#include <ManagmentDefinitionsS.h>
#include "SRTDBESMImpl.h"

#include <math.h>

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr SRTDBESMImpl::PROPERTYNAME()\
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}
	
using namespace BackendsErrors;
using namespace ComponentErrors;
using namespace IRA;
using namespace std;


/*static char *rcsId="@(#) $Id: SRTDBESMImpl.cpp,v 1.7 2011-06-03 18:02:49 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);*/

_IRA_LOGFILTER_DECLARE;

SRTDBESMImpl::SRTDBESMImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_compConfiguration(),
	m_commandSocket(),
	m_paddr_1(this)
{	
	AUTO_TRACE("SRTDBESMImpl::SRTDBESMImpl()");
}

SRTDBESMImpl::~SRTDBESMImpl()
{
	AUTO_TRACE("SRTDBESMImpl::~SRTDBESMImpl()");
}

void SRTDBESMImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SRTDBESMImpl::initialize()");
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZING"));
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"READING_CONFIGURATION"));	
	
	m_compConfiguration.init(getContainerServices()); // throw ComponentErrors::CDBAccessExImpl	

	//addr_1 = m_compConfiguration.addr_1();
	/*addr_2 = m_compConfiguration.addr_2();
	addr_3 = m_compConfiguration.addr_3();
	addr_4 = m_compConfiguration.addr_4(); */

	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"CONFIGURATION READED"));
	
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"ACTIVATING_LOG_REPETITION_FILTER"));
	//_IRA_LOGFILTER_ACTIVATE(m_compConfiguration.repetitionCacheTime(),m_compConfiguration.expireCacheTime());
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"THREAD_CREATION"));

	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"PROPERTY_CREATION"));
	
	try {		
		m_paddr_1=new ROlong(getContainerServices()->getName()+":addr_1",getComponent(),
				new DevIO<CORBA::Long>,true);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"SRTDBESMImpl::initialize()");
		throw dummy;
	} 
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SRTDBESMImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	//ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::execute()",(LM_INFO,"CONNECTING_STATUS_SOCKET"));	
	//m_statusSocket.init(&m_compConfiguration,&m_commonData); // this could throw an exception.....
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::execute()",(LM_INFO,"CONNECTING_CONTROL_SOCKET"));
	
	m_commandSocket.init(&m_compConfiguration); // this could throw an exception.....
	
	ACS::Time timestamp;

	m_paddr_1->getDevIO()->write(15,timestamp);
	
   ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::execute()",(LM_INFO,"COMMAND_SOCKET INITIALIZED"));	
		
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::execute()",(LM_INFO,"CONTROL_THREAD_STARTED"));
	
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRTDBESMImpl::execute");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"SRTDBESMImpl::execute");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SRTDBESMImpl::cleanUp()
{
	AUTO_TRACE("SRTDBESMImpl::cleanUp()");
	
	stopPropertiesMonitoring();	

	m_commandSocket.cleanUp();
	
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::cleanUp()",(LM_INFO,"CONTROL_SOCKET_CLOSED"));
	//m_statusSocket.cleanUp();
	ACS_LOG(LM_FULL_INFO,"SRTDBESMImpl::cleanUp()",(LM_INFO,"STATUS_SOCKET_CLOSED"));
	
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	
	CharacteristicComponentImpl::cleanUp();	
}

void SRTDBESMImpl::aboutToAbort()
{
	AUTO_TRACE("SRTDBESMImpl::aboutToAbort()");

	m_commandSocket.cleanUp();
	
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
}

void SRTDBESMImpl::set_all(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
    m_commandSocket.set_all(cfg_name);
    cout << "function set_all called from CommandSocket.cpp\n" << endl;
}

void SRTDBESMImpl::set_mode(short b_addr, const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
    m_commandSocket.set_mode(b_addr, cfg_name);
    cout << "function set_mode called\n" << endl;
}

void SRTDBESMImpl::set_att(short b_addr, short out_ch, double att_val) throw (BackendsErrors::BackendsErrorsEx)
{
    m_commandSocket.set_att(b_addr, out_ch, att_val);
    cout << "function set_att called\n" << endl;
}

void SRTDBESMImpl::store_allmode(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
    m_commandSocket.store_allmode(cfg_name);
    cout << "function store_allmode called\n" << endl;
}

void SRTDBESMImpl::clr_mode(const char * cfg_name) throw (BackendsErrors::BackendsErrorsEx)
{
    m_commandSocket.clr_mode(cfg_name);
    cout << "function clr_mode called\n" << endl;
}


/*
void SRTDBESMImpl::stow(ACS::CBvoid_ptr cb,const ACS::CBDescIn &desc) throw (CORBA::SystemException)
{
	ACS::CBDescOut dout;
	TIMEVALUE now;
	// copy the tag
	try {
		m_commandSocket.stow(); // send the stow command to the ACU
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_DEBUG);
		dout.id_tag=desc.id_tag;
		dout.estimated_timeout=0;
		_COMPL_FROM_EXCPT(OperationErrorCompletion,dummy,ex,"SRTDBESMImpl::stow()"); // create the completion from the exception
		dummy.setReason("Stow command could not be issued");
		try {
			// the done method required a Completion but the CompletionImpl class inherits from Completion so this call warks perfectly 
			cb->done(dummy,dout); // immediately inform the caller that an error occured 
		}
		catch (...) {
		}
		return;
	}
	//register the action!
	IRA::CIRATools::getTime(now);
	getComponent()->registerAction(BACIValue::type_null,cb,desc,this,STOW_ACTION,BACIValue(now.value().value));
}

void SRTDBESMImpl::preset(CORBA::Double az,CORBA::Double el) throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	try {
		m_commandSocket.preset(az,el);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();		
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();		
	}
}

void SRTDBESMImpl::programTrack(CORBA::Double az,CORBA::Double el,ACS::Time time,CORBA::Boolean restart) 
  throw (CORBA::SystemException, ComponentErrorsEx, AntennaErrorsEx)
{
	try {
		m_commandSocket.programTrack(az,el,time,restart);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& E) {
		E.log(LM_DEBUG);
		throw E.getAntennaErrorsEx();
	}
}
 **/


GET_PROPERTY_REFERENCE(ACS::ROlong,m_paddr_1,addr_1);
/*
GET_PROPERTY_REFERENCE(Antenna::ROTStatus,m_pstatusLineStatus,statusLineStatus);

GET_PROPERTY_REFERENCE(ACS::ROlong,m_pprogramTrackPositions,programTrackPositions);

GET_PROPERTY_REFERENCE(Antenna::ROTSections,m_psection,section);

GET_PROPERTY_REFERENCE(ACS::ROdouble,m_pelevationRate,elevationRate);

GET_PROPERTY_REFERENCE(Management::ROTSystemStatus,m_pmountStatus,mountStatus);

GET_PROPERTY_REFERENCE(ACS::ROpattern,m_pgeneralStatus,generalStatus);

GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq,m_pmotorsUtilization,motorsUtilization);

GET_PROPERTY_REFERENCE(ACS::ROlongSeq,m_pmotorsStatus,motorsStatus);
*/

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SRTDBESMImpl)

/*___oOo___*/
