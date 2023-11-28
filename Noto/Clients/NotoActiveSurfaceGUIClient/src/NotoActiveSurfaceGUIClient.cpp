#include <maciSimpleClient.h>
#include <NotoActiveSurfaceBossC.h>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <IRA>
#include <NotoActiveSurfaceGUIui.h>

using namespace maci;

int main(int argc,char* argv[])
{
	QApplication a(argc,argv);
    	ActiveSurface::NotoActiveSurfaceBoss_var ASBoss;
    	bool NotoActiveSurfaceBossComponent = false;
  
	NotoActiveSurfaceGUI* myNotoActiveSurfaceGUI = new NotoActiveSurfaceGUI;
	myNotoActiveSurfaceGUI->setFixedSize(1270,960);
   	myNotoActiveSurfaceGUI->show();
  	a.setActiveWindow(myNotoActiveSurfaceGUI);

	/**
	* Create the instance of Client and Init() it.
    */
  	SimpleClient* client = new SimpleClient;
	bool loggedIn=false;
	try
	{
      		if (client->init(qApp->argc(),qApp->argv())==0)
		{
			_EXCPT(ClientErrors::CouldntInitExImpl,impl,"NotoActiveSurfaceGUIClient::Main()");
			impl.log();
			delete client;
	  		goto CloseLabel;
		}
		if(client->login()==0)
		{
			_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"NotoActiveSurfaceGUIClient::Main()");
			impl.log();
			delete client;
			goto CloseLabel;
		}
		ACS_SHORT_LOG((LM_INFO, "client %s activated","NotoActiveSurfaceGUIClient"));
		loggedIn = true;
	}
	catch(...)
	{
		_EXCPT(ClientErrors::UnknownExImpl,impl,"NotoActiveSurfaceGUIClient::Main()");
		impl.log();
		delete client;
		goto CloseLabel;
  	}

    // Get reference to the Active Surface Boss component
    try {
        ASBoss = client->getComponent < ActiveSurface::NotoActiveSurfaceBoss > ("AS/Boss", 0, true);
        if (CORBA::is_nil (ASBoss.in ()) == true) {
            _EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"TestClient::Main()");
            impl.setComponentName("AS/Boss");
            impl.log();
            goto CloseLabel;
        }
    }
	catch (maciErrType::CannotGetComponentExImpl &E) {
	    _EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"TestClient::Main()");
        impl.log();
        goto CloseLabel;
	}
	catch(CORBA::SystemException &E) {
        _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"TestClient::Main()");
        impl.setName(E._name());
        impl.setMinor(E.minor());
        impl.log();
        goto CloseLabel;
	}
	catch(...) {
        _EXCPT(ClientErrors::UnknownExImpl,impl,"TestClient::Main()");
        impl.log();
        goto CloseLabel;
   	}
    NotoActiveSurfaceBossComponent = true;
    ACS_SHORT_LOG((LM_INFO, "component %s activated","AS/Boss"));

    // Set parameters for the main thread
    myNotoActiveSurfaceGUI->setParameters(client,ASBoss);

	// Start the GUI
  	a.exec();

CloseLabel:

	// Releasing component
	if (NotoActiveSurfaceBossComponent)
        client->manager()->release_component(client->handle(),"AS/Boss");
    
	// Releasing client
  	if (loggedIn) {
		client->logout();
		delete client;
	}

	// Delete GUI
  	delete myNotoActiveSurfaceGUI;

	a.exit();

  	ACE_OS::sleep(3);

	return 0;
}
