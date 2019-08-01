
#include <new>
#include <ComponentErrors.h>
#include <LogFilter.h>
#include "AntennaBossImpl.h"
#include "DevIOObservedHorizontal.h"
#include "DevIOObservedEquatorial.h"
#include "DevIOObservedGalactic.h"
#include "DevIOGeneratorType.h"
#include "DevIOEnable.h"
#include "DevIOCorrectionOffsets.h"
#include "DevIORawHorizontal.h"
#include "DevIOStatus.h"
#include "DevIOHPBW.h"
#include "DevIOTargetCoordinate.h"
#include "DevIOTargetName.h"
#include "DevIOOffsets.h"
#include "DevIOLambda.h"
#include "DevIOVradDefinition.h"
#include "DevIOVradReferenceFrame.h"

/*static char *rcsId="@(#) $Id: AntennaBossImpl.cpp,v 1.29 2011-06-05 14:44:40 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);*/

using namespace SimpleParser;

AntennaBossImpl::AntennaBossImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) : 
	CharacteristicComponentImpl(CompName,containerServices),
	m_ptarget(this),
	m_prawAzimuth(this),
	m_prawElevation(this),
	m_pobservedAzimuth(this),
	m_pobservedElevation(this),
	m_pobservedRightAscension(this),
	m_pobservedDeclination(this),
	m_pobservedGalLongitude(this),
	m_pobservedGalLatitude(this),
	m_pgeneratorType(this),
	m_pstatus(this),
	m_penabled(this),
	m_ppointingAzimuthCorrection(this),
	m_ppointingElevationCorrection(this),
	m_prefractionCorrection(this),
	m_pFWHM(this),
	m_pwaveLength(this),
	m_ptargetFlux(this),
	m_pcorrectionEnabled(this),
	m_ptargetRightAscension(this),
	m_ptargetDeclination(this),
	m_ptargetVrad(this),
	m_pvradReferenceFrame(this),
    m_pvradDefinition(this),
	m_pazimuthOffset(this),
	m_pelevationOffset(this),
	m_prightAscensionOffset(this),
	m_pdeclinationOffset(this),
	m_plongitudeOffset(this),
	m_platitudeOffset(this),
	m_core(NULL)
{	
	AUTO_TRACE("AntennaBossImpl::AntennaBossImpl()");
}

AntennaBossImpl::~AntennaBossImpl()
{
	AUTO_TRACE("AntennaBossImpl::~AntennaBossImpl()");
	// put this instruction here because I don't want that the SecureArea die before the mutex is released in clean-up function
	if (m_core!=NULL) delete m_core;
}

