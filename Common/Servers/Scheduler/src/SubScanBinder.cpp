#include "Schedule.h"

using namespace Schedule;

CSubScanBinder::CSubScanBinder(Antenna::TTrackingParameters * const primary,Antenna::TTrackingParameters * const secondary,
 MinorServo::MinorServoScan * const servo,Receivers::TReceiversParameters * const receivers,CConfiguration* config):
 m_primary(primary),m_secondary(secondary),m_servo(servo),m_receivers(receivers),m_own(false),m_config(config)
{
	init();
}

CSubScanBinder::CSubScanBinder(CConfiguration* config,bool dispose): m_primary(NULL), m_secondary(NULL), m_servo(NULL),
		m_receivers(NULL),m_own(dispose), m_config(config)
{
	m_primary=new Antenna::TTrackingParameters;
	m_secondary=new Antenna::TTrackingParameters;
	m_servo=new MinorServo::MinorServoScan;
	m_receivers=new Receivers::TReceiversParameters;
	init();
}

CSubScanBinder::~CSubScanBinder()
{
	if (m_own) {
		if (m_primary) delete m_primary;
		if (m_secondary) delete m_secondary;
		if (m_servo) delete m_servo;
		if (m_receivers) delete m_receivers;
	}
}

void CSubScanBinder::addOffsets(const double& lonOff,const double& latOff,const Antenna::TCoordinateFrame& frame)
{
	m_primary->latitudeOffset=latOff;
	m_primary->longitudeOffset=lonOff;
	m_primary->applyOffsets=true;	m_primary->offsetFrame=frame;
}

void CSubScanBinder::peaker(const IRA::CString& axis,const double& span,const ACS::TimeInterval& duration,const Antenna::TTrackingParameters * const sec)
{
	double antennaSpan,bdf;
	Management::TScanAxis axisCode;
	Antenna::TsubScanGeometry geom;
	Antenna::TsubScanDirection direction;
	if (!m_config) {
		bdf=0.0; // do not associate a movement in the telescope.....this corresponds to a simple tracking of the source
	}
	else {
		axisCode=m_config->getAxisFromServoName(axis);
		bdf=m_config->getBDFfromAxis(axisCode);
		geom=m_config->getScanGeometryFromAxis(axisCode);
	}
	antennaSpan=span*bdf;
	if (antennaSpan>=0) {
		direction=Antenna::SUBSCAN_INCREASE;
	}
	else {
		direction=Antenna::SUBSCAN_INCREASE;
		antennaSpan*=-1.0;
	}
	if (antennaSpan>0) {
		OTFC(Antenna::ANT_EQUATORIAL,geom,Antenna::ANT_HORIZONTAL,direction,antennaSpan,duration,sec);
	}
	else {
		copyPrimaryAntenaTrack(sec);
	}
    m_servo->is_empty_scan=false;
    m_servo->axis_code=CORBA::string_dup((const char *)axis);
    m_servo->range=span;
    m_servo->total_time=duration;
	m_receivers->dummy=0;
}

void CSubScanBinder::OTFC(const Antenna::TCoordinateFrame& coordFrame,const Antenna::TsubScanGeometry& geometry,
	const Antenna::TCoordinateFrame& subScanFrame,const Antenna::TsubScanDirection& direction,
	const double& span,const ACS::TimeInterval& subScanDuration,const Antenna::TTrackingParameters * const sec)
{
	m_primary->secondary=true;
	m_primary->enableCorrection=true;
	m_primary->type=Antenna::ANT_OTF;
	m_primary->section=Antenna::ACU_NEUTRAL;
	m_primary->paramNumber=0;

	m_primary->otf.lon1=m_primary->otf.lat1=0.0; // this will be replaced by the antennaboss component
	m_primary->otf.description=Antenna::SUBSCAN_CENTER;
	m_primary->otf.subScanDuration=subScanDuration;
	m_primary->otf.coordFrame=coordFrame;
	m_primary->otf.direction=direction;
	m_primary->otf.geometry=geometry;
	m_primary->otf.subScanFrame=subScanFrame;
	if (m_primary->otf.geometry==Antenna::SUBSCAN_CONSTLON) {
		m_primary->otf.lat2=span;
		m_primary->otf.lon2=0.0;
	}
	else {
		m_primary->otf.lat2=0.0;
		m_primary->otf.lon2=span;
	}
	addSecondaryAntennaTrack(sec);
    m_servo->is_empty_scan=true;
    m_servo->axis_code=CORBA::string_dup("");
    m_servo->range=0;
    m_servo->total_time=0;
}

