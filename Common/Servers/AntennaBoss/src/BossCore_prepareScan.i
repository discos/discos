#ifndef _BOSS_CORE_PREPARESCAN_I_
#define _BOSS_CORE_PREPARESCAN_I_

void CBossCore::copyTrack(Antenna::TTrackingParameters& dest,const Antenna::TTrackingParameters& source,bool copyOffs) const
{
	dest.targetName=CORBA::string_dup(source.targetName);
	dest.type=source.type;
	dest.paramNumber=source.paramNumber;
	for (long k=0;k<Antenna::ANTENNA_TRACKING_PARAMETER_NUMBER;k++) dest.parameters[k]=source.parameters[k];
	dest.frame=source.frame;
	dest.equinox=source.equinox;
	if (copyOffs) {
		dest.longitudeOffset=source.longitudeOffset;
		dest.latitudeOffset=source.latitudeOffset;
		dest.offsetFrame=source.offsetFrame;
		dest.applyOffsets=source.applyOffsets;
	}
	dest.section=source.section;
	dest.secondary=source.secondary;
	dest.enableCorrection=source.enableCorrection;
	dest.otf.lon1=source.otf.lon1;
	dest.otf.lat1=source.otf.lat1;
	dest.otf.lon2=source.otf.lon2;
	dest.otf.lat2=source.otf.lat2;
	dest.otf.coordFrame=source.otf.coordFrame;
	dest.otf.geometry=source.otf.geometry;
	dest.otf.subScanFrame=source.otf.subScanFrame;
	dest.otf.description=source.otf.description;
	dest.otf.direction=source.otf.direction;
	dest.otf.subScanDuration=source.otf.subScanDuration;
	dest.VradFrame=source.VradFrame;
	dest.VradDefinition=source.VradDefinition;
	dest.RadialVelocity=source.RadialVelocity;
}

void CBossCore::mappingScan(Antenna::TTrackingParameters& scan) const
{
	// this is a scan mapping to support goTo (beamPark) with optional parameters
	// in this case the parameters are azimuth and elevation
	if ((scan.type==Antenna::ANT_SIDEREAL) && (scan.frame==Antenna::ANT_HORIZONTAL)) {
		if (scan.parameters[0]<0.0) scan.parameters[0]=m_lastEncoderAzimuth;
		if (scan.parameters[1]<0.0) scan.parameters[1]=m_lastEncoderElevation;
	}
}

