#include <maciSimpleClient.h>
#include <MedicinaMountC.h>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <IRA>
#include <MedicinaMountGUIui.h>

using namespace maci;

int main(int argc,char* argv[])
{
	QApplication a(argc,argv);
  	Antenna::MedicinaMount_var mount;
	bool MedicinaMountComponent = false;

	MedicinaMountGUI* myMedicinaMountGUI = new MedicinaMountGUI;
	
	myMedicinaMountGUI->setFixedSize(1005,715);
    myMedicinaMountGUI->show();

  	a.setActiveWindow(myMedicinaMountGUI);

	/**
	* Create the instance of Client and Init() it.
        */
	SimpleClient* client=new SimpleClient;
	bool loggedIn=false;
	try
	{
      		if(client->init(qApp->argc(),qApp->argv())==0)
		{
			_EXCPT(ClientErrors::CouldntInitExImpl,impl,"MedicinaMountGUIClient::Main()");
			impl.log();
			delete client;
	  		goto CloseLabel;
		}
		if(client->login()==0)
		{
			_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"MedicinaMountGUIClient::Main()");
			impl.log();
			delete client;
			goto CloseLabel;
		}
		ACS_SHORT_LOG((LM_INFO, "client %s activated","MedicinaMountGUIClient"));
	  	loggedIn = true;
	}
	catch(...)
	{
		_EXCPT(ClientErrors::UnknownExImpl,impl,"MedicinaMountGUIClient::Main()");
		impl.log();
		delete client;
		goto CloseLabel;
  	}

	// Get reference to the Mount component
	ACS_SHORT_LOG((LM_INFO, "getting component %s","ANTENNA/Mountt"));
  	try
  	{
    		mount = client->get_object < Antenna::MedicinaMount > ("ANTENNA/Mount", 0, true);
    		if (CORBA::is_nil(mount.in()) == true)
      		{
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"MedicinaMountGUIClient::Main()");
			impl.setComponentName("ANTENNA/Mount");
			impl.log();
			goto CloseLabel;
      		}
   	}
   	catch(CORBA::SystemException &E) {
		_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"MedicinaMountGUIClient::Main()");
		impl.setName(E._name());
		impl.setMinor(E.minor());
		impl.log();
		goto CloseLabel;
	}
	catch(...)
	{
		_EXCPT(ClientErrors::UnknownExImpl,impl,"MedicinaMountGUIClient::Main()");
		impl.log();
		goto CloseLabel;
   	}
	MedicinaMountComponent = true;
	ACS_SHORT_LOG((LM_INFO, "component %s activated","ANTENNA/Mount"));
	
	// Set parameters for the main thread
	myMedicinaMountGUI->setParameters(client, mount);

	// Start the GUI
  	a.exec();

CloseLabel:

	// Releasing components
	if (MedicinaMountComponent)
		client->manager()->release_component(client->handle(),"ANTENNA/Mount");

	// Releasing client
	if (loggedIn)
		client->logout();

	// Delete GUI
  	delete myMedicinaMountGUI;

	a.exit();

	ACE_OS::sleep(3);

	return 0;
}
