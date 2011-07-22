// $Id: SkySourceImpl.cpp,v 1.9 2011-02-18 13:11:35 a.orlati Exp $

#include "SkySourceImpl.h"
#include <Definitions.h>
#include <math.h>
#include <ObservatoryS.h>
#include <slamac.h>

#define GET_PROPERTY_REFERENCE(TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr SkySourceImpl::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}

using namespace ComponentErrors;

static char *rcsId="@(#) $Id: SkySourceImpl.cpp,v 1.9 2011-02-18 13:11:35 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId, (void *) &rcsId);

SkySourceImpl::SkySourceImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
	acscomponent::ACSComponentImpl(CompName, containerServices)
{
	AUTO_TRACE("SkySourceImpl::SkySourceImpl()");
	m_componentName=CString(CompName);
	m_sourceName="";
	m_sourceCatalog=NULL;
}

SkySourceImpl::~SkySourceImpl()
{
	AUTO_TRACE("SkySourceImpl::~SkySourceImpl()");
}

void SkySourceImpl::initialize() throw(ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SkySourceImpl::initialize()");
	ACS_LOG(LM_FULL_INFO, "SkySourceImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		m_sourceCatalog=(CDBTable *)new CDBTable(getContainerServices(),"Source","DataBlock/SourceCatalog");
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"SkySourceImpl::initialize()");
		throw dummy;
	}
	// initialization 
	ACS_LOG(LM_FULL_INFO, "SkySourceImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SkySourceImpl::execute() throw(ACSErr::ACSbaseExImpl)
{
	CError error;
	IRAIDL::TSiteInformation_var site;
	AUTO_TRACE("SkySourceImpl::execute()");

	Antenna::Observatory_var observatory=Antenna::Observatory::_nil();
	try {
		observatory=getContainerServices()->getComponent<Antenna::Observatory>("ANTENNA/Observatory");
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"SkySourceImpl::execute()");
		Impl.setComponentName("ANTENNA/Observatory");
		throw Impl;
	}
	ACS_LOG(LM_FULL_INFO,"SkySourceImpl::execute()",(LM_INFO,
			(const char *)CString(m_componentName+"::OBSERVATORY_LOCATED")));
	try	{
		site=observatory->getSiteSummary();
	}
	catch (CORBA::SystemException& ex)	{
		_EXCPT(ComponentErrors::CORBAProblemExImpl,__dummy,"SkySourceImpl::execute()");
		__dummy.setName(ex._name());
		__dummy.setMinor(ex.minor());
		throw __dummy;
	}
	ACS_LOG(LM_FULL_INFO, "SkySourceImpl::execute()", (LM_INFO,"SITE_INITIALIZED"));
	m_site=CSite(site.out());
	m_dut1=site->DUT1;
	try {
		getContainerServices()->releaseComponent((const char*)observatory->name());
	}
	catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"SkySourceImpl::execute()");
		Impl.setComponentName((const char*)observatory->name());
		throw Impl;
	}
	error.Reset();
	if (!m_sourceCatalog->addField(error, "sourceName", CDataField::STRING)) {
		error.setExtra("Error adding field sourceName", 0);
	}
	else if (!m_sourceCatalog->addField(error, "rightAscension",CDataField::STRING)) {
		error.setExtra("Error adding field rightAscension", 0);
	}
	else if (!m_sourceCatalog->addField(error, "declination",CDataField::STRING)) {
		error.setExtra("Error adding field declination", 0);
	}
	else if (!m_sourceCatalog->addField(error, "epoch", CDataField::STRING)) {
		error.setExtra("Error adding field epoch", 0);
	}
	else if (!m_sourceCatalog->addField(error, "pmRA", CDataField::DOUBLE))	{
		error.setExtra("Error adding field pmRA", 0);
	}
	else if (!m_sourceCatalog->addField(error, "pmDec", CDataField::DOUBLE)) {
		error.setExtra("Error adding field pmDec", 0);
	}
	else if (!m_sourceCatalog->addField(error, "parallax", CDataField::DOUBLE))	{
		error.setExtra("Error adding field parallax", 0);
	}
	else if (!m_sourceCatalog->addField(error, "radialVelocity",CDataField::DOUBLE)) {
		error.setExtra("Error adding field radialVelocity", 0);
	}
	if (!error.isNoError()) {
		_EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,error);
		dummy.setCode(error.getErrorCode());
		dummy.setDescription((const char*)error.getDescription());
		throw dummy;
	}
	if (!m_sourceCatalog->openTable(error))	{
		_EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
		throw dummy;
	}
	ACS_LOG(LM_FULL_INFO, "SkySourceImpl::execute()",(LM_INFO,"SOURCE_CATALOG_OPENED"));
	ACS_LOG(LM_FULL_INFO, "SkySourceImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void SkySourceImpl::cleanUp()
{
	AUTO_TRACE("SkySourceImpl::cleanUp()");
	if (m_sourceCatalog!=NULL) {
		m_sourceCatalog->closeTable();
		delete m_sourceCatalog;
	}
	ACSComponentImpl::cleanUp();
}

void SkySourceImpl::aboutToAbort()
{
	if (m_sourceCatalog!=NULL) {
		m_sourceCatalog->closeTable();
		delete m_sourceCatalog;
	}
	AUTO_TRACE("SkySourceImpl::aboutToAbort()");
}

void SkySourceImpl::getAttributes(Antenna::SkySourceAttributes_out att) throw(CORBA::SystemException)
{
	double jepoch,pmRa,pmDec,prlx,rvel;
	AUTO_TRACE("SkySourceImpl::getAttributes()");
	baci::ThreadSyncGuard guard(&m_mutex);
	CDateTime now;
	now.setCurrentDateTime(m_dut1);
	m_source.process(now,m_site);
	att=new Antenna::SkySourceAttributes;
	m_source.getInputEquatorial(att->inputRightAscension,
			att->inputDeclination,att->inputJEpoch,
			att->inputRaProperMotion,att->inputDecProperMotion,
			att->inputParallax,att->inputRadialVelocity);
	att->inputRaProperMotion*=cos((double)att->inputDeclination);
	m_source.getJ2000Equatorial(att->J2000RightAscension,att->J2000Declination,jepoch,pmRa,pmDec,prlx,rvel);
	m_source.getInputGalactic(att->inputGalacticLongitude,att->inputGalacticLatitude);
	att->sourceID=CORBA::string_dup((const char *)m_sourceName);
	m_source.getApparentEquatorial(att->rightAscension,att->declination,att->julianEpoch);
	m_source.getApparentHorizontal(att->azimuth,att->elevation);
	m_source.getHorizontalOffsets(att->userAzimuthOffset,att->userElevationOffset);
	m_source.getEquatorialOffsets(att->userRightAscensionOffset,att->userDeclinationOffset);
	m_source.getGalacticOffsets(att->userLongitudeOffset,att->userLatitudeOffset);
	att->parallacticAngle=m_source.getParallacticAngle();
}

CORBA::Boolean SkySourceImpl::checkTracking(ACS::Time time,CORBA::Double az,CORBA::Double el,
		CORBA::Double HPBW) throw (CORBA::SystemException)
{
	AUTO_TRACE("SkySourceImpl::checkTracking()")
	double computedAz,computedEl,azErr,elErr,skyErr;
	TIMEVALUE val(time);
	IRA::CDateTime refTime(val,m_dut1);
	baci::ThreadSyncGuard guard(&m_mutex);  // obtain access
	m_source.process(refTime,m_site);
	m_source.getApparentHorizontal(computedAz,computedEl);
	elErr=computedEl-el;
	azErr=(computedAz-az)*cos(el);
	skyErr=sqrt(elErr*elErr+azErr*azErr); //total skyError
	return skyErr<=HPBW*0.1;
}

void SkySourceImpl::setOffsets(CORBA::Double lon,CORBA::Double lat,Antenna::TCoordinateFrame frame) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (frame==Antenna::ANT_GALACTIC) {
		m_source.setGalacticOffsets(lon,lat);
		m_source.setEquatorialOffsets(0.0,0.0);
		m_source.setHorizontalOffsets(0.0,0.0);
	}
	else if (frame==Antenna::ANT_EQUATORIAL) {
		m_source.setGalacticOffsets(0.0,0.0);
		m_source.setEquatorialOffsets(lon,lat);
		m_source.setHorizontalOffsets(0.0,0.0);
	}
	else if (frame==Antenna::ANT_HORIZONTAL) {
		m_source.setGalacticOffsets(0.0,0.0);
		m_source.setEquatorialOffsets(0.0,0.0);
		m_source.setHorizontalOffsets(lon,lat);		
	}
}

