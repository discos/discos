#ifndef _BOSS_CORE_STARTSCANWRAPPERS_I_
#define _BOSS_CORE_STARTSCANWRAPPERS_I_


/*ACS::Time  CBossCore::skydip(const double& el1, const double& el2,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
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
}*/

#endif
