#ifndef _BOSS_CORE_STARTSCANWRAPPERS_I_
#define _BOSS_CORE_STARTSCANWRAPPERS_I_

void CBossCore::track(const char *targetName) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{	
	ACS::Time startUt=0;
	Antenna::TTrackingParameters par,second;
	second.type=Antenna::ANT_NONE;
	second.secondary=false;
	second.applyOffsets=false;
	second.paramNumber=0;
	par.type=Antenna::ANT_SIDEREAL;
	par.secondary=false;
	par.paramNumber=0;
	par.applyOffsets=false;
	par.targetName=CORBA::string_dup(targetName);
	par.section=Antenna::ACU_NEUTRAL;
	startScan(startUt,par,second); // could throw exceptions
}
	
void CBossCore::moon() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,
		AntennaErrors::SecondaryScanErrorExImpl,AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	Antenna::TTrackingParameters par,second;
	second.type=Antenna::ANT_NONE;
	second.secondary=false;
	second.applyOffsets=false;
	second.paramNumber=0;
	par.type=Antenna::ANT_MOON;
	par.secondary=false;
	par.targetName=CORBA::string_dup("Moon");
	par.applyOffsets=false;
	par.section=Antenna::ACU_NEUTRAL;
	startScan(startUt,par,second); // could throw exceptions
}

void CBossCore::sidereal(const char * targetName,const double& ra,const double& dec,const Antenna::TSystemEquinox& eq,const Antenna::TSections& section) throw (
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	Antenna::TTrackingParameters prim,second;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE;
	prim.type=Antenna::ANT_SIDEREAL;
	prim.parameters[0]=ra;
	prim.parameters[1]=dec;
	prim.paramNumber=2;
	prim.frame=Antenna::ANT_EQUATORIAL;	
	prim.equinox=eq;
	prim.section=section;
	prim.targetName=CORBA::string_dup(targetName);
	prim.secondary=false;
	prim.applyOffsets=false;
	startScan(startUt,prim,second);
}

void CBossCore::goOff(const Antenna::TCoordinateFrame& frame,const double& skyOffset) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	Antenna::TTrackingParameters prim,second;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE;
	prim.type=Antenna::ANT_NONE;
	prim.paramNumber=0;
	prim.secondary=true;
	prim.applyOffsets=true;
	if (frame==Antenna::ANT_HORIZONTAL) {
		if (m_lastEncoderElevation>m_config->getCutOffElevation()) {
			prim.longitudeOffset=0.0;
			prim.latitudeOffset=skyOffset;
		}
		else {
			prim.longitudeOffset=skyOffset;
			prim.latitudeOffset=0.0;			
		}
	}
	else {
		prim.longitudeOffset=skyOffset;
		prim.latitudeOffset=0.0;					
	}
	prim.offsetFrame=frame;
	startScan(startUt,prim,second);
}

ACS::Time CBossCore::lonOTFScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl,AntennaErrors::TargetNotReachableExImpl)
{
	ACS::Time startUt=0;
	ACS::TimeInterval slewingTime;
	Antenna::TTrackingParameters prim,second;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE;
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
	if (checkScan(startUt,prim,second,slewingTime)) {
		startScan(startUt,prim,second);
		return startUt;
	}
	else {
		_EXCPT(AntennaErrors::TargetNotReachableExImpl,impl,"CBossCore::lonOTFScan");
		throw impl;
	}
}

ACS::Time CBossCore::latOTFScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl,AntennaErrors::TargetNotReachableExImpl)
{
	ACS::Time startUt=0;
	ACS::TimeInterval slewingTime;
	Antenna::TTrackingParameters prim,second;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE;
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
	if (checkScan(startUt,prim,second,slewingTime)) {
		startScan(startUt,prim,second);
		return startUt;
	}
	else {
		_EXCPT(AntennaErrors::TargetNotReachableExImpl,impl,"CBossCore::lonOTFScan");
		throw impl;
	}		
}
		

#endif