void CSubScanBinder::skydip(const double& lat1,const double& lat2,const ACS::TimeInterval& duration,
		const Antenna::TTrackingParameters * const sec)
{
	//the real target name, as well as the m_primary->secondary=true clause, will be set by the call to addSecondaryAntennaTrack();
	if (lat1>lat2) {
		OTF("skydip",0.0,lat1,0.0,lat2,Antenna::ANT_HORIZONTAL,Antenna::SUBSCAN_CONSTLON,
				Antenna::ANT_HORIZONTAL,Antenna::SUBSCAN_STARTSTOP,Antenna::SUBSCAN_DECREASE,duration);
	}
	else {
		OTF("skydip",0.0,lat2,0.0,lat1,Antenna::ANT_HORIZONTAL,Antenna::SUBSCAN_CONSTLON,
				Antenna::ANT_HORIZONTAL,Antenna::SUBSCAN_STARTSTOP,Antenna::SUBSCAN_INCREASE,duration);
	}
	addSecondaryAntennaTrack(sec);
	m_primary->enableCorrection=false;
}

void CSubScanBinder::OTF(const IRA::CString& target,
						 const double& lon1,const double& lat1,const double& lon2,const double& lat2,
						 const Antenna::TCoordinateFrame& coordFrame,const Antenna::TsubScanGeometry& geometry,
						 const Antenna::TCoordinateFrame& subScanFrame,const Antenna::TsubScanDescription& description,
						 const Antenna::TsubScanDirection& direction,const ACS::TimeInterval& subScanDuration)
{
	m_primary->secondary=false;
	m_primary->enableCorrection=true;
	m_primary->type=Antenna::ANT_OTF;
	m_primary->section=Antenna::ACU_NEUTRAL; // no support for section selection in schedule right now
	m_primary->paramNumber=0;
	m_primary->targetName=CORBA::string_dup((const char *)target);
	m_primary->otf.subScanDuration=subScanDuration;
	m_primary->otf.description=description;
	m_primary->otf.lat1=lat1;
	m_primary->otf.lat2=lat2;
	m_primary->otf.lon1=lon1;
	m_primary->otf.lon2=lon2;
	m_primary->otf.coordFrame=coordFrame;
	m_primary->otf.subScanFrame=subScanFrame;
	m_primary->otf.geometry=geometry;
	m_primary->otf.direction=direction;
	m_primary->latitudeOffset=m_primary->longitudeOffset=0.0;
	m_primary->applyOffsets=false;
	m_secondary->secondary=false; m_secondary->paramNumber=0; m_secondary->applyOffsets=false;
	m_secondary->type=Antenna::ANT_NONE; m_secondary->enableCorrection=true;
    m_servo->is_empty_scan=true;
    m_servo->axis_code=CORBA::string_dup("");
    m_servo->range=0;
    m_servo->total_time=0;
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
    m_servo->is_empty_scan=true;
    m_servo->axis_code=CORBA::string_dup("");
    m_servo->range=0;
    m_servo->total_time=0;
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
    m_servo->is_empty_scan=true;
    m_servo->axis_code=CORBA::string_dup("");
    m_servo->range=0;
    m_servo->total_time=0;
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
    m_servo->is_empty_scan=true;
    m_servo->axis_code=CORBA::string_dup("");
    m_servo->range=0;
    m_servo->total_time=0;
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
    m_servo->is_empty_scan=true;
    m_servo->axis_code=CORBA::string_dup("");
    m_servo->range=0;
    m_servo->total_time=0;
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
    m_servo->is_empty_scan=true;
    m_servo->axis_code=CORBA::string_dup("");
    m_servo->range=0;
    m_servo->total_time=0;
	// The other subsystems can stay with defaults
}

void CSubScanBinder::goTo(const double& az,const double& el)
{
	m_secondary->secondary=false; m_secondary->paramNumber=0; m_secondary->applyOffsets=false;
	m_secondary->type=Antenna::ANT_NONE; m_secondary->enableCorrection=true;
	m_primary->type=Antenna::ANT_SIDEREAL;
	m_primary->parameters[0]=az;
	m_primary->parameters[1]=el;
	m_primary->paramNumber=2;
	m_primary->frame=Antenna::ANT_HORIZONTAL;
	m_primary->equinox=Antenna::ANT_APPARENT;
	m_primary->section=Antenna::ACU_NEUTRAL;
	m_primary->targetName=CORBA::string_dup("BeamPark");
	m_primary->secondary=false;
	m_primary->applyOffsets=false;
	m_primary->enableCorrection=false;
    m_servo->is_empty_scan=true;
    m_servo->axis_code=CORBA::string_dup("");
    m_servo->range=0;
    m_servo->total_time=0;
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
		//m_primary->frame;
		//m_primary->equinox;
		//m_primary->offsetFrame;
		m_primary->VradFrame=Antenna::ANT_UNDEF_FRAME;
		m_primary->VradDefinition=Antenna::ANT_UNDEF_DEF;
		m_primary->RadialVelocity=0.0;
		m_secondary->targetName=CORBA::string_dup("");;
		m_secondary->type=Antenna::ANT_NONE;
		m_secondary->paramNumber=0;
		m_secondary->latitudeOffset=m_secondary->longitudeOffset=0.0;
		m_secondary->section=Antenna::ACU_NEUTRAL;
		m_secondary->secondary=false;
		m_secondary->enableCorrection=true;
		m_secondary->applyOffsets=false;
		//m_secondary->frame;
		//m_secondary->equinox;
		//m_secondary->offsetFrame;
		m_secondary->VradFrame=Antenna::ANT_UNDEF_FRAME;
		m_secondary->VradDefinition=Antenna::ANT_UNDEF_DEF;
		m_secondary->RadialVelocity=0.0;
		m_servo->range=0;
		m_servo->total_time=0;
		m_servo->axis_code=CORBA::string_dup("");;
		m_servo->is_empty_scan=true;
		m_receivers->dummy=0;
}

