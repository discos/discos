// $Id: BackendClient.cpp,v 1.4 2010-06-16 10:27:51 a.orlati Exp $

#include "BackendClient.h"
#include <ManagementErrors.h>

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

/*#define _CATCH_ALL(OUTPUT,ROUTINE,COMP_EXCEPTION) \
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
	catch (ComponentErrors::ComponentErrorsEx &E) { \
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
	}*/

#define COMPONENT_INTERFACE COMPONENT_IDL_MODULE::COMPONENT_IDL_INTERFACE
#define COMPONENT_DECLARATION COMPONENT_IDL_MODULE::COMPONENT_SMARTPOINTER

#define MAX_NUM_INPUTS 19
#define MAX_NUM_SECTIONS 19

#define TEMPLATE_4_ROTBOOLEAN  Management::ROTBoolean_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var

using namespace TW;

IRA::CString tsysFormat(const baci::BACIValue& value,const void* arg)
{
	IRA::CString formatted="";
	baci::BACIValue::Type currentType=value.getType();
	if ((currentType==baci::BACIValue::type_double)) {
		double app;
		app=value.doubleValue();
		if (app<0) formatted="??????";
		else if (app>999.0) formatted="??????";
		else {
			formatted.Format("%06.1lf",app);
		}
	}
	else {
		formatted="??????";
	}
	return formatted;	 
}

IRA::CString polarizationFormat(const baci::BACIValue& value,const void* arg)
{
	if (value.longValue()==Backends::BKND_LCP) {
		return IRA::CString("LEFT-");
	}
	else  if (value.longValue()==Backends::BKND_RCP) {
		return IRA::CString("RIGHT");
	}
	else {
		return IRA::CString("FULL-");
	}
}

