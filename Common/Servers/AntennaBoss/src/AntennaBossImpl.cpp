// $Id: AntennaBossImpl.cpp,v 1.29 2011-06-05 14:44:40 a.orlati Exp $

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

static char *rcsId="@(#) $Id: AntennaBossImpl.cpp,v 1.29 2011-06-05 14:44:40 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
	m_pBWHM(this),
	m_pcorrectionEnabled(this),
	m_ptargetRightAscension(this),
	m_ptargetDeclination(this),
	m_ptargetVlsr(this),
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
		m_config.init(getContainerServices());    //thorw CDBAcessExImpl;
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
		m_pBWHM=new ROdouble(getContainerServices()->getName()+":BWHM",getComponent(),
				new DevIOHPBW(m_core),true);
		m_pcorrectionEnabled=new ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean>
		  (getContainerServices()->getName()+":correctionEnabled",getComponent(),new DevIOEnable(m_core,DevIOEnable::CORRECTION),true);
		m_ptargetRightAscension=new ROdouble(getContainerServices()->getName()+":targetRightAscension",getComponent(),
				new DevIOTargetCoordinate(m_core,DevIOTargetCoordinate::RIGHTASCENSION),true);
		m_ptargetDeclination=new ROdouble(getContainerServices()->getName()+":targetDeclination",getComponent(),
				new DevIOTargetCoordinate(m_core,DevIOTargetCoordinate::DECLINATION),true);
		m_ptargetVlsr=new ROdouble(getContainerServices()->getName()+":targetVlsr",getComponent(),
				new DevIOTargetCoordinate(m_core,DevIOTargetCoordinate::VLSR),true);
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
		m_watchingThread=getContainerServices()->getThreadManager()->create<CWatchingThread,CSecureArea<CBossCore> *>
		  ("BOSSWATCHER",m_core);
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
	m_parser->add<0>("antennaDisable",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::disable));
	m_parser->add<0>("antennaEnable",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::enable));
	m_parser->add<0>("antennaPark",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::stow));
	m_parser->add<0>("antennaStop",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::stop));
	m_parser->add<0>("antennaAzEl",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::setPreset));
	m_parser->add<0>("antennaTrack",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::setProgramTrack));
	m_parser->add<0>("antennaUnstow",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::unstow));
	m_parser->add<1>("antennaSetup",new function1<CBossCore,non_constant,void_type,I<string_type> >(boss,&CBossCore::setup));
	m_parser->add<2>("preset",new function2<CBossCore,non_constant,void_type,I<angle_type<rad> >,I<angle_type<rad> > >(boss,&CBossCore::preset));
	m_parser->add<0>("moon",new function0<CBossCore,non_constant,void_type >(boss,&CBossCore::moon));
	m_parser->add<5>("sidereal",new function5<CBossCore,non_constant,void_type,I<string_type>,I<rightAscension_type<rad,true> >,
			I<declination_type<rad,true> >,I<enum_type<AntennaEquinox2String,Antenna::TSystemEquinox > >,I<enum_type<AntennaSection2String,Antenna::TSections> > >(boss,&CBossCore::sidereal));
	m_parser->add<2>("goOff",new function2<CBossCore,non_constant,void_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<angleOffset_type<rad> > >(boss,&CBossCore::goOff));
	m_parser->add<3>("lonOTF",new function3<CBossCore,non_constant,time_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<angleOffset_type<rad> >, I<interval_type> >(boss,&CBossCore::lonOTFScan));
	m_parser->add<3>("latOTF",new function3<CBossCore,non_constant,time_type,I<enum_type<AntennaFrame2String,Antenna::TCoordinateFrame > >,I<angleOffset_type<rad> >, I<interval_type> >(boss,&CBossCore::latOTFScan));
	m_parser->add<1>("vlsr",new function1<CBossCore,non_constant,void_type,I<double_type> >(boss,&CBossCore::setVlsr));
	m_parser->add<1>("bwhm",new function1<CBossCore,non_constant,void_type,I<angle_type<rad> > >(boss,&CBossCore::setBWHM));
	m_parser->add<1>("track",new function1<CBossCore,non_constant,void_type,I<string_type> >(boss,&CBossCore::track));
	m_parser->add<2>("azelOffsets",new function2<CBossCore,non_constant,void_type,I<angleOffset_type<rad> >,I<angleOffset_type<rad> > >(boss,&CBossCore::setHorizontalOffsets));
	m_parser->add<2>("radecOffsets",new function2<CBossCore,non_constant,void_type,I<angleOffset_type<rad> >,I<angleOffset_type<rad> > >(boss,&CBossCore::setEquatorialOffsets));
	m_parser->add<2>("lonlatOffsets",new function2<CBossCore,non_constant,void_type,I<angleOffset_type<rad> >,I<angleOffset_type<rad> > >(boss,&CBossCore::setGalacticOffsets));	
	//m_parser->add<3>("lonOTF",new function3<CBossCore,non_constant,tyme_type,I<rightAscension_type<rad,true> >,I<declination_type<rad,true> >,I<angleOffset_type<rad> >,I<interval_type> >(boss,&CBossCore::elevationScan));
	
	//m_parser->add<4>("azimuthScan",new function4<CBossCore,non_constant,void_type,I<rightAscension_type<rad,true> >,I<declination_type<rad,true> >,I<angleOffset_type<rad> >,I<interval_type> >(boss,&CBossCore::azimuthScan));*/
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

