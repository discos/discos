
#include "BossCore.h"
#include <ObservatoryS.h>
#include <slamac.h>
#include <slalib.h>
#include <math.h>
#include <SkySource.h>
#include <LogFilter.h>
#include <Definitions.h>
#include <FrequencyTracking.h>
#include <math.h>

#define MAXDATAPOINTS 4000
#define MAXRAWDATAPOINTS 2000

// microseconds in which a status is valid and not considered cleared
#define STATUS_VALIDITY 3000000 

_IRA_LOGFILTER_DECLARE;

CBossCore::CBossCore(ContainerServices *service,CConfiguration *conf,acscomponent::ACSComponentImpl *me) : 
	m_observedHorizontals(MAXDATAPOINTS),
	m_observedEquatorials(MAXDATAPOINTS),
	m_integratedObservedEquatorial(MAXDATAPOINTS),
	m_observedGalactics(MAXDATAPOINTS),
	m_rawCoordinates(MAXRAWDATAPOINTS),
	m_services(service),
	m_config(conf),
	m_notificationChannel(NULL),
	m_thisIsMe(me)
{
	m_callbackUnstow=new CCallback(CCallback::UNSTOW),
	m_callbackStow=new CCallback(CCallback::STOW);
	m_callbackSetup=new CCallback(CCallback::SETUP);
	m_cbUnstow=m_callbackUnstow->_this();
	m_cbStow=m_callbackStow->_this();
	m_cbSetup=m_callbackSetup->_this();
	m_lastEncoderRead = ACS::Time(0);
}

CBossCore::~CBossCore()
{
	//this code is to fix an annoying bug that causes the container to crush when closing.....This was due to
	// CORBA was not aware of the callback object deletation.......so when the container closed it tried to
	// clear all servant resulting to an acces to already freed memory.
	PortableServer::ObjectId *stowId=m_callbackStow->_default_POA()->reference_to_id(m_cbStow);
	m_callbackStow->_default_POA()->deactivate_object(*stowId);
	PortableServer::ObjectId *unstowId=m_callbackUnstow->_default_POA()->reference_to_id(m_cbUnstow);
	m_callbackUnstow->_default_POA()->deactivate_object(*unstowId);
	PortableServer::ObjectId *setupId=m_callbackSetup->_default_POA()->reference_to_id(m_cbSetup);
	m_callbackSetup->_default_POA()->deactivate_object(*setupId);
	CORBA::release(m_cbUnstow);
	CORBA::release(m_cbStow);
	CORBA::release(m_cbSetup);
	delete m_callbackUnstow;
	delete m_callbackStow;
	delete m_callbackSetup;
}

void CBossCore::initialize() throw (ComponentErrors::UnexpectedExImpl)
{
	m_observedHorizontals.empty();
	m_observedEquatorials.empty();
	m_observedGalactics.empty();
	m_integratedObservedEquatorial.empty();
	m_rawCoordinates.empty();
	m_generatorType=Antenna::ANT_NONE;
	m_lastPointTime=0;
	m_newTracking=true; // let's start with a new table to be loaded into the mount
	m_integrationStartTime=0;
	m_enable=true;
	m_correctionEnable=false;  ///by default the correction are not enabled....when the setup is called they enabled automatically
	m_correctionEnable_scan=true;
	m_tracking=false;
	m_newScanEpoch=0;
	m_FWHM=0.0;
	m_waveLength=0.0;
	m_currentObservingFrequency=0.0;
	m_currentAxis=Management::MNG_NO_AXIS;
	m_timeToStop=0;
	m_targetRA=m_targetDec=m_targetVrad=m_targetFlux=0.0;
	m_vradDefinition=Antenna::ANT_UNDEF_DEF;
	m_vradReferenceFrame=Antenna::ANT_UNDEF_FRAME;
	m_pointingAzOffset=m_pointingElOffset=m_refractionOffset=0.0;
	m_longitudeOffset=m_latitudeOffset=0.0;
	m_offsetFrame=Antenna::ANT_HORIZONTAL;
	m_userOffset.lon=m_userOffset.lat=0.0; m_userOffset.frame=Antenna::ANT_HORIZONTAL;
	m_scanOffset.lon=m_scanOffset.lat=0.0; m_scanOffset.frame=Antenna::ANT_HORIZONTAL;
	m_lastScanParameters.type=Antenna::ANT_NONE;
	m_lastScanParameters.applyOffsets=false;
	m_lastScanParameters.secondary=false;
	m_lastScanParameters.paramNumber=0;
	m_workingThread=NULL;
	m_callbackUnstow->setBossCore(this);
	m_callbackStow->setBossCore(this);
	m_callbackSetup->setBossCore(this);
	m_targetName=IRA::CString("none");
	m_bossStatus=Management::MNG_OK;
	m_status=Management::MNG_OK;
	m_lastStatusChange.value((long double)0.0);
	m_currentAxis=Management::MNG_NO_AXIS;
	ACS_LOG(LM_FULL_INFO,"CBossCore::initialize()",(LM_INFO,"AntennaBoss::CONNECTING_NOTIFICATION_CHANNEL"));
	try {
		m_notificationChannel=new nc::SimpleSupplier(Antenna::ANTENNA_DATA_CHANNEL,m_thisIsMe);
	}
	catch (...) {
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::initialize()");
	}
	m_pointingModel=Antenna::PointingModel::_nil();
	m_refraction=Antenna::Refraction::_nil();
	m_mount=Antenna::Mount::_nil();
	m_generator=Antenna::EphemGenerator::_nil();
	m_generatorFlux=Antenna::EphemGenerator::_nil();
	m_mountError=false;
	m_siderealGenerator=m_otfGenerator=m_moonGenerator=m_sunGenerator=m_satelliteGenerator=m_solarBodyGenerator=Antenna::EphemGenerator::_nil();
	IRA::CString coordinatesFile = m_config->getCoordinatesFilename();
	if(coordinatesFile != "")
	{
		outFile.open((const char *)coordinatesFile, std::fstream::out | std::fstream::app);
		if(!outFile.is_open())
		{
			ACS_LOG(LM_FULL_INFO,"CBossCore::initialize()", (LM_ERROR,"Cannot open '%s' text file to write coordinates!", (const char *)coordinatesFile));
		}
	}
}

void CBossCore::execute() throw (ComponentErrors::CouldntGetComponentExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntReleaseComponentExImpl)
{
	Antenna::TSiteInformation_var site;
	Antenna::Observatory_var observatory=Antenna::Observatory::_nil();
	_IRA_LOGFILTER_ACTIVATE(m_config->getRepetitionCacheTime(),m_config->getRepetitionExpireTime());
	try {
		observatory=m_services->getDefaultComponent<Antenna::Observatory>((const char*)m_config->getObservatoryComponent());
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CBossCore::execute()");
		Impl.setComponentName((const char*)m_config->getObservatoryComponent());
		throw Impl;
	}
	catch (maciErrType::NoPermissionExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CBossCore::execute()");
		Impl.setComponentName((const char*)m_config->getObservatoryComponent());
		throw Impl;		
	}
	catch (maciErrType::NoDefaultComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CBossCore::execute()");
		Impl.setComponentName((const char*)m_config->getObservatoryComponent());
		throw Impl;				
	}	
	ACS_LOG(LM_FULL_INFO,"CBossCore::execute()",(LM_INFO,"AntennaBoss::OBSERVATORY_LOCATED"));
	/*try {
		m_pointingModel=m_services->getComponent<Antenna::PointingModel>((const char*)m_config->getPointingModelInstance());
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CBossCore::execute()");
		Impl.setComponentName((const char*)m_config->getPointingModelInstance());
		throw Impl;		
	}
	ACS_LOG(LM_FULL_INFO,"CBossCore::execute()",(LM_INFO,"AntennaBoss::POINTING_MODEL_LOCATED"));
	try {
		m_refraction=m_services->getComponent<Antenna::Refraction>((const char *)m_config->getRefractionInstance());
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CBossCore::execute()");
		Impl.setComponentName((const char*)m_config->getRefractionInstance());
		throw Impl;		
	}
	ACS_LOG(LM_FULL_INFO,"CBossCore::execute()",(LM_INFO,"AntennaBoss::REFRACTION_LOCATED"));*/
	try	{	
		site=observatory->getSiteSummary();  //throw CORBA::SYSTEMEXCEPTION
	}
	catch (CORBA::SystemException& ex)	{
		_EXCPT(ComponentErrors::CORBAProblemExImpl,__dummy,"CBossCore::execute()");
		__dummy.setName(ex._name());
		__dummy.setMinor(ex.minor());
		throw __dummy;
	}
	m_site=CSite(site.out());
	m_dut1=site->DUT1;
	ACS_LOG(LM_FULL_INFO,"CBossCore::execute()",(LM_INFO,"AntennaBoss::SITE_INITIALIZED"));
	try {
		m_services->releaseComponent((const char*)observatory->name());
	}
	catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CBossCore::execute()");
		Impl.setComponentName((const char*)observatory->name());
		throw Impl;
	}	
	ACS_LOG(LM_FULL_INFO,"CBossCore::execute()",(LM_INFO,"AntennaBoss::OBSERVATORY_RELEASED"));
	m_slewCheck.initSite(m_site,m_dut1);
	m_slewCheck.initMount(m_config->getMaxAzimuthRate(),m_config->getMaxElevationRate(),m_config->getMinElevation(),m_config->getMaxElevation(),
			m_config->getMaxAzimuthAcceleration(),m_config->getMaxElevationAcceleration(),m_config->getMinSuggestedElevation(),m_config->getMaxSuggestedElevation());
}