void SkySourceImpl::getHorizontalCoordinate(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el) throw(CORBA::SystemException)
{
	AUTO_TRACE("SkySourceImpl::getHorizontalCoordinate()");
	double azi,ele;
	TIMEVALUE val(time);
	IRA::CDateTime ctime(val,m_dut1);	
	baci::ThreadSyncGuard guard(&m_mutex);	
	m_source.process(ctime,m_site);
	m_source.getApparentHorizontal(azi,ele);
	az=azi; el=ele;
}

void SkySourceImpl::loadSourceFromCatalog(const char* sourceName) throw(CORBA::SystemException,
		AntennaErrors::AntennaErrorsEx,ComponentErrors::ComponentErrorsEx)
{
	AUTO_TRACE("SkySourceImpl::loadSourceFromCatalog()");
	baci::ThreadSyncGuard guard(&m_mutex);
	bool done=false;
	m_sourceCatalog->First();
	CString name(sourceName);
	for (int i=0;i<m_sourceCatalog->recordCount();i++) {
		CString source((*m_sourceCatalog)["sourceName"]->asString());
		if (name==source) {
			done=true;
			break;
		}
		m_sourceCatalog->Next();
	}
	if (done) {
		Antenna::TSystemEquinox eq=Antenna::ANT_J2000;
		double ra=0.0, dec=0.0;
		double ss=0.0;
		unsigned hh=0, mm=0;
		int dd;
		if (sscanf((const char *)(*m_sourceCatalog)["rightAscension"]->asString(),"%d:%d:%lf",&hh,&mm,&ss)!=3) {
			_EXCPT(ComponentErrors::CDBFieldFormatExImpl,__dummy,"SkySourceImpl::loadSourceFromCatalog()");
			__dummy.setFieldName("rightAscension");
			__dummy.log(LM_DEBUG);
			throw __dummy.getComponentErrorsEx();
		}
		ra=hh+mm/60.0+ss/3600.0; // convert ra to radians.
		ra*=(DPI/12.0);
		if (sscanf((const char *)(*m_sourceCatalog)["declination"]->asString(),"%d:%d:%lf",&dd,&mm,&ss)!=3) {
			_EXCPT(ComponentErrors::CDBFieldFormatExImpl,__dummy,"SkySourceImpl::loadSourceFromCatalog()");
			__dummy.setFieldName("declination");
			__dummy.log(LM_DEBUG);
			throw __dummy.getComponentErrorsEx();
		}
		if (dd<0) {
			dec=dd-mm/60.0-ss/3600.0; //convert declination to radians
		}
		else {
			dec=dd+mm/60.0+ss/3600.0;
		}
		dec*=(DPI/180.0);
		if ((*m_sourceCatalog)["epoch"]->asString()=="J2000") {
			eq=Antenna::ANT_J2000;
		}
		else if ((*m_sourceCatalog)["epoch"]->asString()=="B1950")	{
			eq=Antenna::ANT_B1950;
		}
		else {
			_EXCPT(ComponentErrors::CDBFieldFormatExImpl,__dummy,"SkySourceImpl::loadSourceFromCatalog()");
			__dummy.setFieldName("epoch");
			throw __dummy.getComponentErrorsEx();
		}
		setSource((*m_sourceCatalog)["sourceName"]->asString(),
		   ra,dec,eq,
		   (*m_sourceCatalog)["pmRA"]->asDouble(),
		   (*m_sourceCatalog)["pmDec"]->asDouble(),
		   (*m_sourceCatalog)["parallax"]->asDouble(),
		   (*m_sourceCatalog)["radialVelocity"]->asDouble());
		ACS_LOG(LM_FULL_INFO,"SkySourceImpl::loadSourceFromCatalog()",(LM_INFO,"NEW_SOURCE_LOADED_FROM_CATALOG: %s",sourceName));
	}
	else {
		_EXCPT(AntennaErrors::SourceNotFoundExImpl, __dummy,"SkySourceImpl::loadSourceFromCatalog()");
		__dummy.setSourceName(sourceName);
		__dummy.log(LM_DEBUG);
		throw __dummy.getAntennaErrorsEx();
	}
}

