// $Id: OTFImpl.cpp,v 1.18 2011-06-21 16:39:14 a.orlati Exp $                                                               */


#include "OTFImpl.h"
#include <ObservatoryC.h>
#include <IRATools.h>
#include <maciContainerServices.h>

#define _GET_DOUBLE_ATTRIBUTE(ATTRIB,DESCR,FIELD,NAME) { \
	double tmpd; \
	if (!IRA::CIRATools::getDBValue(getContainerServices(),ATTRIB,tmpd,"alma/",NAME)) { \
		_EXCPT(ComponentErrors::CDBAccessExImpl,dummy,"OTFImpl::initialize()"); \
		dummy.setFieldName(ATTRIB); \
		throw dummy; \
	} \
	else { \
		FIELD=tmpd; \
		ACS_DEBUG_PARAM("OTFImpl::initialize()",DESCR" %lf",tmpd); \
	} \
}


static char *rcsId="@(#) $Id: OTFImpl.cpp,v 1.18 2011-06-21 16:39:14 a.orlati Exp $";
static void *use_rcsId = ((void)&use_rcsId, (void *) &rcsId);

OTFImpl::OTFImpl(const ACE_CString& CompName,maci::ContainerServices* containerServices) : 
	acscomponent::ACSComponentImpl(CompName,containerServices){
	m_componentName=CString(CompName);
	m_targetName="otf";
}

OTFImpl::~OTFImpl(){
	}
	
void OTFImpl::initialize() throw (ACSErr::ACSbaseExImpl) {
	AUTO_TRACE("OTFImpl::initialize()");
	ACS_LOG(LM_FULL_INFO, "OTFImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));

	Antenna::TSiteInformation_var site;
	Antenna::Observatory_var observatory=Antenna::Observatory::_nil();
	try {
		observatory=getContainerServices()->getComponent<Antenna::Observatory>("ANTENNA/Observatory");
	}
	catch (maciErrType::CannotGetComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"OTFImpl::initialize()");
		Impl.setComponentName((const char*)m_componentName);
		throw Impl;
	}
	ACS_LOG(LM_FULL_INFO, "OTFImpl::initialize()", (LM_INFO,
			(const char *)CString(m_componentName+"::OBSERVATORY_LOCATED")));
	try	{
		site=observatory->getSiteSummary();
	}
	catch (CORBA::SystemException& ex)	{
		_EXCPT(ComponentErrors::CORBAProblemExImpl,__dummy,"OTFImpl::initialize()");
		__dummy.setName(ex._name());
		__dummy.setMinor(ex.minor());
		throw __dummy;
	}
	ACS_LOG(LM_FULL_INFO, "OTFImpl::initialize()", (LM_INFO,"SITE_INITIALIZED"));
	m_site=CSite(site.out());
	m_dut1=site->DUT1;
	m_subScan.initSite(m_site);
	// Initializing offsets
	double initLonOff=0.0;
	double initLatOff=0.0;
	Antenna::TCoordinateFrame initOffFrame=Antenna::ANT_HORIZONTAL;
	m_subScan.setOffsets(initLonOff,initLatOff,initOffFrame);
	try {
		getContainerServices()->releaseComponent((const char*)observatory->name());
	}
	catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"OTFImpl::initialize()");
		Impl.setComponentName("ANTENNA/Observatory");
		throw Impl;
	}
	//loading parameters from CDB
	double maxAzimuthRate;
	double maxElevationRate;
	double maxReliableAzimuthRate;
	double maxReliableElevationRate;
	double maxAzimuthAcceleration;
	double maxElevationAcceleration;
	double accScaleFactor;
	_GET_DOUBLE_ATTRIBUTE("maxAzimuthRate","Max absolute value for Az rate (degrees/s):",
			maxAzimuthRate,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxElevationRate","Max absolute value for El rate (degrees/s):",
			maxElevationRate,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxReliableAzimuthRate","Max absolute value for reliable Az rate (degrees/s):",
			maxReliableAzimuthRate,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxReliableElevationRate","Max absolute value for reliable El rate (degrees/s):",
			maxReliableElevationRate,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxAzimuthAcceleration","Max absolute value for Az acceleration (degrees/s^2):",
			maxAzimuthAcceleration,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("maxElevationAcceleration","Max absolute value for El acceleration (degrees/s^2):",
			maxElevationAcceleration,"DataBlock/Mount");
	_GET_DOUBLE_ATTRIBUTE("accScaleFactor","Scaling factor for acceleration ramps",
			accScaleFactor,"DataBlock/Mount");
	m_subScan.initAzElRanges(maxAzimuthRate,maxElevationRate,maxReliableAzimuthRate,
			maxReliableElevationRate,maxAzimuthAcceleration,maxElevationAcceleration,
			accScaleFactor);
	// initialization 
	ACS_LOG(LM_FULL_INFO, "OTFImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZED"));
}

void OTFImpl::execute() throw (ACSErr::ACSbaseExImpl) {
	AUTO_TRACE("OTFImpl::execute()");
	ACS_LOG(LM_FULL_INFO, "OTFImpl::execute()",(LM_INFO,"COMPSTATE_OPERATIONAL"));
}

void OTFImpl::cleanup() {
	AUTO_TRACE("OTFImpl::cleanUp()");
	ACSComponentImpl::cleanUp();
}

