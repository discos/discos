#ifndef _BOSS_CORE_STARTSCAN_I_
#define _BOSS_CORE_STARTSCAN_I_

// $Id: BossCore_startScan.i,v 1.5 2011-04-15 09:09:12 a.orlati Exp $

void CBossCore::startScan(const char* targetName,ACS::Time& startUt,const Antenna::TTrackingParameters& parameters) throw(
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
				ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl)
{
	Antenna::TSections section;
	double ra,dec,vlsr;
	IRA::CString name;
	TIMEVALUE now;
	//temporarly stop the working thread
	m_workingThread->suspend();
	if (m_generatorType!=parameters.type) { // the new generator has changed......so the previous generator must be realesed
		try {
			freeGenerator(m_generator.inout()); // could throw CannotReleaseComponent
		}
		catch (ComponentErrors::CouldntReleaseComponentExImpl& ex) {
			changeBossStatus(Management::MNG_FAILURE);
			throw ex;
		}
		m_generatorType=Antenna::ANT_NONE;
		try {
			m_generator=loadGenerator(parameters.type);
		}
		catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
			changeBossStatus(Management::MNG_FAILURE);
			throw ex;			
		}
		catch (ComponentErrors::CORBAProblemExImpl& ex) {
			changeBossStatus(Management::MNG_FAILURE);
			throw ex;
		}
		catch (ComponentErrors::UnexpectedExImpl& ex) {
			changeBossStatus(Management::MNG_FAILURE);
			throw ex;
		}
		m_generatorType=parameters.type;
		// clear the offsets when the genrator is changed
		m_longitudeOffset=m_latitudeOffset=0.0;
		m_offsetFrame=Antenna::ANT_HORIZONTAL;
	}
	try {
		prepareScan(m_generator.in(),startUt,targetName,parameters,section,ra,dec,vlsr,name,m_longitudeOffset,m_latitudeOffset,m_offsetFrame);
	}
	catch (ComponentErrors::CouldntCallOperationExImpl& ex) { //catched just to update the component status and to unload the generator!
		try {
			freeGenerator(m_generator.inout()); // could throw CannotReleaseComponent
		}
		catch (...) { // 
		}
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	catch (ComponentErrors::UnexpectedExImpl& ex) {
		try {
			freeGenerator(m_generator.inout()); // could throw CannotReleaseComponent
		}
		catch (...) { //
		}
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		try {
			freeGenerator(m_generator.inout()); // could throw CannotReleaseComponent
		}
		catch (...) { //
		}
		m_generatorType=Antenna::ANT_NONE;				
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	loadMount(m_mount,m_mountError); // thorw ComponentErrors::CouldntGetComponentExImpl
	try {
		if (section!=Antenna::ACU_NEUTRAL) {
			if (m_enable) {   // command to the mount only if the subsystem is enabled.
				m_mount->forceSection(section);
			}
		}
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::startScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		changeBossStatus(Management::MNG_WARNING);
		m_mountError=true;
		throw impl;
	}
	try {
		ACS_LOG(LM_FULL_INFO,"CBossCore::startScan()",(LM_DEBUG,"LOADING_TRACKING_CURVE"));
		quickTracking();
	}
	catch (ComponentErrors::OperationErrorExImpl& ex) {
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	if (startUt!=0) {
		IRA::CString out;
		IRA::CIRATools::timeToStr(startUt,out);
		ACS_LOG(LM_FULL_INFO,"CBossCore::startScan()",(LM_NOTICE,"NEW_SCAN_WILL_START_AT: %s",(const char *)out));
	}
	else {
		ACS_LOG(LM_FULL_INFO,"CBossCore::startScan()",(LM_NOTICE,"NEW_SCAN_IMMEDIATELY_STARTED"));
	}
	m_targetName=name;
	m_targetRA=ra;
	m_targetDec=dec;
	m_targetVlsr=vlsr;
	IRA::CIRATools::getTime(now);
	m_newScanEpoch=now.value().value;	
	// finally let's resume the working thread
	m_workingThread->resume();
}

void CBossCore::quickTracking() throw (ComponentErrors::OperationErrorExImpl)
{
    // now upload the first set of coordinates
    TIMEVALUE now;
    TIMEDIFFERENCE slice;
    slice.microSecond(m_config->getGapTime());
    IRA::CIRATools::getTime(now);
    now.normalize(true);
	for (int i=0;i<m_config->getMinPointNumber();i++) {
	    if (i!=0) now+=slice.value();
		try {
			loadTrackingPoint(now,(i==0));
		}
		catch (ACSErr::ACSbaseExImpl& E) {
			_ADD_BACKTRACE(ComponentErrors::OperationErrorExImpl,impl,E,"CBossCore::quickTracking()");
			impl.setReason("Cannot load a tracking point");
			throw impl;
		}
	}
}

#endif /*STARTTRACKING_I_*/
