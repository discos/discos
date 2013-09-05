#include "ReceiversBossTextClient.h"
#include "ReceiversDefinitionsC.h"

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

/*#define _GET_PROPERTY_VALUE_ONCE(TYPE,FORMAT,PROPERTY,PROPERTNAME,CONTROL) { \
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
}*/

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

#define TEMPLATE_4_ROTSYSTEMSTATUS  Management::ROTSystemStatus_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var

#include <map>

template <_TW_PROPERTYCOMPONENT_C>
class CCustomPropertyText : public  CPropertyComponent <_TW_PROPERTYCOMPONENT_TL> {
public:
	CCustomPropertyText(PR property): CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>(property) { }
	virtual ~CCustomPropertyText() {}
protected:
	WORD draw() {
		WORD width,height;
		bool done;
		CPoint newPosition;
		IRA::CString tmp;
		WORD iter,step;
		WORD newX,newY;
		IRA::CString value,orig,ret;
		std::map<std::string,int> data;

		int start=0;
		if (!CFrameComponent::getMainFrame()) return 4;
		if (!CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getEnabled()) return 1;
		if (!CFrameComponent::getMainFrame()->insideBorders(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition())) return 2;
		height=CFrameComponent::getEffectiveHeight();
		width=CFrameComponent::getEffectiveWidth();
		orig=CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getValue();
		while (IRA::CIRATools::getNextToken(orig,start,' ',ret)) {
			if ( (ret!=" ") && (ret!="") && (ret!="\n")) {
				data[(const char *)ret]+=1;
			}
		}
		value="";
		for (std::map<std::string,int>::iterator i=data.begin(); i!=data.end(); ++i) {
			IRA::CString temp;
			temp.Format("%s (x%d) ",i->first.c_str(),i->second);
			value+=temp;
		}
		step=value.GetLength()/width;
		newY=CFrameComponent::getHeightAlignment(CPropertyComponent <_TW_PROPERTYCOMPONENT_TL>::getHAlign(),step,height);
		iter=0;
		done=false;
		for (int i=0;i<height;i++) {
			if (!CFrameComponent::getMainFrame()->clearCanvas(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition()+CPoint(0,i),width,
			  CStyle(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle().getColorPair(),0))) return 3;
		}
		while ((newY<height) && (!done)) {
			if (step>0) {
				tmp=value.Mid(iter*width,width);
			}
			else {
				tmp=value;
				done=true;
			}
			iter++;
			newX=CFrameComponent::getWidthAlignment(CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getWAlign(),tmp.GetLength(),width);
			newPosition=CPoint(newX,newY);
			if (!CFrameComponent::getMainFrame()->writeCanvas(newPosition+CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getPosition(),CPropertyComponent<_TW_PROPERTYCOMPONENT_TL>::getStyle(),tmp)) return 3;
			newY++;
		}
		return 0;
	}
};


IRA::CString polarizationFormat(const baci::BACIValue& value,const void* arg)
{
	if (value.longValue()==Receivers::RCV_LCP)  {
		return IRA::CString("LCP    ");
	}
	else  if (value.longValue()==Receivers::RCV_RCP)  {
		return IRA::CString("RCP    ");
	}
	else  if (value.longValue()==Receivers::RCV_VLP)  {
		return IRA::CString("VLP    ");
	}
	else { // Receivers::RCV_HLP)
		return IRA::CString("HLP    ");
	}
}

IRA::CString IFFormat(const baci::BACIValue& value,const void* arg)
{
	long num=value.longValue();
	IRA::CString label("");
	for (long j=0;j<num;j++) {
		IRA::CString temp;
		temp.Format("IF%d     ",j);
		label+=temp;
	}
	return label;
}