void CBossCore::cleanUp()
{
	if (m_notificationChannel!=NULL) {
		m_notificationChannel->disconnect(); // from ACS examples it seems that there is no need to destroy the object.
		m_notificationChannel=NULL;
	}
	try {
		//freeGenerator(m_generator.inout()); ///no need to free it...the realseComponet is called by the corresponding primary generator and the memory if freed by the _var object
		freeGenerator(m_siderealGenerator.inout());
		freeGenerator(m_otfGenerator.inout());
		freeGenerator(m_moonGenerator.inout());
		freeGenerator(m_sunGenerator.inout());
		freeGenerator(m_satelliteGenerator.inout());
		freeGenerator(m_solarBodyGenerator.inout());
		freeGenerator(m_primarySiderealGenerator.inout());
		freeGenerator(m_primaryOtfGenerator.inout());
		freeGenerator(m_primaryMoonGenerator.inout());
		freeGenerator(m_primarySunGenerator.inout());
		freeGenerator(m_primarySatelliteGenerator.inout());
		freeGenerator(m_primarySolarBodyGenerator.inout());
	}
	catch (ComponentErrors::CouldntReleaseComponentExImpl& ex) {
		ex.log(LM_DEBUG);
	}
	if(outFile.is_open()) outFile.close();
	_IRA_LOGFILTER_FLUSH;
	_IRA_LOGFILTER_DESTROY;
	unloadPointingModel(m_pointingModel);
	unloadRefraction(m_refraction);
	unloadMount(m_mount);
}

void CBossCore::getObservedHorizontal(TIMEVALUE& time,TIMEDIFFERENCE& duration,double& az,double& el) const
{
	if (duration.value().value>m_config->getCoordinateIntegrationPeriod()) {
		double ra,dec;
		IRA::CSkySource converter;
		TIMEVALUE startTime(time.value().value-duration.value().value/2);
		TIMEVALUE stopTime(time.value().value+duration.value().value/2);
		TIMEVALUE midTime(time.value().value);
		m_integratedObservedEquatorial.averagePoint(startTime,stopTime,ra,dec);
		converter.setInputEquatorial(ra,dec,IRA::CSkySource::SS_J2000);
		IRA::CDateTime dateTime(midTime,m_dut1);
		converter.process(dateTime,m_site);
		converter.getApparentHorizontal(az,el);
	}
	else {
		//the integration time is not added anymore since we suppose the correct time is requested from caller
		TIMEVALUE midTime(time.value().value/*+duration.value().value/2*/);
		m_observedHorizontals.selectPoint(midTime,az,el);
	}
}

void CBossCore::getObservedEquatorial(TIMEVALUE& time,TIMEDIFFERENCE& duration,double&ra,double& dec) const
{
	if (duration.value().value>m_config->getCoordinateIntegrationPeriod()) {
		TIMEVALUE startTime(time.value().value-duration.value().value/2);
		TIMEVALUE stopTime(time.value().value+duration.value().value/2);
		m_integratedObservedEquatorial.averagePoint(startTime,stopTime,ra,dec);
	}
	else {
		//the integration time is not added anymore since we suppose the correct time is requested from caller
		TIMEVALUE midTime(time.value().value/*+duration.value().value/2*/);
		m_observedEquatorials.selectPoint(midTime,ra,dec);
	}
}

void CBossCore::getObservedGalactic(TIMEVALUE& time,TIMEDIFFERENCE& duration,double &lng,double& lat) const
{
	if (duration.value().value>m_config->getCoordinateIntegrationPeriod()) {
		double ra,dec;
		IRA::CSkySource converter;
		TIMEVALUE startTime(time.value().value-duration.value().value/2);
		TIMEVALUE stopTime(time.value().value+duration.value().value/2);
		m_integratedObservedEquatorial.averagePoint(startTime,stopTime,ra,dec);
		IRA::CSkySource::equatorialToGalactic(ra,dec,lng,lat);
	}
	else {
		//the integration time is not added anymore since we suppose the correct time is requested from caller
		TIMEVALUE midTime(time.value().value/*+duration.value().value/2*/);
		m_observedGalactics.selectPoint(midTime,lng,lat);
	}
}

void CBossCore::getRawHorizontal(const TIMEVALUE& time,double& az,double& el) const
{
	//IRA::CString tempo;
	//IRA::CIRATools::timeToStr(time.value().value,tempo);
	//printf("getRaw: %s ",(const char *)tempo);
	m_rawCoordinates.selectPoint(time,az,el);
	//printf("%lf %lf\n",az,el);
}

void CBossCore::getApparent(const ACS::Time& time,double& az,double& el,double& ra, double& dec,double& jepoch,double& lon,double& lat)
{
	if ((!CORBA::is_nil(m_generator)) && (m_generatorType!=Antenna::ANT_NONE)) {
		try {
			m_generator->getAllCoordinates(time,az,el,ra,dec,jepoch,lon,lat);
		}
		catch (...) {
			az=el=ra=dec=jepoch=lon=lat=0.0;
		}
	}
	else {
		az=el=ra=dec=jepoch=lon=lat=0.0;
	}
}

void CBossCore::stow() throw (ManagementErrors::ParkingErrorExImpl)
{
	ACS::CBDescIn desc;
	//ACS::CBvoid_var cbvoid;
	desc.normal_timeout=4000000;
	//cbvoid=callbackStow._this();
	try {
		loadMount(m_mount,m_mountError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		changeBossStatus(Management::MNG_FAILURE);
		_ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl,managEx,ex,"CBossCore::stow()");
		managEx.setSubsystem("Antenna");
		throw managEx;
	}
	try {
		if (m_enable) {
			m_mount->stow(m_cbStow,desc);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::stow()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_FAILURE);
		_ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl,managEx,impl,"CBossCore::stow()");
		managEx.setSubsystem("Antenna");
		m_mountError=true;
		throw managEx;
	}
	catch (...) {
		changeBossStatus(Management::MNG_FAILURE);
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::stow()");
		_ADD_BACKTRACE(ManagementErrors::ParkingErrorExImpl,managEx,impl,"CBossCore::stow()");
		managEx.setSubsystem("Antenna");
		throw managEx;
	}
	m_workingThread->suspend();
	m_generatorType=Antenna::ANT_NONE;
	m_generator=Antenna::EphemGenerator::_nil(); // it also releases the previous reference.
	m_generatorFlux=Antenna::EphemGenerator::_nil();
	m_targetName=IRA::CString("none");
	m_currentAxis=Management::MNG_NO_AXIS;
	m_targetRA=m_targetDec=m_targetVrad=m_targetFlux=0.0;
	m_vradDefinition=Antenna::ANT_UNDEF_DEF;
	m_vradReferenceFrame=Antenna::ANT_UNDEF_FRAME;
	// clear all user and scan offsets
	m_userOffset=TOffset(0.0,0.0,Antenna::ANT_HORIZONTAL);
	m_scanOffset=TOffset(0.0,0.0,Antenna::ANT_HORIZONTAL);
	addOffsets(m_longitudeOffset,m_latitudeOffset,m_offsetFrame,m_userOffset,m_scanOffset);
	m_lastScanParameters.type=Antenna::ANT_NONE;
	m_lastScanParameters.applyOffsets=false;
	m_lastScanParameters.secondary=false;
	m_lastScanParameters.paramNumber=0;
	m_correctionEnable_scan=true;
}

