#ifndef _BOSS_CORE_STARTSCANWRAPPERS_I_
#define _BOSS_CORE_STARTSCANWRAPPERS_I_


void CBossCore::track(const char *targetName) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{	
	ACS::Time startUt=0;
	//Management::TScanAxis axis;
	Antenna::TTrackingParameters par,second;
	//ACS::TimeInterval slewingTime;
	//Antenna::TAzimuthSection section;
	second.type=Antenna::ANT_NONE;
	second.secondary=false;
	second.applyOffsets=false;
	second.paramNumber=0;
	second.enableCorrection=true;
	par.type=Antenna::ANT_SIDEREAL;
	par.secondary=false;
	par.paramNumber=0;
	par.applyOffsets=false;
	par.targetName=CORBA::string_dup(targetName);
	par.section=Antenna::ACU_NEUTRAL;
	par.enableCorrection=true;
	/*if (!checkScan(startUt,par,second,slewingTime,section,axis,m_config->getMinElevation(),m_config->getMaxElevation())) {   // check if the target is set or not...in case we just signal a possible problem
		ACS_LOG(LM_FULL_INFO,"CBossCore::track()",(LM_WARNING,"SCAN_WILL_NOT_BE_PROPERLY_EXECUTED"));
	} //...but the scan is commanded anyway
	startUt=0; // override the startUt computed by the previous call*/
	startScan(startUt,par,second); // could throw exceptions
}
	
void CBossCore::moon() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,
		AntennaErrors::SecondaryScanErrorExImpl,AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	Antenna::TTrackingParameters par,second;
	//ACS::TimeInterval slewingTime;
	//Antenna::TAzimuthSection section;
	//Management::TScanAxis axis;
	second.type=Antenna::ANT_NONE;
	second.secondary=false;
	second.applyOffsets=false;
	second.paramNumber=0;
	second.enableCorrection=true;
	par.type=Antenna::ANT_MOON;
	par.secondary=false;
	par.targetName=CORBA::string_dup("Moon");
	par.applyOffsets=false;
	par.section=Antenna::ACU_NEUTRAL;
	par.enableCorrection=true;
	/*if (!checkScan(startUt,par,second,slewingTime,section,axis,m_config->getMinElevation(),m_config->getMaxElevation())) {   // check if the target is set or not...in case we just signal a possible problem
		ACS_LOG(LM_FULL_INFO,"CBossCore::moon()",(LM_WARNING,"SCAN_WILL_NOT_BE_PROPERLY_EXECUTED"));
	} //...but the scan is commanded anyway
	startUt=0; // override the startUt computed by the previous call*/
	startScan(startUt,par,second); // could throw exceptions
}

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

void CBossCore::sidereal(const char * targetName,const double& ra,const double& dec,const Antenna::TSystemEquinox& eq,const Antenna::TSections& section) throw (
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
	Antenna::TTrackingParameters prim,second;
	//ACS::TimeInterval slewingTime;
	//Antenna::TAzimuthSection azSection;
	//Management::TScanAxis axis;
	second.secondary=false; second.paramNumber=0; second.applyOffsets=false; second.type=Antenna::ANT_NONE; second.enableCorrection=true;
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
	prim.enableCorrection=true;
	/*if (!checkScan(startUt,prim,second,slewingTime,azSection,axis,m_config->getMinElevation(),m_config->getMaxElevation())) {   // check if the target is set or not...in case we just signal a possible problem
		ACS_LOG(LM_FULL_INFO,"CBossCore::sidereal()",(LM_WARNING,"SCAN_WILL_NOT_BE_PROPERLY_EXECUTED"));
	} //...but the scan is commanded anyway
	startUt=0; // override the startUt computed by the previous call*/
	startScan(startUt,prim,second);
}

void CBossCore::goOff(const Antenna::TCoordinateFrame& frame,const double& skyOffset) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	ACS::Time startUt=0;
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
	/*if (!checkScan(startUt,prim,second,slewingTime,section,axis,m_config->getMinElevation(),m_config->getMaxElevation())) {   // check if the target is set or not...in case we just signal a possible problem
		ACS_LOG(LM_FULL_INFO,"CBossCore::lonOTFScan()",(LM_WARNING,"SCAN_WILL_NOT_BE_PROPERLY_EXECUTED"));
	} //...but the scan is commanded anyway
	startUt=0; // override the startUt computed by the previous call*/
	startScan(startUt,prim,second);
	return startUt;
}

ACS::Time CBossCore::latOTFScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
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
	/*if (!checkScan(startUt,prim,second,slewingTime,section,axis,m_config->getMinElevation(),m_config->getMaxElevation())) {   // check if the target is set or not...in case we just signal a possible problem
		ACS_LOG(LM_FULL_INFO,"CBossCore::latOTFScan()",(LM_WARNING,"SCAN_WILL_NOT_BE_PROPERLY_EXECUTED"));
	} //...but the scan is commanded anyway
	startUt=0; // override the startUt computed by the previous call*/
	startScan(startUt,prim,second);
	return startUt;
}
		

#endif