int main(int argc, char *argv[]) {
	bool loggedIn=false;
	//int fieldCounter;
	maci::SimpleClient client;
	ACE_Time_Value tv(1);
	//IRA::CString fields[MAXFIELDNUMBER];
	IRA::CString inputCommand;
	maci::ComponentInfo_var info;
	CORBA::Object_var obj;
	// Component declaration 
	COMPONENT_DECLARATION component;
		

	/* Add frame controls declaration */
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_ROSTRING> *mode_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_ROSTRING> * actualSetup_field;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSYSTEMSTATUS,Management::TSystemStatus> * status_box;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)> *feeds_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)> *IFs_field;
	CCustomPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)> *polarization_text;
	CCustomPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *initialFrequency_text;
	CCustomPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *bandWidth_text;
	CCustomPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *LO_text;

	/* ******************************* */
	TW::CLabel *output_label;
	TW::CInputCommand *userInput;

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
		_GET_ACS_PROPERTY(ACS::ROlong,IFs);
		_GET_ACS_PROPERTY(ACS::ROlong,feeds);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,initialFrequency);
		_GET_ACS_PROPERTY(ACS::ROlongSeq,polarization);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,bandWidth)
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,LO);;
		_GET_ACS_PROPERTY(ACS::ROstring,mode);
		_GET_ACS_PROPERTY(ACS::ROstring,actualSetup);
		_GET_ACS_PROPERTY(Management::ROTSystemStatus,status);
		/* ********************************* */
		ACE_OS::sleep(1);
		/*ACSErr::Completion_var comp;
		IFnumber=IFs->get_sync(comp.out());*/
		/** Frame controls creation */
		actualSetup_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_ROSTRING>(actualSetup.in());
		mode_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_ROSTRING>(mode.in());
		status_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSYSTEMSTATUS,Management::TSystemStatus> (status.in(),Management::MNG_OK);
		feeds_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)>(feeds.in());
		IFs_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)>(IFs.in());
		initialFrequency_text=new CCustomPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(initialFrequency.in());
		bandWidth_text=new CCustomPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(bandWidth.in());
		LO_text=new CCustomPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(LO.in());
		polarization_text=new CCustomPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(long)>(polarization.in());
		/* ************************ */
		#if USE_OUTPUT_FIELD >=1 
			output_label=new TW::CLabel("");
		#else
			output_label=NULL;
		#endif
		userInput=new TW::CInputCommand();
		
		/** setting up the properties of the components of the frame controls */
		//actualsetup_field
		_TW_SET_COMPONENT(actualSetup_field,18,0,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		//mode_field
		_TW_SET_COMPONENT(mode_field,18,1,10,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		//status_box
		_TW_SET_COMPONENT(status_box,18,2,10,1,BLACK_GREEN,CStyle::BOLD,output_label);
		status_box->setStatusLook(Management::MNG_OK,CStyle(BLACK_GREEN,CStyle::BOLD));
		status_box->setStatusLook(Management::MNG_WARNING,CStyle(BLACK_YELLOW,CStyle::BOLD));
		status_box->setStatusLook(Management::MNG_FAILURE,CStyle(BLACK_RED,CStyle::BOLD));
		//feeds_field
		_TW_SET_COMPONENT(feeds_field,18,3,2,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		//IFs_field
		_TW_SET_COMPONENT(IFs_field,18,4,2,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		//_TW_SET_COMPONENT(IFs_field,18,4,WINDOW_WIDTH-18,1,CColorPair::WHITE_BLACK,0,output_label);
		//IFs_field->setFormatFunction(IFFormat,NULL);
		//LO_text
		_TW_SET_COMPONENT(LO_text,18,5,WINDOW_WIDTH-18,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		LO_text->setWAlign(CFrameComponent::LEFT);
		//LO_text->setHAlign(CFrameComponent::UP);
		LO_text->setFormatFunction(CFormatFunctions::floatingPointFormat,"%07.1lf");
		//initialFrequency_text
		_TW_SET_COMPONENT(initialFrequency_text,18,6,WINDOW_WIDTH-18,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		initialFrequency_text->setWAlign(CFrameComponent::LEFT);
		initialFrequency_text->setFormatFunction(CFormatFunctions::floatingPointFormat,"%07.1lf");
		//bandWidth_text
		_TW_SET_COMPONENT(bandWidth_text,18,7,WINDOW_WIDTH-18,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		bandWidth_text->setWAlign(CFrameComponent::LEFT);
		bandWidth_text->setFormatFunction(CFormatFunctions::floatingPointFormat,"%07.1lf");
		//polarization_text
		_TW_SET_COMPONENT(polarization_text,18,8,WINDOW_WIDTH-18,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		polarization_text->setWAlign(CFrameComponent::LEFT);
		polarization_text->setFormatFunction(polarizationFormat,NULL);
		//polarization_text->setFormatFunction(CFormatFunctions::floatingPointFormat,"%07.1lf");

		/* ****************************************************************** */
		_TW_SET_COMPONENT(userInput,0,WINDOW_HEIGHT-6,WINDOW_WIDTH-1,1,USER_INPUT_COLOR_PAIR,USER_INPUT_STYLE,NULL);
		#if USE_OUTPUT_FIELD >=1 
			_TW_SET_COMPONENT(output_label,0,WINDOW_HEIGHT-(OUTPUT_FIELD_HEIGHT+1),WINDOW_WIDTH-1,OUTPUT_FIELD_HEIGHT,OUTPUT_FIELD_COLOR_PAIR,OUTPUT_FIELD_STYLE,NULL);	
		#endif 
		
		// Monitors
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::MONITORS_INSTALLATION"));
		/** Add all required monitor installation here */
		_INSTALL_MONITOR(actualSetup_field,3000);
		_INSTALL_MONITOR(mode_field,3000);
		_INSTALL_MONITOR(status_box,3000);
		//status_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(LO_text,3000);
		//LO_text->setValueTrigger(0.0,true);
		_INSTALL_MONITOR(bandWidth_text,3000);
		//bandWidth_text->setValueTrigger(0.0,true);
		_INSTALL_MONITOR(initialFrequency_text,3000);
		//initialFrequency_text->setValueTrigger(0.0,true);
		_INSTALL_MONITOR(polarization_text,3000);
		//polarization_text->setValueTrigger(1L,true);
		_INSTALL_MONITOR(feeds_field,3000)
		_INSTALL_MONITOR(IFs_field,3000)
		//feeds_field->setValueTrigger(1L,true);.
		_INSTALL_MONITOR(polarization_text,3000)
		/* ****************************************** */
		ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::DONE"));
		
		/* Add all the static labels */
		_TW_ADD_LABEL("Current Setup   :",0,0,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Mode            :",0,1,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Status          :",0,2,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Feeds           :",0,3,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("IFs             :",0,4,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("LO              :",0,5,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Start Freq.     :",0,6,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Bandwidth       :",0,7,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Polarization    :",0,8,17,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		/* ************************* */
		
		/** Add all required association: components/Frame */
		window.addComponent((CFrameComponent*)actualSetup_field);
		window.addComponent((CFrameComponent*)mode_field);
		window.addComponent((CFrameComponent*)status_box);
		window.addComponent((CFrameComponent*)feeds_field);
		window.addComponent((CFrameComponent*)IFs_field);
		window.addComponent((CFrameComponent*)initialFrequency_text);
		window.addComponent((CFrameComponent*)bandWidth_text);
		window.addComponent((CFrameComponent*)LO_text);
		window.addComponent((CFrameComponent*)polarization_text);
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
		if (userInput->readCommand(inputCommand)) {
			if (inputCommand=="exit")  terminate=true;
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
		//sleep for the required ammount of time
		tv.set(0,MAINTHREADSLEEPTIME*1000);
		client.run(tv);
	}

	/*for(;;)	{
		if ((fieldCounter=userInput->parseCommand(fields,MAXFIELDNUMBER))>0) {
			fields[0].MakeUpper();
			if (fields[0]=="EXIT") break;
			else {
				#if USE_OUTPUT_FIELD >=1 
					IRA::CString Message("Unknown command");
					output_label->setValue(Message);
					output_label->Refresh();
				#endif
			}
		}
		client.run(tv);
		tv.set(0,MAINTHREADSLEEPTIME*1000);		
	}*/
		
	window.closeFrame();
	
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::RELEASING"));
	goto CloseLabel;
ErrorLabel:
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::ABORTING"));	
CloseLabel:
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