void CBossCore::setup(const char *config) throw (ManagementErrors::ConfigurationErrorExImpl)
{
	ACS::CBDescIn desc;
	//ACS::CBvoid_var cbvoid;
	TIMEVALUE now;
	try {
		loadMount(m_mount,m_mountError);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		changeBossStatus(Management::MNG_FAILURE);
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,managEx,ex,"CBossCore::setup()");
		managEx.setSubsystem("Antenna");
		throw managEx;
	}
	m_workingThread->suspend();
	m_generatorType=Antenna::ANT_NONE;
	m_generator=Antenna::EphemGenerator::_nil(); // it also releases the previous reference.
	m_generatorFlux=Antenna::EphemGenerator::_nil();
	m_targetName=IRA::CString("none");
	m_currentAxis=Management::MNG_NO_AXIS;
	m_timeToStop=0;
	m_targetRA=m_targetDec=m_targetVrad=m_targetFlux=0.0;
	m_vradDefinition=Antenna::ANT_UNDEF_DEF;
	m_vradReferenceFrame=Antenna::ANT_UNDEF_FRAME;
	try {
		loadPointingModel(m_pointingModel);
	}
	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
		changeBossStatus(Management::MNG_FAILURE);
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,managEx,ex,"CBossCore::setup()");
		managEx.setSubsystem("Antenna");
		throw managEx;
	}	
	IRA::CIRATools::getTime(now);
	try {
		if (m_enable) {
			m_mount->setTime(now.value().value);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::setup()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,manImpl,impl,"CBossCore::setup()");
		manImpl.setSubsystem("Antenna");
		manImpl.setReason("Could not synchronize time");		
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw manImpl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,impl,ex,"CBossCore::setup()");
		impl.setSubsystem("Antenna");
		impl.setReason("Could not synchronize time");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,impl,ex,"CBossCore::setup()");
		impl.setSubsystem("Antenna");
		impl.setReason("Could not synchronize time");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}	
	catch (...) {
		changeBossStatus(Management::MNG_WARNING);
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::setup()");
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,manImpl,impl,"CBossCore::setup()");
		manImpl.setSubsystem("Antenna");
		manImpl.setReason("Could not synchronize time");
		throw manImpl;
	}
	try {
		m_pointingModel->setReceiver(config);
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,impl,ex,"CBossCore::setup()");
		impl.setSubsystem("Antenna");
		impl.setReason("Could not initialize pointing model");
		changeBossStatus(Management::MNG_WARNING);
		throw impl;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::setup()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,manImpl,impl,"CBossCore::setup()");
		manImpl.setSubsystem("Antenna");
		manImpl.setSubsystem("Could not initialize pointing model");		
		changeBossStatus(Management::MNG_WARNING);
		throw manImpl;
	}
	catch (...) {
		changeBossStatus(Management::MNG_WARNING);
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::setup()");
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,manImpl,impl,"CBossCore::setup()");
		manImpl.setSubsystem("Antenna");
		manImpl.setReason("Could not initialize pointing model");
		throw manImpl;		
	}
	m_correctionEnable=true;  // enable the correction for pointing model and refraction.
	m_correctionEnable_scan=true;
	desc.normal_timeout=4000000;
	//cbvoid=callbackUnstow._this(); 
	try {
		if (m_enable) {
			m_mount->unstow(m_cbSetup,desc);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::setup()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,manImpl,impl,"CBossCore::setup()");
		manImpl.setSubsystem("Antenna");
		manImpl.setReason("Could not unstow the antenna");		
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw manImpl;
	}
	catch (...) {
		changeBossStatus(Management::MNG_WARNING);
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::setup()");
		_ADD_BACKTRACE(ManagementErrors::ConfigurationErrorExImpl,manImpl,impl,"CBossCore::setup()");
		manImpl.setSubsystem("Antenna");
		manImpl.setReason("Could not unstow the antenna");
		throw manImpl;		
	}
	m_userOffset.lon=m_userOffset.lat=0.0; m_userOffset.frame=Antenna::ANT_HORIZONTAL;
	m_scanOffset.lon=m_scanOffset.lat=0.0; m_scanOffset.frame=Antenna::ANT_HORIZONTAL;
	m_lastScanParameters.type=Antenna::ANT_NONE;
	m_lastScanParameters.applyOffsets=false;
	m_lastScanParameters.secondary=false;
	m_lastScanParameters.paramNumber=0;
	m_longitudeOffset=m_latitudeOffset=0.0;  //reset offsets
	m_offsetFrame=Antenna::ANT_HORIZONTAL;
	m_FWHM=0.0;
	m_waveLength=0.0;
	m_currentObservingFrequency=0.0;
}

void CBossCore::disable()
{
	if (m_enable) {
		m_enable=false;
		ACS_LOG(LM_FULL_INFO,"CBossCore::disable()",(LM_NOTICE,"AntennaBoss::COMPONENT_DISABLED"));
	}
}

void CBossCore::enable()
{
	if (!m_enable) {
		m_enable=true;
		ACS_LOG(LM_FULL_INFO,"CBossCore::enable()",(LM_NOTICE,"AntennaBoss::COMPONENT_ENABLED"));		
	}
}

void CBossCore::enableCorrection()
{
	if (!m_correctionEnable) {
		m_correctionEnable=true;
		ACS_LOG(LM_FULL_INFO,"CBossCore::enableCorrection()",(LM_NOTICE,"AntennaBoss::CORRECTIONS_ENABLED"));
	}
}

void CBossCore::disableCorrection()
{
	if (m_correctionEnable) {
		m_correctionEnable=false;
		ACS_LOG(LM_FULL_INFO,"CBossCore::disableCorrection()",(LM_NOTICE,"AntennaBoss::CORRECTIONS_DISABLED"));
	}
}

void CBossCore::setFWHM(const double& val,const double& waveLen)
{ 
	m_FWHM=val;
	m_waveLength=waveLen;
	m_currentObservingFrequency=LIGHTSPEED_MS/waveLen; // frequency in Hz
	m_currentObservingFrequency/=1000000.0; // frequency in MHz
	computeFlux();
	ACS_LOG(LM_FULL_INFO,"CBossCore::setFWHM()",(LM_NOTICE,"FWHM: %lf",val*DR2D));
}

void CBossCore::computeFWHM(const double& taper,const double& waveLen)
{
	double tp=fabs(taper);
	m_FWHM=(1.02+0.0135*tp)*(waveLen/m_config->getDiameter());
	m_waveLength=waveLen;
	m_currentObservingFrequency=LIGHTSPEED_MS/waveLen;
	m_currentObservingFrequency/=1000000.0; // frequency in MHz
	computeFlux();
	ACS_LOG(LM_FULL_INFO,"CBossCore::setFWHM()",(LM_NOTICE,"FWHM: %lf",m_FWHM*DR2D));
}

void CBossCore::getFluxes(const ACS::doubleSeq& freqs,ACS::doubleSeq& fluxes) throw (ComponentErrors::CORBAProblemExImpl)
{
	fluxes.length(freqs.length());
	if( (m_FWHM>0.0) && (!CORBA::is_nil(m_generatorFlux))){
		for (unsigned i=0;i<freqs.length();i++) {
			try {
				m_generatorFlux->computeFlux(freqs[i],m_FWHM,fluxes[i]);
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::getFluxes()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_WARNING);
				throw impl;
			}
		}
	}
	else {
		for (unsigned i=0;i<freqs.length();i++) {
			fluxes[i]=1.0;
		}
	}
}

void CBossCore::radialVelocity(const double& val,const Antenna::TReferenceFrame& vframe, const Antenna::TVradDefinition& vdef)
{
	if (!isnan(val)) {  // the default behavior generated by the wildcard character is to keep the present value. The wildcard correspond to a nan
		m_targetVrad=val;
	}
	if (vframe!=Antenna::ANT_UNDEF_FRAME) {
		m_vradReferenceFrame=vframe;
	}
	if (vdef!=Antenna::ANT_UNDEF_DEF) {
		m_vradDefinition=vdef;
	}
}

void CBossCore::getTopocentricFrequency(const ACS::doubleSeq& rest,ACS::doubleSeq& topo) throw (AntennaErrors::OperationNotPermittedExImpl)
{
	if (rest.length()==0) {
		topo.length(0);
		return;
	}
	if (m_vradReferenceFrame==Antenna::ANT_UNDEF_FRAME) {
		_EXCPT(AntennaErrors::OperationNotPermittedExImpl,impl,"BossCore::getTopocentricFrequency");
		impl.setReason("Reference frame not defined");
		throw impl;
	}
	if (m_vradDefinition==Antenna::ANT_UNDEF_DEF) {
		_EXCPT(AntennaErrors::OperationNotPermittedExImpl,impl,"BossCore::getTopocentricFrequency");
		impl.setReason("velocity definition is not known");
		throw impl;
	}
	topo.length(rest.length());
	TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	CDateTime time(now,m_dut1);
	for (unsigned i=0;i<rest.length();i++) {
		//printf("ra,dec,rest,vrad: %lf,%lf,%lf,%lf\n",m_targetRA,m_targetDec,rest[i],m_targetVrad);
		IRA::CFrequencyTracking track(m_site,m_targetRA,m_targetDec,mapReferenceFrame(m_vradReferenceFrame),
				mapVelocityDefinition(m_vradDefinition),rest[i],m_targetVrad);
		topo[i]=track.getTopocentricFrequency(time);
	}
}