void SkySourceImpl::setFixedPoint(CORBA::Double az, CORBA::Double el) throw(CORBA::SystemException)
{
	AUTO_TRACE("SkySourceImpl::setFixedPoint()");
	baci::ThreadSyncGuard guard(&m_mutex);
	m_source.setInputHorizontal(az,el,m_site);
	ACS_LOG(LM_FULL_INFO,"SkySourceImpl::setFixedPoint()",
	   (LM_INFO,"NEW_FIXED_POSITION: %f,%f",az,el));
}

void SkySourceImpl::setSourceFromGalactic(const char* sourceName,CORBA::Double longitude, CORBA::Double latitude) throw
   (CORBA::SystemException)
{
	AUTO_TRACE("SkySourceImpl::setSourceFromGalactic()");
	baci::ThreadSyncGuard guard(&m_mutex);
	m_source.setInputGalactic(longitude,latitude);
	m_sourceName=CString(sourceName);
	ACS_LOG(LM_FULL_INFO,"SkySourceImpl::setSourceFromGalactic()",
	   (LM_INFO,"NEW_SOURCE_FROM_GALACTIC: %s,%f,%f",sourceName,longitude,latitude));
}

void SkySourceImpl::setSourceFromEquatorial(const char *sourceName,
		CORBA::Double ra,CORBA::Double dec,Antenna::TSystemEquinox equinox,
		CORBA::Double dra,CORBA::Double ddec,CORBA::Double parallax,
		CORBA::Double rvel) throw(CORBA::SystemException)
{
	AUTO_TRACE("SkySourceImpl::setSourceFromEquatorial()");
	baci::ThreadSyncGuard guard(&m_mutex);
	setSource(sourceName,ra,dec,equinox,dra,ddec,parallax,rvel);
	ACS_LOG(LM_FULL_INFO,"SkySourceImpl::setSourceFromEquatorial()",
	   (LM_INFO,"NEW_SOURCE_FROM_EQUATORIAL: %s,%f,%f",sourceName,ra,dec));
}

void SkySourceImpl::setSource(CString sourceName,double ra,double dec,
		Antenna::TSystemEquinox equinox,double dra,double ddec,
		double parallax,double rvel)
{
	IRA::CSkySource::TEquinox eq;
	if (equinox==Antenna::ANT_B1950)
		eq=CSkySource::SS_B1950;
	else if (equinox==Antenna::ANT_J2000)
		eq=CSkySource::SS_J2000;
	else
		eq=CSkySource::SS_APPARENT;
	dra/=cos((double)dec); // the CSkySource object requires dRa/dt not cos(dec)*dRa/dt 
	m_source.setInputEquatorial(ra,dec,eq,dra,ddec,parallax,rvel);
	m_sourceName=sourceName;
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SkySourceImpl)