char * AntennaBossImpl::command(const char *cmd) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx)
{
	AUTO_TRACE("AntennaBossImpl::command()");
	IRA::CString out;
	IRA::CString in;
	//taking the mutex before execution is a workround for thread safeness that works only because the parser is configured for only synchronous calls.
	//If this condition is not true anymore, the mutex acquisition will have to be done directly inside the procedure body.
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	in=IRA::CString(cmd);
	try {
		m_parser->run(in,out);
	}
	catch (ParserErrors::ParserErrorsExImpl &ex) {
		_ADD_BACKTRACE(ManagementErrors::CommandLineErrorExImpl,impl,ex,"AntennaBossImpl::command()");
		impl.setCommand(cmd);
		impl.setErrorMessage((const char *)out);
		impl.log(LM_DEBUG);
		throw impl.getCommandLineErrorEx();
	}
	return CORBA::string_dup((const char *)out);		
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

void AntennaBossImpl::setBWHM(CORBA::Double value) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::setBWHM");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->setBWHM(value);
}

void AntennaBossImpl::computeBWHM(CORBA::Double taper,CORBA::Double waveLength) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::computeBWHM");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->computeBWHM(taper,waveLength);
}


void AntennaBossImpl::setVlsr(CORBA::Double value) throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::setvlsr");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->setVlsr(value);	
}

void AntennaBossImpl::enable() throw (CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::enable()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->enable();	
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
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
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

void AntennaBossImpl::track(const char *targetName) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::track()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		resource->track(targetName);
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
		
void AntennaBossImpl::moon() throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::moon()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		resource->moon();
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

void AntennaBossImpl::sidereal(const char * targetName,CORBA::Double ra,CORBA::Double dec,Antenna::TSystemEquinox eq,Antenna::TSections section) throw (
			ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		resource->sidereal(targetName,ra,dec,eq,section);
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

void AntennaBossImpl::goOff(Antenna::TCoordinateFrame frame,CORBA::Double skyOffset) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		resource->goOff(frame,skyOffset);
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

ACS::Time AntennaBossImpl::lonOTFScan(Antenna::TCoordinateFrame scanFrame,CORBA::Double span,ACS::TimeInterval duration) throw (
			ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		return resource->lonOTFScan(scanFrame,span,duration);
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

ACS::Time AntennaBossImpl::latOTFScan(Antenna::TCoordinateFrame scanFrame,CORBA::Double span,ACS::TimeInterval duration) throw (
			ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		return resource->latOTFScan(scanFrame,span,duration);
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

bool AntennaBossImpl::checkScan(ACS::Time startUt,const Antenna::TTrackingParameters& parameters,const Antenna::TTrackingParameters& secondary,ACS::TimeInterval_out slewingTime) throw (
		ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException)
{
	AUTO_TRACE("AntennaBossImpl::checkScan()");
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	try {
		return resource->checkScan(startUt,parameters,secondary,slewingTime); 
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
	if (!m_core)  {
		type=Antenna::ANT_NONE;
		return CORBA::string_dup("");
	}
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
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
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->getRawHorizontal(requestTime,Az,El);
	az=(CORBA::Double)Az;
	el=(CORBA::Double)El;
}

void AntennaBossImpl::getObservedEquatorial(ACS::Time time,ACS::TimeInterval duration,CORBA::Double_out ra,CORBA::Double_out dec) throw (
		CORBA::SystemException)
{
	double Ra=0.0;
	double Dec=0.0;
	//workaround for strange behaviour: this method seems so be called at startup before initialize() completes
	//this result in a segmentation fault because m_core is not created yet. Has it something to do with the
	//CoordinateGrabber? it strangely uses the methods that are affected by this problem.
	if (!m_core) return;  
	TIMEVALUE requestTime(time);
	TIMEDIFFERENCE requestedDuration(duration);
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->getObservedEquatorial(requestTime,requestedDuration,Ra,Dec);
	ra=(CORBA::Double)Ra;
	dec=(CORBA::Double)Dec;
}

void AntennaBossImpl::getObservedGalactic(ACS::Time time,CORBA::Double_out longitude,CORBA::Double_out latitude) throw (
		CORBA::SystemException)
{
	double Long=0.0;
	double Lat=0.0;
	TIMEVALUE requestTime(time);
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->getObservedGalactic(requestTime,Long,Lat);
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
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
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
	CSecAreaResourceWrapper<CBossCore> resource=m_core->Get();
	resource->getAllOffsets(az,el,ra,dec,lon,lat);
	azOff=(CORBA::Double)az; elOff=(CORBA::Double)el; raOff=(CORBA::Double)ra; decOff=(CORBA::Double)dec; lonOff=(CORBA::Double)lon; latOff=(CORBA::Double)lat;
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
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_pBWHM,BWHM);	
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,Management::ROTBoolean,m_pcorrectionEnabled,correctionEnabled);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ptargetRightAscension,targetRightAscension);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ptargetDeclination,targetDeclination);
_PROPERTY_REFERENCE_CPP(AntennaBossImpl,ACS::ROdouble,m_ptargetVlsr,targetVlsr);
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

