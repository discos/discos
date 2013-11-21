#include <maciSimpleClient.h>
#include <AntennaBossC.h>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <AntennaErrors.h>
#include <IRA>
#include <MedicinaGeneralControlSystemGUIui.h>

using namespace maci;

int main(int argc,char* argv[])
{
	QApplication a(argc,argv);
    maci::ComponentInfo_var info;
	CORBA::Object_var obj;
    Antenna::AntennaBoss_var componentAntennaBoss = Antenna::AntennaBoss::_nil();
	bool componentAntennaBossActivated = false;

    MedicinaGeneralControlSystemGUI* myMedicinaGeneralControlSystemGUI = new MedicinaGeneralControlSystemGUI;
	myMedicinaGeneralControlSystemGUI->setFixedSize(850,450);
    myMedicinaGeneralControlSystemGUI->show();
  	a.setActiveWindow(myMedicinaGeneralControlSystemGUI);

	/**
	* Create the instance of Client and Init() it.
    */
	SimpleClient* client=new SimpleClient;
	bool loggedIn=false;
	try
	{
      		if(client->init(qApp->argc(),qApp->argv())==0)
		{
			_EXCPT(ClientErrors::CouldntInitExImpl,impl,"MedicinaGeneralControlSystemGUIClient::Main()");
			impl.log();
			delete client;
	  		goto CloseLabel;
		}
		if(client->login()==0)
		{
			_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"MedicinaGeneralControlSystemGUIClient::Main()");
			impl.log();
			delete client;
			goto CloseLabel;
		}
		ACS_SHORT_LOG((LM_INFO, "client %s activated","MedicinaGeneralControlSystemGUIClient"));
	  	loggedIn = true;
	}
	catch(...)
	{
		_EXCPT(ClientErrors::UnknownExImpl,impl,"MedicinaGeneralControlSystemGUIClient::Main()");
		impl.log();
		delete client;
		goto CloseLabel;
  	}

	// Get reference to AntennaBoss component
	ACS_SHORT_LOG((LM_INFO, "getting component %s","IDL:alma/Antenna/AntennaBoss:1.0"));
  	try
  	{
        info=client->manager()->get_default_component(client->handle(),"IDL:alma/Antenna/AntennaBoss:1.0");
		obj=info->reference;
    	if (CORBA::is_nil(obj.in())) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"MedicinaGeneralControlSystemGUIClient::Main()");
			impl.setComponentName("IDL:alma/Antenna/AntennaBoss:1.0");
			impl.log();
			goto CloseLabel;
      	}
   	}
    catch (ACSErr::ACSbaseExImpl& E) {
		_ADD_BACKTRACE(ClientErrors::CouldntAccessComponentExImpl,impl,E,"MedicinaGeneralControlSystemGUIClient::Main()");
		impl.setComponentName("IDL:alma/Antenna/AntennaBoss:1.0");
		impl.log();
		goto CloseLabel;		
	}
   	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"MedicinaGeneralControlSystemGUIClient::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto CloseLabel;
	}
	catch(...)
	{
		_EXCPT(ClientErrors::UnknownExImpl,impl,"MedicinaGeneralControlSystemGUIClient::Main()");
		impl.log();
		goto CloseLabel;
   	}
    componentAntennaBoss = Antenna::AntennaBoss::_narrow(obj.in()); 
	componentAntennaBossActivated = true;
	ACS_LOG(LM_FULL_INFO,"MedicinaGeneralControlSystemGUIClient::Main()",(LM_DEBUG,"OK, reference is: %d", componentAntennaBoss.ptr()));

	// Set parameters for the main thread
	myMedicinaGeneralControlSystemGUI->setParameters(client, componentAntennaBoss);

	// Start the GUI
  	a.exec();

CloseLabel:

	// Releasing component
    try {
        if (!CORBA::is_nil(componentAntennaBoss.in())) {
            client->releaseComponent(componentAntennaBoss->name());
        }
    }
    catch (maciErrType::CannotReleaseComponentExImpl& E) {
		E.log();
	}

	// Releasing client
	if (loggedIn) {
		client->logout();
        delete client;
    }

	// Delete GUI
  	delete myMedicinaGeneralControlSystemGUI;

	a.exit();

	ACE_OS::sleep(1);

	return 0;
}
