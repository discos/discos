#include "Schedule.h"

using namespace Schedule;

CSubScanBinder::CSubScanBinder(Antenna::TTrackingParameters * const primary,Antenna::TTrackingParameters * const secondary,
 MinorServo::MinorServoScan * const servo,Receivers::TReceiversParameters * const receievers):
 m_primary(primary),m_secondary(secondary),m_servo(servo),m_receievers(receievers)
{
	init();
}

CSubScanBinder::~CSubScanBinder()
{

}

void CSubScanBinder::lonOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration)
{
	m_secondary->secondary=false; m_secondary->paramNumber=0; m_secondary->applyOffsets=false;
	m_secondary->type=Antenna::ANT_NONE; m_secondary->enableCorrection=true;
	m_primary->type=Antenna::ANT_OTF;
	m_primary->otf.lon1=0.0; m_primary->otf.lat1=0.0; // they will be set by internal antenna engine
	if (scanFrame!=Antenna::ANT_HORIZONTAL) {
		m_primary->otf.coordFrame=scanFrame;
		m_primary->otf.subScanFrame=scanFrame;
	}
	else {
		m_primary->otf.coordFrame=Antenna::ANT_EQUATORIAL;
		m_primary->otf.subScanFrame=Antenna::ANT_HORIZONTAL;
	}
	m_primary->otf.description=Antenna::SUBSCAN_CENTER; // a must !!!!!
	m_primary->otf.subScanDuration=duration;
	m_primary->otf.direction=Antenna::SUBSCAN_INCREASE; // this is
	m_primary->otf.geometry=Antenna::SUBSCAN_CONSTLAT; // a must !!!!!
	m_primary->otf.lon2=span;
	m_primary->otf.lat2=0.0;
	m_primary->applyOffsets=false;  // no offset for this scan!
	m_primary->secondary=true;  // force the engine to look for the secondary track, or if the secondary is not set (this case) to use the previously commanded track
	m_primary->targetName=CORBA::string_dup("otf"); // not strictly necessary as the system will take the last commanded name
	m_primary->section=Antenna::ACU_NEUTRAL;
	m_primary->enableCorrection=true;
	// The other subsystems can stay with defaults
}

void CSubScanBinder::latOTF(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration)
{
	m_secondary->secondary=false; m_secondary->paramNumber=0; m_secondary->applyOffsets=false;
	m_secondary->type=Antenna::ANT_NONE; m_secondary->enableCorrection=true;
	m_primary->type=Antenna::ANT_OTF;
	m_primary->otf.lon1=0.0; // they will be set by internal engine
	m_primary->otf.lat1=0.0;
	if (scanFrame!=Antenna::ANT_HORIZONTAL) {
		m_primary->otf.coordFrame=scanFrame;
		m_primary->otf.subScanFrame=scanFrame;
	}
	else {
		m_primary->otf.coordFrame=Antenna::ANT_EQUATORIAL;
		m_primary->otf.subScanFrame=Antenna::ANT_HORIZONTAL;
	}
	m_primary->otf.description=Antenna::SUBSCAN_CENTER; // a must !!!!!
	m_primary->otf.subScanDuration=duration;
	m_primary->otf.direction=Antenna::SUBSCAN_INCREASE;
	m_primary->otf.geometry=Antenna::SUBSCAN_CONSTLON; // a must !!!!!
	m_primary->otf.lon2=0.0;
	m_primary->otf.lat2=span;
	m_primary->applyOffsets=false;  // no offset for this scan!
	m_primary->secondary=true;  // force the engine to look for the secondary track, or if the secondary is not set (this case) to use the previously commanded track
	m_primary->targetName=CORBA::string_dup("otf"); // not strictly necessary as the system will take the last commanded name
	m_primary->section=Antenna::ACU_NEUTRAL;
	m_primary->enableCorrection=true;
	// The other subsystems can stay with defaults
}

void CSubScanBinder::sidereal(const char * targetName,const double& ra,const double& dec,const Antenna::TSystemEquinox& eq,const Antenna::TSections& section)
{
	m_secondary->secondary=false; m_secondary->paramNumber=0; m_secondary->applyOffsets=false;
	m_secondary->type=Antenna::ANT_NONE; m_secondary->enableCorrection=true;
	m_primary->type=Antenna::ANT_SIDEREAL;
	m_primary->parameters[0]=ra;
	m_primary->parameters[1]=dec;
	m_primary->paramNumber=2;
	m_primary->frame=Antenna::ANT_EQUATORIAL;
	m_primary->equinox=eq;
	m_primary->section=section;
	m_primary->targetName=CORBA::string_dup(targetName);
	m_primary->secondary=false;
	m_primary->applyOffsets=false;
	m_primary->enableCorrection=true;
	// The other subsystems can stay with defaults
}

void CSubScanBinder::moon()
{
	m_secondary->type=Antenna::ANT_NONE;
	m_secondary->secondary=false;
	m_secondary->applyOffsets=false;
	m_secondary->paramNumber=0;
	m_secondary->enableCorrection=true;
	m_primary->type=Antenna::ANT_MOON;
	m_primary->secondary=false;
	m_primary->targetName=CORBA::string_dup("Moon");
	m_primary->applyOffsets=false;
	m_primary->section=Antenna::ACU_NEUTRAL;
	m_primary->enableCorrection=true;
	// The other subsystems can stay with defaults
}

