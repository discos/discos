// $Id: ObservatoryTextClient.cpp,v 1.8 2010-08-19 08:57:43 a.orlati Exp $

#include "ObservatoryTextClient.h"
#include <ComponentErrors.h>
#include <baci.h>

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

#define _GET_PROPERTY_VALUE_ONCE(TYPE,CAST,FORMAT,PROPERTY,PROPERTNAME,CONTROL) { \
	try { \
		TYPE temp; \
		ACSErr::Completion_var cmpl; \
		temp=PROPERTY->get_sync(cmpl.out()); \
		CompletionImpl cmplImpl(cmpl.in()); \
		if (cmplImpl.isErrorFree()) { \
			IRA::CString val(0,FORMAT,(CAST)temp); \
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

#define _CATCH_ALL(OUTPUT,ROUTINE,COMP_EXCEPTION) \
	catch (COMP_EXCEPTION &E) { \
		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,ROUTINE); \
		IRA::CString Message; \
		if (OUTPUT!=NULL) { \
			_EXCPT_TO_CSTRING(Message,impl); \
			OUTPUT->setValue(Message); \
			OUTPUT->Refresh(); \
		} \
		impl.log(); \
	} \
	catch (CORBA::SystemException &C) { \
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,ROUTINE); \
		impl.setName(C._name()); \
		impl.setMinor(C.minor()); \
		if (OUTPUT!=NULL) { \
			IRA::CString Message; \
			_EXCPT_TO_CSTRING(Message,impl); \
			OUTPUT->setValue(Message); \
			OUTPUT->Refresh(); \
		} \
		impl.log(); \
	} \
	catch (...) { \
		_EXCPT(ClientErrors::UnknownExImpl,impl,ROUTINE); \
		if (OUTPUT!=NULL) { \
			IRA::CString Message; \
			_EXCPT_TO_CSTRING(Message,impl); \
			OUTPUT->setValue(Message); \
			OUTPUT->Refresh(); \
		} \
		impl.log(); \
	}

#define COMPONENT_INTERFACE COMPONENT_IDL_MODULE::COMPONENT_IDL_INTERFACE
#define COMPONENT_DECLARATION COMPONENT_IDL_MODULE::COMPONENT_SMARTPOINTER

using namespace TW;

static bool terminate;

void quintHandler(int sig)
{
	terminate=true;
}

#define TEMPLATE_4_ROTGEOMODEL Antenna::ROTGeodeticModel_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var

