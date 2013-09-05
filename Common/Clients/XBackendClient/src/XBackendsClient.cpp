#include "../include/XBackendsClient.h"
#include "../include/xclientGUI.h"
#include "../include/xclientGUIui.h"
#include <maciSimpleClient.h>
#include "XBackendsC.h"
#include <FitsWriterC.h>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <IRA>


#define MAX_GAIN 256

#define _GET_ACS_PROPERTY(TYPE,NAME) TYPE##_var NAME; \
{	\
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,"Trying to get property "#NAME"...")); \
	NAME=sender->NAME(); \
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


#define _GET_PROPERTY_VALUE_ONCE(VAL,PROPERTY,PROPERTNAME) { \
	try { \
		ACSErr::Completion_var cmpl; \
		VAL=PROPERTY->get_sync(cmpl.out()); \
		CompletionImpl cmplImpl(cmpl.in()); \
		if (!(cmplImpl.isErrorFree())) { \
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

// #define _CATCH_ALL(OUTPUT,ROUTINE,COMP_EXCEPTION) \
//	catch (COMP_EXCEPTION &E) { \
//		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,ROUTINE); \
//		IRA::CString Message; \
//		if (OUTPUT!=NULL) { \
//			_EXCPT_TO_CSTRING(Message,impl); \
//			OUTPUT->setValue(Message); \
//			OUTPUT->Refresh(); \
//		} \
//		impl.log(); \
//	} \
//	catch (ComponentErrors::ComponentErrorsEx &E) { \
//		_ADD_BACKTRACE(ClientErrors::CouldntPerformActionExImpl,impl,E,ROUTINE); \
//		IRA::CString Message; \
//		if (OUTPUT!=NULL) { \
//			_EXCPT_TO_CSTRING(Message,impl); \
//			OUTPUT->setValue(Message); \
//			OUTPUT->Refresh(); \
//		} \
//		impl.log(); \
//	} \
//	catch (CORBA::SystemException &C) { \
//		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,ROUTINE); \
//		impl.setName(C._name()); \
//		impl.setMinor(C.minor()); \
//		if (OUTPUT!=NULL) { \
//			IRA::CString Message; \
//			_EXCPT_TO_CSTRING(Message,impl); \
//			OUTPUT->setValue(Message); \
//			OUTPUT->Refresh(); \
//		} \
//		impl.log(); \
//	} \
//	catch (...) { \
//		_EXCPT(ClientErrors::UnknownExImpl,impl,ROUTINE); \
//		if (OUTPUT!=NULL) { \
//			IRA::CString Message; \
//			_EXCPT_TO_CSTRING(Message,impl); \
//			OUTPUT->setValue(Message); \
//			OUTPUT->Refresh(); \
//		} \
//		impl.log(); \
//	}

#define COMPONENT_INTERFACE COMPONENT_IDL_MODULE::COMPONENT_IDL_INTERFACE
#define COMPONENT_DECLARATION COMPONENT_IDL_MODULE::COMPONENT_SMARTPOINTER

#define MAX_NUM_INPUTS 18

#define TEMPLATE_4_ROTBOOLEAN  Management::ROTBoolean_ptr,ACS::Monitorpattern,ACS::Monitorpattern_var,_TW_CBpattern,ACS::CBpattern_var

ACE_RCSID(acsexmpl, acsexmpClient, "$Id: XBackendsClient.cpp,v 1.11 2010/05/24 12:19:53 bliliana Exp $")
using namespace maci;


IRA::CString polarizationFormat(const baci::BACIValue& value,const void* arg)
{
	if (value.longValue()<2) {
		if (value.longValue()==0) {
			return IRA::CString("LCP");
		}
		else {
			return IRA::CString("RCP");
		}
	}
	else if (value.longValue()==2){
		return IRA::CString("FULL_STOKES");
	}
	return IRA::CString("FULL_STOKES");
}

double gainToAttenuation(double gain){ return -20*log10((double)gain/MAX_GAIN); }
double attenuationToGain(double attenuation){ return MAX_GAIN*exp10((double)-attenuation/20); }
      
int main(int argc, char *argv[])
{
	Backends::XBackends_var sender;
	Management::FitsWriter_var recv;
	bool Component = false;
	QApplication a(argc, argv);
	int i=0;
	long integ=0,N=0,V=0;
	CString str("");
	HeadResult tmp;
	//xclientGUIui p;
	xclientGUIui w;
	 //p.show();
	 w.show();

   /**
	* Create the instance of Client and Init() it.
    */
	SimpleClient* client=new SimpleClient;
	bool loggedIn=false;
	try
	{
      	if(client->init(qApp->argc(),qApp->argv())==0)
		{
			_EXCPT(ClientErrors::CouldntInitExImpl,impl,"XBackendsClient::Main()");
			impl.log();
			delete client;
	  		goto CloseLabel;
		}
		if(client->login()==0)
		{
			_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"XBackendsClient::Main()");
			impl.log();
			delete client;
			goto CloseLabel;
		}
		ACS_SHORT_LOG((LM_INFO, "client %s activated","XBackendsClient"));
	  	loggedIn = true;
	}
	catch(...)
	{
		_EXCPT(ClientErrors::UnknownExImpl,impl,"XBackendsClient::Main()");
		impl.log();
		delete client;
		goto CloseLabel;
  	}

	// Get reference to the Mount component
	ACS_SHORT_LOG((LM_INFO, "getting component %s","Backends/XBackends"));
  	try
  	{
  		sender=client->getComponent<Backends::XBackends>("Backends/XBackends",0,true);
		//sender = client->get_object <Backends::XBackends> ("Backends/XBackends", 0, true);
		if (CORBA::is_nil(sender.in()) == true){
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"XBackendsClient::Main()");
			impl.setComponentName("Backends/XBackends");
			impl.log();
			goto CloseLabel;
  		}
   	}
   	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"XBackendsClient::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto CloseLabel;
	}
	catch(...)
	{
		_EXCPT(ClientErrors::UnknownExImpl,impl,"XBackendsClient::Main()");
		impl.log();
		goto CloseLabel;
   	}
	Component = true;
	ACS_SHORT_LOG((LM_INFO, "component %s activated","Backends/XBackends"));
	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_DEBUG,"OK, reference is: %d",sender.ptr()));
	try {
		recv=client->getComponent<Management::FitsWriter>("MANAGEMENT/Writer1",0,true);
		if (CORBA::is_nil(recv.in())==true) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"::main()");
			impl.setComponentName("MANAGEMENT/Writer1");
			impl.log();
			exit(-1);
		}
	}
	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"::main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		exit(-1);
	}
	catch (maciErrType::CannotGetComponentExImpl& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"::main()");
		impl.setComponentName("MANAGEMENT/Writer1");
		impl.log();
		exit(-1);
	}	
	catch(...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
		impl.log();
		exit(-1);
	}	
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"GOT_COMPONENENT: %s","MANAGEMENT/Writer1"));
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_DEBUG,"Reference is: %d",recv.ptr()));
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"ALL_COMPONENTS_RETRIEVED"));
	
	ACE_OS::sleep(1);
	try {
		sender->connect(recv.in());
	}
	catch (...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::main() connect");
		impl.log();
	}	
	
	try {
		/* Add all component properties here */
		_GET_ACS_PROPERTY(ACS::ROuLongLong,time);
		_GET_ACS_PROPERTY(ACS::ROlong,inputsNumber);
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
		_GET_ACS_PROPERTY(ACS::ROstring,fileName);
//DA IMPLEMENTARE LA GESTIONE DELLO STATO!!!
//		ACS::ROpattern_var state=new ACS::pattern;
//		if (status.ptr()!= ACS::ROpattern::_nil()) _GET_PROPERTY_VALUE_ONCE(state,status,"status");
//		if(state)w.spec.SetModo8bit(..);
		w.spec.SetModo8bit(false);
		if (inputsNumber.ptr()!= ACS::ROlong::_nil()) _GET_PROPERTY_VALUE_ONCE(N,inputsNumber,"inputsNumber");
		if (integration.ptr()!= ACS::ROlong::_nil()) _GET_PROPERTY_VALUE_ONCE(integ,integration,"integration");
		ACS::doubleSeq_var freq= new ACS::doubleSeq;
		ACS::longSeq_var pol= new ACS::longSeq;
		ACS::doubleSeq_var bw= new ACS::doubleSeq;
		ACS::doubleSeq_var att= new ACS::doubleSeq;
		ACS::longSeq_var bs=new ACS::longSeq;
		ACS::longSeq_var fd=new ACS::longSeq;
		freq->length(N);
		pol->length(N);
		bw->length(N);
		att->length(N);
		bs->length(N);
		fd->length(N);
		char *file;
		if (frequency.ptr()!= ACS::ROdoubleSeq::_nil()) _GET_PROPERTY_VALUE_ONCE(freq,frequency,"frequency");
		if (polarization.ptr()!= ACS::ROlongSeq::_nil()) _GET_PROPERTY_VALUE_ONCE(pol,polarization,"polarization");
		if (bandWidth.ptr()!= ACS::ROdoubleSeq::_nil()) _GET_PROPERTY_VALUE_ONCE(bw,bandWidth,"bandWidth");
		if (attenuation.ptr()!= ACS::ROdoubleSeq::_nil()) _GET_PROPERTY_VALUE_ONCE(att,attenuation,"attenuation");
		if (bins.ptr()!= ACS::ROlongSeq::_nil()) _GET_PROPERTY_VALUE_ONCE(bs,bins,"binds");
		if (feed.ptr()!= ACS::ROlongSeq::_nil()) _GET_PROPERTY_VALUE_ONCE(fd,feed,"feed");
		if (fileName.ptr()!=ACS::ROstring::_nil()) _GET_PROPERTY_VALUE_ONCE(file,fileName,"fileName");
		V=w.spec.GetNSezioni();
		if(N>V){
			for(i=V;i<N;i++){
				w.spec.SetNSezioni((w.spec.GetNSezioni()+1));
				w.spec.InsertElementArray();
			}
		}
		else{
			for(i=N;i<V;i++) w.spec.specificaSezione[i].SetZeroHeadResult();
		}
		w.spec.SetNSezioni(N);
		for(i=0;i<N;i++){
			if(bw[i]==125) tmp.SetFlo(0);
			else tmp.SetFlo((freq[i]-62.5+bw[i]/2)*1e6);
			tmp.SetBanda(bw[i]*1e6);
			tmp.SetGain(attenuationToGain(att[i]));
			if(pol[i]==2)tmp.SetModoPol(true);
			else tmp.SetModoPol(false);
			tmp.Setindex(i);
			tmp.SetAdc(fd[i]);		
			if (pol[i]!=2) tmp.SetAdc(fd[i]*2+pol[i]);//In Specifiche si settano i ChIn********************************************	
			else tmp.SetAdc(fd[i]*2);
			w.spec.specificaSezione[i]=tmp;

			str.Concat("\n i = ");
			str.Concat((int)i);	
			str.Concat(" ChIn = ");
			str.Concat((int)tmp.GetAdc());			
			str.Concat(" Feed = ");
			str.Concat((int)fd[i]);			
			str.Concat(" Spec = ");
			str.Concat((int)w.spec.specificaSezione[i].GetAdc());	
			if(tmp.GetModoPol()) str.Concat("ModoPolarimetric = TRUE"); 
			else str.Concat("ModoPolarimetric = FALSE");
			str.Concat(" \n");
		}
		
		std::cout << str << std::endl;
		
		w.spec.SetNCicli(0);
		w.spec.SetTempoIntegrazione((int)integ);
		w.Rivisualizza(0);
	w.setParameters(client,sender,recv);
	//p.spec.SetModo8bit(false);
	//p.setParameters(client,sender,recv);	
	ACS_SHORT_LOG((LM_INFO, "getting property %s","Backends/XBackends"));
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
	// Set parameters for the main thread
	

	// Start the GUI	
    a.exec();//Non mettere il return se no non si disconnette
    
ErrorLabel:
    	ACS_LOG(LM_FULL_INFO,MODULE_NAME"::Main()",(LM_INFO,MODULE_NAME"::ABORTING"));    
CloseLabel:
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"TERMINATING"));
	ACE_OS::sleep(1);	
	try {
		sender->terminate();
		sender->disconnect();
		recv->closeReceiver();
	}
	catch (...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"::main()");
		impl.log();
	}
	ACE_OS::sleep(1);	
	try {
		client->manager()->release_component(client->handle(),"Backends/XBackends");
		client->manager()->release_component(client->handle(),"MANAGEMENT/Writer1");
	}
	catch (maciErrType::CannotReleaseComponentExImpl& E) {
		E.log();
	}
	ACS_LOG(LM_FULL_INFO,"::main()",(LM_INFO,"COMPONENTS_RELEASED"));
	
	// Releasing client
	if (loggedIn) client->logout();
	
	// Delete GUIs
	//delete &w;
	a.exit();
	
	ACE_OS::sleep(3);
	
	return 0;

}