void AntennaBossImpl::initialize() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("AntennaBossImpl::initialize()");
	CBossCore *boss;
	ACS_LOG(LM_FULL_INFO,"AntennaBossImpl::initialize()",(LM_INFO,"AntennaBoss::COMPSTATE_INITIALIZING"));
	try {
		m_config.init(getContainerServices());    //throw CDBAcessExImpl;
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"AntennaBossImpl::initialize()");
		throw _dummy;
	}
	try {
		boss=(CBossCore *)new CBossCore(getContainerServices(),&m_config,this);
		m_core=new IRA::CSecureArea<CBossCore>(boss);
		m_ptarget=new ROstring(getContainerServices()->getName()+":target",getComponent(),new DevIOTargetName(m_core),true);
		m_prawAzimuth=new ROdouble(getContainerServices()->getName()+":rawAzimuth",getComponent(),
				new DevIORawHorizontal(m_core,DevIORawHorizontal::AZIMUTH),true);
		m_prawElevation=new ROdouble(getContainerServices()->getName()+":rawElevation",getComponent(),
				new DevIORawHorizontal(m_core,DevIORawHorizontal::ELEVATION),true);
		m_pobservedAzimuth=new ROdouble(getContainerServices()->getName()+":observedAzimuth",getComponent(),
				new DevIOObservedHorizontal(m_core,DevIOObservedHorizontal::AZIMUTH),true);
		m_pobservedElevation=new ROdouble(getContainerServices()->getName()+":observedElevation",getComponent(),
				new DevIOObservedHorizontal(m_core,DevIOObservedHorizontal::ELEVATION),true);
		m_pobservedRightAscension=new ROdouble(getContainerServices()->getName()+":observedRightAscension",getComponent(),
				new DevIOObservedEquatorial(m_core,DevIOObservedEquatorial::RIGHTASCENSION),true);
		m_pobservedDeclination=new ROdouble(getContainerServices()->getName()+":observedDeclination",getComponent(),
				new DevIOObservedEquatorial(m_core,DevIOObservedEquatorial::DECLINATION),true);
		m_pobservedGalLongitude=new ROdouble(getContainerServices()->getName()+":observedGalLongitude",getComponent(),
				new DevIOObservedGalactic(m_core,DevIOObservedGalactic::LONGITUDE),true);
		m_pobservedGalLatitude=new ROdouble(getContainerServices()->getName()+":observedGalLatitude",getComponent(),
				new DevIOObservedGalactic(m_core,DevIOObservedGalactic::LATITUDE),true);		
		m_pgeneratorType=new ROEnumImpl<ACS_ENUM_T(Antenna::TGeneratorType),POA_Antenna::ROTGeneratorType>(
		  getContainerServices()->getName()+":generatorType",getComponent(),new DevIOGeneratorType(m_core),true);
		m_pstatus=new ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus>
		  (getContainerServices()->getName()+":status",getComponent(),new DevIOStatus(m_core),true);	
		m_penabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":enabled",getComponent(),new DevIOEnable(m_core,DevIOEnable::COMPONENT),true);		
		m_ppointingAzimuthCorrection=new ROdouble(getContainerServices()->getName()+":pointingAzimuthCorrection",getComponent(),
				new DevIOCorrectionOffsets(m_core,DevIOCorrectionOffsets::POINTINGAZIMUTHOFFSET),true);
		m_ppointingElevationCorrection=new ROdouble(getContainerServices()->getName()+":pointingElevationCorrection",getComponent(),
				new DevIOCorrectionOffsets(m_core,DevIOCorrectionOffsets::POINTINGELEVATIONOFFSET),true);
		m_prefractionCorrection=new ROdouble(getContainerServices()->getName()+":refractionCorrection",getComponent(),
				new DevIOCorrectionOffsets(m_core,DevIOCorrectionOffsets::REFRACTIONOFFSET),true);
		m_pFWHM=new ROdouble(getContainerServices()->getName()+":FWHM",getComponent(),
				new DevIOHPBW(m_core),true);
		m_pwaveLength=new ROdouble(getContainerServices()->getName()+":waveLength",getComponent(),	new DevIOLambda(m_core),true);
		m_pcorrectionEnabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":correctionEnabled",getComponent(),new DevIOEnable(m_core,DevIOEnable::CORRECTION),true);
		m_ptargetRightAscension=new ROdouble(getContainerServices()->getName()+":targetRightAscension",getComponent(),
				new DevIOTargetCoordinate(m_core,DevIOTargetCoordinate::RIGHTASCENSION),true);
		m_ptargetDeclination=new ROdouble(getContainerServices()->getName()+":targetDeclination",getComponent(),
				new DevIOTargetCoordinate(m_core,DevIOTargetCoordinate::DECLINATION),true);
		m_ptargetVrad=new ROdouble(getContainerServices()->getName()+":targetVrad",getComponent(),
				new DevIOTargetCoordinate(m_core,DevIOTargetCoordinate::VRAD),true);
		m_pvradReferenceFrame=new ROEnumImpl<ACS_ENUM_T(Antenna::TReferenceFrame), POA_Antenna::ROTReferenceFrame>(
				getContainerServices()->getName()+":vradReferenceFrame",getComponent(),new DevIOVradReferenceFrame(m_core),true);
		m_pvradDefinition=new  ROEnumImpl<ACS_ENUM_T(Antenna::TVradDefinition), POA_Antenna::ROTVradDefinition>(
				getContainerServices()->getName()+":vradDefinition",getComponent(),new DevIOVradDefinition(m_core),true) ;
		m_ptargetFlux=new ROdouble(getContainerServices()->getName()+":targetFlux",getComponent(),
				new DevIOTargetCoordinate(m_core,DevIOTargetCoordinate::FLUX),true);
		m_pazimuthOffset=new ROdouble(getContainerServices()->getName()+":azimuthOffset",getComponent(),
				new DevIOOffsets(m_core,DevIOOffsets::AZIMUTHOFF),true);
		m_pelevationOffset=new ROdouble(getContainerServices()->getName()+":elevationOffset",getComponent(),
				new DevIOOffsets(m_core,DevIOOffsets::ELEVATIONOFF),true);
		m_prightAscensionOffset=new ROdouble(getContainerServices()->getName()+":rightAscensionOffset",getComponent(),
				new DevIOOffsets(m_core,DevIOOffsets::RIGHTASCENSIONOFF),true);
		m_pdeclinationOffset=new ROdouble(getContainerServices()->getName()+":declinationOffset",getComponent(),
				new DevIOOffsets(m_core,DevIOOffsets::DECLINATIONOFF),true);
		m_plongitudeOffset=new ROdouble(getContainerServices()->getName()+":longitudeOffset",getComponent(),
				new DevIOOffsets(m_core,DevIOOffsets::LONGITUDEOFF),true);
		m_platitudeOffset=new ROdouble(getContainerServices()->getName()+":latitudeOffset",getComponent(),
				new DevIOOffsets(m_core,DevIOOffsets::LATITUDEOFF),true);
		
		// create the parser for command line execution
		m_parser =  new SimpleParser::CParser<CBossCore>(boss,10);
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"AntennaBossImpl::initialize()");
		throw dummy;
	}
	boss->initialize(); //could throw (ComponentErrors::UnexpectedExImpl)
	try {
		m_watchingThread=getContainerServices()->getThreadManager()->create<CWatchingThread,CBossCore*>
		  ("BOSSWATCHER",boss);
		m_workingThread=getContainerServices()->getThreadManager()->create<CWorkingThread,CSecureArea<CBossCore> *>
		  ("BOSSWORKER",m_core);
	}
	catch (acsthreadErrType::acsthreadErrTypeExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,_dummy,ex,"AntennaBoss::initialize()");
		throw _dummy;
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"AntennaBossImpl::initialize()");
	}
	// inform the boss core of the thread in charge of trajectory update
	boss->setWorkingThread(m_workingThread);
	// configure the parser.....
	m_parser->add("antennaDisable",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::disable),0);
	m_parser->add("antennaEnable",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::enable),0);
	m_parser->add("antennaCorrectionOn",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::enableCorrection),0);
	m_parser->add("antennaCorrectionOff",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::disableCorrection),0);
	m_parser->add("antennaPark",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::stow),0);
	m_parser->add("antennaStop",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::stop),0);
	m_parser->add("antennaAzEl",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::setPreset),0);
	m_parser->add("antennaTrack",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::setProgramTrack),0);
	m_parser->add("antennaUnstow",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::unstow),0);
	m_parser->add("antennaSetup",new function1<CBossCore,non_constant,void_type,I<string_type> >(boss,&CBossCore::setup),1);
	m_parser->add("preset",new function2<CBossCore,non_constant,void_type,I<angle_type<rad> >,I<angle_type<rad> > >(boss,&CBossCore::preset),2);
	//m_parser->add("vlsr",new function1<CBossCore,non_constant,void_type,I<double_type> >(boss,&CBossCore::setVlsr),1);
	m_parser->add("fwhm",new function2<CBossCore,non_constant,void_type,I<angle_type<rad> >, I<double_type> >(boss,&CBossCore::setFWHM),2);
	m_parser->add("azelOffsets",new function2<CBossCore,non_constant,void_type,I<angleOffset_type<rad> >,I<angleOffset_type<rad> > >(boss,&CBossCore::setHorizontalOffsets),2);
	m_parser->add("radecOffsets",new function2<CBossCore,non_constant,void_type,I<angleOffset_type<rad> >,I<angleOffset_type<rad> > >(boss,&CBossCore::setEquatorialOffsets),2);
	m_parser->add("lonlatOffsets",new function2<CBossCore,non_constant,void_type,I<angleOffset_type<rad> >,I<angleOffset_type<rad> > >(boss,&CBossCore::setGalacticOffsets),2);
	//m_parser->add("skydipOTF",new function3<CBossCore,non_constant,time_type,I<elevation_type<rad,false> >,I<elevation_type<rad,false> >,I<interval_type> >(boss,&CBossCore::skydip),3);
	m_parser->add("antennaReset",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::resetFailures),0);
	//m_parser->add("goOff",new function2<CBossCore,non_constant,void_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<double_type > >(boss,&CBossCore::goOff),2);
	m_parser->add("radialVelocity",new function3<CBossCore,non_constant,void_type,I<  basic_type<double,double_converter,VRad_WildCard> >,
			I<enum_type<ReferenceFrame_converter,Antenna::TReferenceFrame,ReferenceFrame_WildCard> >,
			I<enum_type<VradDefinition_converter,Antenna::TVradDefinition,VradDefinition_WildCard> >  >(boss,&CBossCore::radialVelocity),3);
	ACS_LOG(LM_FULL_INFO,"AntennaBossImpl::initialize()",(LM_INFO,"COMPSTATE_INITIALIZED"));
}

void AntennaBossImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("AntennaBossImpl::execute()");
	CSecAreaResourceWrapper<CBossCore>  core=m_core->Get();
	try {
		core->execute();
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::InitializationProblemExImpl,_dummy,E,"AntennaBossImpl::execute()");
		throw _dummy;
	}
	//starts the threads....
	m_workingThread->setSleepTime(m_config.getWorkingThreadTime());
	m_watchingThread->setSleepTime(m_config.getWatchingThreadTime());
	m_watchingThread->resume();
	// working thread will started only if a new tracking is commanded
	try {
		startPropertiesMonitoring();
	}
	catch (acsthreadErrType::CanNotStartThreadExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"AntennaBossImpl::execute()");
		throw __dummy;
	}
	catch (ACSErrTypeCommon::NullPointerExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::ThreadErrorExImpl,__dummy,E,"AntennaBossImpl::execute()");
		throw __dummy;		
	}
	ACS_LOG(LM_FULL_INFO,"AntennaBossImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void AntennaBossImpl::cleanUp()
{
	AUTO_TRACE("AntennaBossImpl::cleanUp()");
	stopPropertiesMonitoring();
	if (m_watchingThread!=NULL) {
		m_watchingThread->suspend();
		getContainerServices()->getThreadManager()->destroy(m_watchingThread);
	}
	if (m_workingThread!=NULL) {
		m_workingThread->suspend();	
		getContainerServices()->getThreadManager()->destroy(m_workingThread);
	}
	ACS_LOG(LM_FULL_INFO,"AntennaBossImpl::cleanUp()",(LM_INFO,"AntennaBoss::THREADS_TERMINATED"));
	if (m_parser!=NULL) delete m_parser;
	ACS_LOG(LM_FULL_INFO,"AntennaBossImpl::cleanUp()",(LM_INFO,"AntennaBoss::PARSER_FREED"));
	CSecAreaResourceWrapper<CBossCore>  core=m_core->Get();
	core->cleanUp();
	ACS_LOG(LM_FULL_INFO,"AntennaBossImpl::cleanUp()",(LM_INFO,"AntennaBoss::BOSS_CORE_FREED"));
	ACS_LOG(LM_FULL_INFO,"AntennaBossImpl::cleanUp()",(LM_INFO,"AntennaBoss::LOG_FLUSHED"));	
	CharacteristicComponentImpl::cleanUp();	
}

void AntennaBossImpl::aboutToAbort()
{
	AUTO_TRACE("AntennaBossImpl::aboutToAbort()");
	if (m_watchingThread!=NULL) m_watchingThread->suspend();
	if (m_workingThread!=NULL) m_workingThread->suspend();
	getContainerServices()->getThreadManager()->destroy(m_watchingThread);
	getContainerServices()->getThreadManager()->destroy(m_workingThread);
	CSecAreaResourceWrapper<CBossCore>  core=m_core->Get();
	core->cleanUp();
	if (m_parser!=NULL) delete m_parser;
}

void AntennaBossImpl::park() throw (CORBA::SystemException,ManagementErrors::ParkingErrorEx)
{
	AUTO_TRACE("AntennaBossImpl::park()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try { 
		resource->stow();	
	}
	catch (ManagementErrors::ParkingErrorExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getParkingErrorEx();
	}	
}

void AntennaBossImpl::setup(const char *config) throw (CORBA::SystemException,ManagementErrors::ConfigurationErrorEx)
{
	AUTO_TRACE("AntennaBossImpl::setup()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try { 
		resource->setup(config);	
	}
	catch (ManagementErrors::ConfigurationErrorExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getConfigurationErrorEx();
	}
}

CORBA::Boolean AntennaBossImpl::command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException)
//char * AntennaBossImpl::command(const char *cmd) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx)
{
	IRA::CString out;
	bool res;
	//taking the mutex before execution is a workaround for thread safeness that works only because the parser is configured for only synchronous calls.
	//If this condition is not true anymore, the mutex acquisition will have to be done directly inside the procedure body.
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:command");
	try {
		m_parser->run(cmd,out);
		res=true;
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		res=false;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_ERROR); // the errors resulting from the execution are logged here as stated in the documentation of CommandInterpreter interface, while the parser errors are never logged.
		res=false;
	}
	answer=CORBA::string_dup((const char *)out);
	return res;
}