void CSubScanBinder::track(const char *targetName)
{
	m_secondary->type=Antenna::ANT_NONE;
	m_secondary->secondary=false;
	m_secondary->applyOffsets=false;
	m_secondary->paramNumber=0;
	m_secondary->enableCorrection=true;
	m_primary->type=Antenna::ANT_SIDEREAL;
	m_primary->secondary=false;
	m_primary->paramNumber=0;
	m_primary->applyOffsets=false;
	m_primary->targetName=CORBA::string_dup(targetName);
	m_primary->section=Antenna::ACU_NEUTRAL;
	m_primary->enableCorrection=true;
	// The other subsystems can stay with defaults
}

void CSubScanBinder::init()
{
		m_primary->targetName=CORBA::string_dup("");;
		m_primary->type=Antenna::ANT_NONE;
		m_primary->paramNumber=0;
		m_primary->latitudeOffset=m_primary->longitudeOffset=0.0;
		m_primary->section=Antenna::ACU_NEUTRAL;
		m_primary->secondary=false;
		m_primary->enableCorrection=true;
		m_primary->applyOffsets=false;
		m_secondary->targetName=CORBA::string_dup("");;
		m_secondary->type=Antenna::ANT_NONE;
		m_secondary->paramNumber=0;
		m_secondary->latitudeOffset=m_secondary->longitudeOffset=0.0;
		m_secondary->section=Antenna::ACU_NEUTRAL;
		m_secondary->secondary=false;
		m_secondary->enableCorrection=true;
		m_secondary->applyOffsets=false;
		m_servo->range=0;
		m_servo->total_time=0;
		m_servo->axis_code=CORBA::string_dup("");;
		m_servo->is_empty_scan=false;
		m_receievers->dummy=0;
}
/*
	ACS::Time startUt=0;
	//ACS::TimeInterval slewingTime;
	//Antenna::TAzimuthSection section;
	Antenna::TTrackingParameters prim,second;
	//Management::TScanAxis axis;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE; second.enableCorrection=true;
	prim.type=Antenna::ANT_OTF;
	prim.otf.lon1=0.0; // they will be set by internal engine
	prim.otf.lat1=0.0;
	if (scanFrame!=Antenna::ANT_HORIZONTAL) {
		prim.otf.coordFrame=scanFrame;
		prim.otf.subScanFrame=scanFrame;
	}
	else {
		prim.otf.coordFrame=Antenna::ANT_EQUATORIAL;
		prim.otf.subScanFrame=Antenna::ANT_HORIZONTAL;
	}
	prim.otf.description=Antenna::SUBSCAN_CENTER; // a must !!!!!
	prim.otf.subScanDuration=duration;
	prim.otf.direction=Antenna::SUBSCAN_INCREASE;
	prim.otf.geometry=Antenna::SUBSCAN_CONSTLON; // a must !!!!!
	prim.otf.lon2=0.0;
	prim.otf.lat2=span;
	prim.applyOffsets=false;  // no offset for this scan!
	prim.secondary=true;  // force the engine to look for the secondary track, or if the secondary is not set (this case) to use the previously commanded track
	prim.targetName=CORBA::string_dup("otf"); // not strictly necessary as the system will take the last commanded name
	prim.section=Antenna::ACU_NEUTRAL;
	prim.enableCorrection=true;
	startScan(startUt,prim,second);
	return startUt;
 */



/*
 ACS::Time CBossCore::lonOTFScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	//ACS::TimeInterval slewingTime;
	//Antenna::TAzimuthSection section;
	//Management::TScanAxis axis;
	Antenna::TTrackingParameters prim,second;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE; second.enableCorrection=true;
	prim.type=Antenna::ANT_OTF;
	prim.otf.lon1=0.0; // they will be set by internal engine
	prim.otf.lat1=0.0;
	if (scanFrame!=Antenna::ANT_HORIZONTAL) {
		prim.otf.coordFrame=scanFrame;
		prim.otf.subScanFrame=scanFrame;
	}
	else {
		prim.otf.coordFrame=Antenna::ANT_EQUATORIAL;
		prim.otf.subScanFrame=Antenna::ANT_HORIZONTAL;
	}
	prim.otf.description=Antenna::SUBSCAN_CENTER; // a must !!!!!
	prim.otf.subScanDuration=duration;
	prim.otf.direction=Antenna::SUBSCAN_INCREASE; // this is
	prim.otf.geometry=Antenna::SUBSCAN_CONSTLAT; // a must !!!!!
	prim.otf.lon2=span;
	prim.otf.lat2=0.0;
	prim.applyOffsets=false;  // no offset for this scan!
	prim.secondary=true;  // force the engine to look for the secondary track, or if the secondary is not set (this case) to use the previously commanded track
	prim.targetName=CORBA::string_dup("otf"); // not strictly necessary as the system will take the last commanded name
	prim.section=Antenna::ACU_NEUTRAL;
	prim.enableCorrection=true;

	startScan(startUt,prim,second);
	return startUt;
}
 */
