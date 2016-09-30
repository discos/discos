
#include "AntennaBossTextClient.h"
#include <slamac.h>
#include <AntennaDefinitionsC.h>
#include <LogFilter.h>
#include <SkySourceC.h>
#include <OTFC.h>
#include <MoonC.h>
#include <ManagementErrors.h>
#include <acsncSimpleConsumer.h>
#include <fstream>

#define _GET_ACS_PROPERTY(TYPE,NAME) TYPE##_var NAME; \
{	\
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,"Trying to get property "#NAME"...")); \
	NAME=component->NAME(); \
	if (NAME.ptr()!=TYPE::_nil()) { \
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_DEBUG,"OK, reference is: %x",NAME.ptr())); \
	} \
	else { \
		_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,MODULE_NAME"::Main()"); \
		impl.setPropertyName(#NAME); \
		impl.log(); \
		goto ErrorLabel; \
	} \
}

#define _INSTALL_MONITOR(COMP,TRIGGERTIME) { \
	if (!COMP->installAutomaticMonitor(GUARDINTERVAL)) { \
		_EXCPT(ClientErrors::CouldntPerformActionExImpl,impl,MODULE_NAME"::Main()"); \
		impl.setAction("Install monitor"); \
		impl.setReason((const char*)COMP->getLastError()); \
		impl.log(); \
		ACE_OS::sleep(1); \
		goto ErrorLabel; \
	} \
	COMP->setTriggerTime(TRIGGERTIME); \
}