void AntennaBossImpl::setOffsets(CORBA::Double lonOff,CORBA::Double latOff,Antenna::TCoordinateFrame frame) throw (
		CORBA::SystemException,ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx)
{	
	AUTO_TRACE("AntennaBossImpl::setOffsets()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		resource->setOffsets(lonOff,latOff,frame);
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getAntennaErrorsEx();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
}

void AntennaBossImpl::stop() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("AntennaBossImpl::stop()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try { 
		resource->stop();	
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}	
}

void AntennaBossImpl::disable() throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::disable()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->disable();
}

void AntennaBossImpl::setFWHM(CORBA::Double value,CORBA::Double waveLen) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::setFWHM");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->setFWHM(value,waveLen);
}

void AntennaBossImpl::computeFWHM(CORBA::Double taper,CORBA::Double waveLength) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::computeFWHM");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->computeFWHM(taper,waveLength);
}

void AntennaBossImpl::reset() throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		resource->resetFailures();
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getAntennaErrorsEx();
	}
}

/*void AntennaBossImpl::setVlsr(CORBA::Double value) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::setvlsr");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->setVlsr(value);
}*/

void AntennaBossImpl::radialVelocity(CORBA::Double vrad, Antenna::TReferenceFrame vref,Antenna::TVradDefinition vdef ) throw (CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->radialVelocity(vrad,vref,vdef);
}