void CBossCore::setOffsets(const double& lonOff,const double& latOff,const Antenna::TCoordinateFrame& frame) throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl)
{
	if ((!CORBA::is_nil(m_generator)) && (m_generatorType!=Antenna::ANT_NONE)) {
		try {
			m_generator->setOffsets(lonOff,latOff,frame);
			if (frame==Antenna::ANT_HORIZONTAL) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::setOffsets()",
					(LM_NOTICE,"NEW_HORIZONTAL_OFFSETS %lf %lf",lonOff,latOff));
			}
			else if (frame==Antenna::ANT_EQUATORIAL) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::setOffsets()",
					(LM_NOTICE,"NEW_EQUATORIAL_OFFSETS %lf %lf",lonOff,latOff));
			}
			else if (frame==Antenna::ANT_GALACTIC) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::setOffsets()",
					(LM_NOTICE,"NEW_GALACTIC_OFFSETS %lf %lf",lonOff,latOff));					
			}
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::setOffsets()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			throw impl;
		}
		catch (...) {
			changeBossStatus(Management::MNG_WARNING);
			_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::setOffsets()");
		}
		try {
			quickTracking();
		}
		catch (ComponentErrors::OperationErrorExImpl& ex) {
			changeBossStatus(Management::MNG_FAILURE);
			throw ex;
		}
	}
	m_userOffset=TOffset(lonOff,latOff,frame);
	m_scanOffset=TOffset(0.0,0.0,frame);
	addOffsets(m_longitudeOffset,m_latitudeOffset,m_offsetFrame,m_userOffset,m_scanOffset);
}

void CBossCore::getAllOffsets(double& azOff,double& elOff,double& raOff,double& decOff,double& lonOff,double& latOff) const
{
	if (m_offsetFrame==Antenna::ANT_HORIZONTAL) {
		azOff=m_longitudeOffset;
		elOff=m_latitudeOffset;
		raOff=0.0;
		decOff=0.0;
		lonOff=0.0;
		latOff=0.0;
	}
	else if (m_offsetFrame==Antenna::ANT_EQUATORIAL) {
		azOff=0.0;
		elOff=0.0;
		raOff=m_longitudeOffset;
		decOff=m_latitudeOffset;
		lonOff=0.0;
		latOff=0.0;		
	}
	else if (m_offsetFrame==Antenna::ANT_GALACTIC) {
		azOff=0.0;
		elOff=0.0;
		raOff=0.0;
		decOff=0.0;
		lonOff=m_longitudeOffset;
		latOff=m_latitudeOffset;		
	}
}

void CBossCore::stop() throw (ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	m_workingThread->suspend();
	m_generatorType=Antenna::ANT_NONE;
	m_generator=Antenna::EphemGenerator::_nil(); // it also releases the previous reference.
	m_generatorFlux=Antenna::EphemGenerator::_nil();
	m_targetName=IRA::CString("none");	
	m_currentAxis=Management::MNG_NO_AXIS;
	m_timeToStop=0;
	m_targetRA=m_targetDec=m_targetVrad=m_targetFlux=0.0;
	m_vradDefinition=Antenna::ANT_UNDEF_DEF;
	m_vradReferenceFrame=Antenna::ANT_UNDEF_FRAME;
	// clear all user and scan offsets
	m_userOffset=TOffset(0.0,0.0,Antenna::ANT_HORIZONTAL);
	m_scanOffset=TOffset(0.0,0.0,Antenna::ANT_HORIZONTAL);
	addOffsets(m_longitudeOffset,m_latitudeOffset,m_offsetFrame,m_userOffset,m_scanOffset);
	m_lastScanParameters.type=Antenna::ANT_NONE;
	m_lastScanParameters.applyOffsets=false;
	m_lastScanParameters.secondary=false;
	m_lastScanParameters.paramNumber=0;
	m_correctionEnable_scan=true;
	loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
	if (m_enable) {
		try {
			m_mount->stop(); 
		}
		catch (ComponentErrors::ComponentErrorsEx& E) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CBossCore::stop()");
			impl.setComponentName((const char*)m_mount->name());
			impl.setOperationName("stop()");
			changeBossStatus(Management::MNG_FAILURE);
			throw impl;
		}
		catch (AntennaErrors::AntennaErrorsEx& E) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,E,"CBossCore::stop()");
			impl.setComponentName((const char*)m_mount->name());
			impl.setOperationName("stop()");
			changeBossStatus(Management::MNG_FAILURE);
			throw impl;
		}		
		catch (CORBA::SystemException& E) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::stop()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			changeBossStatus(Management::MNG_FAILURE);
			m_mountError=true;
			throw impl;
		}
		catch (...) {
			changeBossStatus(Management::MNG_FAILURE);
			_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::stop()");
		}
		ACS_LOG(LM_FULL_INFO,"CBossCore::stop()",(LM_NOTICE,"STOP_ISSUED"));
	}
	m_rawCoordinates.empty();
}

bool CBossCore::checkScan(const ACS::Time& startUt,const Antenna::TTrackingParameters& par,const Antenna::TTrackingParameters& secondary,Antenna::TRunTimeParameters_out rTime,
 const double& minElLimit,const double& maxElLimit) throw (
 ComponentErrors::CouldntGetComponentExImpl,AntennaErrors::ScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
 AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	double azimuth,elevation,stopElevation,stopAzimuth,hwAzimuth;
	Antenna::TSections section;
	ACS::Time startTime=startUt; //it can be changed in the prepareScan routine
	CSlewCheck::TCheckResult visible;
	bool slew;
	Antenna::TGeneratorType generatorType;
	Antenna::EphemGenerator_var generator=Antenna::EphemGenerator::_nil();
	Antenna::EphemGenerator_var generatorFlux=Antenna::EphemGenerator::_nil();
	Antenna::TRunTimeParameters_var antennaInfo=new Antenna::TRunTimeParameters;
	//the getHorizontal says where the antenna should be at the given time, even if the time is far before the start time, in that case the start point of the scan is given
	try {
		double ra,dec,lon,lat,vrad;
		Antenna::TReferenceFrame velFrame;
		Antenna::TVradDefinition velDef;
		//Management::TScanAxis axis;
		TOffset scanOff;
		Antenna::TTrackingParameters lastScan;
		IRA::CString name;
		copyTrack(lastScan,m_lastScanParameters);
		// startUt is changed by preparescan just in case of an OTF scan
		generator=prepareScan(true,startTime,par,secondary,m_userOffset,generatorType,lastScan,section,ra,dec,lon,lat,vrad,velFrame,
				velDef,antennaInfo->timeToStop,name,scanOff,antennaInfo->axis,generatorFlux.out());
		antennaInfo->targetName=CORBA::string_dup((const char *)name);
		if (generatorType==Antenna::ANT_OTF) {
			double pangle;
			TIMEVALUE ct(startTime);
			IRA::CDateTime time(ct,m_dut1);
			generator->getHorizontalCoordinate(startTime,azimuth,elevation); 
			generator->getHorizontalCoordinate(startTime+par.otf.subScanDuration,stopAzimuth,stopElevation); //this is the coordinate at the end of the scan
			antennaInfo->startEpoch=startTime;
			antennaInfo->azimuth=azimuth;
			antennaInfo->elevation=elevation;
			IRA::CSkySource::horizontalToEquatorial(time,m_site,azimuth,elevation,
					antennaInfo->rightAscension,antennaInfo->declination,pangle);
			antennaInfo->onTheFly=true;
			ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_DEBUG,"OTF_AZ_EL: %lf %lf",azimuth,elevation));
			ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_DEBUG,"OTF_STOPAZ_STOPEL: %lf %lf",stopAzimuth,stopElevation));
		}
		else { // all other cases 
			//************************************************
			// Also in this case, as the OTF on secondary target, the start time should be computed by a two iteration process:
			// 1) compute where the source is now and the expected slewing time
			// 2) compute where the source will be in epoch now+slewing time
			// **************************************************
			ACS::Time inputTime;
			TIMEVALUE now;
			IRA::CIRATools::getTime(now);
			inputTime=now.value().value;
			IRA::CString out;
			IRA::CIRATools::timeToStr(inputTime,out);
			ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_DEBUG,"CHECK_TIME_IS: %s",(const char *)out));
			generator->getHorizontalCoordinate(inputTime,azimuth,elevation); //use inputTime (=now), in order to get where the source is now)
			//printf("controllo cordinate di riferimento: %lf, %lf\n",azimuth,elevation);
			antennaInfo->azimuth=azimuth;
			antennaInfo->elevation=elevation;
			antennaInfo->rightAscension=ra;
			antennaInfo->declination=dec;
			antennaInfo->onTheFly=false;
			ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_DEBUG,"TARGET_AZ_EL: %lf %lf",azimuth,elevation));
		}
	}
	catch (ComponentErrors::CouldntCallOperationExImpl& ex) { //caught  just to update the component status
		changeBossStatus(Management::MNG_WARNING);
		throw ex;
	}
	catch (ComponentErrors::UnexpectedExImpl& ex) {
		changeBossStatus(Management::MNG_WARNING);
		throw ex;		
	}
	catch (ComponentErrors::CORBAProblemExImpl& ex) {
		changeBossStatus(Management::MNG_WARNING);
		throw ex;		
	}
	catch (AntennaErrors::ScanErrorExImpl& ex) {
		changeBossStatus(Management::MNG_WARNING);
		throw ex;		
	}
	catch (AntennaErrors::SecondaryScanErrorExImpl& ex) {
		changeBossStatus(Management::MNG_WARNING);
		throw ex;
	}	
	catch (AntennaErrors::MissingTargetExImpl& ex) {
		changeBossStatus(Management::MNG_WARNING);
		throw ex;		
	}
	catch (AntennaErrors::LoadGeneratorErrorExImpl& ex) {
		changeBossStatus(Management::MNG_WARNING);
		throw ex;				
	}
	catch (...) {
		changeBossStatus(Management::MNG_WARNING);
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::checkScan()");
	}
	if ((azimuth*DR2D>=90.0) && (azimuth*DR2D<=270.0)) {
		antennaInfo->section=Antenna::ANT_SOUTH;
	}
	else {
		antennaInfo->section=Antenna::ANT_NORTH;
	}
	loadMount(m_mount,m_mountError); 
	// ask the mount about the azimuth coordinate that will be effectively commanded to the antenna (hardware coordinate)
	try {
		hwAzimuth=m_mount->getHWAzimuth(azimuth*DR2D,section)*DD2R;
		ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_DEBUG,"HARDWARE_AZIMUTH: %lf",hwAzimuth*DR2D));	
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::checkScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw impl;
	}
	//check the elevation limit.....if the generator is OTF take into consideration also the stopElevation otherwise we consider just the elevation
	if (generatorType==Antenna::ANT_OTF) {
		CSlewCheck::TCheckResult v1,v2;
		if  ((par.otf.coordFrame==Antenna::ANT_HORIZONTAL) && (par.otf.subScanFrame==Antenna::ANT_HORIZONTAL)) {  ///this is a special case....essentially skydips
			/*v1=m_slewCheck.checkLimit(elevation,m_config->getMinElevation(),m_config->getMaxElevation());
			v2=m_slewCheck.checkLimit(stopElevation,m_config->getMinElevation(),m_config->getMaxElevation());*/
			v1=v2=CSlewCheck::VISIBLE;
		}
		else {
			v1=m_slewCheck.checkLimit(elevation,minElLimit,maxElLimit);
			v2=m_slewCheck.checkLimit(stopElevation,minElLimit,maxElLimit);
		}
		visible=GETMIN(v1,v2);   // the result of visibility check is the minimum of both checks: NOT_VISIBLE<AVOIDANCE<VISIBLE, i.e. target is visible only if both checks result in VISIBLE
		//visible=m_slewCheck.checkLimit(elevation)&&m_slewCheck.checkLimit(stopElevation);
	}
	else if (generatorType==Antenna::ANT_SIDEREAL) {
		if (par.frame==Antenna::ANT_HORIZONTAL) {
			visible=CSlewCheck::VISIBLE;
		}
		else {
			visible=m_slewCheck.checkLimit(elevation,minElLimit,maxElLimit);
		}
	}
	else {
		visible=m_slewCheck.checkLimit(elevation,minElLimit,maxElLimit);
	}
	if (visible==CSlewCheck::VISIBLE) {
		ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_DEBUG,"TARGET_INSIDE_ELEVATION_RANGE"));
		// if startUt==0 no slewing checks are done...the answer is always true but the slewing time is computed anyway
		slew=m_slewCheck.checkSlewing(m_lastEncoderAzimuth,m_lastEncoderElevation,m_lastEncoderRead,hwAzimuth,elevation,startUt,antennaInfo->slewingTime);
		if (generatorType!=Antenna::ANT_OTF) { // the startime need to be computed only for not-OTF scans. 
			antennaInfo->startEpoch=m_lastEncoderRead+antennaInfo->slewingTime+1000000; // add 0.1s as safety margin
		}
		if (slew) {
			ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_DEBUG,"TARGET_REACHABLE_ON_TIME"));
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_DEBUG,"TARGET_NOT_REACHABLE_ON_TIME"));
		}
		// in case all the check are successful the estimated start time is returned back.
		// generally it works according the following table:
		// startUT          OTF_SCAN                TRACK_SCAN
		//     0            computedUT(prepareScan) computedUT(here)
		//     A            A                       A
		// the last entry is valid only if the antenna is capable to start before A.
		rTime=antennaInfo._retn();
		return slew;
	}
	else if (visible==CSlewCheck::AVOIDANCE) {
		ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_NOTICE,"TARGET_OUTSIDE_ELEVATION_RANGE"));
		rTime=antennaInfo._retn();
		return false;
	}
	else { //NOT_VISIBLE
		ACS_LOG(LM_FULL_INFO,"CBossCore::checkScan()",(LM_NOTICE,"TARGET_IS_SET"));
		rTime=antennaInfo._retn();
		return false;
	}
}

