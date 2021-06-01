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
        m_bodyName="";
}

SolarSystemBodyImpl::~SolarSystemBodyImpl()
{
        AUTO_TRACE("SolarSystemBodyImpl::~SolarSystemBodyImpl()");

}

void SolarSystemBodyImpl::initialize() throw(ACSErr::ACSbaseExImpl)
{
        AUTO_TRACE("SolarSystemBodyImpl::initialize()");
 
 
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

}



void SolarSystemBodyImpl::getAttributes(Antenna::SolarSystemBodyAttributes_out att) throw (CORBA::SystemException)
{

        AUTO_TRACE("SolarSystemBodyImpl::getAttributes()");

     

}

void SolarSystemBodyImpl::setOffsets(CORBA::Double lon,CORBA::Double lat,Antenna::TCoordinateFrame frame) throw (CORBA::SystemException,AntennaErrors::AntennaErrorsEx)
{       
        AUTO_TRACE("SolarSystemBodyImpl::setOffsets()");

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