void AntennaBossImpl::getTopocentricFrequency(const ACS::doubleSeq & restFreq,ACS::doubleSeq_out topo) throw (CORBA::SystemException,
		AntennaErrors::AntennaErrorsEx,ComponentErrors::ComponentErrorsEx)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	ACS::doubleSeq_var cent=new ACS::doubleSeq;
	try {
		resource->getTopocentricFrequency(restFreq,cent);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getAntennaErrorsEx();
	}
	topo=cent._retn();
}

void AntennaBossImpl::enable() throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::enable()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->enable();	
}

void AntennaBossImpl::getFluxes (const ACS::doubleSeq& freqs,ACS::doubleSeq_out fluxes) throw (CORBA::SystemException)
{
	ACS::doubleSeq_var outFlux =new ACS::doubleSeq;
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getFluxes");
	resource->getFluxes(freqs,outFlux);
	fluxes=outFlux._retn();
}

void AntennaBossImpl::correctionEnable() throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::correctionEnable()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->enableCorrection();
}

void AntennaBossImpl::correctionDisable() throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::correctionDisable()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->disableCorrection();
}

void AntennaBossImpl::startScan(ACS::Time& startUT,const Antenna::TTrackingParameters& parameters,const Antenna::TTrackingParameters& secondary) throw (
		CORBA::SystemException,AntennaErrors::AntennaErrorsEx,ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("AntennaBossImpl::startScan()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:startScan");
	try {
		resource->startScan(startUT,parameters,secondary);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getAntennaErrorsEx();
	}		
}

