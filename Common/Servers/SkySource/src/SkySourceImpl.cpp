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

/*static char *rcsId="@(#) $Id: SkySourceImpl.cpp,v 1.11 2011-06-21 16:40:04 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId, (void *) &rcsId);*/

SkySourceImpl::SkySourceImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
	acscomponent::ACSComponentImpl(CompName, containerServices)
{
	AUTO_TRACE("SkySourceImpl::SkySourceImpl()");
	m_componentName=CString(CompName);
	m_sourceName="";
	m_sourceCatalog=NULL;
    fluxParam.init = false;
}

SkySourceImpl::~SkySourceImpl()
{
	AUTO_TRACE("SkySourceImpl::~SkySourceImpl()");
}

void SkySourceImpl::initialize() throw(ACSErr::ACSbaseExImpl)
{
	AUTO_TRACE("SkySourceImpl::initialize()");
	CUSTOM_LOG(LM_FULL_INFO, "SkySourceImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));
	try {
		m_sourceCatalog=(CDBTable *)new CDBTable(getContainerServices(),"Source","DataBlock/SourceCatalog");
	}
	catch (std::bad_alloc& ex) {
		_EXCPT(MemoryAllocationExImpl,dummy,"SkySourceImpl::initialize()");
		throw dummy;
	}
	// initialization 
	CUSTOM_LOG(LM_FULL_INFO, "SkySourceImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void SkySourceImpl::execute() throw(ACSErr::ACSbaseExImpl)
{
	CError error;
	Antenna::TSiteInformation_var site;
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
	CUSTOM_LOG(LM_FULL_INFO,"SkySourceImpl::execute()",(LM_INFO,
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
	CUSTOM_LOG(LM_FULL_INFO, "SkySourceImpl::execute()", (LM_INFO,"SITE_INITIALIZED"));
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
    else if (!m_sourceCatalog->addField(error, "type",CDataField::STRING)) {
		error.setExtra("Error adding field type", 0);
	}
    else if (!m_sourceCatalog->addField(error, "nFreqRange",CDataField::LONGLONG)) {
		error.setExtra("Error adding field nFreqRange", 0);
	}
    else if (!m_sourceCatalog->addField(error, "freqRange",CDataField::STRING)) {
		error.setExtra("Error adding field freqRange", 0);
	}
    else if (!m_sourceCatalog->addField(error, "fluxCoeff1",CDataField::STRING)) {
		error.setExtra("Error adding field fluxCoeff1", 0);
	}
    else if (!m_sourceCatalog->addField(error, "fluxCoeff2",CDataField::STRING)) {
		error.setExtra("Error adding field fluxCoeff2", 0);
	}
    else if (!m_sourceCatalog->addField(error, "fluxCoeff3",CDataField::STRING)) {
		error.setExtra("Error adding field fluxCoeff3", 0);
	}
    else if (!m_sourceCatalog->addField(error, "size",CDataField::DOUBLE)) {
		error.setExtra("Error adding field size", 0);
	}
    else if (!m_sourceCatalog->addField(error, "model",CDataField::STRING)) {
		error.setExtra("Error adding field model", 0);
	}
    else if (!m_sourceCatalog->addField(error, "modelCoeff1",CDataField::DOUBLE)) {
		error.setExtra("Error adding field modelCoeff1", 0);
	}
    else if (!m_sourceCatalog->addField(error, "modelCoeff2",CDataField::DOUBLE)) {
		error.setExtra("Error adding field modelCoeff2", 0);
	}
    else if (!m_sourceCatalog->addField(error, "modelCoeff3",CDataField::DOUBLE)) {
		error.setExtra("Error adding field modelCoeff3", 0);
	}
    else if (!m_sourceCatalog->addField(error, "modelCoeff4",CDataField::DOUBLE)) {
		error.setExtra("Error adding field modelCoeff4", 0);
	}
    else if (!m_sourceCatalog->addField(error, "modelCoeff5",CDataField::DOUBLE)) {
		error.setExtra("Error adding field modelCoeff5", 0);
	}
    else if (!m_sourceCatalog->addField(error, "modelCoeff6",CDataField::DOUBLE)) {
		error.setExtra("Error adding field modelCoeff6", 0);
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
	CUSTOM_LOG(LM_FULL_INFO, "SkySourceImpl::execute()",(LM_INFO,"SOURCE_CATALOG_OPENED"));
	CUSTOM_LOG(LM_FULL_INFO, "SkySourceImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
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
	att->sourceID=CORBA::string_dup((const char *)m_sourceName);
	m_source.getJ2000Equatorial(att->J2000RightAscension,att->J2000Declination,jepoch,pmRa,pmDec,prlx,rvel);
	m_source.getApparentEquatorial(att->rightAscension,att->declination,att->julianEpoch);
	m_source.getApparentGalactic(att->gLongitude,att->gLatitude);
	m_source.getApparentHorizontal(att->azimuth,att->elevation);
	att->parallacticAngle=m_source.getParallacticAngle();
	m_source.getHorizontalOffsets(att->userAzimuthOffset,att->userElevationOffset);
	m_source.getEquatorialOffsets(att->userRightAscensionOffset,att->userDeclinationOffset);
	m_source.getGalacticOffsets(att->userLongitudeOffset,att->userLatitudeOffset);
	m_source.getInputEquatorial(att->inputRightAscension,
			att->inputDeclination,att->inputJEpoch,
			att->inputRaProperMotion,att->inputDecProperMotion,
			att->inputParallax,att->inputRadialVelocity);
	att->inputRaProperMotion*=cos((double)att->inputDeclination);
	m_source.getInputGalactic(att->inputGalacticLongitude,att->inputGalacticLatitude);
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
		//m_source.setEquatorialOffsets(0.0,0.0);
		//m_source.setHorizontalOffsets(0.0,0.0);
	}
	else if (frame==Antenna::ANT_EQUATORIAL) {
		//m_source.setGalacticOffsets(0.0,0.0);
		m_source.setEquatorialOffsets(lon,lat);
		//m_source.setHorizontalOffsets(0.0,0.0);
	}
	else if (frame==Antenna::ANT_HORIZONTAL) {
		//m_source.setGalacticOffsets(0.0,0.0);
		//m_source.setEquatorialOffsets(0.0,0.0);
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

void SkySourceImpl::getAllCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,CORBA::Double_out lon,
		CORBA::Double_out lat) throw (CORBA::SystemException)
{
	TIMEVALUE val(time);
	double azi,ele,rae,dece,lone,late,jepoche;
	IRA::CDateTime ctime(val,m_dut1);
	baci::ThreadSyncGuard guard(&m_mutex);
	m_source.process(ctime,m_site);
	m_source.getApparentHorizontal(azi,ele);
	m_source.getApparentEquatorial(rae,dece,jepoche);
	m_source.getApparentGalactic(lone,late);
	az=azi; el=ele;
	ra=rae; dec=dece; jepoch=jepoch;
	lon=lone; lat=late;
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
		//int dd;
		//if (!IRA::CIRATools::rightAscensionToRad((*m_sourceCatalog)["rightAscension"]->asString(),ra,true)) {
		if (sscanf((const char *)(*m_sourceCatalog)["rightAscension"]->asString(),"%d:%d:%lf",&hh,&mm,&ss)!=3) {
			_EXCPT(ComponentErrors::CDBFieldFormatExImpl,__dummy,"SkySourceImpl::loadSourceFromCatalog()");
			__dummy.setFieldName("rightAscension");
			CUSTOM_EXCPT_LOG(__dummy,LM_DEBUG);
			throw __dummy.getComponentErrorsEx();
		}
		ra=hh+mm/60.0+ss/3600.0; // convert ra to radians.
		ra*=(DPI/12.0);

		if(!IRA::CIRATools::declinationToRad((*m_sourceCatalog)["declination"]->asString(),dec,true)) {
		//if (sscanf((const char *)(*m_sourceCatalog)["declination"]->asString(),"%d:%d:%lf",&dd,&mm,&ss)!=3) {
			_EXCPT(ComponentErrors::CDBFieldFormatExImpl,__dummy,"SkySourceImpl::loadSourceFromCatalog()");
			__dummy.setFieldName("declination");
			CUSTOM_EXCPT_LOG(__dummy,LM_DEBUG);
			throw __dummy.getComponentErrorsEx();
		}
		/*if (dd<0) {
			dec=dd-mm/60.0-ss/3600.0; //convert declination to radians
		}
		else {
			dec=dd+mm/60.0+ss/3600.0;
		}
		dec*=(DPI/180.0);*/
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

        fluxParam.init = true;
        strncpy(fluxParam.name,(const char *)name,11);
        fluxParam.type=((*m_sourceCatalog)["type"]->asString())[0];
        fluxParam.nFreqRange = (int)((*m_sourceCatalog)["nFreqRange"]->asLongLong());
        strncpy(fluxParam.freqRange, (const char*)((*m_sourceCatalog)["freqRange"]->asString()),50);
        strncpy(fluxParam.fluxCoeff1, (const char*)((*m_sourceCatalog)["fluxCoeff1"]->asString()),50);
        strncpy(fluxParam.fluxCoeff2, (const char*)((*m_sourceCatalog)["fluxCoeff2"]->asString()),50);
        strncpy(fluxParam.fluxCoeff3, (const char*)((*m_sourceCatalog)["fluxCoeff3"]->asString()),50);
        fluxParam.size = (*m_sourceCatalog)["size"]->asDouble();
        strncpy(fluxParam.model,(const char*)((*m_sourceCatalog)["model"]->asString()),5);
        fluxParam.mcoeff[0] = (*m_sourceCatalog)["modelCoeff1"]->asDouble();
        fluxParam.mcoeff[1] = (*m_sourceCatalog)["modelCoeff2"]->asDouble();
        fluxParam.mcoeff[2] = (*m_sourceCatalog)["modelCoeff3"]->asDouble();
        fluxParam.mcoeff[3] = (*m_sourceCatalog)["modelCoeff4"]->asDouble();
        fluxParam.mcoeff[4] = (*m_sourceCatalog)["modelCoeff5"]->asDouble();
        fluxParam.mcoeff[5] = (*m_sourceCatalog)["modelCoeff6"]->asDouble();

		CUSTOM_LOG(LM_FULL_INFO,"SkySourceImpl::loadSourceFromCatalog()",(LM_INFO,"NEW_SOURCE_LOADED_FROM_CATALOG: %s",sourceName));
	}
	else {
        fluxParam.init = false;
		_EXCPT(AntennaErrors::SourceNotFoundExImpl, __dummy,"SkySourceImpl::loadSourceFromCatalog()");
		__dummy.setSourceName(sourceName);
		CUSTOM_EXCPT_LOG(__dummy,LM_DEBUG);
		throw __dummy.getAntennaErrorsEx();
	}
    sourceFlux=CSourceFlux(fluxParam);
}

void SkySourceImpl::computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux) throw(CORBA::SystemException)
{
    flux = sourceFlux.computeSourceFlux(freq, fwhm);
    CUSTOM_LOG(LM_FULL_INFO,"SkySourceImpl::computeFlux()", (LM_INFO,"FLUX: %f",flux));
}

void SkySourceImpl::setFixedPoint(CORBA::Double az, CORBA::Double el) throw(CORBA::SystemException)
{
	AUTO_TRACE("SkySourceImpl::setFixedPoint()");
	baci::ThreadSyncGuard guard(&m_mutex);
	m_source.setInputHorizontal(az,el,m_site);
	// in that case the flux computation is not available since the flux parameters are not known
	fluxParam.init = false;
	CUSTOM_LOG(LM_FULL_INFO,"SkySourceImpl::setFixedPoint()",
	   (LM_INFO,"NEW_FIXED_POSITION: %f,%f",az,el));
}

void SkySourceImpl::setSourceFromGalactic(const char* sourceName,CORBA::Double longitude, CORBA::Double latitude) throw
   (CORBA::SystemException)
{
	AUTO_TRACE("SkySourceImpl::setSourceFromGalactic()");
	baci::ThreadSyncGuard guard(&m_mutex);
	m_source.setInputGalactic(longitude,latitude);
	m_sourceName=CString(sourceName);
	// in that case the flux computation is not available since the flux parameters are not known
	fluxParam.init = false;
	CUSTOM_LOG(LM_FULL_INFO,"SkySourceImpl::setSourceFromGalactic()",
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
	// in that case the flux computation is not available since the flux parameters are not known
	fluxParam.init = false;
	CUSTOM_LOG(LM_FULL_INFO,"SkySourceImpl::setSourceFromEquatorial()",
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

