
#include <ComponentErrors.h>
#include "MedicinaMountTextClient.h"
#include <baci.h>
#include <acsncSimpleConsumer.h>
//#include <LogFilter.h>

#define GRAY_COLOR 0
#define BLUE_GRAY 0
#define GRAY_BLUE 1
#define BLACK_RED 2
#define BLACK_GREEN 3
#define BLACK_YELLOW 4
#define BLACK_BLUE 5
#define BLACK_MAGENTA 6
#define BLACK_WHITE 7

#define WINDOW_WIDTH 72
#define WINDOW_HEIGHT 30

/* to send the client to sleep for a certain ammount of time, setup this value (milliseconds) */
#define MAINTHREADSLEEPTIME 250

#define MAXFIELDNUMBER 10

using namespace TW;

static bool terminate_;

void quintHandler(int sig)
{
	terminate_=true;
}
	
#define TEMPLATE_4_ROTMODES Antenna::ROTCommonModes_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTSTATUS Antenna::ROTStatus_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTSECTIONS Antenna::ROTSections_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTSYSTEMSTATUS Management::ROTSystemStatus_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var


void CCallbackVoid::working(const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{	
	int iValue;
	IRA::CString sValue;
	TW::CLabel* out;
	TW::CLedDisplay* led;
	out=(CLabel*)(const_cast<void *>(m_output));
	led=(CLedDisplay*)(const_cast<void *>(m_control));
	sValue=led->getValue();
	iValue=sValue.ToInt();
	if (iValue>0) led->setValue("0");
	else led->setValue("1");
	led->Refresh();
}

void CCallbackVoid::done (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
	ACSErr::CompletionImpl comp(c);
	TW::CLabel* out;
	TW::CLedDisplay* led;
	out=(CLabel*)(const_cast<void *>(m_output));
	led=(CLedDisplay*)(const_cast<void *>(m_control));
	if (comp.isErrorFree()) {
		out->setValue("Done!!!");
		out->Refresh();
		led->setValue("0");
		led->Refresh();
	}
	else {
		IRA::CString Message;
		_COMPL_TO_CSTRING(Message,comp);
		out->setValue(Message);
		out->Refresh();
	}
}

#define COMPONENT_INTERFACE_TPYE "IDL:alma/Antenna/MedicinaMount:1.0"  /* the type of the interface */

/*void NotificationHandler(Antenna::MountDataBlock data,void *handlerParam)
{
	TW::CStatusBox<TTracking> *box;
	box=static_cast<TW::CStatusBox<TTracking>*>(handlerParam);
	if (data.onSource) {
		box->setValue(TRACKING);
		box->Refresh();
	}
	else {
		box->setValue(SLEWING);
		box->Refresh();		
	}
}*/

/**
 * This code is a simple text client for the MedicinaACU component.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
int main(int argc, char *argv[]) {
	bool loggedIn=false;
	int fieldCounter;
	maci::SimpleClient client;
	Antenna::MedicinaMount_var acu;

	ACE_Time_Value tv(1);
	IRA::CString fields[MAXFIELDNUMBER];
	IRA::CString componentName="";
	char formatString[20];
	maci::ComponentInfo_var info;
	CORBA::Object_var obj;
	
	//_IRA_LOGFILTER_DECLARE;

	terminate_=false;

	/* Main frame */
	TW::CFrame window(CPoint(0,0),CPoint(WINDOW_WIDTH,WINDOW_HEIGHT),'|','|','-','-'); 
	
	/* Notification channel */
	//nc::SimpleConsumer<Antenna::MountDataBlock> *simpleConsumer=NULL;
	
	/* Frame controls declaration */
	TW::CInputCommand *usrInput;
	TW::CLabel *outputField;
	TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)> *azimuthServoStatus_display;
	TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)> *elevationServoStatus_display;
	TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)> *servoSystemStatus_display;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *elevation_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *azimuth_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *azimuthError_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *elevationError_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>* elevationRate_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>* azimuthRate_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)> *time_field;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTMODES,Antenna::TCommonModes>* azimuthMode_box;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTMODES,Antenna::TCommonModes>* elevationMode_box;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSTATUS,Antenna::TStatus>* status_box;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSYSTEMSTATUS,Management::TSystemStatus>* mountStatus_box;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSECTIONS,Antenna::TSections>* section_box;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)>* freeStack_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>* deltaTime_field;
	//TW::CStatusBox<TTracking>* onSource_box;
	TW::CLedDisplay* working_led;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *commandedAzimuth_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *commandedElevation_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *azimuthOffset_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *elevationOffset_field;
	
		
	/** Callbacks */
	//ACS::CBDescIn desc;
	CCallbackVoid cbUnstow;
	CCallbackVoid cbStow;

	// disable ctrl+C
	signal(SIGINT,SIG_IGN);
	signal(SIGUSR1,quintHandler);

	ACS_LOG(LM_SOURCE_INFO,"mountTui::Main()",(LM_INFO,"MANAGER_LOGGING"));
	try {
		if (client.init(argc,argv)==0) {
			_EXCPT(ClientErrors::CouldntInitExImpl,impl,"mountTui::Main()");
			impl.log();
			goto ErrorLabel;
		}
		else {
			client.login();
			loggedIn=true;
		}	
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"mountTui::Main()");
		IRA::CString Message;
		impl.log();
		goto ErrorLabel;
	}
	ACS_LOG(LM_SOURCE_INFO,"mountTui::Main()",(LM_INFO,"DONE"));
	/** Window initialization */
	window.initFrame();
	window.setTitle("Mount");
	window.setTitleStyle(CStyle(CColorPair::RED_BLACK,CStyle::BOLD|CStyle::UNDERLINE));
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
	ACS_LOG(LM_SOURCE_INFO,"mountTui::Main()",(LM_INFO,"FRAME_INITIALIZED"));
	ACS_LOG(LM_SOURCE_INFO,"mountTui::Main()",(LM_INFO,"GET_DEFAULT_COMPONENENT: %s",COMPONENT_INTERFACE_TPYE));
	try {
		info=client.manager()->get_default_component(client.handle(),COMPONENT_INTERFACE_TPYE);
		obj=info->reference;
		if (CORBA::is_nil(obj.in())) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"mountTui::Main()");
			impl.setComponentName(COMPONENT_INTERFACE_TPYE);
			impl.log();
			goto ErrorLabel;
		}
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"mountTui::Main()");
		impl.setComponentName(COMPONENT_INTERFACE_TPYE);
		impl.log();
		goto ErrorLabel;		
	}
	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"mountTui::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto ErrorLabel;
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"mountTui::Main()");
		impl.log();
		goto ErrorLabel;
	}
	componentName=info->name;
	acu=Antenna::MedicinaMount::_narrow(obj.in());
	
	ACS_LOG(LM_SOURCE_INFO,"mountTui::Main()",(LM_DEBUG,"OK, reference is: %d",acu.ptr()));
	ACE_OS::sleep(1);
	try {	
		/* Get ACS properties */
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuth,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevation,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuthError,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevationError,acu);				
		_GET_ACS_PROPERTY(ACS::ROpattern,azimuthServoStatus,acu);
		_GET_ACS_PROPERTY(ACS::ROpattern,elevationServoStatus,acu);
		_GET_ACS_PROPERTY(ACS::ROpattern,servoSystemStatus,acu);
		_GET_ACS_PROPERTY(ACS::ROuLongLong,time,acu);
		_GET_ACS_PROPERTY(Antenna::ROTCommonModes,azimuthMode,acu);
		_GET_ACS_PROPERTY(Antenna::ROTCommonModes,elevationMode,acu);
		_GET_ACS_PROPERTY(Antenna::ROTStatus,status,acu);
		_GET_ACS_PROPERTY(Antenna::ROTSections,section,acu);
		_GET_ACS_PROPERTY(ACS::ROlong,freeProgramTrackPosition,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevationRate,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuthRate,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,deltaTime,acu);
		_GET_ACS_PROPERTY(Management::ROTSystemStatus,mountStatus,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuthOffset,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevationOffset,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,commandedAzimuth,acu);
		_GET_ACS_PROPERTY(ACS::ROdouble,commandedElevation,acu);

		ACE_OS::sleep(1);
		
		/* Frame controls creation */
		usrInput=new TW::CInputCommand();
		outputField=new TW::CLabel("");
		elevation_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevation.in());		
		azimuth_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuth.in());		
		azimuthError_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuthError.in());		
		elevationError_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevationError.in());
		freeStack_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)>(freeProgramTrackPosition.in());
		elevationRate_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevationRate.in());
		azimuthRate_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuthRate.in());
		azimuthServoStatus_display=new TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)>(azimuthServoStatus.in(),16);
		elevationServoStatus_display=new TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)>(elevationServoStatus.in(),16);
		servoSystemStatus_display=new TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)>(servoSystemStatus.in(),7);
		time_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)>(time.in());	
		deltaTime_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(deltaTime.in());
		azimuthMode_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTMODES,Antenna::TCommonModes>(azimuthMode.in(),Antenna::ACU_STANDBY);
		elevationMode_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTMODES,Antenna::TCommonModes>(elevationMode.in(),Antenna::ACU_STANDBY);			
		status_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSTATUS,Antenna::TStatus>(status.in(),Antenna::ACU_NOTCNTD);	
		section_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSECTIONS,Antenna::TSections>(section.in(),Antenna::ACU_CW);
		mountStatus_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSYSTEMSTATUS,Management::TSystemStatus>(mountStatus.in(),Management::MNG_OK);
		//onSource_box=new TW::CStatusBox<TTracking>(SLEWING);
		working_led=new TW::CLedDisplay(1);	
		azimuthOffset_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuthOffset.in());	
		elevationOffset_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevationOffset.in());	
		commandedAzimuth_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(commandedAzimuth.in());	
		commandedElevation_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(commandedElevation.in());	
		
		/* Setting up the properties of the frame controls */

		_TW_SET_COMPONENT(azimuth_field,18,1,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);
		azimuth_field->setWAlign(CFrameComponent::RIGHT);
		_TW_SET_COMPONENT(elevation_field,18,2,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);	
		elevation_field->setWAlign(CFrameComponent::RIGHT);		
		_TW_SET_COMPONENT(elevationRate_field,60,2,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);
		_TW_SET_COMPONENT(azimuthRate_field,60,1,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);			
		_TW_SET_COMPONENT(azimuthError_field,30,1,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);
		azimuthError_field->setWAlign(CFrameComponent::RIGHT);
		_TW_SET_COMPONENT(elevationError_field,30,2,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);	
		elevationError_field->setWAlign(CFrameComponent::RIGHT);	
		_TW_SET_COMPONENT(azimuthMode_box,42,1,15,1,BLACK_WHITE,CStyle::BOLD,outputField);
		_TW_SET_COMPONENT(elevationMode_box,42,2,15,1,BLACK_WHITE,CStyle::BOLD,outputField);
		_TW_SET_COMPONENT(azimuthOffset_field,18,3,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);	
		azimuthOffset_field->setWAlign(CFrameComponent::RIGHT);
		_TW_SET_COMPONENT(elevationOffset_field,30,3,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);	
		elevationOffset_field->setWAlign(CFrameComponent::RIGHT);
		_TW_SET_COMPONENT(commandedAzimuth_field,18,4,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);	
		commandedAzimuth_field->setWAlign(CFrameComponent::RIGHT);
		_TW_SET_COMPONENT(commandedElevation_field,30,4,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);	
		commandedElevation_field->setWAlign(CFrameComponent::RIGHT);
		_TW_SET_COMPONENT(time_field,18,5,16,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);	
		_TW_SET_COMPONENT(deltaTime_field,60,5,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);
		_TW_SET_COMPONENT(usrInput,0,WINDOW_HEIGHT-6,WINDOW_WIDTH-1,1,BLUE_GRAY,CStyle::NORMAL,NULL);
		_TW_SET_COMPONENT(outputField,0,WINDOW_HEIGHT-5,WINDOW_WIDTH-1,4,GRAY_BLUE,CStyle::NORMAL,outputField);	
		_TW_SET_COMPONENT(status_box,42,7,15,1,BLACK_WHITE,CStyle::BOLD,outputField);
		//_TW_SET_COMPONENT(onSource_box,18,6,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);			
		_TW_SET_COMPONENT(freeStack_field,42,17,16,1,CColorPair::WHITE_BLACK,CStyle::BOLD,outputField);
		_TW_SET_COMPONENT(section_box,42,18,16,1,BLACK_WHITE,CStyle::BOLD,outputField);
		_TW_SET_COMPONENT(mountStatus_box,42,19,16,1,BLACK_WHITE,CStyle::BOLD,outputField);
		
		strcpy(formatString,"% 010.5lf");
		azimuth_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));	
		elevation_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));	
		azimuthError_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));		
		elevationError_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		azimuthOffset_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		elevationOffset_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		commandedAzimuth_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		commandedElevation_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		time_field->setFormatFunction(CFormatFunctions::timeClockFormat,NULL);
		strcpy(formatString,"% 07.5lf");
		elevationRate_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		azimuthRate_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		strcpy(formatString,"% 08.5lf");
		deltaTime_field->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(formatString));
		azimuthServoStatus_display->setPosition(CPoint(18,8));
		azimuthServoStatus_display->setOrientation(TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)>::HORIZONTAL);
		azimuthServoStatus_display->setOutputComponent((CFrameComponent*) outputField);
		azimuthServoStatus_display->setLedStyle(3,TW::CStyle(CColorPair::YELLOW_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		azimuthServoStatus_display->setLedStyle(4,TW::CStyle(CColorPair::YELLOW_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		azimuthServoStatus_display->setLedStyle(5,TW::CStyle(CColorPair::BLUE_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		azimuthServoStatus_display->setLedStyle(6,TW::CStyle(CColorPair::BLUE_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		azimuthServoStatus_display->setLedStyle(7,TW::CStyle(CColorPair::BLUE_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));		
		elevationServoStatus_display->setPosition(CPoint(21,8));
		elevationServoStatus_display->setOrientation(TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)>::HORIZONTAL);
		elevationServoStatus_display->setOutputComponent((CFrameComponent*) outputField);
		elevationServoStatus_display->setLedStyle(3,TW::CStyle(CColorPair::YELLOW_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		elevationServoStatus_display->setLedStyle(4,TW::CStyle(CColorPair::YELLOW_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		elevationServoStatus_display->setLedStyle(5,TW::CStyle(CColorPair::BLUE_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		elevationServoStatus_display->setLedStyle(6,TW::CStyle(CColorPair::BLUE_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		elevationServoStatus_display->setLedStyle(7,TW::CStyle(CColorPair::BLUE_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));		
		servoSystemStatus_display->setPosition(CPoint(44,9));
		servoSystemStatus_display->setOrientation(TW::CPropertyLedDisplay<_TW_PROPERTYCOMPONENT_T_RO(pattern)>::HORIZONTAL);
		servoSystemStatus_display->setOutputComponent((CFrameComponent*) outputField);
		servoSystemStatus_display->setLedStyle(0,TW::CStyle(CColorPair::RED_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		servoSystemStatus_display->setLedStyle(1,TW::CStyle(CColorPair::RED_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		servoSystemStatus_display->setLedStyle(2,TW::CStyle(CColorPair::RED_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		servoSystemStatus_display->setLedStyle(3,TW::CStyle(CColorPair::YELLOW_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		servoSystemStatus_display->setLedStyle(4,TW::CStyle(CColorPair::YELLOW_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		servoSystemStatus_display->setLedStyle(5,TW::CStyle(CColorPair::RED_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		servoSystemStatus_display->setLedStyle(6,TW::CStyle(CColorPair::BLUE_BLACK,0),TW::CStyle(CColorPair::GREEN_BLACK,0));
		azimuthMode_box->setStatusLook(Antenna::ACU_STANDBY,CStyle(BLACK_BLUE,CStyle::BOLD)); elevationMode_box->setStatusLook(Antenna::ACU_STANDBY,CStyle(BLACK_BLUE,CStyle::BOLD));
		azimuthMode_box->setStatusLook(Antenna::ACU_STOP,CStyle(BLACK_MAGENTA,CStyle::BOLD)); elevationMode_box->setStatusLook(Antenna::ACU_STOP,CStyle(BLACK_MAGENTA,CStyle::BOLD));
		azimuthMode_box->setStatusLook(Antenna::ACU_PRESET); elevationMode_box->setStatusLook(Antenna::ACU_PRESET);
		azimuthMode_box->setStatusLook(Antenna::ACU_PROGRAMTRACK); elevationMode_box->setStatusLook(Antenna::ACU_PROGRAMTRACK);
		azimuthMode_box->setStatusLook(Antenna::ACU_RATE); elevationMode_box->setStatusLook(Antenna::ACU_RATE);
		//azimuthMode_box->setStatusLook(Antenna::ACU_SECTORSCAN); elevationMode_box->setStatusLook(Antenna::ACU_SECTORSCAN);
		azimuthMode_box->setStatusLook(Antenna::ACU_STOW,CStyle(BLACK_GREEN,CStyle::BOLD)); elevationMode_box->setStatusLook(Antenna::ACU_STOW,CStyle(BLACK_GREEN,CStyle::BOLD));
		//azimuthMode_box->setStatusLook(Antenna::ACU_BORESIGHT); elevationMode_box->setStatusLook(Antenna::ACU_BORESIGHT);
		//azimuthMode_box->setStatusLook(Antenna::ACU_STARTRACK); elevationMode_box->setStatusLook(Antenna::ACU_STARTRACK);
		//azimuthMode_box->setStatusLook(Antenna::ACU_POSITIONTRACK); elevationMode_box->setStatusLook(Antenna::ACU_POSITIONTRACK);
		azimuthMode_box->setStatusLook(Antenna::ACU_UNSTOW,CStyle(BLACK_GREEN,CStyle::BOLD)); elevationMode_box->setStatusLook(Antenna::ACU_UNSTOW,CStyle(BLACK_GREEN,CStyle::BOLD));
		azimuthMode_box->setStatusLook(Antenna::ACU_UNKNOWN,CStyle(BLACK_RED,CStyle::BOLD)); elevationMode_box->setStatusLook(Antenna::ACU_UNKNOWN,CStyle(BLACK_RED,CStyle::BOLD));
		status_box->setStatusLook(Antenna::ACU_NOTCNTD,CStyle(BLACK_RED,CStyle::BOLD));
		status_box->setStatusLook(Antenna::ACU_CNTDING,CStyle(BLACK_YELLOW,CStyle::BOLD));
		status_box->setStatusLook(Antenna::ACU_CNTD,CStyle(BLACK_GREEN,CStyle::BOLD));
		status_box->setStatusLook(Antenna::ACU_BSY);
		section_box->setStatusLook(Antenna::ACU_CW);
		section_box->setStatusLook(Antenna::ACU_CCW);
		mountStatus_box->setStatusLook(Management::MNG_OK,CStyle(BLACK_GREEN,CStyle::BOLD));
		mountStatus_box->setStatusLook(Management::MNG_WARNING,CStyle(BLACK_YELLOW,CStyle::BOLD));
		mountStatus_box->setStatusLook(Management::MNG_FAILURE,CStyle(BLACK_RED,CStyle::BOLD));		
		//onSource_box->setStatusLook(TRACKING,"TRACKING",CStyle(BLACK_GREEN,TW::CStyle::BOLD));				
		//onSource_box->setStatusLook(SLEWING,"SLEWING",CStyle(BLACK_YELLOW,TW::CStyle::BOLD));
		working_led->setPosition(CPoint(58,7));
		working_led->setLedStyle(0,TW::CStyle(CColorPair::YELLOW_BLACK,0),TW::CStyle(CColorPair::YELLOW_BLACK,0),'/','|');
		cbUnstow.setControls((const void*)working_led,(const void*)outputField);
		cbStow.setControls((const void*)working_led,(const void*)outputField);		
		
		/* Monitors */
		ACS_LOG(LM_SOURCE_INFO,"mountTui::Main()",(LM_INFO,"MONITORS_INSTALLATION"));
		_INSTALL_MONITOR(azimuth_field,200);
		_INSTALL_MONITOR(elevation_field,200);
		_INSTALL_MONITOR(azimuthError_field,200);
		_INSTALL_MONITOR(elevationError_field,200);
		_INSTALL_MONITOR(elevationOffset_field,1000);
		elevationOffset_field->setValueTrigger(0.0,true);
		_INSTALL_MONITOR(azimuthOffset_field,1000);
		azimuthOffset_field->setValueTrigger(0.0,true);		
		_INSTALL_MONITOR(commandedAzimuth_field,200);
		_INSTALL_MONITOR(commandedElevation_field,200);
		_INSTALL_MONITOR(azimuthServoStatus_display,1000);
		azimuthServoStatus_display->setValueTrigger(1L,true);		
		_INSTALL_MONITOR(elevationServoStatus_display,1000);
		elevationServoStatus_display->setValueTrigger(1L,true);
		_INSTALL_MONITOR(servoSystemStatus_display,1000);
		servoSystemStatus_display->setValueTrigger(1L,true);
		_INSTALL_MONITOR(time_field,200);
		_INSTALL_MONITOR(deltaTime_field,1000);
		_INSTALL_MONITOR(azimuthMode_box,1000);
		azimuthMode_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(elevationMode_box,1000);
		elevationMode_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(status_box,1000);
		status_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(freeStack_field,1000);
		freeStack_field->setValueTrigger(1L,true);
		_INSTALL_MONITOR(section_box,1000);
		section_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(elevationRate_field,200);
		_INSTALL_MONITOR(azimuthRate_field,200);
		_INSTALL_MONITOR(mountStatus_box,1000);
		ACS_LOG(LM_FULL_INFO,"mountTui::Main()",(LM_INFO,"DONE"));
		/** Associate the components to the Frame */
		window.addComponent((CFrameComponent*)usrInput);
		window.addComponent((CFrameComponent*)outputField);
		window.addComponent((CFrameComponent*)azimuth_field);				
		window.addComponent((CFrameComponent*)elevation_field);		
		window.addComponent((CFrameComponent*)azimuthError_field);				
		window.addComponent((CFrameComponent*)elevationError_field);
		window.addComponent((CFrameComponent*)azimuthServoStatus_display);
		window.addComponent((CFrameComponent*)elevationServoStatus_display);		
		window.addComponent((CFrameComponent*)servoSystemStatus_display);		
		window.addComponent((CFrameComponent*)time_field);
		window.addComponent((CFrameComponent*)deltaTime_field);
		window.addComponent((CFrameComponent*)azimuthMode_box);
		window.addComponent((CFrameComponent*)elevationMode_box);
		window.addComponent((CFrameComponent*)status_box);
		//window.addComponent((CFrameComponent*)onSource_box);
		window.addComponent((CFrameComponent*)working_led);
		window.addComponent((CFrameComponent*)section_box);
		window.addComponent((CFrameComponent*)freeStack_field);
		window.addComponent((CFrameComponent*)elevationRate_field);
		window.addComponent((CFrameComponent*)azimuthRate_field);
		window.addComponent((CFrameComponent*)mountStatus_box);
		window.addComponent((CFrameComponent*)azimuthOffset_field);				
		window.addComponent((CFrameComponent*)elevationOffset_field);				
		window.addComponent((CFrameComponent*)commandedAzimuth_field);				
		window.addComponent((CFrameComponent*)commandedElevation_field);				
		
		/** Register for NC consuming.............. */
		
		/*ACS_LOG(LM_FULL_INFO,"MedicinaMountTextClient::Main()",(LM_INFO,"MedicinaMountTextClient::NOTIFICATION_CHANNEL_SUBSCRIPTION"));
		ACS_NEW_SIMPLE_CONSUMER(simpleConsumer,Antenna::MountDataBlock,Antenna::MOUNT_DATA_CHANNEL,NotificationHandler,static_cast<void*>(onSource_box));
		simpleConsumer->consumerReady();
		ACS_LOG(LM_FULL_INFO,"MedicinaMountTextClient::Main()",(LM_INFO,"MedicinaMountTextClient::DONE"));*/
				
		// Adding all the static labels....
		_TW_ADD_LABEL("Errors",32,0,6,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Mode  ",42,0,6,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Rates ",60,0,6,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Azimuth        :",0,1,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Elevation      :",0,2,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Offsets        :",0,3,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Commanded      :",0,4,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);	
		_TW_ADD_LABEL("Time           :",0,5,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("DUT1           :",42,5,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);		
		//_TW_ADD_LABEL("Tracking       :",0,6,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Az El",18,7,5,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Servo",42,8,5,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Compnt status :",24,7,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Emergency Limit ",0,8,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Op.Limit ccw/dwn",0,9,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Op.Limit cw/up  ",0,10,16,1,CColorPair::WHITE_BLACK,0,window);		
		_TW_ADD_LABEL("Prelimit ccw/dwn",0,11,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Prelimit cw/up  ",0,12,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Stow position   ",0,13,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Stow pin ins.   ",0,14,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Stow pin retr.  ",0,15,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Servo failure   ",0,16,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Brake failure   ",0,17,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Encoder failure ",0,18,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Motion failure  ",0,19,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Motor overspeed ",0,20,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Auxiliary mode  ",0,21,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Axis disabled   ",0,22,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Comp. disabled  ",0,23,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Door Interlock  ",24,9,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Safe            ",24,10,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Emergency       ",24,11,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Transfer error  ",24,12,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Time error      ",24,13,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("RF inhibit      ",24,14,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Remote          ",24,15,16,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Free ttpt pos. :",24,17,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Antenna sector :",24,18,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Mount status   :",24,19,16,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		
	}
	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"mountTui::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto ErrorLabel;
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"mountTui::Main()");
		IRA::CString Message;
		impl.log();
		goto ErrorLabel;
	}
	/** now it is possible to show the frame */
	ACS_LOG(LM_FULL_INFO,"mountTui::Main()",(LM_INFO,"START"));
	window.showFrame();
	while(!terminate_) {
		if ((fieldCounter=usrInput->parseCommand(fields,MAXFIELDNUMBER))>0) {
			fields[0].MakeUpper();
			if (fields[0]=="EXIT") terminate_=true;
		}
		client.run(tv);
		tv.set(0,MAINTHREADSLEEPTIME*1000);
	}
		
	window.closeFrame();
	
	ACS_LOG(LM_FULL_INFO,"mountTui::Main()",(LM_INFO,"RELEASING"));
	goto CloseLabel;
ErrorLabel:
	ACS_LOG(LM_FULL_INFO,"mountTui::Main()",(LM_INFO,"ABORTING"));
CloseLabel:
	//ACS_LOG(LM_FULL_INFO,"MedicinaMountTextClient::Main()",(LM_INFO,"MedicinaMountTextClient::FLUSHING_LOG_FILTER"));
	//_IRA_LOGFILTER_FLUSH;
	/*ACS_LOG(LM_SOURCE_INFO,"MedicinaMountTextClient::Main()",(LM_INFO,"MedicinaMountTextClient::NOTIFICATION_CHANNEL_LOGGING_OUT"));
	if (simpleConsumer!=NULL) simpleConsumer->disconnect(); 
	simpleConsumer=NULL;*/	
	window.Destroy();
	ACS_LOG(LM_FULL_INFO,"mountTui::Main()",(LM_INFO,"FRAME_CLOSED"));
	ACE_OS::sleep(1);
	/********************************************************************************/
	try {
		if (componentName!="") {
			client.releaseComponent((const char *)componentName);
		}
	}
	catch (maciErrType::CannotReleaseComponentExImpl& E) {
		E.log();
	}
	/*********************************************************************************/
	ACS_LOG(LM_FULL_INFO,"mountTui::Main()",(LM_INFO,"COMPONENT_RELEASED"));
	if (loggedIn) client.logout();
	ACS_LOG(LM_FULL_INFO,"mountTui::Main()",(LM_INFO,"SHUTTING_DOWN"));
	signal(SIGINT,SIG_DFL);
	ACE_OS::sleep(1);	
	return 0;
}