// **** PRVATE *********

void CSubScanBinder::copyPrimaryAntenaTrack(const Antenna::TTrackingParameters * const prim)
{
	if (prim!=NULL) {
		m_primary->targetName=CORBA::string_dup(prim->targetName);
		m_primary->type=prim->type;
		for (long k=0;k<Antenna::ANTENNA_TRACKING_PARAMETER_NUMBER;k++) m_primary->parameters[k]=prim->parameters[k];
		m_primary->paramNumber=prim->paramNumber;
		m_primary->frame=prim->frame;
		m_primary->equinox=prim->equinox;
		m_primary->longitudeOffset=prim->longitudeOffset;
		m_primary->latitudeOffset=prim->latitudeOffset;
		m_primary->offsetFrame=prim->offsetFrame;
		m_primary->applyOffsets=prim->applyOffsets;
		m_primary->secondary=false;
		m_primary->section=prim->section;
		m_primary->otf.lon1=prim->otf.lon1;
		m_primary->otf.lat1=prim->otf.lat1;
		m_primary->otf.lon2=prim->otf.lon2;
		m_primary->otf.lat2=prim->otf.lat2;
		m_primary->otf.coordFrame=prim->otf.coordFrame;
		m_primary->otf.geometry=prim->otf.geometry;
		m_primary->otf.subScanFrame=prim->otf.subScanFrame;
		m_primary->otf.description=prim->otf.description;
		m_primary->otf.direction=prim->otf.direction;
		m_primary->otf.subScanDuration=prim->otf.subScanDuration;
		m_primary->VradFrame=prim->VradFrame;
		m_primary->VradDefinition=prim->VradDefinition;
		m_primary->RadialVelocity=prim->RadialVelocity;
	}
}

void CSubScanBinder::addSecondaryAntennaTrack(const Antenna::TTrackingParameters * const sec)
{
	m_primary->secondary=true;
	if (sec!=NULL) {
		m_primary->targetName=CORBA::string_dup(sec->targetName);
		m_secondary->targetName=CORBA::string_dup(sec->targetName);
		m_secondary->type=sec->type;
		for (long k=0;k<Antenna::ANTENNA_TRACKING_PARAMETER_NUMBER;k++) m_secondary->parameters[k]=sec->parameters[k];
		m_secondary->paramNumber=sec->paramNumber;
		m_secondary->frame=sec->frame;
		m_secondary->equinox=sec->equinox;
		// offset will be ignore for secondary tracking set
		m_secondary->longitudeOffset=0.0;
		m_secondary->latitudeOffset=0.0;
		m_secondary->offsetFrame=sec->offsetFrame;
		m_secondary->applyOffsets=false;
		// **********************************************
		m_secondary->section=sec->section;
		m_secondary->secondary=false;
		m_secondary->otf.lon1=sec->otf.lon1;
		m_secondary->otf.lat1=sec->otf.lat1;
		m_secondary->otf.lon2=sec->otf.lon2;
		m_secondary->otf.lat2=sec->otf.lat2;
		m_secondary->otf.coordFrame=sec->otf.coordFrame;
		m_secondary->otf.geometry=sec->otf.geometry;
		m_secondary->otf.subScanFrame=sec->otf.subScanFrame;
		m_secondary->otf.description=sec->otf.description;
		m_secondary->otf.direction=sec->otf.direction;
		m_secondary->otf.subScanDuration=sec->otf.subScanDuration;
		m_secondary->VradFrame=sec->VradFrame;
		m_secondary->VradDefinition=sec->VradDefinition;
		m_secondary->RadialVelocity=sec->RadialVelocity;
	}
	else {
		m_secondary->secondary=false; m_secondary->paramNumber=0; m_secondary->applyOffsets=false;
		m_secondary->type=Antenna::ANT_NONE; m_secondary->enableCorrection=true;
	}
}