int main(int argc, char *argv[]) {
	bool loggedIn=false;
	maci::SimpleClient client;
	ACE_Time_Value tv(1);
	char spaces6Float[20],spaces4Float[20],sNotation[20],spaces5Int[20],spaces2Int[20];
	char COMPONENT_NAME[256];
	IRA::CString appTitle(APPLICATION_TITLE);
	IRA::CString inputCommand;
	
	// Component declaration 
	COMPONENT_DECLARATION component=COMPONENT_INTERFACE::_nil();
		
	/* Add notification channels declaration */
	//nc::SimpleConsumer<MEDACU::MEDACUDataBlock> *simpleConsumer=NULL;
	/* ************************************* */

	/* Add frame controls declaration */
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)> *time_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)> *inputs_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)> *sections_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)> *integration_field;
	TW::CPropertyBitField<_TW_PROPERTYCOMPONENT_T_RO(pattern)> * status_bits;
	TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN> * busy_display;	
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *attenuation_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *sampleRate_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *bandWidth_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *frequency_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)> *inputSection_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *tsys_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)> *pol_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)> *bins_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)> *feed_text;
	
	/* ******************************* */
	TW::CLabel *output_label;
	TW::CInputCommand *userInput;

	// mainframe 
	TW::CFrame window(CPoint(0,0),CPoint(WINDOW_WIDTH,WINDOW_HEIGHT),'|','|','-','-'); 
	
	// disable ctrl+C
	signal(SIGINT,SIG_IGN);
	
	strcpy(spaces6Float,"%06.1lf");
	strcpy(spaces4Float,"%04.1lf");
	strcpy(sNotation,"%08.5le");
	strcpy(spaces5Int,"%05ld");
	strcpy(spaces2Int,"%02ld");
	
	if (argc<2) {
		printf("The name of the instance must be provided as command line argument\n");
		return -1;
	}
	strcpy(COMPONENT_NAME,argv[1]);
	
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

	appTitle+=": ";
	appTitle+=COMPONENT_NAME;
	window.setTitle(appTitle);
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
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::GET_COMPONENENT: %s",COMPONENT_NAME));
	try {
		component=client.getComponent<COMPONENT_INTERFACE>(COMPONENT_NAME,0,true);
		if (CORBA::is_nil(component.in())==true) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,MODULE_NAME"::Main()");
			impl.setComponentName(COMPONENT_NAME);
			impl.log();
			goto ErrorLabel;
		}
	}
	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,MODULE_NAME"::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto ErrorLabel;
	}
	catch (maciErrType::CannotGetComponentExImpl& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,MODULE_NAME"::Main()");
		impl.setComponentName(COMPONENT_NAME);
		impl.log();
		goto ErrorLabel;
	}	
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,MODULE_NAME"::Main()");
		impl.log();
		goto ErrorLabel;
	}		
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_DEBUG,"OK, reference is: %d",component.ptr()));
	ACE_OS::sleep(1);
	try {
		/* Add all component properties here */
		_GET_ACS_PROPERTY(ACS::ROuLongLong,time);
		_GET_ACS_PROPERTY(ACS::ROlong,inputsNumber);
		_GET_ACS_PROPERTY(ACS::ROlong,sectionsNumber);
		_GET_ACS_PROPERTY(ACS::ROlong,integration);
		_GET_ACS_PROPERTY(ACS::ROpattern,status);
		_GET_ACS_PROPERTY(Management::ROTBoolean,busy);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,attenuation);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,bandWidth);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,frequency);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,sampleRate);
		_GET_ACS_PROPERTY(ACS::ROlongSeq,polarization);
		_GET_ACS_PROPERTY(ACS::ROlongSeq,bins);
		_GET_ACS_PROPERTY(ACS::ROlongSeq,feed);
		_GET_ACS_PROPERTY(ACS::ROlongSeq,inputSection);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,systemTemperature)
		
		/* ********************************* */
		ACE_OS::sleep(1);
		
		/** Frame controls creation */		
		time_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)>(time.in());
		inputs_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)>(inputsNumber.in());
		sections_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)>(sectionsNumber.in());
		integration_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)>(integration.in());
		busy_display=new TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN>(busy.in(),1);
		status_bits=new TW::CPropertyBitField<_TW_PROPERTYCOMPONENT_T_RO(pattern)>(status.in());
		attenuation_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(attenuation.in());
		sampleRate_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(sampleRate.in());
		bandWidth_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(bandWidth.in());
		frequency_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(frequency.in());
		pol_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)>(polarization.in());
		bins_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)>(bins.in());
		tsys_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(systemTemperature.in());
		inputSection_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)>(inputSection.in());
		feed_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)>(feed.in());
		
		/* ************************ */
		#if USE_OUTPUT_FIELD >=1 
			output_label=new TW::CLabel("");
		#else
			output_label=NULL;
		#endif
		userInput=new TW::CInputCommand();
		
		/** setting up the properties of the components of the frame controls */
		_TW_SET_COMPONENT(time_field,11,0,22,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(inputs_field,63,1,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(sections_field,11,1,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(integration_field,48,0,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		
		_TW_SET_COMPONENT(frequency_text,5,3,7,MAX_NUM_SECTIONS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(bandWidth_text,13,3,7,MAX_NUM_SECTIONS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(feed_text,21,3,3,MAX_NUM_SECTIONS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);		
		_TW_SET_COMPONENT(sampleRate_text,26,3,12,MAX_NUM_SECTIONS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(pol_text,39,3,6,MAX_NUM_SECTIONS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(bins_text,45,3,6,MAX_NUM_SECTIONS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);

		_TW_SET_COMPONENT(status_bits,76,0,22,WINDOW_HEIGHT-7,CColorPair::WHITE_BLACK,CStyle::NORMAL,output_label);
				
		_TW_SET_COMPONENT(attenuation_text,56,3,5,MAX_NUM_INPUTS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(inputSection_text,62,3,3,MAX_NUM_INPUTS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		_TW_SET_COMPONENT(tsys_text,67,3,7,MAX_NUM_INPUTS,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		
		time_field->setFormatFunction(CFormatFunctions::dateTimeClockFormat,NULL);
		busy_display->setPosition(CPoint(68,0));
		busy_display->setOrientation(TW::CPropertyLedDisplay<TEMPLATE_4_ROTBOOLEAN>::HORIZONTAL);
		busy_display->setOutputComponent((CFrameComponent*) output_label);
		busy_display->setLedStyle(0,TW::CStyle(CColorPair::WHITE_BLACK,0),TW::CStyle(CColorPair::YELLOW_BLACK,0),'O','@');
		attenuation_text->setHAlign(CFrameComponent::UP);
		sampleRate_text->setHAlign(CFrameComponent::UP);
		frequency_text->setHAlign(CFrameComponent::UP);
		bandWidth_text->setHAlign(CFrameComponent::UP);
		pol_text->setHAlign(CFrameComponent::UP);
		bins_text->setHAlign(CFrameComponent::UP);
		inputSection_text->setHAlign(CFrameComponent::UP);
		tsys_text->setHAlign(CFrameComponent::UP);
		feed_text->setHAlign(CFrameComponent::UP);
		attenuation_text->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(spaces4Float));
		sampleRate_text->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(sNotation));
		frequency_text->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(spaces6Float));
		bandWidth_text->setFormatFunction(CFormatFunctions::floatingPointFormat,static_cast<const char*>(spaces6Float));
		inputSection_text->setFormatFunction(CFormatFunctions::integerFormat,static_cast<const char*>(spaces2Int));
		feed_text->setFormatFunction(CFormatFunctions::integerFormat,static_cast<const char*>(spaces2Int));		
		tsys_text->setFormatFunction(tsysFormat,NULL);
		pol_text->setFormatFunction(polarizationFormat,NULL);
		bins_text->setFormatFunction(CFormatFunctions::integerFormat,static_cast<const char*>(spaces5Int));
		
		
		/* ****************************************************************** */
		_TW_SET_COMPONENT(userInput,0,WINDOW_HEIGHT-6,WINDOW_WIDTH-1,1,USER_INPUT_COLOR_PAIR,USER_INPUT_STYLE,NULL);
		#if USE_OUTPUT_FIELD >=1 
			_TW_SET_COMPONENT(output_label,0,WINDOW_HEIGHT-(OUTPUT_FIELD_HEIGHT+1),WINDOW_WIDTH-1,OUTPUT_FIELD_HEIGHT,OUTPUT_FIELD_COLOR_PAIR,OUTPUT_FIELD_STYLE,NULL);	
		#endif 
		
		// Monitors
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::MONITORS_INSTALLATION"));
		/** Add all required monitor installation here */
		_INSTALL_MONITOR(time_field,200);
		_INSTALL_MONITOR(inputs_field,500);
		_INSTALL_MONITOR(sections_field,500);
		_INSTALL_MONITOR(integration_field,200);
		_INSTALL_MONITOR(status_bits,200);
		_INSTALL_MONITOR(attenuation_text,800);
		_INSTALL_MONITOR(sampleRate_text,800);
		_INSTALL_MONITOR(bandWidth_text,800);
		_INSTALL_MONITOR(frequency_text,800);
		_INSTALL_MONITOR(pol_text,800);
		_INSTALL_MONITOR(bins_text,800);
		_INSTALL_MONITOR(feed_text,800);
		_INSTALL_MONITOR(busy_display,3000);
		_INSTALL_MONITOR(inputSection_text,800);
		_INSTALL_MONITOR(tsys_text,800);
		busy_display->setValueTrigger(1L,true);
		
		/* ****************************************** */
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::DONE"));
		
		/* Add all the static labels */
		IRA::CString labelCaption;
		_TW_ADD_LABEL("Time     :",0,0,10,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Integration :",34,0,13,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Busy     :",57,0,10,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Sections :",0,1,10,1,CColorPair::BLUE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Inputs    :",52,1,10,1,CColorPair::BLUE_BLACK,CStyle::UNDERLINE,window);		
		for (int jj=0;jj<MAX_NUM_SECTIONS;jj++) {
			labelCaption.Format("S%02d",jj);
			_TW_ADD_LABEL((const char *)labelCaption,0,3+jj,15,1,CColorPair::BLUE_BLACK,CStyle::NORMAL,window);
		}
		for (int jj=0;jj<MAX_NUM_INPUTS;jj++) {
			labelCaption.Format("I%02d",jj);
			_TW_ADD_LABEL((const char *)labelCaption,52,3+jj,15,1,CColorPair::BLUE_BLACK,CStyle::NORMAL,window);
		}	
		_TW_ADD_LABEL("Freq",5,2,7,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("BW",13,2,7,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Feed",21,2,4,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("S.R.",26,2,12,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Pol",39,2,5,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Bins",45,2,6,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		
		_TW_ADD_LABEL("DBs",56,2,5,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Sect",62,2,5,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Tsys",67,2,5,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		//_TW_ADD_LABEL("Frequency",0,4,18,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);*/
		/* ************************* */
		
		/** Add all required association: components/Frame */
		window.addComponent((CFrameComponent*)time_field);
		window.addComponent((CFrameComponent*)inputs_field);
		window.addComponent((CFrameComponent*)sections_field);
		window.addComponent((CFrameComponent*)integration_field);
		window.addComponent((CFrameComponent*)status_bits);
		window.addComponent((CFrameComponent*)attenuation_text);
		window.addComponent((CFrameComponent*)sampleRate_text);
		window.addComponent((CFrameComponent*)bandWidth_text);
		window.addComponent((CFrameComponent*)frequency_text);
		window.addComponent((CFrameComponent*)pol_text);
		window.addComponent((CFrameComponent*)bins_text);
		window.addComponent((CFrameComponent*)busy_display);
		window.addComponent((CFrameComponent*)tsys_text);
		window.addComponent((CFrameComponent*)inputSection_text);
		window.addComponent((CFrameComponent*)feed_text);
		
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
	for(;;)	{
		if (userInput->readCommand(inputCommand)) {
			if (inputCommand=="exit") break;
			if (component->_is_a("IDL:alma/Management/CommandInterpreter:1.0")) {
				try {
					IRA::CString outputAnswer=component->command((const char *)inputCommand);
					output_label->setValue(outputAnswer);
					output_label->Refresh();
				}
				catch (ManagementErrors::CommandLineErrorEx& ex) {
					ManagementErrors::CommandLineErrorExImpl impl(ex);
					IRA::CString Message;
					Message=impl.getErrorMessage();
					output_label->setValue(Message);
					output_label->Refresh();
					impl.log(LM_ERROR);
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
					impl.setReason("comunication error to component server");
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
		client.releaseComponent(COMPONENT_NAME);
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
