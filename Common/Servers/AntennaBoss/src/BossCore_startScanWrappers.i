#ifndef _BOSS_CORE_STARTSCANWRAPPERS_I_
#define _BOSS_CORE_STARTSCANWRAPPERS_I_

void CBossCore::goOff(const Antenna::TCoordinateFrame& frame,const double& beams) throw (ComponentErrors::CouldntReleaseComponentExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl,AntennaErrors::NoTargetCommandedYetExImpl)
{
	// this is a workaround in order to make sure there is a target already commanded
	if (m_rawCoordinates.isEmpty()) {
		_EXCPT(AntennaErrors::NoTargetCommandedYetExImpl,impl,"CBossCore::goOff");
		throw impl;
	}
	double skyOffset=m_FWHM*beams;
	double lonOff=0.0,latOff=0.0;
    ACS::Time startUt=0;
    Antenna::TTrackingParameters prim,second;
    second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE; second.enableCorrection=true;
    prim.type=Antenna::ANT_NONE;
    prim.paramNumber=0;
    prim.secondary=true;
    prim.applyOffsets=true;
	if (frame==Antenna::ANT_HORIZONTAL) {
		if (m_lastEncoderElevation>m_config->getCutOffElevation()) {
			latOff=skyOffset;
			lonOff=0.0;
		}
		else {
			latOff=0.0;
			lonOff=skyOffset;
		}
	}
	else if (frame==Antenna::ANT_EQUATORIAL) {
		latOff=0.0;
		lonOff=skyOffset;
	}
	else if (frame==Antenna::ANT_GALACTIC) {
		latOff=0.0;
		lonOff=skyOffset;
	}
	prim.longitudeOffset=lonOff;
	prim.latitudeOffset=latOff;
    prim.offsetFrame=frame;
    prim.enableCorrection=true;
    startScan(startUt,prim,second);
}

#endif