void AntennaBossImpl::closeScan(ACS::Time& timeToStop) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx,
		ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("AntennaBossImpl::closeScan()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:closeScan");
	try {
		resource->closeScan(timeToStop);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getAntennaErrorsEx();
	}
}

/*ACS::Time AntennaBossImpl::skydipScan(CORBA::Double el1,CORBA::Double el2,ACS::TimeInterval duration) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		return resource->skydip(el1,el2,duration);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getAntennaErrorsEx();
	}
}*/

void AntennaBossImpl::goOff(Antenna::TCoordinateFrame frame,CORBA::Double beams) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:goOff");
	try {
		resource->goOff(frame,beams);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getAntennaErrorsEx();
	}		
}

bool AntennaBossImpl::checkScan(ACS::Time startUt,const Antenna::TTrackingParameters& parameters,
  const Antenna::TTrackingParameters& secondary,
  CORBA::Double minElLimit,CORBA::Double maxElLimit,Antenna::TRunTimeParameters_out runTime) throw (
  ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::checkScan()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:checkScan");
	try {
		return resource->checkScan(startUt,parameters,secondary,runTime,minElLimit,maxElLimit);
	}
	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getComponentErrorsEx();
	}
	catch (AntennaErrors::AntennaErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
		throw ex.getAntennaErrorsEx();
	}	
}

char *AntennaBossImpl::getGeneratorCURL(Antenna::TGeneratorType_out type) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::getGeneratorCURL()");
	//that's another workaround for the same problem described in "getObservedEquatorial". The cause of that (this function called before
	// m_core is initialized) is a mystery to me.
	if (!m_core) {
		type=Antenna::ANT_NONE;
		return CORBA::string_dup("");
	}
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getGeneratorCurl");
	type=resource->m_generatorType;
	if ((!CORBA::is_nil(resource->m_generator)) && (type!=Antenna::ANT_NONE))  
		return CORBA::string_dup(resource->m_generator->name());
	else 
		return CORBA::string_dup("");	
}

void AntennaBossImpl::getRawCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el) throw (
		CORBA::SystemException)
{	
	double Az=0.0;
	double El=0.0;
	TIMEVALUE requestTime(time);
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getRawcoordinate");
	resource->getRawHorizontal(requestTime,Az,El);
	az=(CORBA::Double)Az;
	el=(CORBA::Double)El;
}

void AntennaBossImpl::getApparentCoordinates (ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,
		CORBA::Double_out lon,CORBA::Double_out lat) throw (CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getApparentCoordinates");
	resource->getApparent(time,az,el,ra,dec,jepoch,lon,lat);
}

void AntennaBossImpl::getObservedEquatorial(ACS::Time time,ACS::TimeInterval duration,CORBA::Double_out ra,CORBA::Double_out dec) throw (
		CORBA::SystemException)
{
	double Ra=0.0;
	double Dec=0.0;
	//workaround for strange behavior: this method seems so be called at startup before initialize() completes
	//this result in a segmentation fault because m_core is not created yet. Has it something to do with the
	//CoordinateGrabber? it strangely uses the methods that are affected by this problem.
	if (!m_core) return;  
	TIMEVALUE requestTime(time);
	TIMEDIFFERENCE requestedDuration(duration);
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getObservedEquatorial");
	resource->getObservedEquatorial(requestTime,requestedDuration,Ra,Dec);
	ra=(CORBA::Double)Ra;
	dec=(CORBA::Double)Dec;
}