void CBossCore::resetFailures() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl)
{
	loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		m_mount->reset();
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::resetFailures()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		changeBossStatus(Management::MNG_WARNING);
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::resetFailures()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("reset()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		changeBossStatus(Management::MNG_WARNING);
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::resetFailures()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("reset()");
		throw impl;
	}
	m_status=Management::MNG_OK;	
}


void CBossCore::getAllattributes()
{	
}

bool CBossCore::updateAttributes() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	ACS::Time time;
	double az,el,azOff,elOff;
	double pAngle,appRa,appDec;
	double lng,lat;
	double ra,dec;
	m_trackingTime=0;
	m_tracking=false;
	
	// get the encoders coordinates from the mount
	loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		m_mount->getEncoderCoordinates(time,az,el,azOff,elOff,m_lastAzimuthSection);
		if(time == m_lastEncoderRead) return false;
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::updateAttributes()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		changeBossStatus(Management::MNG_WARNING);
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::updateAttributes()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("getEncoderCoordinates()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		changeBossStatus(Management::MNG_WARNING);
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::updateAttributes()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("getEncoderCoordinates()");
		throw impl;
	}	
	catch (...) {
		changeBossStatus(Management::MNG_WARNING);
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::updateAttributes()");
	}
	m_lastEncoderAzimuth=az*DD2R;
	m_lastEncoderElevation=el*DD2R;
	m_lastEncoderRead=time;
	TIMEVALUE timeMark(time);
	IRA::CDateTime dateTime(timeMark,m_dut1);  // create the CDateTime object starting from the requested Time.
	//subtract the offset to get real coordinates
	az-=azOff; el-=elOff;
	// convert to radians.
	az*=DD2R; el*=DD2R;
	//bring the azimuth inside the 0-2Pi.
	az=slaDranrm(az);
	if (outFile.is_open()) {
		IRA::CString outString;
		CIRATools::timeToStr(time, outString);
		outFile << (const char *)outString << " " << az << " " << el << " ";
	}
	if (m_correctionEnable && m_correctionEnable_scan) {
		loadRefraction(m_refraction); // throw ComponentErrors::CouldntGetComponentExImpl
		loadPointingModel(m_pointingModel); // throw ComponentErrors::CouldntGetComponentExImpl
		// get Offset from refraction;
		try {
			m_refraction->getCorrection(DPI/2.0-el,m_waveLength,m_refractionOffset);
		}
		catch (AntennaErrors::AntennaErrorsEx& ex) {  
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::updateAttributes()");
			impl.setComponentName((const char*)m_refraction->name());
			impl.setOperationName("getCorrection()");
			changeBossStatus(Management::MNG_WARNING);
			throw impl;  // in this case throw the exception......it will be the watching thread to log it
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::updateAttributes()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			throw impl;
		}
		catch (...) {
			changeBossStatus(Management::MNG_WARNING);
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::updateAttributes()");
			throw impl;
		}
		el-=m_refractionOffset;
		try {
			m_pointingModel->getAzElOffsets(az,el,m_pointingAzOffset,m_pointingElOffset);
			// subtract the offsets coming from the pointing model
			az-=m_pointingAzOffset; el-=m_pointingElOffset;
			// bring the azimuth again in the correct range (0-2PI)
			az=slaDranrm(az);
		}
		catch (AntennaErrors::AntennaErrorsEx& ex) {
			//This error is due to the fact that the pointing model is not configured yet.....so it is not an error itself.
			//We just set the pointing offsets to 0.0 and change the boss status to WARNING in order to notify the user in some way
			m_pointingAzOffset=m_pointingElOffset=0.0;
			changeBossStatus(Management::MNG_WARNING);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::updateAttributes()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			throw impl;
		}
		catch (...) {
			changeBossStatus(Management::MNG_WARNING);
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::updateAttributes()");
			throw impl;
		}
	}
	else {
		m_pointingAzOffset=m_pointingElOffset=m_refractionOffset=0.0;
	}
	m_observedHorizontals.addPoint(az,el,timeMark);
	// computes the apparent equatorial coordinates from the horizontal ones  
	IRA::CSkySource:: horizontalToEquatorial(dateTime,m_site,az,el,appRa,appDec,pAngle);
	IRA::CSkySource::apparentToJ2000(appRa,appDec,dateTime,ra,dec);
	m_observedEquatorials.addPoint(ra,dec,timeMark);
	if (outFile.is_open()) {
		outFile << az << " " << el << " " << ra << " " << dec << endl;
	}
	IRA::CSkySource::equatorialToGalactic(ra,dec,lng,lat);
	m_observedGalactics.addPoint(lng,lat,timeMark);
	if (m_integrationStartTime==0) { //integration not started...then start it
		m_integrationStartTime=m_lastEncoderRead;
		m_integratedRa=0.0;
		m_integratedDec=0.0;
		m_integratedSamples=0;
		//printf("PARTITA INTEGRAZIONE\n");
	}
	//printf("INTEGRAZIONE, differenza %llu, integration %ld\n",(m_lastEncoderRead - m_integrationStartTime),m_config->getCoordinateIntegrationPeriod());
	if ((m_lastEncoderRead - m_integrationStartTime) < (unsigned) m_config->getCoordinateIntegrationPeriod()) {
		//printf("INTEGRAZIONE....,\n");
		m_integratedRa+=ra;
		m_integratedDec+=dec;
		m_integratedSamples++;
	}
	else {
		m_integratedRa/=(double)m_integratedSamples;
		m_integratedDec/=(double)m_integratedSamples;
		//printf("INTEGRAZIONE FINITA, tempo %llu, ra %lf, dec %lf\n",m_integrationStartTime,m_integratedRa,m_integratedDec);
		m_integratedObservedEquatorial.addPoint(m_integratedRa,m_integratedDec,m_integrationStartTime);
		m_integrationStartTime=0; //terminate the integration....the next iteration will start the new one.
	}
	// now let's see if we are on tracking or not......
	if ((!CORBA::is_nil(m_generator)) && (m_generatorType!=Antenna::ANT_NONE)) {
		m_trackingTime=time;  //
		if (time<m_newScanEpoch+10000000) {  //this force to consider the tracking flag false for at least one second after the command of a new scan
			m_tracking=false;
			m_newScanEpoch=0;
		}
		else {
			try {
				m_tracking=m_generator->checkTracking(time,az,el,m_FWHM);
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::updateAttributes()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_WARNING);
				throw impl;
			}
			catch (...) {
				changeBossStatus(Management::MNG_FAILURE);
				_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::updateAttributes()");
			}
		}
	}

    return true;
}