Antenna::EphemGenerator_ptr CBossCore::prepareScan(
		bool useInternals,
		ACS::Time& startUT,
		const Antenna::TTrackingParameters& _prim,
		const Antenna::TTrackingParameters& _sec,
		const TOffset& userOffset,
		Antenna::TGeneratorType& generatorType,
		Antenna::TTrackingParameters& lastPar,
		Antenna::TSections& section,
		double& ra,
		double& dec,
		double& lon,
		double& lat,
		double& vrad,
		Antenna::TReferenceFrame& velFrame,
		Antenna::TVradDefinition& velDef,
		ACS::Time& timeToStop,
		IRA::CString& sourceName,
		TOffset& scanOffset,
		Management::TScanAxis& axis,
		Antenna::EphemGenerator_out generatorFlux)
		throw (ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,
			   AntennaErrors::ScanErrorExImpl,AntennaErrors::SecondaryScanErrorExImpl,AntennaErrors::MissingTargetExImpl,AntennaErrors::LoadGeneratorErrorExImpl)
{
	double latOffTmp,lonOffTmp;
	TOffset scanOffTmp(0.0,0.0,Antenna::ANT_HORIZONTAL);
	Antenna::TCoordinateFrame offFrameTmp;
	Antenna::TTrackingParameters primary,secondary,prim,sec;
	double secRa,secDec,secLon,secLat,secVrad;
	Antenna::TReferenceFrame secVelFrame;
	Antenna::TVradDefinition secVelDef;

	IRA::CString secSourceName;
	bool secondaryActive=false;
	bool lastActive=false;
	
	Antenna::EphemGenerator_var currentGenerator;
	Antenna::EphemGenerator_var currentGeneratorFlux;

	// this is a workaround in order to preserve the input arguments before mappingScan
	copyTrack(prim,_prim);
	copyTrack(sec,_sec);
	mappingScan(prim);

	// let's save the primary and secondary tracks information.
	copyTrack(primary,prim);
	copyTrack(secondary,sec); 
	secondary.applyOffsets=false; //the offsets of the secondary track are always ignored as well as the secondary bit
	secondary.secondary=false;
	secondary.enableCorrection=true;
	
	// some preliminary checks and initializations
	if (primary.type==Antenna::ANT_NONE) {
		if (!primary.secondary) { // if primary.type is NONE than the secondary flag has to be given......
			_EXCPT(AntennaErrors::ScanErrorExImpl,impl,"CBossCore::prepareScan()");
			impl.setReason("The scan configuration is not supported");
			throw impl;	
		}
		if (secondary.type==Antenna::ANT_NONE) {
			if (lastPar.type==Antenna::ANT_NONE) {
				_EXCPT(AntennaErrors::MissingTargetExImpl,impl,"CBossCore::prepareScan()");
				throw impl;
			}
			else {
				copyTrack(primary,lastPar,false);
			}
		}
		else {
			copyTrack(primary,secondary,false);
		}	
	}
	else if (primary.secondary) { // // support for secondary scan.......primary.type must be different from NONE
		if (primary.type==Antenna::ANT_OTF) {  // if it is OTF some special check are due
			if ((secondary.type!=Antenna::ANT_NONE)||(lastPar.type!=Antenna::ANT_NONE)) {  //check if the secondary scan is given or the lastScan has been commanded
				//supported combination are : A) description: Center, frame: EQ or GAL B) description: StartStop, frame: HOR
				if (primary.otf.geometry==Antenna::SUBSCAN_GREATCIRCLE) {  // great circle is never supported
					_EXCPT(AntennaErrors::ScanErrorExImpl,impl,"CBossCore::prepareScan()");
					impl.setReason("The geometry of the OTF cannot be GREATCIRCLE in this configuration");
					throw impl;
				}
				if (primary.otf.description==Antenna::SUBSCAN_CENTER) { // if the subscan is center
					if ((primary.otf.coordFrame!=Antenna::ANT_EQUATORIAL)&&(primary.otf.coordFrame!=Antenna::ANT_GALACTIC)) { //... the coordinate frame must be EQ or GAL
						_EXCPT(AntennaErrors::ScanErrorExImpl,impl,"CBossCore::prepareScan()");
						impl.setReason("This OTF configuration support only galactic or equatorial frame");
						throw impl;
					}
				}
				else if (primary.otf.description==Antenna::SUBSCAN_STARTSTOP) { // if the subscan is Start Stop
					if ((primary.otf.coordFrame!=Antenna::ANT_HORIZONTAL)) { //... the coordinate frame must be EQ or GAL
						_EXCPT(AntennaErrors::ScanErrorExImpl,impl,"CBossCore::prepareScan()");
						impl.setReason("This OTF configuration support only horizontal frame");
						throw impl;
					}
				}
				/*if (primary.otf.description!=Antenna::SUBSCAN_CENTER) {
					_EXCPT(AntennaErrors::ScanErrorExImpl,impl,"CBossCore::prepareScan()");
					impl.setReason("The description of the OTF can be only CENTER in this configuration");
					throw impl;
				}
				if ((primary.otf.coordFrame!=Antenna::ANT_EQUATORIAL)&&(primary.otf.coordFrame==Antenna::ANT_GALACTIC)) {
					_EXCPT(AntennaErrors::ScanErrorExImpl,impl,"CBossCore::prepareScan()");
					impl.setReason("This OTF configuration support only galactic or equatorial frame");
					throw impl;									
				}*/
			}
			else { // if primary.secondary==true and neither secondary.type!=NONE lastPar.type!=NONE
				_EXCPT(AntennaErrors::MissingTargetExImpl,impl,"CBossCore::prepareScan()");
				throw impl;
			}			
			if (secondary.type!=Antenna::ANT_NONE) { // the secondary track takes the precedence
				bool result;
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_ON_SECONDARY_TRACK"));
				currentGeneratorFlux=prepareOTFSecondary(useInternals,secondary,secSourceName,secRa,secDec,secLon,secLat,secVrad,secVelFrame,secVelDef,result);
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"GENERATORE %lu",(unsigned long)currentGeneratorFlux.in()));
				if (!result) {
					_EXCPT(AntennaErrors::SecondaryScanErrorExImpl,ex,"CBossCore::prepareScan()");
					throw ex;
				}
				if (primary.otf.coordFrame==Antenna::ANT_EQUATORIAL) {     // this is description==CENTER
					ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"UTILIZZO_COORDINATE_EQUATORIALI"));
					primary.otf.lon1=secRa;
					primary.otf.lat1=secDec;
				}
				else if (primary.otf.coordFrame==Antenna::ANT_GALACTIC) { // this is description==CENTER
					ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"UTILIZZO_COORDINATE_GALATTICHE"));
					primary.otf.lon1=secLon;
					primary.otf.lat1=secLat;
				}
				else if (primary.otf.coordFrame==Antenna::ANT_HORIZONTAL) { // this is description==STARTSTOP
					ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"UTILIZZO_COORDINATE_ORRIZZONTALI"));
					TIMEVALUE timeNow;
					double presentAz,presentEl;
					IRA::CIRATools::getTime(timeNow);
					m_slewCheck.coordConvert(secLon,secLat,Antenna::ANT_GALACTIC,Antenna::ANT_J2000,timeNow,presentAz,presentEl);
					if (primary.otf.geometry==Antenna::SUBSCAN_CONSTLON) {
						primary.otf.lon1=presentAz;
						primary.otf.lon2=presentAz;
					}
					else if (primary.otf.geometry==Antenna::SUBSCAN_CONSTLAT) {
						primary.otf.lat1=presentEl;
						primary.otf.lat2=presentEl;
					}
					// GREATCIRCLE is signaled as error in the control code above.....
				}
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_SECONDARY_PREPARED_ON_LONLAT %lf %lf",primary.otf.lon1,primary.otf.lat1));
				secondaryActive=true;
			}
			else if (lastPar.type!=Antenna::ANT_NONE) { // otherwise the scan commanded the previous time is used as secondary
				bool result;
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_ON_LAST_COMMANDED_TRACK"));
				currentGeneratorFlux=prepareOTFSecondary(useInternals,lastPar,secSourceName,secRa,secDec,secLon,secLat,secVrad,secVelFrame,secVelDef,result);
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"GENERATORE %lu",(unsigned long)currentGeneratorFlux.in()));
				if (!result) {
					_EXCPT(AntennaErrors::SecondaryScanErrorExImpl,ex,"CBossCore::prepareScan()");
					throw ex;
				}
				if (primary.otf.coordFrame==Antenna::ANT_EQUATORIAL) { // this is description==CENTER
					ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"UTILIZZO_COORDINATE_EQUATORIALI"));
					primary.otf.lon1=secRa;
					primary.otf.lat1=secDec;
				}
				else if (primary.otf.coordFrame==Antenna::ANT_GALACTIC) { // this is description==CENTER
					ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"UTILIZZO_COORDINATE_GALATTICHE"));
					primary.otf.lon1=secLon;
					primary.otf.lat1=secLat;
				}
				else if (primary.otf.coordFrame==Antenna::ANT_HORIZONTAL) { // this is description==STARTSTOP
					ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"UTILIZZO_COORDINATE_ORRIZZONTALI"));
					TIMEVALUE timeNow;
					double presentAz,presentEl;
					IRA::CIRATools::getTime(timeNow);
					m_slewCheck.coordConvert(secLon,secLat,Antenna::ANT_GALACTIC,Antenna::ANT_J2000,timeNow,presentAz,presentEl);
					if (primary.otf.geometry==Antenna::SUBSCAN_CONSTLON) {
						primary.otf.lon1=presentAz;
						primary.otf.lon2=presentAz;
					}
					else if (primary.otf.geometry==Antenna::SUBSCAN_CONSTLAT) {
						primary.otf.lat1=presentEl;
						primary.otf.lat2=presentEl;
					}
					// GREATCIRCLE is signaled as error in the control code above.....
				}
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_LAST_TRACK_PREPARED_ON_LONLAT %lf %lf",primary.otf.lon1,primary.otf.lat1));
				lastActive=true;
			}
		}
		else { // if it is not OTF
			_EXCPT(AntennaErrors::ScanErrorExImpl,impl,"CBossCore::prepareScan()");
			impl.setReason("The scan configuration is not supported");
			throw impl;	
		}
	}
	generatorType=primary.type;
	try {
		if (!useInternals) {
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"LOADING_REQUIRED_PRIMARY_GENERATOR"));
			currentGenerator=loadPrimaryGenerator(generatorType);
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"PRIMARY_GENERATOR_IS: %lx",(unsigned long)currentGenerator.in()));
		}
		else {
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"LOADING_REQUIRED_INTERNAL_GENERATOR"));
			currentGenerator=loadInternalGenerator(generatorType);
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"INTERNAL_GENERATOR_IS: %lx",(unsigned long)currentGenerator.in()));
		}
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		_ADD_BACKTRACE(AntennaErrors::LoadGeneratorErrorExImpl,impl,ex,"CBossCore::prepareScan()");
		throw impl;
	}
	if (primary.applyOffsets) { //offsets are newer overloaded by the secondary track offsets...because the secondary offsets are always ignored!
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"APPLYING_OFFSETS: %lf %lf",primary.longitudeOffset,primary.latitudeOffset));
		scanOffTmp=TOffset(primary.longitudeOffset,primary.latitudeOffset,primary.offsetFrame);
		addOffsets(lonOffTmp,latOffTmp,offFrameTmp,userOffset,scanOffTmp);
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"TOTAL_OFFSETS: %lf %lf",lonOffTmp,latOffTmp));
	}
	else { //keep the originals
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"KEEP_ORIGINAL_OFFSETS"));
		latOffTmp=userOffset.lat;
		lonOffTmp=userOffset.lon;
		offFrameTmp=userOffset.frame;
		// the addOffsets is the frame of user and scan offset are different uses the scan offset, in that case the scan offset are null so I want to make sure that
		// the composition of user and scan offset is equal to user offsets in that case.
		scanOffTmp.frame=userOffset.frame; 
	}
	try {
		currentGenerator->setOffsets(lonOffTmp,latOffTmp,offFrameTmp); //could throw an AntennaErrorsEx exception
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CBossCore::prepareScan()");
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		throw impl;
	}
	catch(AntennaErrors::AntennaErrorsEx& ex) {
		_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::prepareScan()");
		impl.setOperationName("setOffsets()");
		throw impl;
	}
	// by default we choose the section reported in tracking point structure
	section=primary.section;
	if (primary.type==Antenna::ANT_SIDEREAL) {
		Antenna::SkySource_var tracker;
		tracker=Antenna::SkySource::_narrow(currentGenerator);
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SIDEREAL_TRACKING"));
		try {
			if (primary.paramNumber==0) {
				tracker->loadSourceFromCatalog(primary.targetName);
			}
			else if (primary.frame==Antenna::ANT_EQUATORIAL) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SOURCE_RA_DEC: %lf %lf",primary.parameters[0],primary.parameters[1]));
				if (primary.paramNumber==2) {
					tracker->setSourceFromEquatorial(primary.targetName,primary.parameters[0],primary.parameters[1],primary.equinox,0.0,0.0,0.0,0.0);
				}
				else {
					tracker->setSourceFromEquatorial(primary.targetName,primary.parameters[0],primary.parameters[1],primary.equinox,primary.parameters[2],primary.parameters[3],primary.parameters[4],
							primary.parameters[5]);					
				}
			}
			else if (primary.frame==Antenna::ANT_GALACTIC) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SOURCE_LON_LAT: %lf %lf",primary.parameters[0],primary.parameters[1]));
				tracker->setSourceFromGalactic(primary.targetName,primary.parameters[0],primary.parameters[1]);
			}
			else if (primary.frame==Antenna::ANT_HORIZONTAL) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SOURCE_AZ_EL: %lf %lf",primary.parameters[0],primary.parameters[1]));
				printf("parametri beam park: %lf, %lf",primary.parameters[0],primary.parameters[1]);
				tracker->setFixedPoint(primary.targetName,primary.parameters[0],primary.parameters[1]);
			}
			//copy the current track and store it
			copyTrack(lastPar,primary);
			lastPar.applyOffsets=false;
			lastPar.secondary=false;
		}
		catch(AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(AntennaErrors::ScanErrorExImpl,impl,ex,"CBossCore::prepareScan()");
			impl.setReason("Unable to load the scan configuration into the generator");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(AntennaErrors::ScanErrorExImpl,impl,ex,"CBossCore::prepareScan()");
			impl.setReason("Unable to load the scan configuration into the generator");
			throw impl;
		}
		catch (...) {
			_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::prepareScan()");
		}
		try {
			Antenna::SkySourceAttributes_var att;
			tracker->getAttributes(att);
			ra=att->J2000RightAscension;
			dec=att->J2000Declination;
			lon=att->gLongitude;
			lat=att->gLatitude;
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SKYSOURCE RA_DEC LAT_LON %lf %lf %lf %lf",ra,dec,lat,lon));
			vrad=att->inputRadialVelocity;
			velFrame=att->inputVradFrame;
			velDef=att->inputVradDefinition;
			sourceName=IRA::CString(att->sourceID);
			axis=att->axis;
			currentGeneratorFlux=currentGenerator; // the flux computer is the sky source generator itself...make a deep copy
		}
		catch (CORBA::SystemException& ex) {
			sourceName=IRA::CString("????");
			ra=dec=0.0; // in that case I do not want to rise an error
			lon=lat=0.0;
			axis=Management::MNG_NO_AXIS;
			vrad=0.0;
			velFrame=Antenna::ANT_UNDEF_FRAME;
			velDef=Antenna::ANT_UNDEF_DEF;
		}
		//if a radial velocity has been provided the it will override the one coming from the generator.....
		if ((primary.VradFrame!=Antenna::ANT_UNDEF_FRAME) && (primary.VradDefinition!=Antenna::ANT_UNDEF_DEF)) {
			vrad=primary.RadialVelocity;
			velFrame=primary.VradFrame;
			velDef=primary.VradDefinition;
		}
		timeToStop=0; // no need to compute a stop duration
	}
	else if (primary.type==Antenna::ANT_MOON) {
		// moon has nothing to do...no configuration
		Antenna::Moon_var tracker;
		tracker=Antenna::Moon::_narrow(currentGenerator);
		//copy the current track and store it
		copyTrack(lastPar,primary);
		lastPar.applyOffsets=false;
		lastPar.secondary=false;
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"MOON_TRACKING"));
		try {
			Antenna::MoonAttributes_var att;
			tracker->getAttributes(att);
			ra=att->J2000RightAscension;
			dec=att->J2000Declination;
			lon=att->gLongitude;
			lat=att->gLatitude;
			//vrad=0.0;
			//velFrame=Antenna::ANT_UNDEF_FRAME;
			//velDef=Antenna::ANT_UNDEF_DEF;
			axis=att->axis;
			sourceName=IRA::CString(att->sourceID);
			currentGeneratorFlux=currentGenerator; // the flux computer is the moon generator itself...make a deep copy
		}
		catch (CORBA::SystemException& ex) {
			sourceName=IRA::CString("????");
			ra=dec=0.0; // in that case I do not want to rise an error
			//vrad=0.0;
			//velFrame=Antenna::ANT_UNDEF_FRAME;
			//velDef=Antenna::ANT_UNDEF_DEF;
			axis=Management::MNG_NO_AXIS;
		}
		vrad=primary.RadialVelocity;
		velFrame=primary.VradFrame;
		velDef=primary.VradDefinition;
		timeToStop=0;
	}
	else if (primary.type==Antenna::ANT_OTF) {
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_SCANNING"));
		Antenna::OTF_var tracker;
		tracker=Antenna::OTF::_narrow(currentGenerator);
		try {
			//for otf the section reported in par structure is overcome by the one computed by the OTF generator....
			// if startUT==0 (start as soon as possible) the component will set back the estimated startUT.
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_LON_LAT: %lf %lf %lf %lf",primary.otf.lon1,primary.otf.lat1,primary.otf.lon2,primary.otf.lat2));
			double roundAz=slaDranrm(m_lastEncoderAzimuth);
			section=tracker->setSubScan(primary.targetName,roundAz,m_lastAzimuthSection,m_lastEncoderElevation,m_lastEncoderRead,primary.otf.lon1,primary.otf.lat1,primary.otf.lon2,primary.otf.lat2,primary.otf.coordFrame,
					primary.otf.geometry,primary.otf.subScanFrame,primary.otf.description,primary.otf.direction,startUT,primary.otf.subScanDuration);
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SECTION_IS: %d",section));
			if (secondaryActive) {
				copyTrack(lastPar,secondary);
			}
			else if (lastActive) {
				// if lastActive no need to do anything because the last scan parameters remains the same
			}
			else {
				copyTrack(lastPar,primary);
				lastPar.applyOffsets=false;
				lastPar.secondary=false;
			}
		}
		catch(AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(AntennaErrors::ScanErrorExImpl,impl,ex,"CBossCore::prepareScan()");
			impl.setReason("Unable to load the scan configuration into the generator");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(AntennaErrors::ScanErrorExImpl,impl,ex,"CBossCore::prepareScan()");
			impl.setReason("Unable to load the scan configuration into the generator");
			throw impl;
		}
		catch (...) {
			_THROW_EXCPT(ComponentErrors::UnexpectedExImpl,"CBossCore::prepareScan()");
		}
		try {
			Antenna::OTFAttributes_var att;
			tracker->getAttributes(att);
			ra=att->centerRA;
			dec=att->centerDec;
			lon=att->centerGLon;
			lat=att->centerGLat;
			axis=att->axis;

			startUT=att->startUT; //very important....we want to save the start time computed by OTF
			timeToStop=startUT+att->subScanDuration+att->rampDuration; // estimate the stop scan epoch.
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_CENTER_RA_DEC LAT_LON %lf %lf %lf %lf",ra,dec,lat,lon));
			if (secondaryActive||lastActive) { // in case of a secondary track....it is possible that vlsr and flux are computable
				vrad=secVrad;
				velFrame=secVelFrame;
				velDef=secVelDef;
				sourceName=secSourceName;
			}
			else {
				vrad=primary.RadialVelocity;
				velFrame=primary.VradFrame;
				velDef=primary.VradDefinition;
				sourceName=IRA::CString(att->sourceID);
				currentGeneratorFlux=currentGenerator; // if no secondary scan is used, the generator in charge to compute the flux is the OTF itself...make a deep copy
			}
		}
		catch (CORBA::SystemException& ex) {
			sourceName=IRA::CString("????");
			ra=dec=0.0; // in that case I do not want to rise an error
			lon=lat=0.0;
			vrad=0.0;
			velFrame=Antenna::ANT_UNDEF_FRAME;
			velDef=Antenna::ANT_UNDEF_DEF;
			axis=Management::MNG_NO_AXIS;
			//startUT=0; //keep the input value
			timeToStop=0;
		}
	}
	else if (primary.type==Antenna::ANT_SATELLITE) {
	}
	else if (primary.type==Antenna::ANT_SOLARSYTEMBODY) {
	}
	else if (primary.type==Antenna::ANT_SUN) { 
	}
	// if everything looks ok....return back the offsets.....
	/*
	lonOff=lonOffTmp;
	offFrame=offFrameTmp;*/
	scanOffset=scanOffTmp;
	generatorFlux=currentGeneratorFlux._retn();
	return currentGenerator._retn();
}

