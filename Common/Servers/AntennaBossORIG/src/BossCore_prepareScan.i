#ifndef _BOSS_CORE_PREPARESCAN_I_
#define _BOSS_CORE_PREPARESCAN_I_

// $Id: BossCore_prepareScan.i,v 1.7 2011-04-22 18:58:08 a.orlati Exp $


void CBossCore::prepareScan(Antenna::EphemGenerator_ptr generator,ACS::Time& startUT,const char *targetName,const Antenna::TTrackingParameters& par,Antenna::TSections& section,
		double& ra,double& dec,double& vlsr,IRA::CString& sourceName,double& lonOff,double& latOff,Antenna::TCoordinateFrame& offFrame) throw 
		(ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl)
{
	double latOffTmp,lonOffTmp;
	Antenna::TCoordinateFrame offFrameTmp;
	if (par.applyOffsets) {
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"APPLYING_OFFSETS: %lf %lf",par.longitudeOffset,par.latitudeOffset));
		latOffTmp=par.latitudeOffset;
		lonOffTmp=par.longitudeOffset;
		offFrameTmp=par.offsetFrame;
	}
	else { //keep the orginals
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"KEEP_ORIGINAL_OFFSETS"));		
		latOffTmp=latOff;
		lonOffTmp=lonOff;
		offFrameTmp=offFrame;
	}
	try {
		generator->setOffsets(lonOffTmp,latOffTmp,offFrameTmp); //could throw an AntennaErrorsEx exception
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
	section=par.section;
	if (par.type==Antenna::ANT_SIDEREAL) {
		Antenna::SkySource_var tracker;
		tracker=Antenna::SkySource::_narrow(generator);
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SIDEREAL_TRACKING"));
		try {
			if (par.paramNumber==0) {
				tracker->loadSourceFromCatalog(targetName);
			}
			else if (par.frame==Antenna::ANT_EQUATORIAL) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SOURCE_RA_DEC: %lf %lf",par.parameters[0],par.parameters[1]));
				if (par.paramNumber==2) {
					tracker->setSourceFromEquatorial(targetName,par.parameters[0],par.parameters[1],par.equinox,0.0,0.0,0.0,0.0);
				}
				else {
					tracker->setSourceFromEquatorial(targetName,par.parameters[0],par.parameters[1],par.equinox,par.parameters[2],par.parameters[3],par.parameters[4],
						par.parameters[5]);					
				}
			}
			else if (par.frame==Antenna::ANT_GALACTIC) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SOURCE_LON_LAT: %lf %lf",par.parameters[0],par.parameters[1]));
				tracker->setSourceFromGalactic(targetName,par.parameters[0],par.parameters[1]);
			}
			else if (par.frame==Antenna::ANT_HORIZONTAL) {
				ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SOURCE_AZ_EL: %lf %lf",par.parameters[0],par.parameters[1]));
				tracker->setFixedPoint(par.parameters[0],par.parameters[1]);
			}
		}
		catch(AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::prepareScan()");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::prepareScan()");
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
			vlsr=att->inputRadialVelocity;
			sourceName=IRA::CString(att->sourceID);
		}
		catch (CORBA::SystemException& ex) {
			sourceName=IRA::CString("????");
			ra=dec=0.0; // in that case I do not want to rise an error
		}		
	}
	else if (par.type==Antenna::ANT_MOON) {
		// moon has nothing to do...no configuration
		Antenna::Moon_var tracker;
		tracker=Antenna::Moon::_narrow(generator);
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"MOON_TRACKING"));
		try {
			Antenna::MoonAttributes_var att;
			tracker->getAttributes(att);
			ra=att->J2000RightAscension;
			dec=att->J2000Declination;
			vlsr=0.0; 
			sourceName=IRA::CString(att->sourceID);
		}
		catch (CORBA::SystemException& ex) {
			sourceName=IRA::CString("????");
			ra=dec=0.0; // in that case I do not want to rise an error
		}		
	}
	else if (par.type==Antenna::ANT_OTF) {
		Antenna::OTF_var tracker;
		tracker=Antenna::OTF::_narrow(generator);
		ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_SCANNING"));
		try {
			//for otf the section reported in par structure is ovecome by the one computed by the OTF generator....
			// if startUT==0 (start as soon as possible) the component will set back the estimated startUT.
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"OTF_LON_LAT: %lf %lf %lf %lf",par.otf.lon1,par.otf.lat1,par.otf.lon2,par.otf.lat2));
			double roundAz=slaDranrm(m_lastEncoderAzimuth);
			section=tracker->setSubScan(roundAz,m_lastAzimuthSection,m_lastEncoderElevation,m_lastEncoderRead,
					par.otf.lon1,par.otf.lat1,par.otf.lon2,par.otf.lat2,par.otf.coordFrame,par.otf.geometry,par.otf.subScanFrame,par.otf.description,
					par.otf.direction,startUT,par.otf.subScanDuration);
			ACS_LOG(LM_FULL_INFO,"CBossCore::prepareScan()",(LM_DEBUG,"SECTION_IS: %ld",section));
		}
		catch(AntennaErrors::AntennaErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::prepareScan()");
			throw impl;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntCallOperationExImpl,impl,ex,"CBossCore::prepareScan()");
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
			startUT=att->startUT; //very important....we want to save the start time computed by OTF
			vlsr=0.0;
			sourceName=IRA::CString(att->sourceID);
		}
		catch (CORBA::SystemException& ex) {
			sourceName=IRA::CString("????");
			ra=dec=0.0; // in that case I do not want to rise an error
		}
	}
	else if (par.type==Antenna::ANT_SATELLITE) {
	}
	else if (par.type==Antenna::ANT_SOLARSYTEMBODY) {
	}
	else if (par.type==Antenna::ANT_SUN) { 
	}
	// if everything looks ok....return back the offsets.....
	latOff=latOffTmp;
	lonOff=lonOffTmp;
	offFrame=offFrameTmp;
}

#endif