#define _GET_PROPERTY_VALUE_ONCE(TYPE,FORMAT,PROPERTY,PROPERTNAME,CONTROL) { \
	try { \
		TYPE temp; \
		ACSErr::Completion_var cmpl; \
		temp=PROPERTY->get_sync(cmpl.out()); \
		CompletionImpl cmplImpl(cmpl.in()); \
		if (cmplImpl.isErrorFree()) { \
			IRA::CString val(0,FORMAT,temp); \
			CONTROL->setValue(val); \
			CONTROL->Refresh(); \
		} \
		else { \
			_ADD_BACKTRACE(ClientErrors::CouldntAccessPropertyExImpl,impl,cmplImpl,"::Main()"); \
			impl.setPropertyName(PROPERTNAME); \
			impl.log(); \
			goto ErrorLabel; \
		} \
	} \
	catch (...) { \
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()"); \
		impl.log(); \
		goto ErrorLabel; \
	} \
}

#define COMPONENT_INTERFACE COMPONENT_IDL_MODULE::COMPONENT_IDL_INTERFACE
#define COMPONENT_DECLARATION COMPONENT_IDL_MODULE::COMPONENT_SMARTPOINTER

#define TEMPLATE_4_ROTGENERATORTYPE Antenna::ROTGeneratorType_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTBOOLEAN  Management::ROTBoolean_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTSYSTEMSTATUS  Management::ROTSystemStatus_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var

#define TEMPLATE_4_ROTREFERENCEFRAME Antenna::ROTReferenceFrame_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTVRADDEFINITION Antenna::ROTVradDefinition_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTCOORDINATEFRAME Antenna::ROTCoordinateFrame_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var


using namespace TW;

static bool terminate;

void quintHandler(int sig)
{
	terminate=true;
}

IRA::CLogGuard guard(GUARDINTERVAL*1000);
TW::CLabel* extraLabel1;
TW::CLabel* extraLabel2;
TW::CLabel* extraLabel3;
TW::CLabel* extraLabel4;
TW::CLabel* extraLabel5;
TW::CLabel* extraLabel6;
// Component declaration 
COMPONENT_DECLARATION component=COMPONENT_INTERFACE::_nil();
nc::SimpleConsumer<Antenna::AntennaDataBlock> *simpleConsumer=NULL;

#include "UpdateGenerator.i"

void NotificationHandler(Antenna::AntennaDataBlock data,void *handlerParam)
{
	TW::CLedDisplay *control;;
	control=static_cast<TW::CLedDisplay*>(handlerParam);
	if (data.tracking) {
		control->setValue("1");
		control->Refresh();
	}
	else {
		control->setValue("0");
		control->Refresh();
	}
}

int main(int argc, char *argv[]) {
	bool loggedIn=false;
	//int fieldCounter;
	maci::SimpleClient client;
	ACE_Time_Value tv(3);
	//IRA::CString fields[MAXFIELDNUMBER];
	IRA::CString inputCommand;
	
	maci::ComponentInfo_var info;
	CORBA::Object_var obj; 
		
	Antenna::TGeneratorType lastGeneratorType=Antenna::ANT_NONE;
	Antenna::EphemGenerator_var lastGenerator=Antenna::EphemGenerator::_nil();

	char fluxFormat[20];
	
	/* Add frame controls declaration */
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_ROSTRING> *target_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *targetRA_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *targetDec_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *targetVrad_field;
	/*TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *azOff_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *elOff_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *raOff_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *decOff_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *lonOff_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *latOff_field;*/
	
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *scanLonOff_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *scanLatOff_field;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTCOORDINATEFRAME,Antenna::TCoordinateFrame> *scanFrameOff_box;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *sysAzOff_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *sysElOff_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *rawAzimuth_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *rawElevation_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *observedAzimuth_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *observedElevation_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *observedRightAscension_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *observedDeclination_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *observedGalLongitude_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *observedGalLatitude_field;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTGENERATORTYPE,Antenna::TGeneratorType>* generatorType_box;
	TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN> * enabled_display;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *pointingAzimuthCorrection_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *pointingElevationCorrection_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *refractionCorrection_field;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSYSTEMSTATUS,Management::TSystemStatus> * status_box;

	TW::CPropertyStatusBox<TEMPLATE_4_ROTREFERENCEFRAME,Antenna::TReferenceFrame> * refFrame_box;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTVRADDEFINITION,Antenna::TVradDefinition> * velDef_box;

	TW::CLedDisplay * tracking_display;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *FWHM_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *targetFlux_field;
	TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN> * correctionEnabled_display;
	
	/* ******************************* */
	TW::CLabel *output_label;
	TW::CInputCommand *userInput;

	terminate=false;

	// mainframe 
	TW::CFrame window(CPoint(0,0),CPoint(WINDOW_WIDTH,WINDOW_HEIGHT),'|','|','-','-'); 
	
	// disable ctrl+C
	signal(SIGINT,SIG_IGN);
	signal(SIGUSR1,quintHandler);
	
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"MANAGER_LOGGING"));
	try {
		if (client.init(argc,argv)==0) {
			_EXCPT(ClientErrors::CouldntInitExImpl,impl,MODULE_NAME"::Main()");
			impl.log();		
			goto ErrorLabel;
		}
		else {
			if (client.login()==0) {
				_EXCPT(ClientErrors::CouldntLoginExImpl,impl,MODULE_NAME"::Main()");
				impl.log();		
				goto ErrorLabel;
			}
			loggedIn=true;
		}	
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,MODULE_NAME"::Main()");
		IRA::CString Message;
		impl.log();
		goto ErrorLabel;
	}
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::DONE"));	
	// Window initialization
	window.initFrame();
	window.setTitle(APPLICATION_TITLE);
	// Change the style of the main frame 
	window.setTitleStyle(CStyle(TITLE_COLOR_PAIR,TITLE_STYLE));
	
	if (window.colorReady()) {
		window.defineColor(GRAY_COLOR,255,255,255);		
		window.defineColorPair(BLUE_GRAY,CColor::BLUE,GRAY_COLOR);
		window.defineColorPair(GRAY_BLUE,GRAY_COLOR,CColor::BLUE);		
	}
	else {
		window.defineColorPair(BLUE_GRAY,CColor::BLUE,CColor::WHITE);
		window.defineColorPair(GRAY_BLUE,CColor::WHITE,CColor::BLUE);	
		window.defineColorPair(BLACK_RED,CColor::BLACK,CColor::RED);	
		window.defineColorPair(BLACK_GREEN,CColor::BLACK,CColor::GREEN);	
		window.defineColorPair(BLACK_YELLOW,CColor::BLACK,CColor::YELLOW);	
		window.defineColorPair(BLACK_BLUE,CColor::BLACK,CColor::BLUE);		
		window.defineColorPair(BLACK_MAGENTA,CColor::BLACK,CColor::MAGENTA);	
		window.defineColorPair(BLACK_WHITE,CColor::BLACK,CColor::WHITE	);			
	}
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::FRAME_INITIALIZED"));
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::GET_DEFAULT_COMPONENENT: %s",COMPONENT_INTERFACE_TPYE));
	
	try {
		info=client.manager()->get_default_component(client.handle(),COMPONENT_INTERFACE_TPYE);
		obj=info->reference;
		if (CORBA::is_nil(obj.in())) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,MODULE_NAME"::Main()");
			impl.setComponentName(COMPONENT_INTERFACE_TPYE);
			impl.log();
			goto ErrorLabel;
		}
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,MODULE_NAME"::Main()");
		impl.setComponentName(COMPONENT_INTERFACE_TPYE);
		impl.log();
		goto ErrorLabel;		
	}
	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,MODULE_NAME"::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto ErrorLabel;
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,MODULE_NAME"::Main()");
		impl.log();
		goto ErrorLabel;
	}
	component=COMPONENT_INTERFACE::_narrow(obj.in());
	
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_DEBUG,"OK, reference is: %d",component.ptr()));
	ACE_OS::sleep(1);
	try {
		/* Add all component properties here */
		_GET_ACS_PROPERTY(ACS::ROstring,target);
		_GET_ACS_PROPERTY(ACS::ROdouble,targetRightAscension);
		_GET_ACS_PROPERTY(ACS::ROdouble,targetDeclination);
		_GET_ACS_PROPERTY(ACS::ROdouble,targetVrad);
		_GET_ACS_PROPERTY(Antenna::ROTReferenceFrame,vradReferenceFrame);
		_GET_ACS_PROPERTY(Antenna::ROTVradDefinition,vradDefinition);
		/*_GET_ACS_PROPERTY(ACS::ROdouble,azimuthOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevationOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,rightAscensionOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,declinationOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,longitudeOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,latitudeOffset);*/
		_GET_ACS_PROPERTY(ACS::ROdouble,subScanLonOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,subScanLatOffset);
		_GET_ACS_PROPERTY(Antenna::ROTCoordinateFrame,subScanOffsetFrame);
		_GET_ACS_PROPERTY(ACS::ROdouble,systemAzimuthOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,systemElevationOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,rawAzimuth);
		_GET_ACS_PROPERTY(ACS::ROdouble,rawElevation);		
		_GET_ACS_PROPERTY(ACS::ROdouble,observedAzimuth);
		_GET_ACS_PROPERTY(ACS::ROdouble,observedElevation);
		_GET_ACS_PROPERTY(ACS::ROdouble,observedRightAscension);
		_GET_ACS_PROPERTY(ACS::ROdouble,observedDeclination);
		_GET_ACS_PROPERTY(ACS::ROdouble,observedGalLongitude);
		_GET_ACS_PROPERTY(ACS::ROdouble,observedGalLatitude);	
		_GET_ACS_PROPERTY(Antenna::ROTGeneratorType,generatorType);
		_GET_ACS_PROPERTY(Management::ROTBoolean,enabled);
		_GET_ACS_PROPERTY(Management::ROTBoolean,correctionEnabled);
		_GET_ACS_PROPERTY(ACS::ROdouble,pointingAzimuthCorrection);
		_GET_ACS_PROPERTY(ACS::ROdouble,pointingElevationCorrection);
		_GET_ACS_PROPERTY(ACS::ROdouble,refractionCorrection);
		_GET_ACS_PROPERTY(Management::ROTSystemStatus,status);
		_GET_ACS_PROPERTY(ACS::ROdouble,FWHM);
		_GET_ACS_PROPERTY(ACS::ROdouble,targetFlux);
		
		/* ********************************* */
		ACE_OS::sleep(1);
		
		/** Frame controls creation */	
		target_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_ROSTRING>(target.in());
		targetRA_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(targetRightAscension.in());
		targetDec_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(targetDeclination.in());
		targetVrad_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(targetVrad.in());
		/*azOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuthOffset.in());
		elOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevationOffset.in());
		raOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(rightAscensionOffset.in());
		decOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(declinationOffset.in());
		lonOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(longitudeOffset.in());
		latOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(latitudeOffset.in());*/
		
		scanLonOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(subScanLonOffset.in());
		scanLatOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(subScanLatOffset.in());
		scanFrameOff_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTCOORDINATEFRAME,Antenna::TCoordinateFrame>(subScanOffsetFrame.in(),Antenna::ANT_HORIZONTAL);
		sysAzOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(systemAzimuthOffset.in());
		sysElOff_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(systemElevationOffset.in());		
		
		rawAzimuth_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(rawAzimuth.in());	
		rawElevation_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(rawElevation.in());			
		observedAzimuth_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(observedAzimuth.in());	
		observedElevation_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(observedElevation.in());	
		observedRightAscension_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(observedRightAscension.in());	
		observedDeclination_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(observedDeclination.in());
		observedGalLongitude_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(observedGalLongitude.in());
		observedGalLatitude_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(observedGalLatitude.in());
		generatorType_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTGENERATORTYPE,Antenna::TGeneratorType>
		  (generatorType.in(),Antenna::ANT_NONE);
		enabled_display=new TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN>(enabled.in(),1);
		correctionEnabled_display=new TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN>(correctionEnabled.in(),1);
		pointingAzimuthCorrection_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(pointingAzimuthCorrection.in());
		pointingElevationCorrection_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(pointingElevationCorrection.in());
		refractionCorrection_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(refractionCorrection.in());
		status_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSYSTEMSTATUS,Management::TSystemStatus> (status.in(),Management::MNG_OK);
		refFrame_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTREFERENCEFRAME,Antenna::TReferenceFrame>(vradReferenceFrame.in(),Antenna::ANT_UNDEF_FRAME);
		velDef_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTVRADDEFINITION,Antenna::TVradDefinition>(vradDefinition.in(),Antenna::ANT_UNDEF_DEF);
		FWHM_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(FWHM.in());
		targetFlux_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(targetFlux.in());
		extraLabel1=new CLabel("");
		extraLabel2=new CLabel("");
		extraLabel3=new CLabel("");
		extraLabel4=new CLabel("");		
		extraLabel5=new CLabel("");		
		extraLabel6=new CLabel("");
		tracking_display=new TW::CLedDisplay(1);
		
		/* ************************ */
		#if USE_OUTPUT_FIELD >=1 
			output_label=new TW::CLabel("");
		#else
			output_label=NULL;
		#endif
		userInput=new TW::CInputCommand();
		
		/** setting up the properties of the components of the frame controls */	
		_TW_SET_COMPONENT(targetRA_field,22,1,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(targetDec_field,37,1,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(targetFlux_field,52,1,11,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(target_field,0,1,17,1,CColorPair::BLUE_BLACK,CStyle::UNDERLINE|CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(targetVrad_field,22,2,11,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(refFrame_box,34,2,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(velDef_box,47,2,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);

		_TW_SET_COMPONENT(scanLonOff_field,22,3,11,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(scanLatOff_field,34,3,11,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(scanFrameOff_box,47,3,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		scanFrameOff_box->setStatusLook(Antenna::ANT_HORIZONTAL);
		scanFrameOff_box->setStatusLook(Antenna::ANT_EQUATORIAL);
		scanFrameOff_box->setStatusLook(Antenna::ANT_GALACTIC);
		_TW_SET_COMPONENT(sysAzOff_field,22,4,11,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(sysElOff_field,34,4,11,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		
		/*_TW_SET_COMPONENT(raOff_field,22,4,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(decOff_field,37,4,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(lonOff_field,22,5,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(latOff_field,37,5,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);*/
		_TW_SET_COMPONENT(rawAzimuth_field,22,6,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(rawElevation_field,37,6,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(observedAzimuth_field,22,7,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(observedElevation_field,37,7,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(observedRightAscension_field,22,8,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(observedDeclination_field,37,8,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(observedGalLongitude_field,22,9,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(observedGalLatitude_field,37,9,14,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(generatorType_box,22,10,10,1,BLACK_GREEN,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(pointingAzimuthCorrection_field,22,11,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(pointingElevationCorrection_field,32,11,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(refractionCorrection_field,42,11,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(FWHM_field,22,12,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		enabled_display->setPosition(CPoint(22,13));
		tracking_display->setPosition(CPoint(22,14));
		correctionEnabled_display->setPosition(CPoint(22,15));
		_TW_SET_COMPONENT(status_box,22,16,10,1,BLACK_GREEN,CStyle::BOLD,output_label);

		// setting up the controls look and feel........
		enabled_display->setOrientation(TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN>::HORIZONTAL);
		enabled_display->setOutputComponent((CFrameComponent*) output_label);
		enabled_display->setLedStyle(0,TW::CStyle(CColorPair::RED_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0),'o','@');
		correctionEnabled_display->setOrientation(TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN>::HORIZONTAL);
		correctionEnabled_display->setOutputComponent((CFrameComponent*) output_label);
		correctionEnabled_display->setLedStyle(0,TW::CStyle(CColorPair::YELLOW_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0),'o','@');
		rawAzimuth_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		rawElevation_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		observedAzimuth_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		observedElevation_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		observedRightAscension_field->setFormatFunction(CFormatFunctions::angleFormat,static_cast<void *>(&observedRightAscension_field));
		observedDeclination_field->setFormatFunction(CFormatFunctions::angleFormat,NULL);
		targetRA_field->setFormatFunction(CFormatFunctions::angleFormat,static_cast<void *>(&targetRA_field));
		targetDec_field->setFormatFunction(CFormatFunctions::angleFormat,NULL);
		targetVrad_field->setFormatFunction(CFormatFunctions::floatingPointFormat,NULL);
		strcpy(fluxFormat,"(%05.1lf Jy)");
		targetFlux_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<void *>(fluxFormat));
		/*azOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		elOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		raOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL); //format as hh.mm.ss.ss
		decOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		lonOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		latOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);*/
		
		scanLonOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL); //format as hh.mm.ss.ss
		scanLatOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		sysAzOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		sysElOff_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		observedGalLongitude_field->setFormatFunction(CFormatFunctions::angleFormat,NULL);
		observedGalLatitude_field->setFormatFunction(CFormatFunctions::angleFormat,NULL);
		pointingAzimuthCorrection_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		pointingElevationCorrection_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		refractionCorrection_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		generatorType_box->setStatusLook(Antenna::ANT_NONE,CStyle(BLACK_RED,CStyle::BOLD));
		generatorType_box->setStatusLook(Antenna::ANT_SIDEREAL);
		generatorType_box->setStatusLook(Antenna::ANT_MOON); 
		generatorType_box->setStatusLook(Antenna::ANT_SUN); 
		generatorType_box->setStatusLook(Antenna::ANT_SOLARSYTEMBODY); 
		generatorType_box->setStatusLook(Antenna::ANT_SATELLITE); 
		generatorType_box->setStatusLook(Antenna::ANT_OTF); 
		
		status_box->setStatusLook(Management::MNG_OK,CStyle(BLACK_GREEN,CStyle::BOLD));
		status_box->setStatusLook(Management::MNG_WARNING,CStyle(BLACK_YELLOW,CStyle::BOLD));
		status_box->setStatusLook(Management::MNG_FAILURE,CStyle(BLACK_RED,CStyle::BOLD));

		refFrame_box->setStatusLook(Antenna::ANT_BARY);
		refFrame_box->setStatusLook(Antenna::ANT_LSRK);
		refFrame_box->setStatusLook(Antenna::ANT_LSRD);
		refFrame_box->setStatusLook(Antenna::ANT_GALCEN);
		refFrame_box->setStatusLook(Antenna::ANT_LGROUP);
		refFrame_box->setStatusLook(Antenna::ANT_TOPOCEN);
		refFrame_box->setStatusLook(Antenna::ANT_UNDEF_FRAME);
		velDef_box->setStatusLook(Antenna::ANT_RADIO);
		velDef_box->setStatusLook(Antenna::ANT_OPTICAL);
		velDef_box->setStatusLook(Antenna::ANT_REDSHIFT);
		velDef_box->setStatusLook(Antenna::ANT_UNDEF_DEF);

		tracking_display->setOrientation(TW::CLedDisplay::HORIZONTAL);
		tracking_display->setLedStyle(0,TW::CStyle(CColorPair::GREEN_BLACK,0),TW::CStyle(CColorPair::RED_BLACK,0));
		FWHM_field->setFormatFunction(CFormatFunctions::coordinateFormat,NULL);
		
		// extra labels..........
		extraLabel1->setWidth(WINDOW_WIDTH-2); extraLabel1->setHeight(1); extraLabel1->setPosition(TW::CPoint(0,19));
		extraLabel2->setWidth(WINDOW_WIDTH-2); extraLabel2->setHeight(1); extraLabel2->setPosition(TW::CPoint(0,20));
		extraLabel3->setWidth(WINDOW_WIDTH-2); extraLabel3->setHeight(1); extraLabel3->setPosition(TW::CPoint(0,21));
		extraLabel4->setWidth(WINDOW_WIDTH-2); extraLabel4->setHeight(1); extraLabel4->setPosition(TW::CPoint(0,22));
		extraLabel5->setWidth(WINDOW_WIDTH-2); extraLabel5->setHeight(1); extraLabel5->setPosition(TW::CPoint(0,23));
		extraLabel6->setWidth(WINDOW_WIDTH-2); extraLabel6->setHeight(1); extraLabel6->setPosition(TW::CPoint(0,24));
		
		/* ****************************************************************** */
		_TW_SET_COMPONENT(userInput,0,WINDOW_HEIGHT-6,WINDOW_WIDTH-1,1,USER_INPUT_COLOR_PAIR,USER_INPUT_STYLE,NULL);
		#if USE_OUTPUT_FIELD >=1 
			_TW_SET_COMPONENT(output_label,0,WINDOW_HEIGHT-(OUTPUT_FIELD_HEIGHT+1),WINDOW_WIDTH-1,OUTPUT_FIELD_HEIGHT,OUTPUT_FIELD_COLOR_PAIR,OUTPUT_FIELD_STYLE,NULL);	
		#endif 
		
		// Monitors
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::MONITORS_INSTALLATION"));
		/** Add all required monitor installation here */

		_INSTALL_MONITOR(target_field,2000);
		_INSTALL_MONITOR(targetRA_field,2000);
		_INSTALL_MONITOR(targetDec_field,2000);
		_INSTALL_MONITOR(targetVrad_field,2000);
		_INSTALL_MONITOR(refFrame_box,2000);
		_INSTALL_MONITOR(velDef_box,2000);
		_INSTALL_MONITOR(targetFlux_field,2000);
		/*_INSTALL_MONITOR(azOff_field,500);
		_INSTALL_MONITOR(elOff_field,500);
		_INSTALL_MONITOR(raOff_field,500);
		_INSTALL_MONITOR(decOff_field,500);
		_INSTALL_MONITOR(lonOff_field,500);
		_INSTALL_MONITOR(latOff_field,500);*/

		_INSTALL_MONITOR(scanLonOff_field,500);
		_INSTALL_MONITOR(scanLatOff_field,500);
		_INSTALL_MONITOR(scanFrameOff_box,500);
		_INSTALL_MONITOR(sysAzOff_field,500);
		_INSTALL_MONITOR(sysElOff_field,500);
		
		
		_INSTALL_MONITOR(rawAzimuth_field,300);
		_INSTALL_MONITOR(rawElevation_field,300);
		_INSTALL_MONITOR(observedAzimuth_field,300);
		_INSTALL_MONITOR(observedElevation_field,300);
		_INSTALL_MONITOR(observedRightAscension_field,300);
		_INSTALL_MONITOR(observedDeclination_field,300);
		_INSTALL_MONITOR(observedGalLongitude_field,300);
		_INSTALL_MONITOR(observedGalLatitude_field,300);
		_INSTALL_MONITOR(pointingAzimuthCorrection_field,300);
		_INSTALL_MONITOR(pointingElevationCorrection_field,300);
		_INSTALL_MONITOR(refractionCorrection_field,300);
		_INSTALL_MONITOR(generatorType_box,3000);
		generatorType_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(enabled_display,3000);
		enabled_display->setValueTrigger(1L,true);
		_INSTALL_MONITOR(correctionEnabled_display,3000);
		correctionEnabled_display->setValueTrigger(1L,true);
		_INSTALL_MONITOR(status_box,3000);
		status_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(FWHM_field,3000);
		FWHM_field->setValueTrigger(0.001,true);
		
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::NOTIFICATION_CHANNEL_SUBSCRIPTION"));
		ACS_NEW_SIMPLE_CONSUMER(simpleConsumer,Antenna::AntennaDataBlock,Antenna::ANTENNA_DATA_CHANNEL,
				NotificationHandler,static_cast<void*>(tracking_display));
		simpleConsumer->consumerReady();
		
		/* ****************************************** */
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::DONE"));
		
		/* Add all the static labels */		
		//_TW_ADD_LABEL(":",18,1,1,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Radial Velocity  :",0,2,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Scan Offs        :",0,3,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("System Offs      :",0,4,18,1,CColorPair::WHITE_BLACK,0,window);
		//_TW_ADD_LABEL("Galac. Offs       :",0,5,18,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Raw Horiz.       :",0,6,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Observed Horiz.  :",0,7,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Observed Equat.  :",0,8,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Observed Galac.  :",0,9,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Generator Type   : ",0,10,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Corr.  az/el/ref : ",0,11,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("FWHM             : ",0,12,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Enabled          : ",0,13,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Tracking         : ",0,14,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Correction       : ",0,15,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Status           : ",0,16,18,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL(" _______________________________Generator__________________________________________",0,17,WINDOW_WIDTH-2,1,CColorPair::GREEN_BLACK,CStyle::BOLD,window);
		
		/* ************************* */
		
		/** Add all required association: components/Frame */
		window.addComponent((CFrameComponent*)target_field);
		window.addComponent((CFrameComponent*)targetRA_field);
		window.addComponent((CFrameComponent*)targetDec_field);
		window.addComponent((CFrameComponent*)targetVrad_field);
		window.addComponent((CFrameComponent*)refFrame_box);
		window.addComponent((CFrameComponent*)velDef_box);
		window.addComponent((CFrameComponent*)targetFlux_field);		
		window.addComponent((CFrameComponent*)scanLonOff_field);
		window.addComponent((CFrameComponent*)scanLatOff_field);
		window.addComponent((CFrameComponent*)scanFrameOff_box);
		window.addComponent((CFrameComponent*)sysAzOff_field);
		window.addComponent((CFrameComponent*)sysElOff_field);
		
		/*window.addComponent((CFrameComponent*)azOff_field);
		window.addComponent((CFrameComponent*)elOff_field);
		window.addComponent((CFrameComponent*)raOff_field);
		window.addComponent((CFrameComponent*)decOff_field);
		window.addComponent((CFrameComponent*)lonOff_field);
		window.addComponent((CFrameComponent*)latOff_field);*/
		window.addComponent((CFrameComponent*)rawAzimuth_field);
		window.addComponent((CFrameComponent*)rawElevation_field);
		window.addComponent((CFrameComponent*)observedAzimuth_field);
		window.addComponent((CFrameComponent*)observedElevation_field);
		window.addComponent((CFrameComponent*)observedRightAscension_field);
		window.addComponent((CFrameComponent*)observedDeclination_field);
		window.addComponent((CFrameComponent*)observedGalLongitude_field);
		window.addComponent((CFrameComponent*)observedGalLatitude_field);		
		window.addComponent((CFrameComponent*)generatorType_box);
		window.addComponent((CFrameComponent*)enabled_display);
		window.addComponent((CFrameComponent*)correctionEnabled_display);
		window.addComponent((CFrameComponent*)pointingAzimuthCorrection_field);
		window.addComponent((CFrameComponent*)pointingElevationCorrection_field);
		window.addComponent((CFrameComponent*)refractionCorrection_field);
		window.addComponent((CFrameComponent*)status_box);
		window.addComponent((CFrameComponent*)tracking_display);
		window.addComponent((CFrameComponent*)FWHM_field);
		window.addComponent((CFrameComponent*)extraLabel1); 
		window.addComponent((CFrameComponent*)extraLabel2); 
		window.addComponent((CFrameComponent*)extraLabel3); 
		window.addComponent((CFrameComponent*)extraLabel4); 		
		window.addComponent((CFrameComponent*)extraLabel5); 		
		window.addComponent((CFrameComponent*)extraLabel6); 			
		
		/* ********************************************** */
		window.addComponent((CFrameComponent*)userInput);		
		#if USE_OUTPUT_FIELD >=1 
			window.addComponent((CFrameComponent*)output_label);
		#endif
	}
	catch(CORBA::SystemException &E) {		
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,MODULE_NAME"::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto ErrorLabel;
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,MODULE_NAME"::Main()");
		IRA::CString Message;
		impl.log();
		goto ErrorLabel;
	}
	// now it is possible to show the frame
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::START"));
	
	window.showFrame();
	 
	while(!terminate) {
		//if ((fieldCounter=userInput->parseCommand(fields,MAXFIELDNUMBER))>0) {  // there is something input
		if (userInput->readCommand(inputCommand)) {
			if (inputCommand=="exit") terminate=true;
			else if (component->_is_a("IDL:alma/Management/CommandInterpreter:1.0")) {
				try {
					char * outputAnswer;
					component->command((const char *)inputCommand,outputAnswer);
					output_label->setValue(outputAnswer);
					CORBA::string_free(outputAnswer);
					output_label->Refresh();
				}
				catch (CORBA::SystemException& ex) {
					_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"Main()");
					impl.setName(ex._name());
					impl.setMinor(ex.minor());
					IRA::CString Message;
					_EXCPT_TO_CSTRING(Message,impl);
					output_label->setValue(Message);
					output_label->Refresh();					
					impl.log(LM_ERROR);
				}
				catch(...) {
					_EXCPT(ClientErrors::CouldntPerformActionExImpl,impl,"Main()");
					impl.setAction("command()");
					impl.setReason("communication error to component server");
					IRA::CString Message;
					_EXCPT_TO_CSTRING(Message,impl);
					output_label->setValue(Message);
					output_label->Refresh();
	                impl.log(LM_ERROR); 					
				}
			}
			else {
				output_label->setValue("not enabled");
				output_label->Refresh();				
			}
		}
		// automatic update of some controls.
		updateGenerator(client,lastGeneratorType,lastGenerator);
		//sleep for the required ammount of time
		tv.set(0,MAINTHREADSLEEPTIME*1000);
		client.run(tv);			
	}
		
	window.closeFrame();
	
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::RELEASING"));
	goto CloseLabel;
ErrorLabel:
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::ABORTING"));	
CloseLabel:
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::NOTIFICATION_CHANNEL_LOGGING_OUT"));
	if (simpleConsumer!=NULL) simpleConsumer->disconnect(); 
	simpleConsumer=NULL;	
	window.Destroy();
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::FRAME_CLOSED"));	
	ACE_OS::sleep(1);		
	try {
		if (!CORBA::is_nil(component.in())) {
			client.releaseComponent(component->name());
		}
	}
	catch (maciErrType::CannotReleaseComponentExImpl& E) {
		E.log();
	}
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::COMPONENT_RELEASED"));
	if (loggedIn) client.logout();
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::SHUTTING_DOWN"));
	signal(SIGINT,SIG_DFL);
	ACE_OS::sleep(1);	
	return 0;
}
