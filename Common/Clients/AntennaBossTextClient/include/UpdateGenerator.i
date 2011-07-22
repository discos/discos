#ifndef _UPDATEGENERATOR_I_
#define _UPDATEGENERATOR_I_

// $Id: UpdateGenerator.i,v 1.8 2010-10-14 12:24:34 a.orlati Exp $

void updateGenerator(maci::SimpleClient& client) {
	//Antenna::EphemGenerator_var gen;
	char *gen;
	IRA::CString generatorName;
	Antenna::TGeneratorType type;
	IRA::CString outString,tmpString;
	baci::BACIValue bValue;
	try {
		//gen=component->getGeneratorPointer(type);
		gen=component->getGeneratorCURL(type);
		generatorName=IRA::CString(gen);
		CORBA::string_free(gen);
	}
	catch (CORBA::SystemException& ex) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"::Main()"); 
		impl.setName(ex._name());
		impl.setMinor(ex.minor());
		_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
		return;
	}
	catch (...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
		_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
		return;
	}
	if (generatorName!=lastGenerator) {
		try {
			if (lastGenerator!="") {
				client.releaseComponent(lastGenerator);
			}
		}
		catch (maciErrType::CannotReleaseComponentExImpl& E) {
			_IRA_LOGGUARD_LOG_EXCEPTION(guard,E,LM_ERROR);
			return;
		}
	}
	//if (!CORBA::is_nil(gen)) {
	if (generatorName!="") {
		switch (type) {
			case Antenna::ANT_SIDEREAL : {
				Antenna::SkySourceAttributes_var att;
				Antenna::SkySource_var sidereal;
				try {
					sidereal=client.getComponent<Antenna::SkySource>(generatorName,0,true);
					lastGenerator=generatorName;
					//sidereal=Antenna::SkySource::_narrow(gen);
					if (!CORBA::is_nil(sidereal)) {
						sidereal->getAttributes(att);
					}
				}
				catch (maciErrType::CannotGetComponentExImpl& E) {
					_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,MODULE_NAME"::Main()");
					impl.setComponentName((const char *)generatorName);
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}	
				catch (CORBA::SystemException& ex) {
					_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"::Main()"); 
					impl.setName(ex._name());
					impl.setMinor(ex.minor());
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}
				catch (...) {
					_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}
				tmpString=(const char*)att->sourceID;
				outString="Source name        : "+tmpString;
				extraLabel1->setValue(outString);
				extraLabel1->Refresh();
				bValue.setValue((CORBA::Double)att->inputRightAscension);
				outString="Input equatorial   : "+CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue))+" ";
				bValue.setValue((CORBA::Double)att->inputDeclination);
				outString+=CFormatFunctions::angleFormat(bValue,NULL)+" ";
				tmpString.Format("%.1lf %.2lf %.2lf %.2lf %.2lf",att->inputJEpoch,att->inputRaProperMotion,att->inputDecProperMotion,
						att->inputParallax,att->inputRadialVelocity);
				outString+=tmpString;
				extraLabel2-> setValue(outString);
				extraLabel2->Refresh();
				tmpString.Format("%lf %lf %.4lf %.4lf",att->inputGalacticLongitude*DR2D,att->inputGalacticLatitude*DR2D,
						att->userLongitudeOffset*DR2D,att->userLatitudeOffset*DR2D);
				outString="Input galactic     : "+tmpString;
				extraLabel3->setValue(outString);
				extraLabel3->Refresh();			
				bValue.setValue((CORBA::Double)att->rightAscension);
				outString="Apparent equatorial: "+CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue))+" ";
				bValue.setValue((CORBA::Double)att->declination);
				outString+=CFormatFunctions::angleFormat(bValue,NULL)+" ";
				tmpString.Format("%.5lf %.4lf %.4lf",att->julianEpoch,att->userRightAscensionOffset*DR2D,att->userDeclinationOffset*DR2D);
				outString+=tmpString;
				extraLabel4->setValue(outString);
				extraLabel4->Refresh();	
				outString.Format("Apparent horizontal: %lf %lf %.4lf %.4lf",att->azimuth*DR2D,att->elevation*DR2D,
						att->userAzimuthOffset*DR2D,att->userElevationOffset*DR2D);
				extraLabel5->setValue(outString);
				extraLabel5->Refresh();
				extraLabel6->setValue("");
				extraLabel6->Refresh();
				break;
			}
			case Antenna::ANT_MOON : {
				Antenna::MoonAttributes_var att;
				Antenna::Moon_var moon;
				try {
					moon=client.getComponent<Antenna::Moon>(generatorName,0,true);
					lastGenerator=generatorName;
					//sidereal=Antenna::SkySource::_narrow(gen);
					if (!CORBA::is_nil(moon)) {
						moon->getAttributes(att);
					}
				}
				catch (maciErrType::CannotGetComponentExImpl& E) {
					_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,MODULE_NAME"::Main()");
					impl.setComponentName((const char *)generatorName);
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}	
				catch (CORBA::SystemException& ex) {
					_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"::Main()"); 
					impl.setName(ex._name());
					impl.setMinor(ex.minor());
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}
				catch (...) {
					_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}
				tmpString=(const char*)att->sourceID;
				outString="Source name        : "+tmpString;
				extraLabel1->setValue(outString);
				extraLabel1->Refresh();
				bValue.setValue((CORBA::Double)att->rightAscension);
				outString="Apparent equatorial: "+CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue))+" ";
				bValue.setValue((CORBA::Double)att->declination);
				outString+=CFormatFunctions::angleFormat(bValue,NULL)+" ";
				tmpString.Format("%.5lf %.4lf %.4lf",att->julianEpoch,att->userRightAscensionOffset*DR2D,att->userDeclinationOffset*DR2D);
				outString+=tmpString;
				extraLabel2->setValue(outString);
				extraLabel2->Refresh();	
				outString.Format("Apparent horizontal: %lf %lf %.4lf %.4lf",att->azimuth*DR2D,att->elevation*DR2D,
						att->userAzimuthOffset*DR2D,att->userElevationOffset*DR2D);
				extraLabel3->setValue(outString);
				extraLabel3->Refresh();
				extraLabel4->setValue("");
				extraLabel4->Refresh();			
				extraLabel5->setValue("");
				extraLabel5->Refresh();			
				extraLabel6->setValue("");
				extraLabel6->Refresh();	
				break;
			}
			case Antenna::ANT_OTF: {
				Antenna::OTFAttributes_var att;
				Antenna::OTF_var otf;
				try {
					otf=client.getComponent<Antenna::OTF>(generatorName,0,true);
					lastGenerator=generatorName;
					if (!CORBA::is_nil(otf)) {
						otf->getAttributes(att);
					}
				}
				catch (maciErrType::CannotGetComponentExImpl& E) {
					_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,MODULE_NAME"::Main()");
					impl.setComponentName((const char *)generatorName);
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}	
				catch (CORBA::SystemException& ex) {
					_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"::Main()"); 
					impl.setName(ex._name());
					impl.setMinor(ex.minor());
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}
				catch (...) {
					_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}								
				bValue.setValue((CORBA::Double)att->J2000RightAscension);
				outString="J2000 equatorial:     "+CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue))+" ";
				bValue.setValue((CORBA::Double)att->J2000Declination);
				outString+=CFormatFunctions::angleFormat(bValue,NULL)+" ";
				extraLabel1->setValue(outString);
				extraLabel1->Refresh();				
				bValue.setValue((CORBA::Double)att->rightAscension);
				outString="Apparent equatorial:  "+CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue))+" ";
				bValue.setValue((CORBA::Double)att->declination);
				outString+=CFormatFunctions::angleFormat(bValue,NULL)+" ";
				tmpString.Format("%.4lf",att->julianEpoch);
				outString+=tmpString;
				extraLabel2->setValue(outString);
				extraLabel2->Refresh();
				tmpString.Format("%lf %lf %.4lf",att->azimuth*DR2D,att->elevation*DR2D,
						att->parallacticAngle*DR2D);
				outString="Apparent horizontal:  "+tmpString;
				extraLabel3->setValue(outString);
				extraLabel3->Refresh();
				tmpString.Format("%lf %lf",att->gLongitude*DR2D,att->gLatitude*DR2D);
				outString="Galactic:             "+tmpString;
				extraLabel4->setValue(outString);
				extraLabel4->Refresh();
				tmpString.Format("%.4lf %.4lf %.4lf %.4lf %.4lf %.4lf",att->startLon*DR2D,att->startLat*DR2D,
						att->stopLon*DR2D,att->stopLat*DR2D,att->centerLon*DR2D,att->centerLat*DR2D);
				outString="Scan margin/center:   "+tmpString;
				extraLabel5->setValue(outString);
				extraLabel5->Refresh();
				tmpString.Format("%.4lf %.4lf %.4lf %.4lf %.4lf",att->lonSpan*DR2D,att->latSpan*DR2D,
						att->lonRate*DR2D,att->latRate*DR2D,att->skyRate*DR2D);
				outString="Scan span/rates:      "+tmpString;
				extraLabel6->setValue(outString);
				extraLabel6->Refresh();						
				break;
			}
			default : { //ANT_NONE
				extraLabel1-> setValue("");
				extraLabel1->Refresh();
				extraLabel2-> setValue("");
				extraLabel2->Refresh();
				extraLabel3-> setValue("");
				extraLabel3->Refresh();
				extraLabel4-> setValue("");
				extraLabel4->Refresh();		
				extraLabel5-> setValue("");
				extraLabel5->Refresh();		
				extraLabel6-> setValue("");
				extraLabel6->Refresh();	
			}
		}
	}
	else {
		extraLabel1-> setValue("");
		extraLabel1->Refresh();
		extraLabel2-> setValue("");
		extraLabel2->Refresh();
		extraLabel3-> setValue("");
		extraLabel3->Refresh();
		extraLabel4-> setValue("");
		extraLabel4->Refresh();		
		extraLabel5-> setValue("");
		extraLabel5->Refresh();		
		extraLabel6-> setValue("");
		extraLabel6->Refresh();	
	}
}


#endif /*UPDATEGENERATOR_I_*/