void CBossCore::publishData() throw (ComponentErrors::NotificationChannelErrorExImpl)
{
	bool sendData;
	static TIMEVALUE lastEvent(0.0L);
	static Antenna::AntennaDataBlock prvData={0,false,Management::MNG_OK};
	TIMEVALUE now(0.0L);
	IRA::CIRATools::getTime(now);
	if (CIRATools::timeDifference(lastEvent,now)>=1000000) {  //one second from last event
		sendData=true;
	}
	else if ((prvData.tracking!=m_tracking) || (prvData.status!=m_status)) {
		sendData=true;
	}
	else {
		sendData=false;
	}
	if (sendData) {
		prvData.tracking=m_tracking;
		prvData.timeMark=m_trackingTime;
		prvData.status=m_status;
		try {
			m_notificationChannel->publishData<Antenna::AntennaDataBlock>(prvData);
		}
		catch (acsncErrType::PublishEventFailureExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::NotificationChannelErrorExImpl,impl,ex,"CBossCore::publishData()");
			changeBossStatus(Management::MNG_WARNING);
			throw impl;
		}
		IRA::CIRATools::timeCopy(lastEvent,now);
	}
}

void CBossCore::setHorizontalOffsets(const double& azOff,const double& elOff) throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl)
{
	setOffsets(azOff,elOff,Antenna::ANT_HORIZONTAL); // could throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl)
}

void CBossCore::setEquatorialOffsets(const double& raOff,const double& decOff) throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl)
{
	setOffsets(raOff,decOff,Antenna::ANT_EQUATORIAL); // could throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl)
}

void CBossCore::setGalacticOffsets(const double& lonOff,const double& latOff) throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl)
{
	setOffsets(lonOff,latOff,Antenna::ANT_GALACTIC); // could throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl)	
}

#include "BossCore_startScan.i"

#include "BossCore_prepareScan.i"

#include "BossCore_startScanWrappers.i"

#include "BossCore_extra.i"

/////// PRIVATES

void CBossCore::loadTrackingPoint(const TIMEVALUE& time,bool restart) throw (ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::UnexpectedExImpl,AntennaErrors::TrackingNotStartedExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	ACS::Time inputTime;
	double az,el;
	inputTime=const_cast<TIMEVALUE&>(time).value().value;
	if ((!CORBA::is_nil(m_generator)) && (m_generatorType!=Antenna::ANT_NONE)) {
		try {
			m_generator->getHorizontalCoordinate(inputTime,az,el);
		}
		catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::loadTrackingPoint()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_FAILURE);
				throw impl;
		}
		catch (...) {
			changeBossStatus(Management::MNG_FAILURE);
			_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::loadTrackingPoint()");
		}
	}
	else {
		changeBossStatus(Management::MNG_WARNING); // if the ephem generator is not loaded
		_THROW_EXCPT(AntennaErrors::TrackingNotStartedExImpl,"CBossCore::loadTrackingPoint()")
	}	
	if (m_correctionEnable && m_correctionEnable_scan) {
		double azOff,elOff;
		double refOff;
		loadPointingModel(m_pointingModel); //throw CouldntGetComponentExImpl
		loadRefraction(m_refraction); //throw CouldntGetComponentExImpl
		azOff=elOff=0.0;
		try {
			m_pointingModel->getAzElOffsets(az,el,azOff,elOff);
		}
		catch (AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::loadTrackingPoint()");
			impl.setComponentName((const char*)m_pointingModel->name());
			impl.setOperationName("getAzElOffsets()");
			changeBossStatus(Management::MNG_WARNING);  // enter warning status and log the message but go ahead
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::loadTrackingPoint()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		catch (...) {
			changeBossStatus(Management::MNG_WARNING);
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::loadTrackingPoint()");
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		az+=azOff;
		el+=elOff;
		refOff=0.0;
		try {
			if (el>0.0) {
				m_refraction->getCorrection(DPI/2.0-el,m_waveLength,refOff);
			}
		}
		catch (AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::loadTrackingPoint()");
			impl.setComponentName((const char*)m_refraction->name());
			impl.setOperationName("getCorrection()");
			changeBossStatus(Management::MNG_WARNING);  // enter warning status and log the message but go ahead
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::loadTrackingPoint()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			changeBossStatus(Management::MNG_WARNING);
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		catch (...) {
			changeBossStatus(Management::MNG_WARNING);
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::loadTrackingPoint()");
			_IRA_LOGFILTER_LOG_EXCEPTION(impl,LM_ERROR);
		}
		el+=refOff;
	}
	loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (m_enable) {   // command the coordinate only if the subsystem is enabled.
			m_mount->programTrack(az*DR2D,el*DR2D,inputTime,restart);
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::loadTrackingPoint()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw impl;
	}
	catch (ComponentErrors::ComponentErrorsEx& ex) {
		changeBossStatus(Management::MNG_WARNING);
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::loadTrackingPoint()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("programTrack()");
		throw impl;
	}
	catch (AntennaErrors::AntennaErrorsEx& ex) {
		changeBossStatus(Management::MNG_WARNING);
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::loadTrackingPoint()");
		impl.setComponentName((const char*)m_mount->name());
		impl.setOperationName("programTrack()");
		throw impl;
	}	
	catch (...) {
		changeBossStatus(Management::MNG_WARNING);
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::loadTrackingPoint()");
	}
	// update the last time
	m_lastPointTime=inputTime;
	// add the coordinates in the raw coordinates stack
	if (restart) {
		m_rawCoordinates.empty();
	}
	m_rawCoordinates.addPoint(az,el,time);
}

void CBossCore::checkStatus() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetAttributeExImpl,ComponentErrors::CouldntGetComponentExImpl)
{
	static TIMEVALUE lastExecution((long double)0.0);
	TIMEVALUE now;
	ACSErr::Completion_var comp;
	Management::ROTSystemStatus_var mountStatus=Management::ROTSystemStatus::_nil();
	Management::TSystemStatus st=Management::MNG_OK;
	IRA::CIRATools::getTime(now);
	if (m_FWHM==0.0) changeBossStatus(Management::MNG_WARNING);
	if (IRA::CIRATools::timeDifference(lastExecution,now)>1000000) {
		if (m_enable) {
			loadMount(m_mount,m_mountError); // throw ComponentErrors::CouldntGetComponentExImpl
			try {
				mountStatus=m_mount->mountStatus();
				if (!CORBA::is_nil(mountStatus)) {
					st=mountStatus->get_sync(comp.out());
					CompletionImpl local(comp);
					if (!local.isErrorFree()) {
						_ADD_BACKTRACE(ComponentErrors::CouldntGetAttributeExImpl,impl,local,"CBossCore::checkStatus()");
						 impl.setComponentName((const char*)m_mount->name()); 
						 impl.setAttributeName("mountStatus");
						 changeBossStatus(Management::MNG_WARNING);
						 m_status=Management::MNG_WARNING;
						 throw impl;
					}					
				}
				else {
					st=Management::MNG_WARNING;
				}
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::checkStatus()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				changeBossStatus(Management::MNG_WARNING);
				m_status=Management::MNG_WARNING;
				m_mountError=true;
				throw impl;
			}
			if (st==Management::MNG_OK) {
				if (IRA::CIRATools::timeDifference(m_lastStatusChange,now)<STATUS_VALIDITY) {
					m_status=m_bossStatus;
				}
				else {  // the boss error is cleared......
					m_status=Management::MNG_OK;
				}
			}
			else {
				m_status=st;
				return;
			}
		}
		else {
			m_status=Management::MNG_WARNING;
			return;
		}
	}
}

void CBossCore::changeBossStatus(const Management::TSystemStatus& status)
{
	if (status>=m_bossStatus) { // if the new state has an higher priority...update the status 
		m_bossStatus=status;
		IRA::CIRATools::getTime(m_lastStatusChange);
	}
	else { // if the priority is low update only if the previous error is cleared (happened more than STATUS_VALIDITY micros ago)
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		if (IRA::CIRATools::timeDifference(m_lastStatusChange,now)>STATUS_VALIDITY) {
			m_bossStatus=status;
			IRA::CIRATools::getTime(m_lastStatusChange);			
		}
	}
}

void CBossCore::loadMount(Antenna::Mount_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	ThreadSyncGuard guard(&m_mountMutex);  // We take the mutex in order to avoid concurrency errors between the WatchingThread and the WorkingThread
	if ((ref!=Antenna::Mount::_nil()) && (errorDetected)) { // if reference was already taken, but an error was found....dispose the reference
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (...) { //dispose silently...if an error...no matter
		}
		ref=Antenna::Mount::_nil();
	}
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {  
			// no default component because we use the parents interface Antenna::Mount not the site specific interface Antenna::MedicinaMount or Antenna::SRTMount
			ref=m_services->getComponent<Antenna::Mount>((const char*)m_config->getMountInstance());
			ACS_LOG(LM_FULL_INFO,"CBossCore::loadMount()",(LM_INFO,"MOUNT_LOCATED"));
			errorDetected=false;
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CBossCore::loadMount()");
			Impl.setComponentName((const char*)m_config->getMountInstance());
			throw Impl;		
		}
		catch (...) {
			_EXCPT(ComponentErrors::CouldntGetComponentExImpl,impl,"CBossCore::loadMount()");
			impl.setComponentName((const char*)m_config->getMountInstance());
			throw impl;
		}		
	}
}

