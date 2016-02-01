// System includes
#include <ctime>
#include <deque>
#include <exception>

// ACS includes
#include <acstime.h>
#include <acsncSimpleConsumer.h>
#include "XControlSystem.h"

// QT includes

XControlSystem::XControlSystem (QObject *parent) : QThread(parent)
{
    BUSY=false;
    eventEnd = false;
	NameFile.Format("");
}

//XControlSystem::~XControlSystem()
//{
//}

void XControlSystem::setSimpleClient (maci::SimpleClient* theClient)
{
	sc = theClient;
}

void XControlSystem::stop ()
{
  	eventEnd = true;
}

void XControlSystem::setSender (Backends::XBackends_var theSender)
{
	tSender = theSender;
}

void XControlSystem::setRecv (Management::FitsWriter_var theRecv)
{
	tRecv = theRecv;
}

void XControlSystem::run()//Main
{
	try {
		// status
//		printf("*********************ENTRO NEL RUN");
		ACS::ROpattern_var Status_var= tSender->status();
		if (Status_var.ptr() ==ACS::ROpattern::_nil()) {
			ACE_CString ACE_Error_str("No access to Status property");
			_EXCPT(ClientErrors::CouldntAccessPropertyExImpl,impl,"XControlSystemClient::Main() run");
			impl.setPropertyName(ACE_Error_str);
			impl.log();
		}
		// status monitor
		ACS::CBpattern_var Status_cb = _this();
		ACS::CBDescIn Status_desc;
		ACS::Monitorpattern_var Status_md = Status_var->create_monitor(Status_cb.in(), Status_desc);
		if (Status_md.ptr() != ACS::Monitorpattern::_nil())
			Status_md->set_timer_trigger(MONITOR_TIME_TRIGGER);
		else {
			ACE_CString ACE_Error_str("Status Monitor: No Action");
			_EXCPT(ClientErrors::CouldntPerformActionExImpl,impl,"XControlSystemClient::Main() run");
			impl.setAction(ACE_Error_str);
			impl.log();
		}
		eventEnd = false;
		ACE_Time_Value t;
    		while (!eventEnd) {
			t.set (5L, 0L);
      			sc->run(t);
		}
//    	printf("ESCO AL RUN**************************\n");	
    	//printf("\n NameFile %s",(const char *)NameFile);
		// Destroy monitors
		Status_md->destroy();
		ACE_OS::sleep(1);
	}
	catch (CORBA::SystemException &Ex) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"XControlSystemGUIClient::Main run()");
		impl.setName(Ex._name());
		impl.setMinor(Ex.minor());
		impl.log();
    	}
	catch (...) {
		_EXCPT(ClientErrors::UnknownExImpl,impl,"XControlSystemGUIClient::Main run()");
		impl.log();
	}
}

void XControlSystem::working(ACS::pattern Status_val, const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{
	char status=0x00;
	ACSErr::Completion_var cmpl;
	CompletionImpl comp(c);
	char * fp;
	if (comp.isErrorFree()){
		status=Status_val;		
		
		if ((status&0x08)&&(!(status&0x20))){
			try {
				ACS::ROstring_var FileName_var= tSender->fileName();
				if (FileName_var.ptr()!=ACS::ROstring::_nil()){
					fp=FileName_var->get_sync(cmpl.out());
					NameFile.Format(fp);
					//printf("\n fp %s",fp);
				}
			}		
			catch (...) {			
				_EXCPT(ClientErrors::UnknownExImpl,impl,"XControlSystemGUIClient::working()");
				impl.log();
			}			
		}
		try {
			Management::ROTBoolean_var busy_var= tSender->busy();
			if (busy_var.ptr()!=Management::ROTBoolean::_nil()){
				BUSY=busy_var->get_sync(cmpl.out());
			}
		}		
		catch (...) {			
			_EXCPT(ClientErrors::UnknownExImpl,impl,"XControlSystemGUIClient::working()");
			impl.log();
		}
		emit upState(status);//Aggiorno lo stato nella Gui
	}
	else {
		comp.log();
	}
}

void XControlSystem::done(ACS::pattern Status_val, const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException)
{	
}