Antenna::EphemGenerator_ptr CBossCore::prepareOTFSecondary(const bool& useInternal,const Antenna::TTrackingParameters& sec,IRA::CString& sourceName,double& ra,double& dec,double& lon,
		double& lat,double& vrad,Antenna::TReferenceFrame& velFrame,Antenna::TVradDefinition& velDef,bool& result)
{
	ACS::Time inputTime;
	TIMEVALUE now;
	TOffset scanOff;
	IRA::CString name;
	Antenna::TSections section;
	Antenna::TTrackingParameters nullScan,lastScan;
	ACS::Time timeToStop;
	
	Antenna::EphemGenerator_var tmp=Antenna::EphemGenerator::_nil();
	Antenna::EphemGenerator_var tmpFlux=Antenna::EphemGenerator::_nil();
	Antenna::TGeneratorType genType;
	
	nullScan.type=Antenna::ANT_NONE;
	nullScan.secondary=false;
	nullScan.applyOffsets=false;
	
	lastScan.type=Antenna::ANT_NONE;
	lastScan.secondary=false;
	lastScan.applyOffsets=false;
	
	Management::TScanAxis axis;

	// get current Time
	IRA::CIRATools::getTime(now);
	inputTime=now.value().value;

	result=true;
	/***************************************************************************/
	// in order to fine tune this should be a two steps algorithm: in the first step compute the position...then compute the estimated arrival time (of the telescope)
	// then recompute the position using that time. This is to optimize in case of object with fast proper motion, like satellites or planets
	/******************************************************************************/

	try {
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareOTFSecondary()",(LM_DEBUG,"PREPARE_SECONDARY_FOR OTF"));
		tmp=prepareScan(useInternal,inputTime,sec,nullScan,m_userOffset,genType,lastScan,section,ra,dec,lon,lat,vrad,velFrame,velDef,timeToStop,
				sourceName,scanOff,axis,tmpFlux.out());
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_DEBUG);
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareOTFSecondary()",(LM_DEBUG,"ERROR_ON_PREPARATION_OF_SECONDARY_OTF"));
		result=false;
	}
	return tmp._retn();
}

#endif

