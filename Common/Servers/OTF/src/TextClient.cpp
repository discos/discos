/** 
 * $Id: TextClient.cpp,v 1.8 2011-05-02 10:18:37 a.orlati Exp $
*/

// Comment this out in order to avoid to get the initial azimuth and elevation 
// from the mount, in that case 180 and 45 are taken as defaults
//#define REAL_COORDINATE

#include "TextClient.h"
#include <getopt.h>
#include <iostream>
#include <fstream>

using namespace maci;

void printHelp() {
	printf("Test OTF component which is a subscan calculator\n");
	printf("\n");
	printf("OTFtDisplay [-h help] [-r read antenna init] [-f scanFrame] [-c coordFrame] [-g geometry] [-d description] [-v direction] [-u startUT] \n");
	printf("            [-l duration] [-w sector] [-a init azimuth] [-e init elevation] \n");
	printf(" 			startLon startLat stopLon stopLat\n");						
	printf("\n");
	printf("[-h help]               Shows this help\n");
	printf("[-r read antenna init]  Grabs the actual antenna coordinates for initialization, otherwise default is az=180.0 el=45.0.\n");	
	printf("[-a init azimuth]    Specifies azimuth at init time (degrees)\n");
	printf("[-e init elevation]  Specifies elevation at init time (degrees)\n");
	printf("[-c coordFrame]       Selects the frame the given coordinates are referred to. Default is 'eq'. It can be: \n");
	printf("                 'gal' for galactic frame.\n");
	printf("                 'eq' for equatorial frame.\n");
	printf("                 'hor' for horizontal frame.\n");
	printf("[-f scanFrame]  Selects the frame in which the scan is performed. Default is 'eq'. It can be:\n");
	printf("                   'gal' for galactic frame.\n");
	printf("                   'eq' for equatorial frame.\n");
	printf("                   'hor' for horizontal frame.\n");
	printf("[-g geometry]      Selects the geometry of the scan. Default is 'gc'. It can be:\n");
	printf("                   'lon' perform the scan using constant longitude.\n");
	printf("                   'lat' perform the scan using constant latitude.\n");
	printf("                   'gc'  perform the scan along the great circle.\n");
	printf("[-d description]   Selects how the provided coordinates are interpreted. Default is 'ss'. It can be:\n");
	printf("                   'ss' The provided coordinates are the limits of the scan.\n");
	printf("                   'cen' The first coordinate pair is the center of the scan, the second gives the total spans.\n");
	printf("[-v direction]     Selects if the changing coordinate increases or decreases along the scan. \n");
	printf("                   Applies to lon or lat geometries only. Default is 'inc'. It can be:\n");
	printf("                   'inc' the coordinate increases.\n");
	printf("                   'dec' the coordinate decreases.\n");
	printf("[-u startUT]       Selects scan start time. Default is 120 seconds after the current time.\n");
	printf("                   The format must be the following yyyy-ddd-hh:mm-ss.sss\n");
	printf("[-l duration]     Selects scan durations in seconds. Default is 120 seconds.\n");
	printf("[-s savefile]     Saves data into the given file name as text\n");
	printf("[-w sector]		 Specifies if the antenna initial azimuth is cw or ccw. It can be:\n");
	printf("				 'cw' for clockwise sections\n");
	printf("				 'ccw' for counterclockwise sections\n");	
	printf("startLon         Represents the longitude of the first point (degrees). \n");
	printf("                 If description is 'ss' it is the longitude of the start point \n");
	printf("                 else if description is 'cen' it is the longitude of the central point \n");
	printf("startLat         Represents the latitude of the first point (degrees). \n"); 
	printf("                 If description is 'ss' it is the latitude of the start point \n");
	printf("                 else if description is 'cen' it is the latitude of the central point \n");
	printf("stopLon          Represents the longitude of the second point (degrees). \n"); 
	printf("                 If description is 'ss' it is the longitude of the end point \n");
	printf("                 else if description is 'cen' it is the total longitude span of the subscan \n");
	printf("stopLat          Represents the latitude of the second point (degrees). \n");
	printf("                 If description is 'ss' it is the latitude of the end point \n");
	printf("                 else if description is 'cen' it is the total latitude span of the subscan \n");
}

