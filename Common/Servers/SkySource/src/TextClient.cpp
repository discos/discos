// $id: $


#include "TextClient.h"

using namespace maci;

void printHelp() {
	printf("Test SkuSource component which is an ephemeris calculation for a given source\n");
	printf("\n");
	printf("SkySourceTextDisplay -h|--help \n");
	printf("SkySourceTextDisplay -e|--equatorial SourceName RightAscension Declination Epoch [pmRa] [pmDec] [plx] [rvel]\n");
	printf("SkySourceTextDisplay -g|--galactic SourceName RightAscension Declination \n");
	printf("SkySourceTextDisplay -o|--horizontal Azimuth Elevation\n");
	printf("SkySourceTextDisplay -c|--catalog SourceName \n");
	printf("\n");
	printf("-h  --help          Shows this help\n");
	printf("-e  --equatorial    Selects the source in the equatorial frame \n");
	printf("    Right Ascension     hh:mm:ss.sss\n");
	printf("    Declination         dd:mm:ss.sss\n");
	printf("    Epoch               It can be 1950,2000 or -1\n"); 
	printf("    pmRa                proper motion in right ascension mas/year\n"); 
	printf("    pmDec               proper motion in declination mas/year\n"); 
	printf("    plx                 parallax in mas\n"); 
	printf("    rvel                radial velocity in Km/sec\n"); 	
	printf("-g  --galactic      Selects the source in the galactic frame \n");
	printf("    Longitude           ddd.dddd\n");
	printf("    Latitude            ddd.dddd\n");
	printf("-o  --horizontal    Command the source as a fixed point (sidereal tracking disabled)");
	printf("    azimuth           ddd.dddd\n");
	printf("    elevation         ddd.dddd\n");	
	printf("-c  --catalog       Selects the source from the catalog in the CDB \n");
	printf("/n");
	printf("Interactive Mode:\n");
	printf("    A : increase azimuth offset by 0.5 degrees\n");
	printf("    a : decrease azimuth offset by 0.5 degrees\n");
	printf("    E : increase elevation offset by 0.5 degrees\n");
	printf("    e : decrease elevation offset by 0.5 degrees\n");
	printf("    R : increase right ascension offset by 0.5 degrees\n");
	printf("    r : decrease right ascension offset by 0.5 degrees\n");
	printf("    D : increase declination offset by 0.5 degrees\n");
	printf("    d : decrease declination offset by 0.5 degrees\n");
	printf("    L : increase galactic longitude offset by 0.5 degrees\n");
	printf("    l : decrease galactic longitude offset by 0.5 degrees\n");
	printf("    B : increase galactic latitude offset by 0.5 degrees\n");
	printf("    b : decrease galactic latitude  offset by 0.5 degrees\n");
	printf("    any other : exit\n");
}

using namespace TW;
using namespace IRA;

