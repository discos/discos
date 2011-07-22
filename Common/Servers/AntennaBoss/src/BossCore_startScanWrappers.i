#ifndef _BOSS_CORE_STARTSCANWRAPPERS_I_
#define _BOSS_CORE_STARTSCANWRAPPERS_I_

// $Id: BossCore_startScanWrappers.i,v 1.4 2010-10-14 12:23:36 a.orlati Exp $


void CBossCore::track(const char *targetName) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl)
{	
	ACS::Time startUt=0;
	Antenna::TTrackingParameters par;
	par.type=Antenna::ANT_SIDEREAL;
	par.paramNumber=0;
	par.applyOffsets=false;
	par.section=Antenna::ACU_NEUTRAL;
	startScan(targetName,startUt,par); // could throw exceptions
}
	
void CBossCore::moon() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl)
{
	ACS::Time startUt=0;
	Antenna::TTrackingParameters par;
	par.type=Antenna::ANT_MOON;
	IRA::CString dummy("dummy");
	par.applyOffsets=false;
	par.section=Antenna::ACU_NEUTRAL;
	startScan((const char *)dummy,startUt,par); // could throw exceptions
}
			
void CBossCore::elevationScan(const double& cRa,const double& cDec,const double& span,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntReleaseComponentExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
		ComponentErrors::OperationErrorExImpl)
{	
	ACS::Time startUt=0;
	IRA::CString dummy("dummy");
	Antenna::TTrackingParameters par;
	par.type=Antenna::ANT_OTF;
	par.otf.lon1=cRa;
	par.otf.lat1=cDec;
	par.otf.coordFrame=Antenna::ANT_EQUATORIAL;
	par.otf.subScanFrame=Antenna::ANT_HORIZONTAL;
	par.otf.description=Antenna::SUBSCAN_CENTER;
	par.otf.subScanDuration=duration;
	par.otf.direction=Antenna::SUBSCAN_INCREASE;
	par.otf.geometry=Antenna::SUBSCAN_CONSTLON;
	par.otf.lon2=0.0;
	par.otf.lat2=span;
	par.applyOffsets=false;
	par.section=Antenna::ACU_NEUTRAL;
	startScan((const char *)dummy,startUt,par); // could throw exceptions	
}

void CBossCore::azimuthScan(const double& cRa,const double& cDec,const double& span,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntReleaseComponentExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
		ComponentErrors::OperationErrorExImpl)
{
	ACS::Time startUt=0;
	IRA::CString dummy("dummy");
	Antenna::TTrackingParameters par;
	par.type=Antenna::ANT_OTF;
	par.otf.lon1=cRa;
	par.otf.lat1=cDec;
	par.otf.coordFrame=Antenna::ANT_EQUATORIAL;
	par.otf.subScanFrame=Antenna::ANT_HORIZONTAL;
	par.otf.description=Antenna::SUBSCAN_CENTER;
	par.otf.subScanDuration=duration;
	par.otf.direction=Antenna::SUBSCAN_INCREASE;
	par.otf.geometry=Antenna::SUBSCAN_CONSTLAT;
	par.otf.lon2=span;
	par.otf.lat2=0.0;
	par.applyOffsets=false;
	par.section=Antenna::ACU_NEUTRAL;	
	startScan((const char *)dummy,startUt,par); // could throw exceptions		
}

void CBossCore::source(const char *sourceName,const double& ra,const double& dec,const Antenna::TSystemEquinox& epoch,const Antenna::TSections& section) throw (ComponentErrors::CouldntReleaseComponentExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
		ComponentErrors::OperationErrorExImpl)
{
	ACS::Time startUt=0;
	IRA::CString name(sourceName);
	Antenna::TTrackingParameters par;
	par.type=Antenna::ANT_SIDEREAL;
	par.parameters[0]=ra;
	par.parameters[1]=dec;
	par.paramNumber=2;
	par.frame=Antenna::ANT_EQUATORIAL;	
	par.equinox=epoch;
	par.applyOffsets=false;
	par.section=section;
	startScan((const char *)name,startUt,par);
}

#endif
