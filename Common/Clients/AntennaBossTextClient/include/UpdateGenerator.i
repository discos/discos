#ifndef _UPDATEGENERATOR_I_
#define _UPDATEGENERATOR_I_

// $Id: UpdateGenerator.i,v 1.9 2011-05-26 14:18:34 a.orlati Exp $

void updateGenerator(maci::SimpleClient& client,Antenna::TGeneratorType& lastGeneratorType,Antenna::EphemGenerator_var& lastGenerator) {
	//Antenna::EphemGenerator_var gen;
	char *gen;
	IRA::CString generatorName;
	Antenna::TGeneratorType type;
	IRA::CString outString,tmpString;
	baci::BACIValue bValue;
	try {
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
	if ((type==Antenna::ANT_NONE) || (type!=lastGeneratorType)) { // the current generator should be released
		if (!CORBA::is_nil(lastGenerator)) {
			try {
				client.releaseComponent(lastGenerator->name());
			}
			catch (maciErrType::CannotReleaseComponentExImpl& E) {
				_IRA_LOGGUARD_LOG_EXCEPTION(guard,E,LM_ERROR);
			}
			catch (...) {
			}
			lastGenerator=Antenna::EphemGenerator::_nil();
			lastGeneratorType=Antenna::ANT_NONE;
		}
	}
	//if (!CORBA::is_nil(gen)) {
	if (type!=lastGeneratorType) {
		try {
			lastGenerator=client.getComponent<Antenna::EphemGenerator>(generatorName,0,true);
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
		lastGeneratorType=type;
	}
	if (!CORBA::is_nil(lastGenerator)) {
		switch (type) {
			case Antenna::ANT_SIDEREAL : {
				IRA::CString str1,str2;
				Antenna::SkySourceAttributes_var att;
				Antenna::SkySource_var sidereal;
				try {
					sidereal=Antenna::SkySource::_narrow(lastGenerator);
					//sidereal=Antenna::SkySource::_narrow(gen);
					if (!CORBA::is_nil(sidereal)) {
						sidereal->getAttributes(att);
					}
				}
				catch (...) {
					_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}
				tmpString=(const char*)att->sourceID;
				outString="Source name    : "+tmpString;
				extraLabel1->setValue(outString);
				extraLabel1->Refresh();	
				IRA::CIRATools::radToHourAngle(att->inputRightAscension,str1);
				IRA::CIRATools::radToSexagesimalAngle(att->inputDeclination,str2);
				outString="Catalog Eq.    : "+str1+"/"+str2+"/";
				tmpString.Format("%.1lf %.2lf %.2lf %.2lf %.2lf",att->inputJEpoch,att->inputRaProperMotion,att->inputDecProperMotion,
						att->inputParallax,att->inputRadialVelocity);
				outString+=tmpString;
				extraLabel2-> setValue(outString);
				extraLabel2->Refresh();
				IRA::CIRATools::radToAngle(att->inputGalacticLongitude,str1);
				IRA::CIRATools::radToAngle(att->inputGalacticLatitude,str2);
				outString="Catalog Gal.   : "+str1+"/"+str2;
				extraLabel3->setValue(outString);
				extraLabel3->Refresh();
				IRA::CIRATools::radToHourAngle(att->rightAscension,str1);
				IRA::CIRATools::radToSexagesimalAngle(att->declination,str2);
				outString="Apparent Eq.   : "+str1+"/"+str2+"/";
				str1.Format("%.5lf",att->julianEpoch);
				outString+=str1;
				extraLabel4->setValue(outString);
				extraLabel4->Refresh();
				IRA::CIRATools::radToAngle(att->gLongitude,str1);
				IRA::CIRATools::radToAngle(att->gLatitude,str2);
				outString="Galactic       : "+str1+"/"+str2;
				extraLabel5->setValue(outString);
				extraLabel5->Refresh();
				IRA::CIRATools::radToAngle(att->azimuth,str1);
				IRA::CIRATools::radToAngle(att->elevation,str2);
				outString="Horizontal     : "+str1+"/"+str2;
				extraLabel6->setValue(outString);
				extraLabel6->Refresh();				
				break;
			}
			case Antenna::ANT_MOON : {
				IRA::CString str1,str2;
				Antenna::MoonAttributes_var att;
				Antenna::Moon_var moon;
				try {
					moon=Antenna::Moon::_narrow(lastGenerator);
					if (!CORBA::is_nil(moon)) {
						moon->getAttributes(att);
					}
				}
				catch (...) {
					_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}			
				tmpString=(const char*)att->sourceID;
				outString="Source name    : "+tmpString;
				extraLabel1->setValue(outString);
				extraLabel1->Refresh();
				IRA::CIRATools::radToHourAngle(att->rightAscension,str1);
				IRA::CIRATools::radToSexagesimalAngle(att->declination,str2);
				outString="Apparent Eq.   : "+str1+"/"+str2+"/";
				str1.Format("%.5lf",att->julianEpoch);
				outString+=str1;
				extraLabel2->setValue(outString);
				extraLabel2->Refresh();
				IRA::CIRATools::radToAngle(att->gLongitude,str1);
				IRA::CIRATools::radToAngle(att->gLatitude,str2);
				outString="Galactic       : "+str1+"/"+str2;
				extraLabel3->setValue(outString);
				extraLabel4->Refresh();
				IRA::CIRATools::radToAngle(att->azimuth,str1);
				IRA::CIRATools::radToAngle(att->elevation,str2);
				outString="Horizontal     : "+str1+"/"+str2;
				extraLabel4->setValue(outString);
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
				IRA::CString str1,str2;
				try {
					otf=Antenna::OTF::_narrow(lastGenerator);
					if (!CORBA::is_nil(otf)) {
						otf->getAttributes(att);
					}
				}
				catch (...) {
					_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
					_IRA_LOGGUARD_LOG_EXCEPTION(guard,impl,LM_ERROR);
					return;
				}				
				IRA::CIRATools::radToAngle(att->startLon,str1);
				IRA::CIRATools::radToAngle(att->startLat,str2);
				outString="Start-Center-Stop: "+str1+"/"+str2+" - ";
				IRA::CIRATools::radToAngle(att->centerLon,str1);
				IRA::CIRATools::radToAngle(att->centerLat,str2);
				outString+=str1+"/"+str2+" - ";
				IRA::CIRATools::radToAngle(att->stopLon,str1);
				IRA::CIRATools::radToAngle(att->stopLat,str2);
				outString+=str1+"/"+str2;
				extraLabel1->setValue(outString);
				extraLabel1->Refresh();			
				IRA::CIRATools::timeToStr(att->startUT,str1);
				IRA::CIRATools::intervalToStr(att->subScanDuration,str2);
				outString="StartTime-Dur.   : "+str1+" - "+str2;
				extraLabel2->setValue(outString);
				extraLabel2->Refresh();			
				IRA::CIRATools::radToHourAngle(att->J2000RightAscension,str1);
				IRA::CIRATools::radToSexagesimalAngle(att->J2000Declination,str2);
				outString="J2000-Apparent eq: "+str1+"/"+str2+" - ";
				IRA::CIRATools::radToHourAngle(att->rightAscension,str1);
				IRA::CIRATools::radToSexagesimalAngle(att->declination,str2);
				outString+=str1+"/"+str2;
				extraLabel3->setValue(outString);
				extraLabel3->Refresh();
				IRA::CIRATools::radToAngle(att->azimuth,str1);
				IRA::CIRATools::radToAngle(att->elevation,str2);
				outString="Horizontal       : "+str1+"/"+str2;
				extraLabel4->setValue(outString);
				extraLabel4->Refresh();
				IRA::CIRATools::radToAngle(att->gLongitude,str1);
				IRA::CIRATools::radToAngle(att->gLatitude,str2);
				outString="Galactic         : "+str1+"/"+str2;
				extraLabel5->setValue(outString);
				extraLabel5->Refresh();
				str1.Format("%.4lf",att->skyRate*DR2D);
				str2.Format("%.4lf",att->phiRate*DR2D);
				outString="Sky/Phi rates    : "+str1+"/"+str1;
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