void AntennaBossImpl::getObservedGalactic(ACS::Time time,ACS::TimeInterval duration,CORBA::Double_out longitude,CORBA::Double_out latitude) throw (
		CORBA::SystemException)
{
	double Long=0.0;
	double Lat=0.0;
	if (!m_core) return;
	TIMEVALUE requestTime(time);
	TIMEDIFFERENCE requestedDuration(duration);
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getObservedgalactic");
	resource->getObservedGalactic(requestTime,requestedDuration,Long,Lat);
	longitude=(CORBA::Double)Long;
	latitude=(CORBA::Double)Lat;
}

void AntennaBossImpl::getObservedHorizontal(ACS::Time time,ACS::TimeInterval duration,CORBA::Double_out az,CORBA::Double_out el) throw (
		CORBA::SystemException)
{
	double Az=0.0;
	double El=0.0;
	// see GetObservedEquatorial for a comment
	if (!m_core) return;
	TIMEVALUE requestTime(time);
	TIMEDIFFERENCE requestedDuration(duration);
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getObservedHorizontal");
	resource->getObservedHorizontal(requestTime,requestedDuration,Az,El);
	az=(CORBA::Double)Az;
	el=(CORBA::Double)El;
}

void AntennaBossImpl::getAllOffsets(CORBA::Double_out azOff,CORBA::Double_out elOff,CORBA::Double_out raOff,CORBA::Double_out decOff,CORBA::Double_out lonOff,CORBA::Double_out latOff) throw (
		CORBA::SystemException)
{
	double az,el,ra,dec,lon,lat;
	if (!m_core) {
		azOff=elOff=raOff=decOff=lonOff=latOff=0.0;
		return;
	}
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getAllOffsets");
	resource->getAllOffsets(az,el,ra,dec,lon,lat);
	azOff=(CORBA::Double)az; elOff=(CORBA::Double)el; raOff=(CORBA::Double)ra; decOff=(CORBA::Double)dec; lonOff=(CORBA::Double)lon; latOff=(CORBA::Double)lat;
}

void AntennaBossImpl::getScanAxis (Management::TScanAxis_out axis) throw (CORBA::SystemException)
{
	if (!m_core) {
		axis=Management::MNG_NO_AXIS;
		return;
	}
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get("IMPL:getScanAxis");
	axis=resource->getCurrentAxis();
}

_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROstring,m_ptarget,target);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_prawAzimuth,rawAzimuth);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_prawElevation,rawElevation);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pobservedAzimuth,observedAzimuth);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pobservedElevation,observedElevation);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pobservedRightAscension,observedRightAscension);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pobservedDeclination,observedDeclination);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pobservedGalLongitude,observedGalLongitude);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pobservedGalLatitude,observedGalLatitude);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,Antenna::ROTGeneratorType,m_pgeneratorType,generatorType);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,Management::ROTSystemStatus,m_pstatus,status);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,Management::ROTBoolean,m_penabled,enabled);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ppointingAzimuthCorrection,pointingAzimuthCorrection);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ppointingElevationCorrection,pointingElevationCorrection);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_prefractionCorrection,refractionCorrection);	
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pFWHM,FWHM);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pwaveLength,waveLength);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,Management::ROTBoolean,m_pcorrectionEnabled,correctionEnabled);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ptargetRightAscension,targetRightAscension);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ptargetDeclination,targetDeclination);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ptargetVrad,targetVrad);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,Antenna::ROTReferenceFrame,m_pvradReferenceFrame,vradReferenceFrame);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,Antenna::ROTVradDefinition,m_pvradDefinition,vradDefinition);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ptargetFlux,targetFlux);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pazimuthOffset,azimuthOffset);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pelevationOffset,elevationOffset);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_prightAscensionOffset,rightAscensionOffset);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pdeclinationOffset,declinationOffset);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_plongitudeOffset,longitudeOffset);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_platitudeOffset,latitudeOffset);

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(AntennaBossImpl)

/*___oOo___*/