void CBossCore::unloadMount(Antenna::Mount_var& ref) const
{
	ThreadSyncGuard guard(&m_mountMutex);  // We take the mutex in order to avoid concurrency errors between the WatchingThread and the WorkingThread
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CBossCore::unloadMount()");
			Impl.setComponentName((const char *)m_config->getMountInstance());
			Impl.log(LM_WARNING);
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::unloadMount())");
			impl.log(LM_WARNING);
		}
		ref=Antenna::Mount::_nil();
	}
}

void CBossCore::loadPointingModel(Antenna::PointingModel_var& ref) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	ThreadSyncGuard guard(&m_pmMutex);  // We take the mutex in order to avoid concurrency errors between the WatchingThread and the WorkingThread
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {
			ref=m_services->getComponent<Antenna::PointingModel>((const char*)m_config->getPointingModelInstance());
			ACS_LOG(LM_FULL_INFO,"CBossCore::loadPointingModel()",(LM_INFO,"POINTING_MODEL_LOCATED"));
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CBossCore::loadPointingModel()");
			Impl.setComponentName((const char*)m_config->getPointingModelInstance());
			throw Impl;		
		}
	}
}

void CBossCore::loadRefraction(Antenna::Refraction_var& ref) const throw (ComponentErrors::CouldntGetComponentExImpl)
{
	ThreadSyncGuard guard(&m_refractionMutex);  // We take the mutex in order to avoid concurrency errors between the WatchingThread and the WorkingThread
	if (CORBA::is_nil(ref)) {  //only if it has not been retrieved yet
		try {
			ref=m_services->getComponent<Antenna::Refraction>((const char*)m_config->getRefractionInstance());
			ACS_LOG(LM_FULL_INFO,"CBossCore::loadRefraction()",(LM_INFO,"REFRACTION_LOCATED"));
		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CBossCore::loadRefraction()");
			Impl.setComponentName((const char*)m_config->getRefractionInstance());
			throw Impl;		
		}
	}
}

void CBossCore::unloadPointingModel(Antenna::PointingModel_var& ref) const
{
	ThreadSyncGuard guard(&m_pmMutex);  // We take the mutex in order to avoid concurrency errors between the WatchingThread and the WorkingThread
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CBossCore::unloadPointingModel()");
			Impl.setComponentName((const char *)m_config->getPointingModelInstance());
			Impl.log(LM_WARNING);
		}
	}
	ref=Antenna::PointingModel::_nil();
}

void CBossCore::unloadRefraction(Antenna::Refraction_var& ref) const
{
	ThreadSyncGuard guard(&m_refractionMutex);  // We take the mutex in order to avoid concurrency errors between the WatchingThread and the WorkingThread
	if (!CORBA::is_nil(ref)) {
		try {
			m_services->releaseComponent((const char*)ref->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CBossCore::unloadRefraction()");
			Impl.setComponentName((const char *)m_config->getRefractionInstance());
			Impl.log(LM_WARNING);
		}
	}
	ref=Antenna::Refraction::_nil();
}

Antenna::EphemGenerator_ptr CBossCore::loadInternalGenerator(const Antenna::TGeneratorType& type) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	Antenna::EphemGenerator_var ref=Antenna::EphemGenerator::_nil();
	switch (type) {
		case Antenna::ANT_SIDEREAL : {
			if (CORBA::is_nil(m_siderealGenerator)) {
				m_siderealGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_siderealGenerator);
			break;
		}
		case Antenna::ANT_SUN : {
			if (CORBA::is_nil(m_sunGenerator)) {
				m_sunGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_sunGenerator);
			break;
		}
		case Antenna::ANT_MOON : {
			if (CORBA::is_nil(m_moonGenerator)) {
				m_moonGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_moonGenerator);
			break;
		}
		case Antenna::ANT_SATELLITE : {
			if (CORBA::is_nil(m_satelliteGenerator)) {
				m_satelliteGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_satelliteGenerator);
			break;
		}
		case Antenna::ANT_SOLARSYTEMBODY : {
			if (CORBA::is_nil(m_solarBodyGenerator)) {
				m_solarBodyGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_solarBodyGenerator);
			break;
		}
		case Antenna::ANT_OTF : {
			if (CORBA::is_nil(m_otfGenerator)) {
				m_otfGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_otfGenerator);			
			break;
		}
		default: { //Antenna::ANT_NONE
			break;
		}
	}
	return ref._retn();
}

Antenna::EphemGenerator_ptr CBossCore::loadPrimaryGenerator(const Antenna::TGeneratorType& type) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	Antenna::EphemGenerator_var ref=Antenna::EphemGenerator::_nil();
	switch (type) {
		case Antenna::ANT_SIDEREAL : {
			if (CORBA::is_nil(m_primarySiderealGenerator)) {
				m_primarySiderealGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_primarySiderealGenerator);
			break;
		}
		case Antenna::ANT_SUN : {
			if (CORBA::is_nil(m_primarySunGenerator)) {
				m_primarySunGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_primarySunGenerator);
			break;
		}
		case Antenna::ANT_MOON : {
			if (CORBA::is_nil(m_primaryMoonGenerator)) {
				m_primaryMoonGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_primaryMoonGenerator);
			break;
		}
		case Antenna::ANT_SATELLITE : {
			if (CORBA::is_nil(m_primarySatelliteGenerator)) {
				m_primarySatelliteGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_primarySatelliteGenerator);
			break;
		}
		case Antenna::ANT_SOLARSYTEMBODY : {
			if (CORBA::is_nil(m_primarySolarBodyGenerator)) {
				m_primarySolarBodyGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_primarySolarBodyGenerator);
			break;
		}
		case Antenna::ANT_OTF : {
			if (CORBA::is_nil(m_primaryOtfGenerator)) {
				m_primaryOtfGenerator=loadGenerator(type); // could throw exceptions
			}
			ref=Antenna::EphemGenerator::_duplicate(m_primaryOtfGenerator);			
			break;
		}
		default: { //Antenna::ANT_NONE
			break;
		}
	}
	return ref._retn();
}

