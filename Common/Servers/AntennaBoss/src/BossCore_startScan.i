#ifndef _BOSS_CORE_STARTSCAN_I_
#define _BOSS_CORE_STARTSCAN_I_

void CBossCore::startScan(ACS::Time& startUt,const Antenna::TTrackingParameters& parameters,const Antenna::TTrackingParameters& secondary) throw(
		ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl,AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,
		AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	Antenna::TSections section;
	double ra,dec,vrad,lon,lat;
	Antenna::TReferenceFrame velFrame;
	Antenna::TVradDefinition velDef;
	IRA::CString name;
	TIMEVALUE now;
	Management::TScanAxis axis;
	//Temporarily stop the working thread
	m_workingThread->suspend();
	//make sure that scan offset are reset
	m_scanOffset=TOffset(0.0,0.0,m_userOffset.frame);
	addOffsets(m_longitudeOffset,m_latitudeOffset,m_offsetFrame,m_userOffset,m_scanOffset);
	try {
		m_generatorType=Antenna::ANT_NONE;
		m_generator=Antenna::EphemGenerator::_nil(); // it also releases the previous reference.
		m_generatorFlux=Antenna::EphemGenerator::_nil(); // it also releases the previous reference.
		m_generator=prepareScan(false,startUt,parameters,secondary,m_userOffset,m_generatorType,m_lastScanParameters,section,ra,dec,lon,lat,vrad,velFrame,velDef,name,
				m_scanOffset,axis,m_generatorFlux.out());
		//computes the resulting offset, coming from the user and the scan
		addOffsets(m_longitudeOffset,m_latitudeOffset,m_offsetFrame,m_userOffset,m_scanOffset);
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"TOTAL_OFFSETS: %lf %lf",m_longitudeOffset,m_latitudeOffset));
	}
	catch (ComponentErrors::CouldntCallOperationExImpl& ex) { //catch just to update the component status and to unload the generator!
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	catch (ComponentErrors::UnexpectedExImpl& ex) {
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	catch (ComponentErrors::CORBAProblemExImpl& ex) {
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	catch (AntennaErrors::ScanErrorExImpl& ex) {
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	catch (AntennaErrors::SecondaryScanErrorExImpl& ex) {
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	catch (AntennaErrors::MissingTargetExImpl& ex) {
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}
	catch (AntennaErrors::LoadGeneratorErrorExImpl& ex) {
		m_generatorType=Antenna::ANT_NONE;		
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;		
	}
	catch (...) {
		m_generatorType=Antenna::ANT_NONE;				
		changeBossStatus(Management::MNG_FAILURE);
		_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::startScan()");
	}
	loadMount(m_mount,m_mountError); // Throw ComponentErrors::CouldntGetComponentExImpl
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
	m_correctionEnable_scan=parameters.enableCorrection;
	if (!m_correctionEnable_scan) {
		ACS_LOG(LM_FULL_INFO,"CBossCore::startScan()",(LM_NOTICE,"POINTING_CORRECTIONS_DISABLED_FOR_CURRENT_SCAN"));
	}
	/*try {
		ACS_LOG(LM_FULL_INFO,"CBossCore::startScan()",(LM_DEBUG,"LOADING_TRACKING_CURVE"));
		quickTracking();
	}
	catch (ComponentErrors::OperationErrorExImpl& ex) {
		changeBossStatus(Management::MNG_FAILURE);
		throw ex;
	}*/
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
	m_targetVrad=vrad;
	m_vradReferenceFrame=velFrame;
	m_vradDefinition=velDef;
	m_currentAxis=axis;
	computeFlux();
	IRA::CIRATools::getTime(now);
	m_newScanEpoch=now.value().value;
	m_newTracking=true; // let's begin with a new program track table......
	//Reset observed coordinates arrays
	m_observedHorizontals.empty();
	m_observedEquatorials.empty();
	m_observedGalactics.empty();
	m_integratedObservedEquatorial.empty();
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
