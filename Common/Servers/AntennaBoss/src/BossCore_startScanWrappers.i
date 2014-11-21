#ifndef _BOSS_CORE_STARTSCANWRAPPERS_I_
#define _BOSS_CORE_STARTSCANWRAPPERS_I_


ACS::Time  CBossCore::skydip(const double& el1, const double& el2,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	Antenna::TTrackingParameters par,second;
	double rel1,rel2;
	double defEl1,defEl2;
	second.type=Antenna::ANT_NONE;
	second.secondary=false;
	second.applyOffsets=false;
	second.paramNumber=0;
	second.enableCorrection=true;
	m_config->getSkydipRange(defEl1,defEl2);
	if (el1<0) {
		rel1=defEl1;
	}
	else {
		rel1=el1;
	}
	if (el2<0) {
		rel2=defEl2;
	}
	else {
		rel2=el2;
	}
	par.targetName=CORBA::string_dup("skydip");
	par.type=Antenna::ANT_OTF;
	par.paramNumber=0;
	par.otf.lon1=m_lastEncoderAzimuth;
	par.otf.lat1=GETMAX(rel1,rel2);
	par.otf.lon2=m_lastEncoderAzimuth;
	par.otf.lat2=GETMIN(rel1,rel2);
	par.otf.coordFrame=Antenna::ANT_HORIZONTAL;
	par.otf.geometry=Antenna::SUBSCAN_CONSTLON;
	par.otf.subScanFrame=Antenna::ANT_HORIZONTAL;
	par.otf.description=Antenna::SUBSCAN_STARTSTOP;
	par.otf.direction=Antenna::SUBSCAN_DECREASE;
	par.otf.subScanDuration=duration;
	par.secondary=false;
	par.applyOffsets=false;
	par.section=Antenna::ACU_NEUTRAL;
	par.enableCorrection=false; // disable correction just for this can
	//in this case we skip the scan check because we want to pass over the scan suggested limits.....
	startScan(startUt,par,second);
	return startUt;
}

void CBossCore::goOff(const Antenna::TCoordinateFrame& frame,const double& beams) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	double skyOffset=m_FWHM*beams;
	Antenna::TTrackingParameters prim,second;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE; second.enableCorrection=true;
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
	prim.enableCorrection=true;
	startScan(startUt,prim,second);
}

void CBossCore::goTo(const double& az,const double& el) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	Antenna::TTrackingParameters prim,second;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE; second.enableCorrection=true;
	prim.type=Antenna::ANT_SIDEREAL;
	if (az<0.0) {
		prim.parameters[0]=m_lastEncoderAzimuth;
	}
	else {
		prim.parameters[0]=az;
	}
	if (el<0.0) {
		prim.parameters[1]=m_lastEncoderElevation;
	}
	else {
		prim.parameters[1]=el;
	}
	prim.paramNumber=2;
	prim.frame=Antenna::ANT_HORIZONTAL;
	prim.equinox=Antenna::ANT_APPARENT;
	prim.section=Antenna::ACU_NEUTRAL;
	prim.targetName=CORBA::string_dup("fixed");
	prim.secondary=false;
	prim.applyOffsets=false;
	prim.enableCorrection=false;
	startScan(startUt,prim,second);
}

#endif
