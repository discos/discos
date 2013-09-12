#include <maciSimpleClient.h>
#include <SRTActiveSurfaceBossC.h>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <IRA>
#include <SRTActiveSurfaceGUIui.h>

using namespace maci;

int main(int argc,char* argv[])
{
	QApplication a(argc,argv);
    	ActiveSurface::SRTActiveSurfaceBoss_var ASBoss;
    	bool SRTActiveSurfaceBossComponent = false;
  
	SRTActiveSurfaceGUI* mySRTActiveSurfaceGUI = new SRTActiveSurfaceGUI;
	mySRTActiveSurfaceGUI->setFixedSize(1270,960);
   	mySRTActiveSurfaceGUI->show();
  	a.setActiveWindow(mySRTActiveSurfaceGUI);

	/**
	* Create the instance of Client and Init() it.
    */
  	SimpleClient* client = new SimpleClient;
	bool loggedIn=false;
	try
	{
      		if (client->init(qApp->argc(),qApp->argv())==0)
		{
			_EXCPT(ClientErrors::CouldntInitExImpl,impl,"SRTActiveSurfaceGUIClient::Main()");
			impl.log();
			delete client;
	  		goto CloseLabel;
		}
		if(client->login()==0)
		{
			_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"SRTActiveSurfaceGUIClient::Main()");
			impl.log();
			delete client;
			goto CloseLabel;
		}
		ACS_SHORT_LOG((LM_INFO, "client %s activated","SRTActiveSurfaceGUIClient"));
		loggedIn = true;
	}
	catch(...)
	{
		_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceGUIClient::Main()");
		impl.log();
		delete client;
		goto CloseLabel;
  	}

    // Get reference to the Active Surface Boss component
    try {
        ASBoss = client->getComponent < ActiveSurface::SRTActiveSurfaceBoss > ("AS/Boss", 0, true);
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
    SRTActiveSurfaceBossComponent = true;
    ACS_SHORT_LOG((LM_INFO, "component %s activated","AS/Boss"));

    // Set parameters for the main thread
    mySRTActiveSurfaceGUI->setParameters(client,ASBoss);

	// Start the GUI
  	a.exec();

CloseLabel:

	// Releasing component
	if (SRTActiveSurfaceBossComponent)
        client->manager()->release_component(client->handle(),"AS/Boss");
    
	// Releasing client
  	if (loggedIn) {
		client->logout();
		delete client;
	}

	// Delete GUI
  	delete mySRTActiveSurfaceGUI;

	a.exit();

  	ACE_OS::sleep(3);

	return 0;
}