/**
 * This code is a simple client for the Observatory component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
int main(int argc, char *argv[]) {
	bool loggedIn=false;
	int fieldCounter;
	maci::SimpleClient client;
	ACE_Time_Value tv(1);
	IRA::CString fields[MAXFIELDNUMBER];
	char formatString[20];
	
	
	maci::ComponentInfo_var info;
	CORBA::Object_var obj;
	// Component declaration 
	COMPONENT_DECLARATION component=COMPONENT_INTERFACE::_nil();
		
	/* ************************************* */
	
	/* Add frame controls declaration */
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)> *universalTime_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *julianDay_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *mjd_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)> *GAST_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)> *LocalST_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(string)> *name_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *longitude_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *latitude_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *height_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *xGeoid_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *yGeoid_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *zGeoid_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *DUT1_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *xPolarMotion_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *yPolarMotion_field;	
	TW::CPropertyStatusBox<TEMPLATE_4_ROTGEOMODEL,Antenna::TGeodeticModel>* geodeticModel_box;
	/* ******************************* */
	TW::CLabel *output_label;
	TW::CInputCommand *userInput;

	/* Add callbacks object declaration */
	//ACS::CBDescIn desc;
	//CCallbackVoid cbUnstow;
	//CCallbackVoid cbStow;
	/* ******************************** */

	terminate=false;

	// mainframe 
	TW::CFrame window(CPoint(0,0),CPoint(WINDOW_WIDTH,WINDOW_HEIGHT),'|','|','-','-'); 
	
	// disable ctrl+C
	signal(SIGINT,SIG_IGN);
	signal(SIGUSR1,quintHandler);
	
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::MANAGER_LOGGING"));
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
		_GET_ACS_PROPERTY(ACS::ROuLongLong,universalTime);
		_GET_ACS_PROPERTY(ACS::ROdouble,julianDay);
		_GET_ACS_PROPERTY(ACS::ROdouble,MJD);
		_GET_ACS_PROPERTY(ACS::ROuLongLong,GAST);
		_GET_ACS_PROPERTY(ACS::ROdouble,xPolarMotion);
		_GET_ACS_PROPERTY(ACS::ROdouble,yPolarMotion);
		_GET_ACS_PROPERTY(ACS::ROuLongLong,LST);
		_GET_ACS_PROPERTY(ACS::ROstring,observatoryName);
		_GET_ACS_PROPERTY(ACS::ROdouble,longitude);
		_GET_ACS_PROPERTY(ACS::ROdouble,latitude);
		_GET_ACS_PROPERTY(ACS::ROdouble,height);
		_GET_ACS_PROPERTY(ACS::ROdouble,xGeod);
		_GET_ACS_PROPERTY(ACS::ROdouble,yGeod);
		_GET_ACS_PROPERTY(ACS::ROdouble,zGeod);
		_GET_ACS_PROPERTY(ACS::ROdouble,DUT1);
		_GET_ACS_PROPERTY(Antenna::ROTGeodeticModel,geodeticModel);
		/* ********************************* */
		ACE_OS::sleep(1);
		
		/** Frame controls creation */		
		universalTime_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)>(universalTime.in());	
		GAST_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)>(GAST.in());	
		julianDay_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(julianDay.in());
		mjd_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(MJD.in());
		LocalST_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)>(LST.in());
		name_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(string)>(observatoryName.in());
		longitude_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(longitude.in());
		latitude_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(latitude.in());
		height_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(height.in());
		xGeoid_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(xGeod.in());
		yGeoid_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(yGeod.in());
		zGeoid_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(zGeod.in());
		DUT1_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(DUT1.in());
		xPolarMotion_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(xPolarMotion.in());
		yPolarMotion_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(yPolarMotion.in());
		geodeticModel_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTGEOMODEL,Antenna::TGeodeticModel>(geodeticModel.in(),Antenna::GEOID_WGS84);
		/* ************************ */
		#if USE_OUTPUT_FIELD >=1 
			output_label=new TW::CLabel("");
		#else
			output_label=NULL;
		#endif
		userInput=new TW::CInputCommand();
		
		/** setting up the properties of the components of the frame controls */
		
		_TW_SET_COMPONENT(name_field,26,0,22,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(universalTime_field,26,1,22,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		universalTime_field->setFormatFunction(CFormatFunctions::dateTimeClockFormat,NULL);
		_TW_SET_COMPONENT(julianDay_field,26,2,20,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		julianDay_field->setFormatFunction(CFormatFunctions::floatingPointFormat,NULL);
		_TW_SET_COMPONENT(mjd_field,26,3,20,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		mjd_field->setFormatFunction(CFormatFunctions::floatingPointFormat,NULL);
		_TW_SET_COMPONENT(GAST_field,26,4,26,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		GAST_field->setFormatFunction(CFormatFunctions::clockFormat,NULL);
		_TW_SET_COMPONENT(LocalST_field,26,5,26,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		LocalST_field->setFormatFunction(CFormatFunctions::clockFormat,NULL);		
		_TW_SET_COMPONENT(DUT1_field,26,6,15,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(xPolarMotion_field,26,7,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(yPolarMotion_field,39,7,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		
		_TW_SET_COMPONENT(latitude_field,26,8,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(longitude_field,39,8,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(height_field,52,8,6,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		strcpy(formatString,"%.3lf");
		_TW_SET_COMPONENT(xGeoid_field,26,9,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		xGeoid_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		_TW_SET_COMPONENT(yGeoid_field,39,9,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		yGeoid_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		_TW_SET_COMPONENT(zGeoid_field,52,9,12,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		zGeoid_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));	
		_TW_SET_COMPONENT(geodeticModel_box,59,8,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		geodeticModel_box->setStatusLook(Antenna::GEOID_WGS84);
		geodeticModel_box->setStatusLook(Antenna::GEOID_GRS80);
		geodeticModel_box->setStatusLook(Antenna::GEOID_MERIT83);
		geodeticModel_box->setStatusLook(Antenna::GEOID_OSU91A);
		geodeticModel_box->setStatusLook(Antenna::GEOID_SOVIET85);			
		/* ****************************************************************** */
		_TW_SET_COMPONENT(userInput,0,WINDOW_HEIGHT-6,WINDOW_WIDTH-1,1,USER_INPUT_COLOR_PAIR,USER_INPUT_STYLE,NULL);
		#if USE_OUTPUT_FIELD >=1 
			_TW_SET_COMPONENT(output_label,0,WINDOW_HEIGHT-(OUTPUT_FIELD_HEIGHT+1),WINDOW_WIDTH-1,OUTPUT_FIELD_HEIGHT,OUTPUT_FIELD_COLOR_PAIR,OUTPUT_FIELD_STYLE,NULL);	
		#endif 
		
		_GET_PROPERTY_VALUE_ONCE(IRA::CString,const char *,"%s",observatoryName,"observatoryName",name_field)			
		_GET_PROPERTY_VALUE_ONCE(double,double,"%.6lf",latitude,"latitude",latitude_field);
		_GET_PROPERTY_VALUE_ONCE(double,double,"%.6lf",longitude,"longitude",longitude_field);
		_GET_PROPERTY_VALUE_ONCE(double,double,"%.1lf",height,"height",height_field);
			
		// Monitors
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::MONITORS_INSTALLATION"));
		/** Add all required monitor installation here */
		_INSTALL_MONITOR(universalTime_field,200);
		_INSTALL_MONITOR(julianDay_field,200);
		_INSTALL_MONITOR(mjd_field,200);
		_INSTALL_MONITOR(GAST_field,200);
		_INSTALL_MONITOR(LocalST_field,200);		
		_INSTALL_MONITOR(xGeoid_field,5000);
		xGeoid_field->setValueTrigger(0.00001,true);
		_INSTALL_MONITOR(zGeoid_field,5000);
		zGeoid_field->setValueTrigger(0.00001,true);
		_INSTALL_MONITOR(yGeoid_field,5000);		
		yGeoid_field->setValueTrigger(0.00001,true);
		_INSTALL_MONITOR(DUT1_field,5000);
		DUT1_field->setValueTrigger(1e-6,true);
		_INSTALL_MONITOR(xPolarMotion_field,5000);
		xPolarMotion_field->setValueTrigger(1e-6,true);
		_INSTALL_MONITOR(yPolarMotion_field,5000);
		yPolarMotion_field->setValueTrigger(1e-6,true);		
		_INSTALL_MONITOR(geodeticModel_box,5000);
		geodeticModel_box->setValueTrigger(1L,true);		
		/* ****************************************** */
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::DONE"));
		
		/* Add all the static labels */
		_TW_ADD_LABEL("Observatory Name     :",0,0,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Universal Time       :",0,1,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Julian Day           :",0,2,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Modified Julian Day  :",0,3,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Apparent GST         :",0,4,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("LST                  :",0,5,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);				
		_TW_ADD_LABEL("DUT1                 :",0,6,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Pole motion (x,y):   :",0,7,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);		
		_TW_ADD_LABEL("Geodetic Coords      :",0,8,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Geocentric Coords    :",0,9,22,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		/* ************************* */
		
		/** Add all required association: components/Frame */
		window.addComponent((CFrameComponent*)universalTime_field);
		window.addComponent((CFrameComponent*)GAST_field);
		window.addComponent((CFrameComponent*)LocalST_field);		
		window.addComponent((CFrameComponent*)julianDay_field);
		window.addComponent((CFrameComponent*)mjd_field);
		window.addComponent((CFrameComponent*)latitude_field);
		window.addComponent((CFrameComponent*)name_field);		
		window.addComponent((CFrameComponent*)longitude_field);
		window.addComponent((CFrameComponent*)height_field);
		window.addComponent((CFrameComponent*)xGeoid_field);
		window.addComponent((CFrameComponent*)yGeoid_field);
		window.addComponent((CFrameComponent*)zGeoid_field);
		window.addComponent((CFrameComponent*)DUT1_field);
		window.addComponent((CFrameComponent*)geodeticModel_box);
		window.addComponent((CFrameComponent*)xPolarMotion_field);
		window.addComponent((CFrameComponent*)yPolarMotion_field);
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
		if ((fieldCounter=userInput->parseCommand(fields,MAXFIELDNUMBER))>0) {
			fields[0].MakeUpper();
			if (fields[0]=="EXIT") terminate=true;
		}
		client.run(tv);
		tv.set(0,MAINTHREADSLEEPTIME*1000);
	}	
	window.closeFrame();
	
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::RELEASING"));
	goto CloseLabel;
ErrorLabel:
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::ABORTING"));	
CloseLabel:
	/* Instructions to free the notification channel */
	// ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::NOTIFICATION_CHANNEL_LOGGING_OUT"));
	// if (simpleConsumer!=NULL) simpleConsumer->disconnect(); 
	// simpleConsumer=NULL;		
	/* ******************************************** */
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
