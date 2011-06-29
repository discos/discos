#include <maciSimpleClient.h>
#include <RefractionC.h>
#include <RefractionImpl.h>
#include <iostream>
#include <stdio.h>
#include <slamac.h>

using namespace std;
using namespace maci;

int main (int argc, char *argv[])
{

	SimpleClient *client = new SimpleClient;
	ACSErr::Completion_var completion;
	Antenna::Refraction_var Refraction;


  	/**
   	* Create the instance of Client and Init() it.
  	*/
    	ACS_SHORT_LOG((LM_INFO, "Activating client: %s","TestClient"));
  	if (client->init (argc, argv) == 0)
    	{
      		ACS_SHORT_LOG ((LM_ERROR, "TestClient::main: Cannot init client."));
      		return -1;
    	}
  	else
    	{
      		client->login();
    	}
    	ACS_SHORT_LOG((LM_INFO, "OK activation client: %s","TestClient"));
	ACE_OS::sleep (2); // wait two seconds


	//Get reference to the component
	ACS_SHORT_LOG((LM_INFO, "Getting component: %s","ANTENNA/Refraction"));
  	try
  	{
    		Refraction = client->get_object < Antenna::Refraction > ("ANTENNA/Refraction", 0, true);
    		if (CORBA::is_nil(Refraction.in()) == true)
      		{
			ACS_SHORT_LOG ((LM_ERROR, "TestClient::main: Failed to get a reference to Refraction component."));
			return -1;
      		}
   	}
   	catch(...)
	{
		ACS_SHORT_LOG((LM_ERROR,"Error!"));
		return -1;
   	}
	ACS_SHORT_LOG((LM_INFO, "OK activation component: %s","Refraction"));

	double zenith, zenithCorrected;
	//double temperature = 20.0;	// Metrology.getTemperature();
	//double humidity = 0.5;	// Metrology.getHumidity();
	//double pressure = 1013.0;	// Metrology.getPressure();
	double i = 0.0;
	for (i = -10.0; i < 9.0; i++) {
		zenith = 90 - i*10.0;
		printf("Elevation = %f\n", i*10.0);
		//Refraction->setMeteoParameters(20-i, 0.5+i/100., 1013+i);
		Refraction->getCorrection(zenith*DD2R, zenithCorrected);
		printf ("zenithCorrected = %f arcseconds\n", zenithCorrected*DR2AS);
		printf ("elevation = %f, elevation corrected = %f\n", i*10, i*10+zenithCorrected*DR2D);
		ACE_OS::sleep (1);
	}

	ACE_OS::sleep (3);
	ACS_SHORT_LOG ((LM_INFO, "Releasing..."));
  	client->manager ()->release_component (client->handle (), "ANTENNA/Refraction");
  	client->logout ();
  	delete client;

	//Sleep for 3 sec to allow everytihng to cleanup and stabilize
	ACE_OS::sleep (3);

	return 0;
}