int main(int argc, char *argv[])
{
	bool loggedIn=false;
	char ch;
	bool keyPressed;
	Antenna::TSystemEquinox epoch=Antenna::ANT_J2000;
	TIMEDIFFERENCE tDiff;
	baci::BACIValue bValue;
	CString name;
	int dd,mm;
	double ss;
	SimpleClient client;
	ComponentInfo_var cInfo;
	Antenna::SkySource_var hwRef;
	Antenna::SkySourceAttributes_var att;
	int frame=0;  //0=equatorial, 1=galactic
	double ra=0.0;
	double dec=0.0;
	double pmRA=0.0,pmDEC=0.0,plx=0.0,rvel=0.0;
	
	CText *RA0_text;
	CText *DEC0_text;
	CText *Epoch0_text;
	CText *pmRa0_text;
	CText *pmDec0_text;
	CText *Plx0_text;
	CText *RVel0_text;
	CText *sourceName_text;
	CText *RA_text;
	CText *DEC_text;
	CText *RA1_text;
	CText *DEC1_text;
	CText *Epoch1_text;
	CText *galLong0_text;
	CText *galLat0_text;
	CText *galLong2_text;
	CText * galLat2_text;
	CText *Az_text;
	CText *El_text;
	CText *pAngle_text;
	CText *userAzOff_text;
	CText *userElOff_text;
	CText *userRaOff_text;
	CText *userDecOff_text;
	CText *userLonOff_text;
	CText *userLatOff_text;
	
	/* Check Parameters */
	if (argc==1) {
		printHelp();
		return 0;
	}
	if ((strcmp(argv[1],"-h")==0) || (strcmp(argv[1],"--help")==0)) {
		printHelp();
		return 0;
	} 
	else if ((strcmp(argv[1],"-c")==0) || (strcmp(argv[1],"--catalog")==0)) {
		name=CString(argv[2]);
		frame=2;
	}
	else if ((strcmp(argv[1],"-e")==0) || (strcmp(argv[1],"--equatorial")==0)) {
		name=CString(argv[2]);
		frame=0;
		if (sscanf(argv[3],"%d:%d:%lf",&dd,&mm,&ss)!=3) {
			printHelp();
			return 2;
		}
		else {
			ra=dd+mm/60.0+ss/3600.0;
			ra*=(3.141592653589793/12.0);
		}
		if (sscanf(argv[4],"%d:%d:%lf",&dd,&mm,&ss)!=3) {
			printHelp();
			return 2;
		}		
		else {
			if (dd<0) dec=dd-mm/60.0-ss/3600.0;
			else dec=dd+mm/60.0+ss/3600.0;
			dec*=(3.141592653589793/180.0);
		}
		sscanf(argv[5],"%lf",&ss);
		if ((ss!=1950.0) && (ss!=2000.0) && (ss!=-1)) {
			printHelp();
			return 2;
		}
		else {
			if (ss==1950.0) epoch=Antenna::ANT_B1950;
			else if (ss==2000.0) epoch=Antenna::ANT_J2000;
			else epoch=Antenna::ANT_APPARENT;
		}
		if (argc>6) {
			if (sscanf(argv[6],"%lf",&pmRA)!=1) {
				printHelp();
				return 2;
			}
			printf("%lf\n",pmRA);
		}
		if (argc>7) {
			if (sscanf(argv[7],"%lf",&pmDEC)!=1) {
				printHelp();
				return 2;
			}
			printf("%lf\n",pmDEC);
		}
		if (argc>8) {
			if (sscanf(argv[8],"%lf",&plx)!=1) {
				printHelp();
				return 2;
			}
			printf("%lf\n",plx);
		}
		if (argc>9) {
			if (sscanf(argv[9],"%lf",&rvel)!=1) {
				printHelp();
				return 2;
			}
		}
	}
	else if ((strcmp(argv[1],"-g")==0) || (strcmp(argv[1],"--galactic")==0)) {
		name=CString(argv[2]);
		frame=1;
		if (sscanf(argv[3],"%lf",&ss)!=1) {
			printHelp();
			return 2;
		}
		else {
			ra=ss*(3.141592653589793/180.0);
		}
		if (sscanf(argv[4],"%lf",&ss)!=1) {
			printHelp();
			return 2;
		}		
		else {
			dec=ss*(3.141592653589793/180.0);
		}
	}
	else if ((strcmp(argv[1],"-o")==0) || (strcmp(argv[1],"--horizontal")==0)) {
		frame=3;
		if (sscanf(argv[2],"%lf",&ss)!=1) {
			printHelp();
			return 2;
		}
		else {
			ra=ss*(3.141592653589793/180.0);
		}
		if (sscanf(argv[3],"%lf",&ss)!=1) {
			printHelp();
			return 2;
		}		
		else {
			dec=ss*(3.141592653589793/180.0);
		}
	}
		
	/* Window initialization */
	CFrame window(CPoint(0,0),CPoint(97,13),'|','|','-','-');
	window.initFrame();
	window.setTitle(CString("SkySource text display"));
	window.setTitleStyle(CStyle(CColorPair::RED_BLACK,CStyle::BOLD)); 
	sourceName_text=new CText();
	RA0_text=new CText();
	DEC0_text=new CText();
	Epoch0_text=new CText();
	pmRa0_text=new CText();
	pmDec0_text=new CText();
	Plx0_text=new CText();
	RVel0_text=new CText();
	RA_text=new CText();
	DEC_text=new CText();
	galLong0_text=new CText();
	galLat0_text=new CText();
	galLong2_text=new CText();
	galLat2_text=new CText();
	RA1_text=new CText();
	DEC1_text=new CText();
	Epoch1_text=new CText();
	Az_text=new CText();
	El_text=new CText();
	pAngle_text=new CText();
	userAzOff_text=new CText();
	userElOff_text=new CText();
	userRaOff_text=new CText();
	userDecOff_text=new CText();
	userLonOff_text=new CText();
	userLatOff_text=new CText();
	
	_TW_SET_COMPONENT(sourceName_text,24,0,15,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL)
	_TW_SET_COMPONENT(RA0_text,24,1,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(DEC0_text,37,1,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(Epoch0_text,51,1,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(pmRa0_text,60,1,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(pmDec0_text,69,1,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(Plx0_text,78,1,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(RVel0_text,87,1,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(galLong0_text,24,2,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);	
	_TW_SET_COMPONENT(galLat0_text,39,2,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(RA_text,24,3,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(DEC_text,37,3,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(RA1_text,24,4,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(DEC1_text,37,4,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(Epoch1_text,51,4,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(galLong2_text,24,5,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(galLat2_text,39,5,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(Az_text,24,6,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(El_text,37,6,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(pAngle_text,24,7,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(userAzOff_text,24,8,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(userElOff_text,37,8,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(userRaOff_text,24,9,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(userDecOff_text,37,9,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(userLonOff_text,24,10,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(userLatOff_text,37,10,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	
	window.addComponent((CFrameComponent*)sourceName_text);
	window.addComponent((CFrameComponent*)RA0_text);
	window.addComponent((CFrameComponent*)DEC0_text);
	window.addComponent((CFrameComponent*)Epoch0_text);	
	window.addComponent((CFrameComponent*)pmRa0_text);
	window.addComponent((CFrameComponent*)pmDec0_text);
	window.addComponent((CFrameComponent*)Plx0_text);
	window.addComponent((CFrameComponent*)RVel0_text);
	window.addComponent((CFrameComponent*)galLong0_text);
	window.addComponent((CFrameComponent*)galLat0_text);
	window.addComponent((CFrameComponent*)galLong2_text);
	window.addComponent((CFrameComponent*)galLat2_text);
	window.addComponent((CFrameComponent*)RA_text);
	window.addComponent((CFrameComponent*)DEC_text);	
	window.addComponent((CFrameComponent*)RA1_text);
	window.addComponent((CFrameComponent*)DEC1_text);
	window.addComponent((CFrameComponent*)Epoch1_text);
	window.addComponent((CFrameComponent*)Az_text);
	window.addComponent((CFrameComponent*)El_text);
	window.addComponent((CFrameComponent*)pAngle_text);	
	window.addComponent((CFrameComponent*)userAzOff_text);	
	window.addComponent((CFrameComponent*)userElOff_text);	
	window.addComponent((CFrameComponent*)userRaOff_text);	
	window.addComponent((CFrameComponent*)userDecOff_text);
	window.addComponent((CFrameComponent*)userLatOff_text);
	window.addComponent((CFrameComponent*)userLonOff_text);
	
	_TW_ADD_LABEL("Source Name           :",0,0,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Input Equat. Coord    :",0,1,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Input Galac. Coord    :",0,2,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("J2000 Equat. Coord    :",0,3,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);	
	_TW_ADD_LABEL("Apparent Equat. Coord :",0,4,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Apparent Gal. Coord :",0,5,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Apparent Horiz. Coord :",0,6,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Parallactic Angle     :",0,7,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("User Horiz. Offsets   :",0,8,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("User Equat. Offsets   :",0,9,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("User Galactic. Offsets  :",0,10,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);

		
	ACS_LOG(LM_FULL_INFO,"SkySourceDisplay::Main()",(LM_INFO,"SkySourceDisplay::MANAGER_LOGGING"));
	if (client.init(argc,argv)==0) {
		_EXCPT(ClientErrors::CouldntInitExImpl,impl,"SkySourceDisplay::Main()");
		impl.log();		
		goto ErrorLabel;
	}
	else {
		if (client.login()==0) {
			_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"SkySourceDisplay::Main()");
			impl.log();		
			goto ErrorLabel;
		}
	}
	try {
		ComponentSpec_var cSpec = new ComponentSpec();    //use _var type for automatic memory management
		cSpec->component_name = CORBA::string_dup(COMPONENT_SPEC_ANY);    //name of the component
		cSpec->component_type = CORBA::string_dup("IDL:alma/Antenna/SkySource:1.0");    //IDL interface implemented by the component
		cSpec->component_code = CORBA::string_dup(COMPONENT_SPEC_ANY);     //executable code for the component (e.g. DLL)
		cSpec->container_name = CORBA::string_dup(COMPONENT_SPEC_ANY);     //container where the component is deployed
		cInfo=client.manager()->get_dynamic_component(client.handle(),cSpec.in(),false);
		//As always, the reference must be CORBA casted to it's correct type.
		hwRef=Antenna::SkySource::_narrow(cInfo->reference.in());
		//Ensure it's a valid reference
		if (CORBA::is_nil(hwRef.in())==false) {
			ACS_LOG(LM_FULL_INFO,"SkySourceDisplay::Main()",(LM_DEBUG,"OK, reference is: %d",hwRef.ptr()));
		}
		else {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"SkySourceDisplay::Main()");
			impl.setComponentName((const char *)cInfo->name);
			impl.log();
			goto ErrorLabel;	
		}
	}
	// first we catch CORBA user exception ...
	catch( maciErrType::IncompleteComponentSpecEx &E)  {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"SkySourceDisplay::Main()");
		impl.setComponentName((const char *)cInfo->name);
		impl.log();
		goto ErrorLabel;
	}
	catch( maciErrType::InvalidComponentSpecEx &E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"SkySourceDisplay::Main()");
		impl.setComponentName((const char *)cInfo->name);
		impl.log();
		goto ErrorLabel;
	}
	catch( maciErrType::ComponentSpecIncompatibleWithActiveComponentEx &E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"SkySourceDisplay::Main()");
		impl.setComponentName((const char *)cInfo->name);
		impl.log();
		goto ErrorLabel;
	}
	catch( maciErrType::CannotGetComponentEx &E) // can be thrown by get_dynamic_component
	{
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"SkySourceDisplay::Main()");
		impl.setComponentName((const char *)cInfo->name);
		impl.log();
		goto ErrorLabel;
	}
	catch( CORBA::SystemException &E)  // ... than CORBA system exception
	{
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SkySourceDisplay::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto ErrorLabel;
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"SkySourceDisplay::Main()");
		impl.log();
		goto ErrorLabel;
	}//try-catch*/
	
	// disable ctrl+C
	signal(SIGINT,SIG_IGN);

	try {
		if (frame==0) {
			CORBA::Double pmRaC,pmDecC,plxC,rvelC,raC,decC;
			pmRaC=pmRA; pmDecC=pmDEC; plxC=plx; rvelC=rvel;
			raC=ra;decC=dec;
			hwRef->setSourceFromEquatorial((const char *)name,raC,decC,epoch,pmRaC,pmDecC,plxC,rvelC);
		}
		else if (frame==1) {
			CORBA::Double longit,latit;
			longit=ra; latit=dec;
			hwRef->setSourceFromGalactic((const char*)name,longit,latit);
		}
		else if (frame==2) {
			hwRef->loadSourceFromCatalog((const char *)name);
		}
		else if (frame==3) {
			CORBA::Double Az,El;
			Az=ra,El=dec;
			hwRef->setFixedPoint(Az,El);
		}
	}
	catch (CORBA::SystemException &C) { 
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SkySourceDisplay::Main()"); 
		impl.setName(C._name()); 
		impl.setMinor(C.minor()); 
		impl.log(); 
		goto ErrorLabel;
	} 
	catch (ComponentErrors::ComponentErrorsEx& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,"SkySourceDisplay::Main()");
		impl.log();
		goto ErrorLabel;
	}
	catch (AntennaErrors::AntennaErrorsEx& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,"SkySourceDisplay::Main()");
		impl.log();
		goto ErrorLabel;
	}
	catch (...) { 
		_EXCPT(ClientErrors::UnknownExImpl,impl,"SkySourceDisplay::Main()"); 
		impl.log(); 
		goto ErrorLabel;
	}
	try {
		hwRef->getAttributes(att);
	}
	catch (CORBA::SystemException& E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SkySourceDisplay::Main()"); 
		impl.setName(E._name()); 
		impl.setMinor(E.minor()); 
		impl.log(); 
		goto ErrorLabel;
	}
	catch (...) { 
		_EXCPT(ClientErrors::UnknownExImpl,impl,"SkySourceDisplay::Main()"); 
		impl.log(); 
		goto ErrorLabel;
	}
	sourceName_text->setValue(CString(att->sourceID));
	bValue.setValue((CORBA::Double)att->inputRightAscension);
	RA0_text->setValue(CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue)));
	bValue.setValue((CORBA::Double)att->inputDeclination);		
	DEC0_text->setValue(CFormatFunctions::angleFormat(bValue,NULL));
	bValue.setValue((CORBA::Double)att->inputJEpoch);
	Epoch0_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
	bValue.setValue((CORBA::Double)att->inputRaProperMotion);
	pmRa0_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
	bValue.setValue((CORBA::Double)att->inputDecProperMotion);
	pmDec0_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
	bValue.setValue((CORBA::Double)att->inputParallax);
	Plx0_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
	bValue.setValue((CORBA::Double)att->inputRadialVelocity);
	RVel0_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
	bValue.setValue((CORBA::Double)att->J2000RightAscension);
	RA_text->setValue(CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue)));
	bValue.setValue((CORBA::Double)att->J2000Declination);		
	DEC_text->setValue(CFormatFunctions::angleFormat(bValue,NULL));
	bValue.setValue((CORBA::Double)(att->inputGalacticLongitude*DR2D));
	galLong0_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
	bValue.setValue((CORBA::Double)(att->inputGalacticLatitude*DR2D));		
	galLat0_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
	keyPressed=false;
	window.showFrame();
	while(!keyPressed) {
		try {
			hwRef->getAttributes(att.out());
		}
		catch (CORBA::SystemException& E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SkySourceDisplay::Main()"); 
			impl.setName(E._name()); 
			impl.setMinor(E.minor()); 
			impl.log(); 			
		}
		catch (...) { 
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SkySourceDisplay::Main()"); 
			impl.log(); 
		}
		bValue.setValue((CORBA::Double)att->J2000RightAscension);
		RA_text->setValue(CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue)));
		bValue.setValue((CORBA::Double)att->J2000Declination);		
		DEC_text->setValue(CFormatFunctions::angleFormat(bValue,NULL));
		bValue.setValue((CORBA::Double)att->rightAscension);
		RA1_text->setValue(CFormatFunctions::angleFormat(bValue,static_cast<void *>(&bValue)));
		bValue.setValue((CORBA::Double)att->declination);
		DEC1_text->setValue(CFormatFunctions::angleFormat(bValue,NULL));
		//DEC1_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue((CORBA::Double)att->julianEpoch);
		Epoch1_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue((CORBA::Double)(att->gLatitude*DR2D));
		galLat2_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue((CORBA::Double)(att->gLongitude*DR2D));
		galLong2_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue((CORBA::Double)(att->azimuth*DR2D));	
		Az_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue((CORBA::Double)(att->elevation*DR2D));	
		El_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue(CORBA::Double(att->parallacticAngle*DR2D));
		pAngle_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		
		bValue.setValue(CORBA::Double(att->userAzimuthOffset*DR2D));
		userAzOff_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue(CORBA::Double(att->userElevationOffset*DR2D));
		userElOff_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue(CORBA::Double(att->userRightAscensionOffset*DR2D));
		userRaOff_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue(CORBA::Double(att->userDeclinationOffset*DR2D));
		userDecOff_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));

		bValue.setValue(CORBA::Double(att->userLongitudeOffset*DR2D));
		userLonOff_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue(CORBA::Double(att->userLatitudeOffset*DR2D));
		userLatOff_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		
		window.Refresh();
		keyPressed=window.readCanvas(ch);
		if ((ch=='l') || (ch=='L') || (ch=='b') || (ch=='B')) {
				keyPressed=false;
				CORBA::Double lonOff=att->userLongitudeOffset;
				CORBA::Double latOff=att->userLatitudeOffset;
				Antenna::TCoordinateFrame offFrame;
				if (ch=='L') lonOff+=0.5*DD2R;
				else if (ch=='l') lonOff-=0.5*DD2R;
				else if (ch=='B') latOff+=0.5*DD2R;
				else if (ch=='b') latOff-=0.5*DD2R;
				ch=0;
				try {
					offFrame=Antenna::ANT_GALACTIC;
					hwRef->setOffsets(lonOff,latOff,offFrame);
				}
				catch (CORBA::SystemException& E) {
					_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SkySourceDisplay::Main()"); 
					impl.setName(E._name()); 
					impl.setMinor(E.minor()); 
					impl.log(); 			
				}
				catch (...) { 
					_EXCPT(ClientErrors::UnknownExImpl,impl,"SkySourceDisplay::Main()"); 
					impl.log(); 
				}
		}
		if ((ch=='a') || (ch=='A') || (ch=='e') || (ch=='E')) {
			keyPressed=false;
			CORBA::Double AzOff=att->userAzimuthOffset;
			CORBA::Double ElOff=att->userElevationOffset;
			Antenna::TCoordinateFrame offFrame;
			if (ch=='A') AzOff+=0.5*DD2R;
			else if (ch=='a') AzOff-=0.5*DD2R;
			else if (ch=='E') ElOff+=0.5*DD2R;
			else if (ch=='e') ElOff-=0.5*DD2R;
			ch=0;
			try {
				offFrame=Antenna::ANT_HORIZONTAL;
				hwRef->setOffsets(AzOff,ElOff,offFrame);
			}
			catch (CORBA::SystemException& E) {
				_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SkySourceDisplay::Main()"); 
				impl.setName(E._name()); 
				impl.setMinor(E.minor()); 
				impl.log(); 			
			}
			catch (...) { 
				_EXCPT(ClientErrors::UnknownExImpl,impl,"SkySourceDisplay::Main()"); 
				impl.log(); 
			}
		}
		if ((ch=='r') || (ch=='R') || (ch=='d') || (ch=='D')) {
			keyPressed=false;
			CORBA::Double RaOff=att->userRightAscensionOffset;
			CORBA::Double DecOff=att->userDeclinationOffset;
			Antenna::TCoordinateFrame offFrame;
			if (ch=='R') RaOff+=0.5*DD2R;
			else if (ch=='r') RaOff-=0.5*DD2R;
			else if (ch=='D') DecOff+=0.5*DD2R;
			else if (ch=='d') DecOff-=0.5*DD2R;
			ch=0;
			try {
				offFrame=Antenna::ANT_EQUATORIAL;
				hwRef->setOffsets(RaOff,DecOff,offFrame);
			}
			catch (CORBA::SystemException& E) {
				_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SkySourceDisplay::Main()"); 
				impl.setName(E._name()); 
				impl.setMinor(E.minor()); 
				impl.log(); 			
			}
			catch (...) { 
				_EXCPT(ClientErrors::UnknownExImpl,impl,"SkySourceDisplay::Main()"); 
				impl.log(); 
			}
		}
		CIRATools::Wait(0,400000);
	}
	window.closeFrame();
	ACS_LOG(LM_FULL_INFO,"SkySourceDisplay::Main()",(LM_INFO,"SkySourceDisplay::RELEASING"));
	goto CloseLabel;
	
ErrorLabel:
		ACS_LOG(LM_FULL_INFO,"SkySourceDisplay::Main()",(LM_INFO,"SkySourceDisplay::ABORTING"));	
CloseLabel:
		window.Destroy();
		ACS_LOG(LM_FULL_INFO,"SkySourceDisplay::Main()",(LM_INFO,"SkySourceDisplay::FRAME_CLOSED"));	
		ACE_OS::sleep(1);	
		try {
			client.releaseComponent((const char*) cInfo->name);
		}
		catch (maciErrType::CannotReleaseComponentExImpl& E) {
			E.log();
		}
		ACS_LOG(LM_FULL_INFO,"SkySourceDisplay::Main()",(LM_INFO,"SkySourceDisplay::COMPONENT_RELEASED"));
		if (loggedIn) client.logout();
		ACS_LOG(LM_FULL_INFO,"SkySourceDisplay::Main()",(LM_INFO,"SkySourceDisplay::SHUTTING_DOWN"));		
		signal(SIGINT,SIG_DFL);
		ACE_OS::sleep(1);	
		return 0;	
}