Antenna::EphemGenerator_ptr CBossCore::loadGenerator(const Antenna::TGeneratorType& type) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl)
{
	Antenna::EphemGenerator_var generator=Antenna::EphemGenerator::_nil();
	// prepare the structure for loading the dynamic component
	maci::ComponentSpec_var cSpec=new maci::ComponentSpec();
	cSpec->component_name = CORBA::string_dup(COMPONENT_SPEC_ANY);    //name of the component
	cSpec->component_code = CORBA::string_dup(COMPONENT_SPEC_ANY);     //executable code for the component (e.g. DLL)
	cSpec->container_name = CORBA::string_dup(COMPONENT_SPEC_ANY);     //container where the component is deployed
	switch (type) {
		case Antenna::ANT_SIDEREAL : {
			cSpec->component_type = CORBA::string_dup((const char*)m_config->getSiderealInterface());    //IDL interface implemented by the component
			break;
		}
		case Antenna::ANT_SUN : {
			cSpec->component_type = CORBA::string_dup((const char*)m_config->getSunInterface());    //IDL interface implemented by the component
			break;
		}
		case Antenna::ANT_MOON : {
			cSpec->component_type = CORBA::string_dup((const char*)m_config->getMoonInterface());    //IDL interface implemented by the component
			break;
		}
		case Antenna::ANT_SATELLITE : {
			cSpec->component_type = CORBA::string_dup((const char*)m_config->getSatelliteInterface());    //IDL interface implemented by the component
			break;
		}
		case Antenna::ANT_SOLARSYTEMBODY : {
			cSpec->component_type = CORBA::string_dup((const char*)m_config->getSolarSystemBodyInterface());    //IDL interface implemented by the component
			break;
		}
		case Antenna::ANT_OTF : {
			cSpec->component_type = CORBA::string_dup((const char*)m_config->getOTFInterface());    //IDL interface implemented by the component
			break;
		}
		default: { //Antenna::ANT_NONE
			_EXCPT(ComponentErrors::CouldntGetComponentExImpl,impl,"CBossCore::loadGenerator()");
			impl.setComponentName("NONE");
			throw impl;   
		}
	}
	try {
		generator=m_services->getDynamicComponent<Antenna::EphemGenerator>(cSpec.in(),false);
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,impl,E,"CBossCore::loadGenerator()");
		impl.setComponentName((const char *)cSpec->component_type);
		throw impl;   
	}
	catch( CORBA::SystemException &E) {  // ... than CORBA system exception
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::loadGenerator()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		throw impl;
	}
	catch(...) {
		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CBossCore::loadGenerator()");
		throw impl;
	}
	// Make sure it's a valid reference
	if (CORBA::is_nil(generator)) {
		_EXCPT(ComponentErrors::CouldntGetComponentExImpl,impl,"CBossCore::loadGenerator()");
		throw impl;
	}
	return generator._retn();
}

void CBossCore::freeGenerator(Antenna::EphemGenerator_ptr& generator) throw (ComponentErrors::CouldntReleaseComponentExImpl)
{
	if (!CORBA::is_nil(generator)) {
		CString name=(const char *)generator->name();
		try {
			m_services->releaseComponent((const char*)name);
		}
		catch (maciErrType::CannotReleaseComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CBossCore::freeGenerator()");
			Impl.setComponentName((const char *)name);
			throw Impl;
		}
		generator=Antenna::EphemGenerator::_nil();
	}
}

void CBossCore::addOffsets(double &lon,double& lat,Antenna::TCoordinateFrame& frame,const TOffset& userOffset,const TOffset& scanOffset) const
{
	if (userOffset.frame==scanOffset.frame) { // if the frames differ the scanOffset prevails  
		lon=userOffset.lon+scanOffset.lon;
		lat=userOffset.lat+scanOffset.lat;
	}
	else {
		lon=scanOffset.lon;
		lat=scanOffset.lat;
	}
	frame=scanOffset.frame;
}

void CBossCore::computeFlux()
{
	m_targetFlux=0.0;
	if( (m_FWHM>0.0) && (!CORBA::is_nil(m_generatorFlux))){
		try {
			m_generatorFlux->computeFlux(m_currentObservingFrequency,m_FWHM,m_targetFlux);
		}
		catch (CORBA::SystemException& ex) {
		}
	}
}

Antenna::TReferenceFrame CBossCore::mapReferenceFrame(const IRA::CFrequencyTracking::TFrame& frame)
{
	if (frame==IRA::CFrequencyTracking::LSRK) {
		return Antenna::ANT_LSRK;
	}
	else if (frame==IRA::CFrequencyTracking::LSRD) {
		return Antenna::ANT_LSRD;
	}
	else if (frame==IRA::CFrequencyTracking::BARY) {
		return Antenna::ANT_BARY;
	}
	else if (frame==IRA::CFrequencyTracking::GALCEN) {
		return Antenna::ANT_GALCEN;
	}
	else if (frame==IRA::CFrequencyTracking::LGROUP) {
		return Antenna::ANT_LGROUP;
	}
	else if (frame==IRA::CFrequencyTracking::TOPOCEN) {
		return Antenna::ANT_TOPOCEN;
	}
	else  {
		return Antenna::ANT_UNDEF_FRAME;
	}
}

bool CBossCore::mapReferenceFrame(const IRA::CString& frame,Antenna::TReferenceFrame& outFrame)
{
	if (frame=="LSRK") {
		outFrame=Antenna::ANT_LSRK;
		return true;
	}
	else if (frame=="LSRD") {
		outFrame=Antenna::ANT_LSRD;
		return true;
	}
	else if (frame=="BARY") {
		outFrame=Antenna::ANT_BARY;
		return true;
	}
	else if (frame=="GALCEN") {
		outFrame=Antenna::ANT_GALCEN;
		return true;
	}
	else if (frame=="LGRP") {
		outFrame=Antenna::ANT_LGROUP;
		return true;
	}
	else if (frame=="TOPOCEN") {
		outFrame=Antenna::ANT_TOPOCEN;
		return true;
	}
	else  if (frame=="UNDEF") {
		outFrame=Antenna::ANT_UNDEF_FRAME;
		return true;
	}
	else return false;
}

void  CBossCore::mapReferenceFrame(const Antenna::TReferenceFrame& frame,IRA::CString& outFrame)
{
	if (frame==Antenna::ANT_LSRK) {
		outFrame="LSRK";
	}
	else if (frame==Antenna::ANT_LSRD) {
		outFrame="LSRD";
	}
	else if (frame==Antenna::ANT_BARY) {
		outFrame="BARY";
	}
	else if (frame==Antenna::ANT_GALCEN) {
		outFrame="GALCEN";
	}
	else if (frame==Antenna::ANT_LGROUP) {
		outFrame="LGRP";
	}
	else if (frame==Antenna::ANT_TOPOCEN) {
		outFrame="TOPOCEN";
	}
	else  { //if (frame==Antenna::ANT_UNDEF_FRAME)
		outFrame="UNDEF";
	}
}

IRA::CFrequencyTracking::TFrame CBossCore::mapReferenceFrame(const Antenna::TReferenceFrame& frame)
{
	if (frame==Antenna::ANT_LSRK) {
		return IRA::CFrequencyTracking::LSRK;
	}
	else if (frame==Antenna::ANT_LSRD) {
		return IRA::CFrequencyTracking::LSRD;
	}
	else if (frame==Antenna::ANT_BARY) {
		return IRA::CFrequencyTracking::BARY;
	}
	else if (frame==Antenna::ANT_GALCEN) {
		return IRA::CFrequencyTracking::GALCEN;
	}
	else if (frame==Antenna::ANT_LGROUP) {
		return IRA::CFrequencyTracking::LGROUP;
	}
	else { // (frame==Antenna::ANT_TOPOCEN)
		return IRA::CFrequencyTracking::TOPOCEN;
	}
}

Antenna::TVradDefinition CBossCore::mapVelocityDefinition(const  IRA::CFrequencyTracking::TDefinition & def)
{
	if (def==IRA::CFrequencyTracking::RADIO) {
		return Antenna::ANT_RADIO;
	}
	else if (def==IRA::CFrequencyTracking::OPTICAL) {
		return Antenna::ANT_OPTICAL;
	}
	else if (def==IRA::CFrequencyTracking::REDSHIFT) {
		return Antenna::ANT_REDSHIFT;
	}
	else  {
		return Antenna::ANT_UNDEF_DEF;
	}
}

bool CBossCore::mapVelocityDefinition(const  IRA::CString& def,Antenna::TVradDefinition& outDef)
{
	if (def=="RD") {
		outDef=Antenna::ANT_RADIO;
		return true;
	}
	else if (def=="OP") {
		outDef=Antenna::ANT_OPTICAL;
		return true;
	}
	else if (def=="Z") {
		outDef=Antenna::ANT_REDSHIFT;
		return true;
	}
	else  if  (def=="UNDEF"){
		outDef=Antenna::ANT_UNDEF_DEF;
		return true;
	}
	else return false;
}

void CBossCore::mapVelocityDefinition(const  Antenna::TVradDefinition& def,IRA::CString& outDef)
{
	if (def==Antenna::ANT_RADIO) {
		outDef="RD";
	}
	else if (def==Antenna::ANT_OPTICAL) {
		outDef="OP";
	}
	else if (def==Antenna::ANT_REDSHIFT) {
		outDef="Z";
	}
	else  { //if  (def==Antenna::ANT_UNDEF_DEF)
		outDef="UNDEF";
	}
}

IRA::CFrequencyTracking::TDefinition CBossCore::mapVelocityDefinition(const Antenna::TVradDefinition& def)
{
	if (def==Antenna::ANT_RADIO) {
		return IRA::CFrequencyTracking::RADIO;
	}
	else if (def==Antenna::ANT_OPTICAL) {
		return IRA::CFrequencyTracking::OPTICAL;
	}
	else { // if (frame==Antenna::ANT_REDSHIFT)
		return IRA::CFrequencyTracking::REDSHIFT;
	}
}