bool parseTime(const IRA::CString& str,ACS::Time& time)
{
	int nil;
	long hh,mm,year,doy;
	double ss;
	double fract;
	sscanf((const char*)str,"%d",&nil);
	//printf("Ho letto lo zero \n\n");
	if (nil==0) {
		time=0;
		printf("Time: %lld\n",time );
		return true;
	} else {
		if (sscanf((const char*)str,"%ld-%ld-%ld:%ld:%lf",&year,&doy,&hh,&mm,&ss)==5 ) {
			TIMEVALUE tt;
			tt.reset();
			tt.normalize(true);
			tt.year((unsigned long)year);
			tt.dayOfYear(doy);
			tt.hour(hh);
			tt.minute(mm);
			tt.second((long)ss);
			fract=ss-(long)ss;
			tt.microSecond((long)(fract*1000000));
			time=tt.value().value;
			return true;
		}
		else {
			return false;
		}
	}
}

using namespace TW;
using namespace IRA;

int main(int argc, char *argv[])
{
	bool loggedIn=false;
	IRA::CString staticTargetName("otf");
	char ch;
	bool keyPressed;
	TIMEDIFFERENCE tDiff;
	baci::BACIValue bValue;
	CString name;
	SimpleClient client;
	ComponentInfo_var cInfo;
	Antenna::OTF_var hwRef;
	Antenna::OTFAttributes_var att;
	Antenna::Mount_var mount;
	Antenna::TCoordinateFrame coordFrame,subScanFrame;
	Antenna::TsubScanGeometry geometry;
	Antenna::TsubScanDescription description;
	Antenna::TsubScanDirection direction;
	double lon1,lat1;
	double lon2,lat2;
	ACS::TimeInterval duration;
	double durationSeconds;
	double initAz,initEl;
	Antenna::TSections initSector;
	Antenna::TSections commandedSector;
	ACS::Time initTime;
	ACS::Time startUT;
	bool utGiven=false;
	TIMEVALUE now;
	CString outString;
	bool getReal;
	CString saveFile;
	ofstream file;
	
	CText *RA_text;
	CText *DEC_text;
	CText *Az_text;
	CText *El_text;
	CText *julianEpoch_text;
	CText *J2000Ra_text;
	CText *J2000Dec_text;
	CText *galLon_text;
	CText *galLat_text;
	CText *pAngle_text;
	CText *sub1_text;
	CText *sub2_text;
	CText *sub3_text;
	CText *sub4_text;
	CText *sub5_text;
	CText *sub6_text;
	CText *sub7_text;
	CText *steer_text;

	
	/* Check Parameters */
	if (argc==1) {
		printf("Missing  arguments.......\n\n");
		printHelp();
		exit(-1);
	}
	int out;
	//default values
	coordFrame=subScanFrame=Antenna::ANT_EQUATORIAL;
	geometry=Antenna::SUBSCAN_GREATCIRCLE;
	description=Antenna::SUBSCAN_STARTSTOP;
	direction=Antenna::SUBSCAN_INCREASE;
	durationSeconds=120.0;
	initSector=Antenna::ACU_CW;
	initAz=180*DD2R;
	initEl=45*DD2R;
	startUT=0;
	getReal=false;
	saveFile="";
	while ((out=getopt(argc,argv,"rhf:a:e:c:g:d:v:u:l:s:t:w:"))!=-1) {
		switch (out) {
			case 'r' : {
				getReal=true;
				break;
			}
			case 's': {
				saveFile=CString(optarg);
				break;
			}
			case 'h': {
				printHelp();
				exit(0);
			}
			case 'c': {
				if (strcmp(optarg,"gal")==0) coordFrame=Antenna::ANT_GALACTIC;
				else if (strcmp(optarg,"eq")==0) coordFrame=Antenna::ANT_EQUATORIAL;
				else if (strcmp(optarg,"hor")==0) coordFrame=Antenna::ANT_HORIZONTAL;
				else {
					printf("no such coordinate frame \n\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 'f': {
				if (strcmp(optarg,"gal")==0) subScanFrame=Antenna::ANT_GALACTIC;
				else if (strcmp(optarg,"eq")==0) subScanFrame=Antenna::ANT_EQUATORIAL;
				else if (strcmp(optarg,"hor")==0) subScanFrame=Antenna::ANT_HORIZONTAL;
				else {
					printf("no such subscan frame \n\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 'a':{
				if (sscanf(optarg,"%lf",&initAz) !=1) {
				printf("incorrect init azimuth\n\n");
				printHelp();
				exit(-1);
				}else initAz=initAz*DD2R;
				break;
			}
			case 'e':{
				if (sscanf(optarg,"%lf",&initEl) !=1) {
				printf("incorrect init elevation\n\n");
				printHelp();
				exit(-1);
				} else initEl=initEl*DD2R;
				break;
			}
			case 'g' : {
				if (strcmp(optarg,"lat")==0) geometry=Antenna::SUBSCAN_CONSTLAT;
				else if (strcmp(optarg,"lon")==0) geometry=Antenna::SUBSCAN_CONSTLON;
				else if (strcmp(optarg,"gc")==0) geometry=Antenna::SUBSCAN_GREATCIRCLE;
				else {
					printf("no such geometry \n\n");
					printHelp();
					exit(-1);
				}				
				break;
			}
			case 'd' : {
				if (strcmp(optarg,"ss")==0) description=Antenna::SUBSCAN_STARTSTOP;
				else if (strcmp(optarg,"cen")==0) description=Antenna::SUBSCAN_CENTER;
				else {
					printf("no such description \n\n");
					printHelp();
					exit(-1);
				}				
				break;
			}
			case 'v': {
				if (strcmp(optarg,"inc")==0) direction=Antenna::SUBSCAN_INCREASE;
				else if (strcmp(optarg,"dec")==0) direction=Antenna::SUBSCAN_DECREASE;
				else {
					printf("no such direction \n\n");
					printHelp();
					exit(-1);
				}				
				break;
			}
			case 'u': {
				if (!parseTime(optarg,startUT)) {
					printf("wrong date or time format\n\n");
					printHelp();
					exit(-1);					
				}
				utGiven=true;
				break;
			}
			case 'l': {
				if (sscanf(optarg,"%lf",&durationSeconds) !=1) {
					printf("incorrect duration time\n\n");
					printHelp();
					exit(-1);
				}
				break;
			}
			case 'w': {
				if (strcmp(optarg,"cw")==0) {
					initSector=Antenna::ACU_CW;
				} else if (strcmp(optarg,"ccw")==0) {
					initSector=Antenna::ACU_CCW;
				} else {
					printf("incorrect sector indication\n\n");
					printHelp();
					exit(-1);
				}
				break;
			}
		}
	}
	if (sscanf(argv[optind],"%lf",&lon1)!=1) {
		printf("incorrect start longitude\n\n");
		printHelp();
		exit(-1);
	}
	if (sscanf(argv[optind+1],"%lf",&lat1) !=1) {
		printf("incorrect start latitude\n\n");
		printHelp();
		exit(-1);
	}
	if (sscanf(argv[optind+2],"%lf",&lon2) !=1) {
		printf("incorrect stop longitude\n\n");
		printHelp();
		exit(-1);
	}
	if (sscanf(argv[optind+3],"%lf",&lat2)!=1) {
		printf("incorrect stop latitude\n\n");
		printHelp();
		exit(-1);		
	}
	// print parameters......
	printf("start lon: %lf\n",lon1 );
	printf("start lat: %lf\n",lat1 );
	printf("stop lon: %lf\n",lon2 );
	printf("stop lat: %lf\n",lat2 );
	if (coordFrame==Antenna::ANT_EQUATORIAL) printf ("frame: equatorial\n");
	else if (coordFrame==Antenna::ANT_GALACTIC) printf ("frame: galactic\n");
	else if (coordFrame==Antenna::ANT_HORIZONTAL) printf ("frame: horizontal\n");
	if (subScanFrame==Antenna::ANT_EQUATORIAL) printf ("subScanframe: equatorial\n");
	else if (subScanFrame==Antenna::ANT_GALACTIC) printf ("subScanFrame: galactic\n");
	else if (subScanFrame==Antenna::ANT_HORIZONTAL) printf ("subScanFrame: horizontal\n");
	if (geometry==Antenna::SUBSCAN_CONSTLAT) printf ("geometry: constant latitude\n");
	else if (geometry==Antenna::SUBSCAN_CONSTLON) printf ("geometry: constant longitude\n");
	else if (geometry==Antenna::SUBSCAN_GREATCIRCLE) printf ("geometry: great circle\n");
	if (description==Antenna::SUBSCAN_STARTSTOP) printf ("description: startstop\n");
	else if (description==Antenna::SUBSCAN_CENTER) printf ("description: center\n");
	if (direction==Antenna::SUBSCAN_INCREASE) printf ("direction: increase\n");
	else if (direction==Antenna::SUBSCAN_DECREASE) printf ("direction: decrease\n");	
	TIMEVALUE printTime(startUT);
	if (initSector==Antenna::ACU_CW) printf("sector section is CW\n");
	else printf("sector section is CCW\n");
	if (utGiven) {
		printf("start UT: %d-%d-%d:%d:%lf\n",printTime.year(),printTime.dayOfYear(),printTime.hour(),printTime.minute(),
			(double)printTime.second()+printTime.microSecond()/1000000.0);
	}
	else {
		printf("start UT: start as soon as possible\n");
	}
	printf("duration : %lf secs\n",durationSeconds);
	CIRATools::Wait(2,0);
	
	// coordinates in radians
	lon1*=DD2R;
	lon2*=DD2R;
	lat1*=DD2R;
	lat2*=DD2R;
	TIMEDIFFERENCE tmp((long double)durationSeconds);
	duration=tmp.value().value;
	
	/* Window initialization */
	CFrame window(CPoint(0,0),CPoint(97,16),'|','|','-','-'); 
	window.initFrame();
	window.setTitle(CString("OTF text display"));
	window.setTitleStyle(CStyle(CColorPair::RED_BLACK,CStyle::BOLD)); 
	
	RA_text=new CText();
	DEC_text=new CText();
	Az_text=new CText();
	El_text=new CText();
	julianEpoch_text=new CText();
	J2000Ra_text=new CText();
	J2000Dec_text=new CText();
	galLon_text=new CText();
	galLat_text=new CText();
	pAngle_text=new CText();
	sub1_text=new CText();
	sub2_text=new CText();
	sub3_text=new CText();
	sub4_text=new CText();
	sub5_text=new CText();
	sub6_text=new CText();
	sub7_text=new CText();
	steer_text=new CText();
	
	_TW_SET_COMPONENT(J2000Ra_text,24,0,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(J2000Dec_text,37,0,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);		
	_TW_SET_COMPONENT(RA_text,24,1,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(DEC_text,37,1,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(julianEpoch_text,51,1,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(Az_text,24,2,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(El_text,37,2,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(galLon_text,24,3,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(galLat_text,37,3,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(pAngle_text,24,4,13,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(sub1_text,24,5,75,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(sub2_text,24,6,75,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(sub3_text,24,7,75,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(sub4_text,24,8,75,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(sub7_text,24,9,95,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(sub5_text,0,10,95,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(sub6_text,0,11,95,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	_TW_SET_COMPONENT(steer_text,24,12,5,1,CColorPair::WHITE_BLACK,CStyle::BOLD,NULL);
	
	window.addComponent((CFrameComponent*)RA_text);
	window.addComponent((CFrameComponent*)DEC_text);
	window.addComponent((CFrameComponent*)julianEpoch_text);
	window.addComponent((CFrameComponent*)Az_text);
	window.addComponent((CFrameComponent*)El_text);
	window.addComponent((CFrameComponent*)J2000Ra_text);
	window.addComponent((CFrameComponent*)J2000Dec_text);
	window.addComponent((CFrameComponent*)galLon_text);
	window.addComponent((CFrameComponent*)galLat_text);
	window.addComponent((CFrameComponent*)pAngle_text);
	window.addComponent((CFrameComponent*)sub1_text);
	window.addComponent((CFrameComponent*)sub2_text);
	window.addComponent((CFrameComponent*)sub3_text);
	window.addComponent((CFrameComponent*)sub4_text);
	window.addComponent((CFrameComponent*)sub5_text);
	window.addComponent((CFrameComponent*)sub6_text);
	window.addComponent((CFrameComponent*)sub7_text);
	window.addComponent((CFrameComponent*)steer_text);


	_TW_ADD_LABEL("J2000  Equat. Coord     :",0,0,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Apparent Equat. Coord:",0,1,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Apparent Horiz. Coord :",0,2,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Galactic Coord.             :",0,3,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Parallactic Angle           :",0,4,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Start/stop lon/lat          :",0,5,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Scan center                 :",0,6,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Scan lon/lat/sky rates       :",0,7,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("GC span & phi_rate          :",0,8,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("Start UT/Duration           :",0,9,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
	_TW_ADD_LABEL("CW/CCW Section              :",0,12,23,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);

	if (saveFile!="") {
		file.open((const char *)saveFile,ios_base::out|ios_base::trunc);
		if (!file.is_open()) {
			ACS_LOG(LM_FULL_INFO,"OTFDisplay::Main()",(LM_INFO,"OTFDisplay::FILE_OPENING_FAILED"));
		}
		else {
			file << "Time \t J2000RA \t J2000DEC \t Azimuth \t Elevation \t GLon \t GLat\n";
		}
	}
	
	
	ACS_LOG(LM_FULL_INFO,"OTFDisplay::Main()",(LM_INFO,"OTFDisplay::MANAGER_LOGGING"));
	if (client.init(argc,argv)==0) {
		_EXCPT(ClientErrors::CouldntInitExImpl,impl,"OTFDisplay::Main()");
		impl.log();		
		goto ErrorLabel;
	}
	else {
		if (client.login()==0) {
			_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"OTFDisplay::Main()");
			impl.log();		
			goto ErrorLabel;
		}
	}
	try {
		ComponentSpec_var cSpec = new ComponentSpec();    //use _var type for automatic memory management
		cSpec->component_name = CORBA::string_dup(COMPONENT_SPEC_ANY);    //name of the component
		cSpec->component_type = CORBA::string_dup("IDL:alma/Antenna/OTF:1.0");    //IDL interface implemented by the component
		cSpec->component_code = CORBA::string_dup(COMPONENT_SPEC_ANY);     //executable code for the component (e.g. DLL)
		cSpec->container_name = CORBA::string_dup(COMPONENT_SPEC_ANY);     //container where the component is deployed
		cInfo=client.manager()->get_dynamic_component(client.handle(),cSpec.in(),false);
		//As always, the reference must be CORBA casted to it's correct type.
		hwRef=Antenna::OTF::_narrow(cInfo->reference.in());
		//Ensure it's a valid reference
		if (CORBA::is_nil(hwRef.in())==false) {
			ACS_LOG(LM_FULL_INFO,"OTFDisplay::Main()",(LM_DEBUG,"OK, reference is: %d",hwRef.ptr()));
		}
		else {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"OTFDisplay::Main()");
			impl.setComponentName((const char *)cInfo->name);
			impl.log();
			goto ErrorLabel;	
		}
	}
	// first we catch CORBA user exception ...
	catch( maciErrType::IncompleteComponentSpecEx &E)  {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"OTFDisplay::Main()");
		impl.setComponentName((const char *)cInfo->name);
		impl.log();
		goto ErrorLabel;
	}
	catch( maciErrType::InvalidComponentSpecEx &E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"OTFDisplay::Main()");
		impl.setComponentName((const char *)cInfo->name);
		impl.log();
		goto ErrorLabel;
	}
	catch( maciErrType::ComponentSpecIncompatibleWithActiveComponentEx &E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"OTFDisplay::Main()");
		impl.setComponentName((const char *)cInfo->name);
		impl.log();
		goto ErrorLabel;
	}
	catch( maciErrType::CannotGetComponentEx &E) // can be thrown by get_dynamic_component
	{
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"OTFDisplay::Main()");
		impl.setComponentName((const char *)cInfo->name);
		impl.log();
		goto ErrorLabel;
	}
	catch( CORBA::SystemException &E)  // ... than CORBA system exception
	{
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"OTFDisplay::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto ErrorLabel;
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"OTFDisplay::Main()");
		impl.log();
		goto ErrorLabel;
	}//try-catch*/
	// get mount component
	if (getReal) {
		try {
			mount=client.get_object<Antenna::Mount>("ANTENNA/Mount",0,true);
			if (CORBA::is_nil(mount.in())==true) {
				_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"OTFDisplay::Main()");
				impl.setComponentName("ANTENNA/Mount");
				impl.log();
				goto ErrorLabel;
			}
		}
		catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"OTFDisplay::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			goto ErrorLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"OTFDisplay::Main()");
			impl.log();
			goto ErrorLabel;
		}
	}
	// get time
	double azOff,elOff;
	IRA::CIRATools::getTime(now);
	initTime=now.value().value;
	if (!utGiven) {
		//TIMEDIFFERENCE delay((long double)0.0);
		//now.add(delay.value());
		//startUT=now.value().value;
		startUT=0;
	}
	if (getReal) {
		try {
			mount->getEncoderCoordinates(initTime,initAz,initEl,azOff,elOff,initSector);
		}
		catch (CORBA::SystemException& ex) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"OTFDisplay::Main()");
			impl.setName(ex._name());
			impl.setMinor(ex.minor());
			impl.log();
			goto ErrorLabel;
		}
		catch (ComponentErrors::ComponentErrorsEx& ex) {
			_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,ex,"OTFDisplay::Main()");
			impl.setAction("Mount::getEncoderCoordinates()");
			impl.log();
			goto ErrorLabel;
		}
	}
	initAz-=azOff;
	initEl-=elOff;
	if (getReal)  {
		try {
		client.releaseComponent((const char*)mount->name());
		}
		catch (maciErrType::CannotReleaseComponentExImpl& E) {
			E.log();
		}
	}
	// disable ctrl+C
	signal(SIGINT,SIG_IGN);

	try {
		hwRef->setOffsets(10.0*DD2R,10.0*DD2R,Antenna::ANT_HORIZONTAL);
		commandedSector=hwRef->setSubScan((const char *)staticTargetName,initAz,initSector,initEl,initTime,lon1,lat1,lon2,lat2,coordFrame,geometry,subScanFrame,description,
				direction,startUT,duration);
	}
	catch (CORBA::SystemException &C) { 
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"OTFDisplay::Main()"); 
		impl.setName(C._name()); 
		impl.setMinor(C.minor()); 
		impl.log(); 
		goto ErrorLabel;
	} 
	catch (ComponentErrors::ComponentErrorsEx& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,"OTFDisplay::Main()");
		impl.log();
		goto ErrorLabel;
	}
	catch (AntennaErrors::AntennaErrorsEx& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,"OTFDisplay::Main()");
		impl.log();
		goto ErrorLabel;
	}
	catch (...) { 
		_EXCPT(ClientErrors::UnknownExImpl,impl,"OTFDisplay::Main()"); 
		impl.log(); 
		goto ErrorLabel;
	}
	keyPressed=false;
	window.showFrame();
	if (commandedSector==Antenna::ACU_CCW) {
		steer_text->setValue("CCW");
	}
	else {
		steer_text->setValue("CW");
	}
	steer_text->Refresh();
	while(!keyPressed) {
		try {
			hwRef->getAttributes(att.out());
		}
		catch (CORBA::SystemException& E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"OTFDisplay::Main()"); 
			impl.setName(E._name()); 
			impl.setMinor(E.minor()); 
			impl.log(); 			
		}
		catch (...) { 
			_EXCPT(ClientErrors::UnknownExImpl,impl,"OTFDisplay::Main()"); 
			impl.log(); 
		}
		
		bValue.setValue((CORBA::Double)att->J2000RightAscension*DR2D);
		J2000Ra_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		bValue.setValue((CORBA::Double)att->J2000Declination*DR2D);		
		J2000Dec_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		bValue.setValue((CORBA::Double)att->rightAscension*DR2D);		
		RA_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		bValue.setValue((CORBA::Double)att->declination*DR2D);		
		DEC_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		bValue.setValue((CORBA::Double)att->julianEpoch);
		julianEpoch_text->setValue(CFormatFunctions::floatingPointFormat(bValue,NULL));
		bValue.setValue((CORBA::Double)att->azimuth*DR2D);		
		Az_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		bValue.setValue((CORBA::Double)att->elevation*DR2D);		
		El_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		bValue.setValue((CORBA::Double)att->gLongitude*DR2D);		
		galLon_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		bValue.setValue((CORBA::Double)att->gLatitude*DR2D);		
		galLat_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		bValue.setValue((CORBA::Double)att->parallacticAngle*DR2D);		
		pAngle_text->setValue(CFormatFunctions::floatingPointFormat(bValue,"%08.4lf"));
		outString.Format("%08.4lf  %08.4lf  %08.4lf  %08.4lf",att->startLon*DR2D,att->startLat*DR2D,
				att->stopLon*DR2D,att->stopLat*DR2D);
		sub1_text->setValue(outString);
		outString.Format("%08.4lf  %08.4lf",att->centerLon*DR2D,att->centerLat*DR2D);
		sub2_text->setValue(outString);
		outString.Format("%08.4lf  %08.4lf %08.4lf",att->lonRate*DR2D*60.0,att->latRate*DR2D*60.0,att->skyRate*DR2D*60.0);
		sub3_text->setValue(outString);
		outString.Format("%08.4lf  %08.4lf",att->subScanSpan*DR2D,att->phiRate*DR2D*60.0);
		sub4_text->setValue(outString);	
		
		if (att->coordFrame==Antenna::ANT_EQUATORIAL) outString="frame: equatorial  ";
		else if (att->coordFrame==Antenna::ANT_GALACTIC) outString="frame: galactic  ";
		else if (att->coordFrame==Antenna::ANT_HORIZONTAL) outString="frame: horizontal  ";
		if (att->subScanFrame==Antenna::ANT_EQUATORIAL) outString+="subScanframe: equatorial  ";
		else if (att->subScanFrame==Antenna::ANT_GALACTIC) outString+="subScanFrame: galactic  ";
		else if (att->subScanFrame==Antenna::ANT_HORIZONTAL) outString+="subScanFrame: horizontal  ";
		if (att->geometry==Antenna::SUBSCAN_CONSTLAT) outString+="geometry: constant latitude  ";
		else if (att->geometry==Antenna::SUBSCAN_CONSTLON) outString+="geometry: constant longitude  ";
		else if (att->geometry==Antenna::SUBSCAN_GREATCIRCLE) outString+="geometry: great circle  ";
		sub5_text->setValue(outString);
		
		if (att->description==Antenna::SUBSCAN_STARTSTOP) outString="description: startstop  ";
		else if (att->description==Antenna::SUBSCAN_CENTER) outString="description: center  ";
		if (att->direction==Antenna::SUBSCAN_INCREASE) outString+="direction: increase  ";
		else if (att->direction==Antenna::SUBSCAN_DECREASE) outString+="direction: decrease  ";
		sub6_text->setValue(outString);
		TIMEVALUE pT(att->startUT);
		TIMEDIFFERENCE pD(att->subScanDuration);
		outString.Format("%d-%d-%02d:%02d:%08.6lf   %02d:%02d:%08.6lf",pT.year(),pT.dayOfYear(),pT.hour(),pT.minute(),
				(double)pT.second()+pT.microSecond()/1000000.0,pD.hour(),pD.minute(),
				(double)pD.second()+pD.microSecond()/1000000.0);
		sub7_text->setValue(outString);	
		window.Refresh();
		keyPressed=window.readCanvas(ch);
		if (file.is_open()) {
			TIMEVALUE now;
			IRA::CIRATools::getTime(now);
			outString.Format("%d-%d-%02d:%02d:%08.6lf %lf %lf %lf %lf %lf %lf\n",now.year(),now.dayOfYear(),now.hour(),
					now.minute(),(double)now.second()+now.microSecond()/1000000.0,
					att->J2000RightAscension,att->J2000Declination,att->azimuth,att->elevation,
					att->gLongitude,att->gLatitude);
			file<<outString;
		}
		CIRATools::Wait(0,400000);
	}
	window.closeFrame();
	ACS_LOG(LM_FULL_INFO,"OTFDisplay::Main()",(LM_INFO,"OTFDisplay::RELEASING"));
	goto CloseLabel;
	
ErrorLabel:
		ACS_LOG(LM_FULL_INFO,"OTFDisplay::Main()",(LM_INFO,"OTFDisplay::ABORTING"));	
CloseLabel:
		if (file.is_open()) {
			file.close();
		}
		window.Destroy();
		ACS_LOG(LM_FULL_INFO,"OTFDisplay::Main()",(LM_INFO,"OTFDisplay::FRAME_CLOSED"));	
		ACE_OS::sleep(1);	
		try {
			client.releaseComponent((const char*) cInfo->name);
		}
		catch (maciErrType::CannotReleaseComponentExImpl& E) {
			E.log();
		}
		ACS_LOG(LM_FULL_INFO,"OTFDisplay::Main()",(LM_INFO,"OTFDisplay::COMPONENT_RELEASED"));
		if (loggedIn) client.logout();
		ACS_LOG(LM_FULL_INFO,"OTFDisplay::Main()",(LM_INFO,"OTFDisplay::SHUTTING_DOWN"));		
		signal(SIGINT,SIG_DFL);
		ACE_OS::sleep(1);	
		return 0;	
}
