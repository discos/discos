// $Id: SRTMountTextClient.cpp,v 1.5 2011-06-01 18:25:35 a.orlati Exp $

#include "SRTMountTextClient.h"

#define _GET_ACS_PROPERTY(TYPE,NAME) TYPE##_var NAME; \
{	\
	ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"Trying to get property "#NAME"...")); \
	NAME=acu->NAME(); \
	if (NAME.ptr()!=TYPE::_nil()) { \
		ACS_LOG(LM_FULL_INFO,"::Main()",(LM_DEBUG,"OK, reference is: %x",NAME.ptr())); \
	} \
	else { \
		_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"::Main()"); \
		impl.setPropertyName(#NAME); \
		impl.log(); \
		goto ErrorLabel; \
	} \
}

#define _INSTALL_MONITOR(COMP,TRIGGERTIME) { \
	if (!COMP->installAutomaticMonitor(GUARDINTERVAL)) { \
		_EXCPT(ClientErrors::CouldntPerformActionExImpl,impl,"::Main()"); \
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

#define TEMPLATE_4_ROTSTATUS Antenna::ROTStatus_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTMODES Antenna::ROTCommonModes_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTSECTIONS Antenna::ROTSections_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var
#define TEMPLATE_4_ROTSYSTEMSTATUS Management::ROTSystemStatus_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var

#define COMPONENT_INTERFACE_TYPE "IDL:alma/Antenna/SRTMount:1.0"  /* the type of the interface */


void CMotorsStatusCallback::working (const ACS::longSeq& value,const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
	ACSErr::CompletionImpl comp(c);
	if (!comp.isErrorFree()) {
		_IRA_LOGGUARD_LOG_COMPLETION(m_guard,comp,LM_ERROR);
	}
	else {
		for (WORD j=0;j<value.length();j++) {
			char val[64];
			sprintf(val,"%d",value[j]);
			m_control[j]->setValue(IRA::CString(val));
			m_control[j]->Refresh();
		}
	}
}

void CMotorsStatusCallback::done (const ACS::longSeq& vlaue,const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
	ACSErr::CompletionImpl comp(c);
	if (!comp.isErrorFree()) {
		_IRA_LOGGUARD_LOG_COMPLETION(m_guard,comp,LM_ERROR);
	}
}

using namespace TW;

int main(int argc, char *argv[]) {
	bool loggedIn=false;
	int fieldCounter;
	maci::SimpleClient client;
	ACE_Time_Value tv(1);
	IRA::CString fields[MAXFIELDNUMBER];
	
	IRA::CString componentName="";
	//char formatString[20];
	maci::ComponentInfo_var info;
	CORBA::Object_var obj;
	
	ACS::ConditionSeq motorsStatusSet,motorsStatusCleared;
	ACS::stringSeq motorsStatusDescription;
	
	CMotorsStatusCallback motorsStatusCallbackObject;
	ACS::CBlongSeq_var motorsStatusCB;
	ACS::Monitorlong_var motorsStatusMonitor=ACS::Monitorlong::_nil();
	ACS::CBDescIn desc;
	
	// Component declaration 
	COMPONENT_DECLARATION acu;
	
	/* Add frame controls declaration */
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSTATUS,Antenna::TStatus>* controlLineStatus_box;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSTATUS,Antenna::TStatus>* statusLineStatus_box;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *elevation_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *azimuth_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *azimuthError_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *elevationError_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *azimuthTrackError_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *elevationTrackError_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>* elevationRate_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>* azimuthRate_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)> *time_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>* deltaTime_field;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTMODES,Antenna::TCommonModes>* azimuthMode_box;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTMODES,Antenna::TCommonModes>* elevationMode_box;	
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *commandedAzimuth_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *commandedElevation_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *azimuthOffset_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *elevationOffset_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *commandedAzimuthOffset_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *commandedAzimuthRate_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *commandedElevationOffset_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *commandedElevationRate_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *cableWrapPosition_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *cableWrapTrackingError_field;
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)> *cableWrapRate_field;
	TW::CPropertyBitField<_TW_PROPERTYCOMPONENT_T_RO(pattern)> * generalStatus_bits;
	TW::CPropertyBitField<_TW_PROPERTYCOMPONENT_T_RO(pattern)> * azimuthStatus_bits;
	TW::CPropertyBitField<_TW_PROPERTYCOMPONENT_T_RO(pattern)> * elevationStatus_bits;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSECTIONS,Antenna::TSections>* section_box;
	TW::CPropertyStatusBox<TEMPLATE_4_ROTSYSTEMSTATUS,Management::TSystemStatus>* mountStatus_box;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *motorsPosition_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *motorsSpeed_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *motorsTorque_text;
	TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)> *motorsUtilization_text;
	TW::CLedDisplay *motorsStatus_led[MOTORS_NUMBER];
	TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)> * freepTTPos_field;
	
	TW::CLabel *output_label;
	TW::CInputCommand *userInput;

	// mainframe 
	TW::CFrame window(CPoint(0,0),CPoint(WINDOW_WIDTH,WINDOW_HEIGHT),'|','|','-','-'); 
	
	// disable ctrl+C
	signal(SIGINT,SIG_IGN);
	
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
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::GET_COMPONENENT: %s",COMPONENT_INTERFACE_TYPE));
	try {
		info=client.manager()->get_default_component(client.handle(),COMPONENT_INTERFACE_TYPE);
		obj=info->reference;
		if (CORBA::is_nil(obj.in())) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,MODULE_NAME"::Main()");
			impl.setComponentName(COMPONENT_INTERFACE_TYPE);
			impl.log();
			goto ErrorLabel;
		}
	}
	catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"::Main()");
		impl.setComponentName(COMPONENT_INTERFACE_TYPE);
		impl.log();
		goto ErrorLabel;		
	}
	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto ErrorLabel;
	}
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::Main()");
		impl.log();
		goto ErrorLabel;
	}
	componentName=info->name;
	acu=Antenna::SRTMount::_narrow(obj.in());
	
	ACS_LOG(LM_SOURCE_INFO,"Main()",(LM_DEBUG,"OK, reference is: %d",acu.ptr()));
	ACE_OS::sleep(1);
	
	try {
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuth);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevation);
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuthError);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevationError);
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuthTrackingError);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevationTrackingError);
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuthOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevationOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,commandedAzimuth);
		_GET_ACS_PROPERTY(ACS::ROdouble,commandedElevation);
		_GET_ACS_PROPERTY(ACS::ROuLongLong,time);
		_GET_ACS_PROPERTY(ACS::ROdouble,deltaTime);
		_GET_ACS_PROPERTY(Antenna::ROTCommonModes,azimuthMode);
		_GET_ACS_PROPERTY(Antenna::ROTCommonModes,elevationMode);
		_GET_ACS_PROPERTY(ACS::ROdouble,elevationRate);
		_GET_ACS_PROPERTY(ACS::ROdouble,azimuthRate);
		_GET_ACS_PROPERTY(Antenna::ROTStatus,controlLineStatus);
		_GET_ACS_PROPERTY(Antenna::ROTStatus,statusLineStatus);
		_GET_ACS_PROPERTY(Antenna::ROTSections,section);
		_GET_ACS_PROPERTY(ACS::ROdouble,commandedAzimuthRate);
		_GET_ACS_PROPERTY(ACS::ROdouble,commandedElevationRate);
		_GET_ACS_PROPERTY(ACS::ROdouble,commandedAzimuthOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,commandedElevationOffset);
		_GET_ACS_PROPERTY(ACS::ROdouble,cableWrapPosition);
		_GET_ACS_PROPERTY(ACS::ROdouble,cableWrapTrackingError);
		_GET_ACS_PROPERTY(ACS::ROdouble,cableWrapRate);
		_GET_ACS_PROPERTY(ACS::ROpattern,generalStatus);
		_GET_ACS_PROPERTY(ACS::ROpattern,azimuthStatus);
		_GET_ACS_PROPERTY(ACS::ROpattern,elevationStatus);
		_GET_ACS_PROPERTY(Management::ROTSystemStatus,mountStatus);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,motorsPosition);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,motorsSpeed);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,motorsTorque);
		_GET_ACS_PROPERTY(ACS::ROdoubleSeq,motorsUtilization);
		_GET_ACS_PROPERTY(ACS::ROlongSeq,motorsStatus);
		_GET_ACS_PROPERTY(ACS::ROlong,programTrackPositions);
		
		
		// get some more characteristics from the motorsStatus property
		try {
			const char *val;
			CORBA::Any_var anyVar=motorsStatus->get_characteristic_by_name("whenSet");
			if (!(*anyVar>>= val)) {
				_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"::Main()");
				impl.setPropertyName("motorsStatus/whenSet");
				impl.log();
				goto ErrorLabel;
			}
			else {
				CORBA::String_var tmpStr;
				// need to copy the string into another buffer since we need to change it and the memory is owned by the anyVar
				tmpStr=CORBA::string_dup(val);
				ACE_Tokenizer tok(tmpStr);
				tok.delimiter_replace(',',0);
				WORD len=0;
				long lng;
				for (char * p=tok.next();p;p=tok.next()) {	
					if (sscanf(p,"%ld",&lng)!=1) {
						_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"::Main()");
						impl.setPropertyName("motorsStatus/whenSet");
						impl.log();
						goto ErrorLabel;
					}
					motorsStatusSet.length(len+1);
					motorsStatusSet[len]=(ACS::Condition)lng;
					len++; 
				}
			}
			anyVar=motorsStatus->get_characteristic_by_name("whenCleared");
			if (!(*anyVar>>= val)) {
				_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"::Main()");
				impl.setPropertyName("motorsStatus/whenCleared");
				impl.log();
				goto ErrorLabel;
			}
			else {
				CORBA::String_var tmpStr;
				// need to copy the string into another buffer since we need to change it and the memory is owned by the anyVar
				tmpStr=CORBA::string_dup(val);
				ACE_Tokenizer tok(tmpStr);
				tok.delimiter_replace(',',0);
				WORD len=0;
				long lng;
				for (char * p=tok.next();p;p=tok.next()) {	
					if (sscanf(p,"%ld",&lng)!=1) {
						_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"::Main()");
						impl.setPropertyName("motorsStatus/whenCleared");
						impl.log();
						goto ErrorLabel;
					}
					motorsStatusCleared.length(len+1);
					motorsStatusCleared[len]=(ACS::Condition)lng;
					len++; 
				}
			}
			anyVar=motorsStatus->get_characteristic_by_name("bitDescription");
			if (!(*anyVar>>= val)) {
				_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"::Main()");
				impl.setPropertyName("motorsStatus/bitDescription");
				impl.log();
				goto ErrorLabel;
			}
			else {
				CORBA::String_var tmpStr;
				// need to copy the string into another buffer since we need to change it and the memory is owned by the anyVar
				tmpStr=CORBA::string_dup(val);
				ACE_Tokenizer tok(tmpStr);
				tok.delimiter_replace(',',0);
				WORD len=0;
				for (char * p=tok.next();p;p=tok.next()) {	
					motorsStatusDescription.length(len+1);
					motorsStatusDescription[len]=CORBA::string_dup(p);
					len++; 
				}
			}			
		}
		catch (...) {
			_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"::Main()");
			impl.setPropertyName("motorsStatus");
			impl.log();
			goto ErrorLabel;
		}
		
		ACE_OS::sleep(1);
		
		/** Frame controls creation */	
		controlLineStatus_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSTATUS,Antenna::TStatus>(controlLineStatus.in(),Antenna::ACU_NOTCNTD);	
		statusLineStatus_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSTATUS,Antenna::TStatus>(statusLineStatus.in(),Antenna::ACU_NOTCNTD);
		elevation_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevation.in());
		azimuth_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuth.in());
		azimuthError_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuthError.in());
		elevationError_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevationError.in());
		azimuthTrackError_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuthTrackingError.in());
		elevationTrackError_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevationTrackingError.in());
		elevationRate_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevationRate.in());
		azimuthRate_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuthRate.in());
		azimuthMode_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTMODES,Antenna::TCommonModes>(azimuthMode.in(),Antenna::ACU_STANDBY);
		elevationMode_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTMODES,Antenna::TCommonModes>(elevationMode.in(),Antenna::ACU_STANDBY);	
		commandedAzimuth_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(commandedAzimuth.in());
		commandedElevation_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(commandedElevation.in());
		azimuthOffset_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(azimuthOffset.in());
		elevationOffset_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(elevationOffset.in());
		time_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(uLongLong)>(time.in());
		deltaTime_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(deltaTime.in());
		section_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSECTIONS,Antenna::TSections>(section.in(),Antenna::ACU_CW);
		commandedAzimuthOffset_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(commandedAzimuthOffset.in());
		commandedAzimuthRate_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(commandedAzimuthRate.in());
		commandedElevationOffset_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(commandedElevationOffset.in());
		commandedElevationRate_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(commandedElevationRate.in());
		cableWrapPosition_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(cableWrapPosition.in());
		cableWrapTrackingError_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(cableWrapTrackingError.in());
		cableWrapRate_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(double)>(cableWrapRate.in());
		generalStatus_bits=new TW::CPropertyBitField<_TW_PROPERTYCOMPONENT_T_RO(pattern)>(generalStatus.in());
		azimuthStatus_bits=new TW::CPropertyBitField<_TW_PROPERTYCOMPONENT_T_RO(pattern)>(azimuthStatus.in());
		elevationStatus_bits=new TW::CPropertyBitField<_TW_PROPERTYCOMPONENT_T_RO(pattern)>(elevationStatus.in());
		mountStatus_box=new TW::CPropertyStatusBox<TEMPLATE_4_ROTSYSTEMSTATUS,Management::TSystemStatus>(mountStatus.in(),Management::MNG_OK);
		motorsPosition_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(motorsPosition.in());
		motorsSpeed_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(motorsSpeed.in());
		motorsTorque_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(motorsTorque.in());
		motorsUtilization_text=new TW::CPropertyText<_TW_SEQPROPERTYCOMPONENT_T_RO(double)>(motorsUtilization.in());
		for (WORD j=0;j<MOTORS_NUMBER;j++) {
			motorsStatus_led[j]=new TW::CLedDisplay(motorsStatusSet.length());
		}
		freepTTPos_field=new TW::CPropertyText<_TW_PROPERTYCOMPONENT_T_RO(long)>(programTrackPositions.in());
		
		
		#if USE_OUTPUT_FIELD >=1 
			output_label=new TW::CLabel("");
		#else
			output_label=NULL;
		#endif
		userInput=new TW::CInputCommand();

		// azimuth_field
		_TW_SET_COMPONENT(azimuth_field,10,1,9,1,CColorPair::BLUE_BLACK,CStyle::BOLD,output_label);
		azimuth_field->setWAlign(CFrameComponent::RIGHT);
		azimuth_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.4lf");	
		// elevation_field
		_TW_SET_COMPONENT(elevation_field,10,3,9,1,CColorPair::BLUE_BLACK,CStyle::BOLD,output_label);	
		elevation_field->setWAlign(CFrameComponent::RIGHT);
		elevation_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.4lf");
		//azimuthRate_field
		_TW_SET_COMPONENT(azimuthRate_field,20,1,7,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);	
		azimuthRate_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+07.4lf");
		//elevationRate_field
		_TW_SET_COMPONENT(elevationRate_field,20,3,7,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		elevationRate_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+07.4lf");
		// azimuthOffset_field
		_TW_SET_COMPONENT(azimuthOffset_field,28,1,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);	
		azimuthOffset_field->setWAlign(CFrameComponent::RIGHT);
		azimuthOffset_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+08.4lf");
		// elevationOffset_field
		_TW_SET_COMPONENT(elevationOffset_field,28,3,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);	
		elevationOffset_field->setWAlign(CFrameComponent::RIGHT);
		elevationOffset_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+08.4lf");		
		// azimuthError_field
		_TW_SET_COMPONENT(azimuthError_field,37,1,9,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		azimuthError_field->setWAlign(CFrameComponent::RIGHT);
		azimuthError_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.4lf");
		// elevationError_field
		_TW_SET_COMPONENT(elevationError_field,37,3,9,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);	
		elevationError_field->setWAlign(CFrameComponent::RIGHT);
		elevationError_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.4lf");
		// azimuthTrackError_field
		_TW_SET_COMPONENT(azimuthTrackError_field,47,1,9,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		azimuthTrackError_field->setWAlign(CFrameComponent::RIGHT);
		azimuthTrackError_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.6lf");
		// elevationTrackError_field
		_TW_SET_COMPONENT(elevationTrackError_field,47,3,9,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);	
		elevationTrackError_field->setWAlign(CFrameComponent::RIGHT);
		elevationTrackError_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.6lf");	
		//azimuthMode_box
		_TW_SET_COMPONENT(azimuthMode_box,57,1,15,1,BLACK_WHITE,CStyle::BOLD,output_label);
		azimuthMode_box->setStatusLook(Antenna::ACU_STANDBY,CStyle(BLACK_BLUE,CStyle::BOLD)); 
		azimuthMode_box->setStatusLook(Antenna::ACU_STOP,CStyle(BLACK_MAGENTA,CStyle::BOLD)); 
		azimuthMode_box->setStatusLook(Antenna::ACU_PRESET); 
		azimuthMode_box->setStatusLook(Antenna::ACU_PROGRAMTRACK); 
		azimuthMode_box->setStatusLook(Antenna::ACU_RATE); 
		azimuthMode_box->setStatusLook(Antenna::ACU_STOW,CStyle(BLACK_GREEN,CStyle::BOLD));
		azimuthMode_box->setStatusLook(Antenna::ACU_UNSTOW,CStyle(BLACK_GREEN,CStyle::BOLD)); 
		azimuthMode_box->setStatusLook(Antenna::ACU_UNKNOWN,CStyle(BLACK_RED,CStyle::BOLD)); 
		//elevationMode_box
		_TW_SET_COMPONENT(elevationMode_box,57,3,15,1,BLACK_WHITE,CStyle::BOLD,output_label);
		elevationMode_box->setStatusLook(Antenna::ACU_STANDBY,CStyle(BLACK_BLUE,CStyle::BOLD));
		elevationMode_box->setStatusLook(Antenna::ACU_STOP,CStyle(BLACK_MAGENTA,CStyle::BOLD));
		elevationMode_box->setStatusLook(Antenna::ACU_PRESET);
		elevationMode_box->setStatusLook(Antenna::ACU_PROGRAMTRACK);
		elevationMode_box->setStatusLook(Antenna::ACU_RATE);
		elevationMode_box->setStatusLook(Antenna::ACU_STOW,CStyle(BLACK_GREEN,CStyle::BOLD));
		elevationMode_box->setStatusLook(Antenna::ACU_UNSTOW,CStyle(BLACK_GREEN,CStyle::BOLD));
		elevationMode_box->setStatusLook(Antenna::ACU_UNKNOWN,CStyle(BLACK_RED,CStyle::BOLD));
		//commandedAzimuth_field
		_TW_SET_COMPONENT(commandedAzimuth_field,10,2,9,1,CColorPair::WHITE_BLACK,0,output_label);	
		commandedAzimuth_field->setWAlign(CFrameComponent::RIGHT);
		commandedAzimuth_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.4lf");		
		//commandedElevation_field
		_TW_SET_COMPONENT(commandedElevation_field,10,4,9,1,CColorPair::WHITE_BLACK,0,output_label);	
		commandedElevation_field->setWAlign(CFrameComponent::RIGHT);
		commandedElevation_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.4lf");
		//commandedAzimuthRate_field
		_TW_SET_COMPONENT(commandedAzimuthRate_field,20,2,7,1,CColorPair::WHITE_BLACK,0,output_label);	
		commandedAzimuthRate_field->setWAlign(CFrameComponent::RIGHT);
		commandedAzimuthRate_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+07.4lf");		
		//commandedElevationRate_field
		_TW_SET_COMPONENT(commandedElevationRate_field,20,4,7,1,CColorPair::WHITE_BLACK,0,output_label);	
		commandedElevationRate_field->setWAlign(CFrameComponent::RIGHT);
		commandedElevationRate_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+07.4lf");
		//commandedAzimuthOffset_field
		_TW_SET_COMPONENT(commandedAzimuthOffset_field,28,2,8,1,CColorPair::WHITE_BLACK,0,output_label);	
		commandedAzimuthOffset_field->setWAlign(CFrameComponent::RIGHT);
		commandedAzimuthOffset_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+08.4lf");		
		//commandedElevationOffset_field
		_TW_SET_COMPONENT(commandedElevationOffset_field,28,4,8,1,CColorPair::WHITE_BLACK,0,output_label);	
		commandedElevationOffset_field->setWAlign(CFrameComponent::RIGHT);
		commandedElevationOffset_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+08.4lf");
		// cableWrapPosition_field
		_TW_SET_COMPONENT(cableWrapPosition_field,10,5,9,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		cableWrapPosition_field->setWAlign(CFrameComponent::RIGHT);
		cableWrapPosition_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.4lf");
		//cableWrapRate_field
		_TW_SET_COMPONENT(cableWrapRate_field,20,5,7,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		cableWrapRate_field->setWAlign(CFrameComponent::RIGHT);
		cableWrapRate_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+07.4lf");
		// cableWrapTrackingError_field
		_TW_SET_COMPONENT(cableWrapTrackingError_field,47,5,9,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		cableWrapTrackingError_field->setWAlign(CFrameComponent::RIGHT);
		cableWrapTrackingError_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+09.6lf");
		//time_field
		_TW_SET_COMPONENT(time_field,10,7,21,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		time_field->setFormatFunction(CFormatFunctions::dateTimeClockFormat,NULL);
		// deltaTime_field
		_TW_SET_COMPONENT(deltaTime_field,38,7,8,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		deltaTime_field->setFormatFunction(CFormatFunctions::floatingPointFormat,"%08.5lf");
		// section_box
		_TW_SET_COMPONENT(section_box,57,5,8,1,BLACK_WHITE,CStyle::BOLD,output_label);
		section_box->setStatusLook(Antenna::ACU_CW);
		section_box->setStatusLook(Antenna::ACU_CCW);
		// controlLineStatus_box
		_TW_SET_COMPONENT(controlLineStatus_box,52,9,15,1,BLACK_WHITE,CStyle::BOLD,output_label);
		controlLineStatus_box->setStatusLook(Antenna::ACU_NOTCNTD,CStyle(BLACK_RED,CStyle::BOLD));
		controlLineStatus_box->setStatusLook(Antenna::ACU_CNTDING,CStyle(BLACK_YELLOW,CStyle::BOLD));
		controlLineStatus_box->setStatusLook(Antenna::ACU_CNTD,CStyle(BLACK_GREEN,CStyle::BOLD));
		controlLineStatus_box->setStatusLook(Antenna::ACU_BSY);
		// statusLineStatus_box
		_TW_SET_COMPONENT(statusLineStatus_box,52,10,15,1,BLACK_WHITE,CStyle::BOLD,output_label);
		statusLineStatus_box->setStatusLook(Antenna::ACU_NOTCNTD,CStyle(BLACK_RED,CStyle::BOLD));
		statusLineStatus_box->setStatusLook(Antenna::ACU_CNTDING,CStyle(BLACK_YELLOW,CStyle::BOLD));
		statusLineStatus_box->setStatusLook(Antenna::ACU_CNTD,CStyle(BLACK_GREEN,CStyle::BOLD));
		statusLineStatus_box->setStatusLook(Antenna::ACU_BSY);
		/// generalStatus_bits
		_TW_SET_COMPONENT(generalStatus_bits,73,0,22,9,CColorPair::WHITE_BLACK,CStyle::NORMAL,output_label);
		/// azimuthStatus_bits
		_TW_SET_COMPONENT(azimuthStatus_bits,0,9,22,13,CColorPair::WHITE_BLACK,CStyle::NORMAL,output_label);
		/// elevationStatus_bits
		_TW_SET_COMPONENT(elevationStatus_bits,30,9,1,13,CColorPair::WHITE_BLACK,CStyle::NORMAL,output_label);
		elevationStatus_bits->showDescriptionLabels(false);
		//mountStatus_box
		_TW_SET_COMPONENT(mountStatus_box,52,11,15,1,BLACK_WHITE,CStyle::BOLD,output_label);
		mountStatus_box->setStatusLook(Management::MNG_OK,CStyle(BLACK_GREEN,CStyle::BOLD));
		mountStatus_box->setStatusLook(Management::MNG_WARNING,CStyle(BLACK_YELLOW,CStyle::BOLD));
		mountStatus_box->setStatusLook(Management::MNG_FAILURE,CStyle(BLACK_RED,CStyle::BOLD));	
		//motorsPositions_text
		_TW_SET_COMPONENT(motorsPosition_text,41,15,9,MOTORS_NUMBER,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		motorsPosition_text->setHAlign(CFrameComponent::UP);
		motorsPosition_text->setWAlign(CFrameComponent::LEFT);
		motorsPosition_text->setFormatFunction(CFormatFunctions::floatingPointFormat,"%08.2lf");
		//motorsSpeed_text
		_TW_SET_COMPONENT(motorsSpeed_text,51,15,9,MOTORS_NUMBER,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		motorsSpeed_text->setHAlign(CFrameComponent::UP);
		motorsSpeed_text->setWAlign(CFrameComponent::LEFT);
		motorsSpeed_text->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+08.2lf");
		//motorsTorque_text
		_TW_SET_COMPONENT(motorsTorque_text,61,15,7,MOTORS_NUMBER,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		motorsTorque_text->setHAlign(CFrameComponent::UP);
		motorsTorque_text->setWAlign(CFrameComponent::LEFT);
		motorsTorque_text->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+06.3lf");
		// motorsUtilization_text
		_TW_SET_COMPONENT(motorsUtilization_text,69,15,7,MOTORS_NUMBER,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		motorsUtilization_text->setHAlign(CFrameComponent::UP);
		motorsUtilization_text->setWAlign(CFrameComponent::LEFT);
		motorsUtilization_text->setFormatFunction(CFormatFunctions::floatingPointFormat,"%+06.1lf");
		// motorsStatus_led
		for (WORD j=0;j<MOTORS_NUMBER;j++) {
			TW::CPoint pos(77,15+j);
			motorsStatus_led[j]->setPosition(pos);
			motorsStatus_led[j]->setOrientation(TW::CLedDisplay::VERTICAL);
		}
		for (WORD i=0;i<motorsStatusSet.length();i++) {
			TW::CStyle setStyle;
			TW::CStyle clearedStyle;
			if (motorsStatusSet[i]==ACS::RED) {
				setStyle.setColorPair(CColorPair::RED_BLACK);
			}
			else if (motorsStatusSet[i]==ACS::YELLOW) {
				setStyle.setColorPair(CColorPair::YELLOW_BLACK);
			}
			else if (motorsStatusSet[i]==ACS::GREEN) {
				setStyle.setColorPair(CColorPair::GREEN_BLACK);
			}
			else if (motorsStatusSet[i]==ACS::GREY) {
				setStyle.setColorPair(CColorPair::WHITE_BLACK);
			}				
			if (motorsStatusCleared[i]==ACS::RED) {
				clearedStyle.setColorPair(CColorPair::RED_BLACK);
			}
			else if (motorsStatusCleared[i]==ACS::YELLOW) {
				clearedStyle.setColorPair(CColorPair::YELLOW_BLACK);
			}
			else if (motorsStatusCleared[i]==ACS::GREEN) {
				clearedStyle.setColorPair(CColorPair::GREEN_BLACK);
			}
			else if (motorsStatusCleared[i]==ACS::GREY) {
				clearedStyle.setColorPair(CColorPair::WHITE_BLACK);
			}				
			for (WORD j=0;j<MOTORS_NUMBER;j++) {
				motorsStatus_led[j]->setLedStyle(i,setStyle,clearedStyle,'o','o');
				
			}			
		}
		//freepTTPos_field
		_TW_SET_COMPONENT(freepTTPos_field,52,12,4,1,CColorPair::WHITE_BLACK,CStyle::BOLD,output_label);
		freepTTPos_field->setWAlign(CFrameComponent::LEFT);

		_TW_SET_COMPONENT(userInput,0,WINDOW_HEIGHT-6,WINDOW_WIDTH-1,1,USER_INPUT_COLOR_PAIR,USER_INPUT_STYLE,NULL);
		#if USE_OUTPUT_FIELD >=1 
			_TW_SET_COMPONENT(output_label,0,WINDOW_HEIGHT-(OUTPUT_FIELD_HEIGHT+1),WINDOW_WIDTH-1,OUTPUT_FIELD_HEIGHT,OUTPUT_FIELD_COLOR_PAIR,OUTPUT_FIELD_STYLE,NULL);	
		#endif 
		
		// Monitors
		ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"MONITORS_INSTALLATION"));
		
		_INSTALL_MONITOR(azimuth_field,250);
		_INSTALL_MONITOR(elevation_field,250);
		_INSTALL_MONITOR(azimuthError_field,250);
		_INSTALL_MONITOR(elevationError_field,250);
		_INSTALL_MONITOR(azimuthTrackError_field,250);
		_INSTALL_MONITOR(elevationTrackError_field,250);
		_INSTALL_MONITOR(time_field,200);
		_INSTALL_MONITOR(deltaTime_field,1000);
		_INSTALL_MONITOR(elevationOffset_field,1000);
		elevationOffset_field->setValueTrigger(0.0,true);
		_INSTALL_MONITOR(azimuthOffset_field,1000);
		azimuthOffset_field->setValueTrigger(0.0,true);		
		_INSTALL_MONITOR(commandedAzimuth_field,250);
		_INSTALL_MONITOR(commandedElevation_field,250);
		_INSTALL_MONITOR(azimuthMode_box,1000);
		azimuthMode_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(elevationMode_box,1000);
		elevationMode_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(elevationRate_field,250);
		_INSTALL_MONITOR(azimuthRate_field,250);
		_INSTALL_MONITOR(controlLineStatus_box,1000);
		controlLineStatus_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(statusLineStatus_box,1000);
		statusLineStatus_box->setValueTrigger(1L,true);
		_INSTALL_MONITOR(section_box,1000);
		_INSTALL_MONITOR(commandedAzimuthOffset_field,250);
		_INSTALL_MONITOR(commandedAzimuthRate_field,250);
		_INSTALL_MONITOR(commandedElevationOffset_field,250);
		_INSTALL_MONITOR(commandedElevationRate_field,250);
		_INSTALL_MONITOR(cableWrapPosition_field,250);		
		_INSTALL_MONITOR(cableWrapTrackingError_field,250);
		_INSTALL_MONITOR(cableWrapRate_field,250);
		_INSTALL_MONITOR(generalStatus_bits,1000);
		_INSTALL_MONITOR(azimuthStatus_bits,1000);
		_INSTALL_MONITOR(elevationStatus_bits,1000);
		_INSTALL_MONITOR(mountStatus_box,1000);
		_INSTALL_MONITOR(motorsPosition_text,500);
		_INSTALL_MONITOR(motorsSpeed_text,500);
		_INSTALL_MONITOR(motorsTorque_text,500);
		_INSTALL_MONITOR(motorsUtilization_text,500);
		_INSTALL_MONITOR(freepTTPos_field,500);
		// now istall monitor for the motorsStatus, manually
		
		motorsStatusCallbackObject.setControls(motorsStatus_led);
		motorsStatusCB=motorsStatusCallbackObject._this();
		motorsStatusMonitor=motorsStatus->create_monitor(motorsStatusCB.in(),desc);
		if (CORBA::is_nil(motorsStatusMonitor)) {
			_EXCPT(ClientErrors::CouldntPerformActionExImpl,impl,"::Main()");
			impl.setAction("Install monitor");
			impl.log();
			ACE_OS::sleep(1);
			goto ErrorLabel;
		}
		else {
			motorsStatusMonitor->set_timer_trigger(5000000); // half a second
		}
		
		ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"DONE"));
		
		_TW_ADD_LABEL("Azimuth",0,1,9,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Cmd Az.",0,2,9,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Elevation",0,3,9,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Cmd El.",0,4,9,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Wrap",0,5,9,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Time",0,7,9,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("D ut1",32,7,9,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);		
		_TW_ADD_LABEL("Az1Az2Az3Az4Az5Az6Az7Az8El1El2El3El4Wrp",36,15,3,MOTORS_NUMBER,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Ctrl socket",36,9,11,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Status socket",36,10,13,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Mount status",36,11,12,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		_TW_ADD_LABEL("Track points",36,12,12,1,CColorPair::WHITE_BLACK,CStyle::UNDERLINE,window);
		
		_TW_ADD_LABEL("Position",10,0,8,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Rate",20,0,6,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Offset",28,0,8,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Pos.Error",37,0,9,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Trk.Error",47,0,9,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Mode",57,0,4,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Trk.Error",47,0,9,1,CColorPair::WHITE_BLACK,0,window);	
		_TW_ADD_LABEL("Azimuth",18,8,7,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Elevation",26,8,9,1,CColorPair::WHITE_BLACK,0,window);
		_TW_ADD_LABEL("Pos[rot]  [rot/min] Torque  Use[%]",41,14,34,1,CColorPair::WHITE_BLACK,0,window);
		
		for (WORD j=0;j<motorsStatusDescription.length();j++) {
			TW::CLabel *dummy=new TW::CLabel((const char *)motorsStatusDescription[j]);
			dummy->setWidth(1);
			dummy->setHeight(5);
			dummy->setPosition(TW::CPoint(77+j,10));
			if ((j%2)==0) {
				dummy->setStyle(TW::CStyle(CColorPair::WHITE_BLACK,TW::CStyle::BOLD));
			}
			else {
				dummy->setStyle(TW::CStyle(CColorPair::WHITE_BLACK,0));
			}
			dummy->setWAlign(TW::CFrameComponent::RIGHT);
			dummy->setHAlign(TW::CFrameComponent::DOWN);
			window.addComponent((CFrameComponent*)dummy);
		}
		
		window.addComponent((CFrameComponent*)azimuth_field);				
		window.addComponent((CFrameComponent*)elevation_field);		
		window.addComponent((CFrameComponent*)azimuthError_field);				
		window.addComponent((CFrameComponent*)elevationError_field);
		window.addComponent((CFrameComponent*)azimuthTrackError_field);				
		window.addComponent((CFrameComponent*)elevationTrackError_field);
		window.addComponent((CFrameComponent*)time_field);
		window.addComponent((CFrameComponent*)deltaTime_field);
		window.addComponent((CFrameComponent*)azimuthOffset_field);				
		window.addComponent((CFrameComponent*)elevationOffset_field);				
		window.addComponent((CFrameComponent*)commandedAzimuth_field);				
		window.addComponent((CFrameComponent*)commandedElevation_field);						
		window.addComponent((CFrameComponent*)azimuthMode_box);
		window.addComponent((CFrameComponent*)elevationMode_box);
		window.addComponent((CFrameComponent*)elevationRate_field);
		window.addComponent((CFrameComponent*)azimuthRate_field);		
		window.addComponent((CFrameComponent*)statusLineStatus_box);
		window.addComponent((CFrameComponent*)controlLineStatus_box);
		window.addComponent((CFrameComponent*)section_box);
		window.addComponent((CFrameComponent*)commandedAzimuthOffset_field);
		window.addComponent((CFrameComponent*)commandedAzimuthRate_field);
		window.addComponent((CFrameComponent*)commandedElevationOffset_field);
		window.addComponent((CFrameComponent*)commandedElevationRate_field);
		window.addComponent((CFrameComponent*)cableWrapPosition_field);
		window.addComponent((CFrameComponent*)cableWrapTrackingError_field);
		window.addComponent((CFrameComponent*)cableWrapRate_field);
		window.addComponent((CFrameComponent*)generalStatus_bits);
		window.addComponent((CFrameComponent*)azimuthStatus_bits);
		window.addComponent((CFrameComponent*)elevationStatus_bits);
		window.addComponent((CFrameComponent*)mountStatus_box);
		window.addComponent((CFrameComponent*)motorsPosition_text);
		window.addComponent((CFrameComponent*)motorsSpeed_text);		
		window.addComponent((CFrameComponent*)motorsTorque_text);
		window.addComponent((CFrameComponent*)motorsUtilization_text);
		for (WORD j=0;j<MOTORS_NUMBER;j++) {
			window.addComponent((CFrameComponent*)motorsStatus_led[j]);
		}
		window.addComponent((CFrameComponent*)freepTTPos_field);
		
		
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
	ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"START"));
	window.showFrame();
	for(;;)	{
		if ((fieldCounter=userInput->parseCommand(fields,MAXFIELDNUMBER))>0) {
			fields[0].MakeUpper();
			if (fields[0]=="EXIT") break;
		}
		client.run(tv);
		tv.set(0,MAINTHREADSLEEPTIME*1000);		
	}	
	window.closeFrame();
	
	ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"RELEASING"));
	goto CloseLabel;
ErrorLabel:
	ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"ABORTING"));	
CloseLabel:

	if (!CORBA::is_nil(motorsStatusMonitor)) {
		motorsStatusMonitor->destroy();
	}
	window.Destroy();
	ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"FRAME_CLOSED"));	
	ACE_OS::sleep(1);	
	try {
		if (componentName!="") {
			client.releaseComponent((const char *)componentName);
		}
	}
	catch (maciErrType::CannotReleaseComponentExImpl& E) {
		E.log();
	}	
	ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"COMPONENT_RELEASED"));
	if (loggedIn) client.logout();
	ACS_LOG(LM_FULL_INFO,"::Main()",(LM_INFO,"SHUTTING_DOWN"));		
	signal(SIGINT,SIG_DFL);
	ACE_OS::sleep(1);	
	return 0;
}