void OTFImpl::aboutToAbort() {
	AUTO_TRACE("OTFImpl::aboutToAbort()");
}
	
void OTFImpl::getHorizontalCoordinate (
	        ACS::Time time,
	        CORBA::Double_out az,
	        CORBA::Double_out el
	      ) throw (CORBA::SystemException) {
	baci::ThreadSyncGuard guard(&m_mutex);
	AUTO_TRACE("OTFImpl::getHorizontalCoordinate()");
	TIMEVALUE UT(time);
	m_subScan.computePointingForUT(UT);
	double outAz,outEl;
	m_subScan.fillAzEl(outAz,outEl);
	az=outAz; 
	el=outEl;
}
	
void OTFImpl::computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux) throw (CORBA::SystemException)
{
	flux=0.0;
}

void OTFImpl::getAttributes (Antenna::OTFAttributes_out att) throw (CORBA::SystemException) {
	baci::ThreadSyncGuard guard(&m_mutex);
	AUTO_TRACE("OTFImpl::getAttributes()");
	att=new Antenna::OTFAttributes;
	/*TIMEVALUE now;
	IRA::CIRATools::getTime(now);
	m_subScan.computePointingForUT(now);*/
	m_subScan.fillAllAttributes(att);
	att->sourceID=CORBA::string_dup((const char *)m_targetName);
}

void OTFImpl::getAllCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,CORBA::Double_out lon,
		CORBA::Double_out lat) throw (CORBA::SystemException)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	TIMEVALUE UT(time);
	m_subScan.computePointingForUT(UT);
	m_subScan.fillApparent(az,el,ra,dec,jepoch,lon,lat);
}

CORBA::Boolean OTFImpl::checkTracking(ACS::Time time, CORBA::Double az, CORBA::Double el, CORBA::Double HPBW
		 ) throw (CORBA::SystemException) {
	baci::ThreadSyncGuard guard(&m_mutex);
	AUTO_TRACE("OTFImpl::checkTracking()");
	long response;
	m_subScan.checkPointingForUT(time,az,el,HPBW,response);
	if (response==1) return true;
	else return false;
}

CORBA::Long OTFImpl::checkPath(ACS::Time time, CORBA::Double az, CORBA::Double el
		 ) throw (CORBA::SystemException) {
	baci::ThreadSyncGuard guard(&m_mutex);
	AUTO_TRACE("OTFImpl::checkPath()");
	long response;
	m_subScan.checkPathSection(time,az,el,response);
	return response;
}

Antenna::TSections OTFImpl::setSubScan(
		const char *targetName,
		CORBA::Double initAz,
		Antenna::TSections initSector,
		CORBA::Double initEl,
		ACS::Time initTime,
		CORBA::Double lon1,
	    CORBA::Double lat1,
	    CORBA::Double lon2,
	    CORBA::Double lat2,
	    Antenna::TCoordinateFrame coordFrame,
	    Antenna::TsubScanGeometry geometry,
	    Antenna::TCoordinateFrame subScanFrame,
	    Antenna::TsubScanDescription description,
	    Antenna::TsubScanDirection direction,
	    ACS::Time startUT,
	    ACS::TimeInterval subScanDuration
	) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx,
	    		ComponentErrors::ComponentErrorsEx) {
	
		baci::ThreadSyncGuard guard(&m_mutex);
	    AUTO_TRACE("OTFImpl::setSubScan()");

	    Antenna::TSections steer=Antenna::ACU_NEUTRAL;
		try {
			//initializing the subscan
			steer=m_subScan.initScan((double)initAz, initSector, (double)initEl, initTime, (double)lon1, (double)lat1, (double)lon2,
					(double)lat2, coordFrame, geometry, subScanFrame, description, direction, startUT, m_dut1,
					subScanDuration);
			}
		catch (ComponentErrors::ComponentErrorsExImpl& ex) {
			// Aggiungere un ulteriore errore?
			ex.log(LM_DEBUG);
			throw ex.getComponentErrorsEx();
		}
		catch (AntennaErrors::ExceedingSlewingTimeExImpl& ex) {
			// This exception must not be propagated, or the subscan
			// schedule is stopped
			ex.log(LM_WARNING);
		}
		catch (AntennaErrors::AntennaErrorsExImpl& ex) {
			ex.log(LM_DEBUG);
			throw ex.getAntennaErrorsEx();			
		}
		catch (...) {
			_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"OTFImpl::setSubScan()");
			impl.log(LM_DEBUG);
			throw impl.getComponentErrorsEx();
		}
		m_targetName=IRA::CString(targetName);
		ACS_LOG(LM_FULL_INFO, "OTFImpl::setSubScan()", (LM_INFO,"NEW_SUBSCAN_COMMANDED"));
		return steer;
}

void OTFImpl::setOffsets(CORBA::Double lon, CORBA::Double lat, Antenna::TCoordinateFrame frame)
throw(CORBA::SystemException, AntennaErrors::AntennaErrorsEx) {
	baci::ThreadSyncGuard guard(&m_mutex);
	m_subScan.setOffsets(lon,lat,frame);
}

/* --------------- [ MACI DLL support functions ] -----------------*/
#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(OTFImpl)	

