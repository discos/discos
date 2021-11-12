#include "SolarSystemBodyImpl.h"
#include <IRA>
#include <Definitions.h>
#include <ObservatoryS.h>
#include <slamac.h>
#include <slalib.h>
#include <baci.h>
#include <acstimeEpochHelper.h>
#include <Site.h>
#include <DateTime.h>
#include <SkySource.h>
#include <iostream>

using namespace ComponentErrors;
// using namespace baci;
using namespace IRA;

using namespace baci;


SolarSystemBodyImpl::SolarSystemBodyImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices) :
        acscomponent::ACSComponentImpl(CompName, containerServices)
{
        AUTO_TRACE("SolarSystemBodyImpl::SolarSystemBodyImpl()");
        m_componentName=CString(CompName);
        m_bodyName="Unset";
}

SolarSystemBodyImpl::~SolarSystemBodyImpl()
{
        AUTO_TRACE("SolarSystemBodyImpl::~SolarSystemBodyImpl()");

}

void SolarSystemBodyImpl::initialize() throw(ACSErr::ACSbaseExImpl)
{
        AUTO_TRACE("SolarSystemBodyImpl::initialize()");
 
	ra_off = dec_off = 0.0;
	az_off = el_off = 0.0;
	m_offsetFrame=Antenna::ANT_HORIZONTAL;
 
 
        ACS_LOG(LM_FULL_INFO, "SolarSystemBodyImpl::initialize()", (LM_INFO,"COMPSTATE_INITIALIZING"));
}

void SolarSystemBodyImpl::cleanUp()
{
        AUTO_TRACE("SolarSystemBodyImpl::cleanUp()");
        
        ACSComponentImpl::cleanUp();
}

void SolarSystemBodyImpl::aboutToAbort()
{
        AUTO_TRACE("SolarSystemBodyImpl::aboutToAbort()");
}

void SolarSystemBodyImpl::execute() throw (ACSErr::ACSbaseExImpl)
{
        AUTO_TRACE("SolarSystemBodyImpl::execute()");
        CError error;
	Antenna::TSiteInformation_var site;
 
	
	Antenna::Observatory_var observatory=Antenna::Observatory::_nil();
	try {
		observatory=getContainerServices()->getComponent<Antenna::Observatory>("ANTENNA/Observatory");
	}
	catch (maciErrType::CannotGetComponentExImpl & ex){
		_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"SolarSystemBodyImpl::execute()");
		Impl.setComponentName("ANTENNA/Observatory");
		throw Impl;
	}
	
	ACS_LOG(LM_FULL_INFO,"MoonImpl::execute()", (LM_INFO, (const char *)CString(m_componentName+"::OBSERVATORY_LOCATED")));
	try {
		site=observatory->getSiteSummary();
	}
	catch (CORBA::SystemException& ex)		{
		_EXCPT(ComponentErrors::CORBAProblemExImpl,_dummy, "SolarSystemBodyImpl::execute()");
		_dummy.setName(ex._name());
		_dummy.setMinor(ex.minor());
		throw _dummy;
	}
    m_site=CSite(site.out());
	m_dut1=site->DUT1;
	m_longitude=site->longitude;
	m_latitude=site->latitude;
	m_height=site->height;
	try {
		getContainerServices()->releaseComponent((const char*)observatory->name());
	}
	catch  (maciErrType::CannotReleaseComponentExImpl& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"SolarSystemBodyImpl::initialize()");
		Impl.setComponentName("ANTENNA/Observatory");
		throw Impl;
	} 
	ACS_LOG(LM_FULL_INFO,"SolarSystemBodyImpl::execute()", (LM_INFO,"SITE_INITIALIZED"));        
}



void SolarSystemBodyImpl::getAttributes(Antenna::SolarSystemBodyAttributes_out att) throw (CORBA::SystemException)
{

        AUTO_TRACE("SolarSystemBodyImpl::getAttributes()");

     

}

void SolarSystemBodyImpl::setOffsets(CORBA::Double lon,CORBA::Double lat,Antenna::TCoordinateFrame frame) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx)
{       
        AUTO_TRACE("SolarSystemBodyImpl::setOffsets()");
        if (frame==Antenna::ANT_HORIZONTAL) {
		az_off=lon;
		el_off=lat;
		ra_off=0.0;
		dec_off=0.0;
	}
	else if (frame==Antenna::ANT_EQUATORIAL) {
		az_off=0.0;
		el_off=0.0;
		ra_off=lon;
		dec_off=lat;
	}
	else {
		_EXCPT(AntennaErrors::OffsetFrameNotSupportedExImpl,impl,"MoonImpl::setOffsets()");
		throw impl.getAntennaErrorsEx();
	}

}

void SolarSystemBodyImpl::getHorizontalCoordinate(ACS::Time time, CORBA::Double_out az, CORBA::Double_out el) throw (CORBA::SystemException)

{

        AUTO_TRACE("SolarSystemBodyImpl::getHorizontalCoordinate()");


}

void SolarSystemBodyImpl::getAllCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,CORBA::Double_out lon,
                CORBA::Double_out lat) throw (CORBA::SystemException)
{

        AUTO_TRACE("SolarSystemBodyImpl::getAllCoordinates()");


}

bool SolarSystemBodyImpl::checkTracking(ACS::Time time,CORBA::Double az,CORBA::Double el,CORBA::Double HPBW) throw (CORBA::SystemException)
{
     
     AUTO_TRACE("SolarSystemBodyImpl::getAllCoordinates()");

     return false;
     
}


void SolarSystemBodyImpl::setBodyName(const char* bodyName) throw (CORBA::SystemException)
{

        AUTO_TRACE("SolarSystemBodyImpl::setBodyName()");
        m_bodyName=CString(bodyName);
        std::cout << "name:" << bodyName <<std::endl;
        

}

void SolarSystemBodyImpl::computeFlux(CORBA::Double freq, CORBA::Double fwhm, CORBA::Double_out flux) throw (CORBA::SystemException)
{
        flux=0.0;
        AUTO_TRACE("SolarSystemBodyImpl::computeFlux()");

}



void SolarSystemBodyImpl::BodyPosition(TIMEVALUE &time)
{

        AUTO_TRACE("BodyPosition::computeFlux()");

}

#include <maciACSComponentDefines.h>
MACI_DLL_SUPPORT_FUNCTIONS(SolarSystemBodyImpl)



