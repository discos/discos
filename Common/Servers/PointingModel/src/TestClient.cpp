#include <maciSimpleClient.h>
#include <PointingModelC.h>
#include <PointingModelImpl.h>
#include <iostream>
#include <stdio.h>
#include <slamac.h>

using namespace std;
using namespace maci;

int main (int argc, char *argv[])
{

	SimpleClient *client = new SimpleClient;
	ACSErr::Completion_var completion;
	Antenna::PointingModel_var PointM;


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
	ACS_SHORT_LOG((LM_INFO, "Getting component: %s","ANTENNA/PointingModel"));
  	try
  	{
    		PointM = client->get_object < Antenna::PointingModel > ("ANTENNA/PointingModel", 0, true);
    		if (CORBA::is_nil(PointM.in()) == true)
      		{
			ACS_SHORT_LOG ((LM_ERROR, "TestClient::main: Failed to get a reference to PointingModel component."));
			return -1;
      		}
   	}
   	catch(...)
	{
		ACS_SHORT_LOG((LM_ERROR,"Error!"));
		return -1;
   	}
	ACS_SHORT_LOG((LM_INFO, "OK activation component: %s","PointingModel"));

	PointM->setReceiver("KKC");

	double azimuth, elevation, azOffset, elOffset;
	double i = 0.0;
	for (i = 0.0; i < 360.0; i++) {
		azimuth = i;
		elevation = 45.0;
		PointM->getAzElOffsets(azimuth*DD2R, elevation*DD2R, azOffset, elOffset);
		printf("azOffset = %f, elOffset = %f\n", azOffset*DR2D, elOffset*DR2D);
		ACE_OS::sleep (1);
		i = i * 5.0;
	}

	ACS_SHORT_LOG ((LM_INFO, "Releasing..."));
  	client->manager ()->release_component (client->handle (), "ANTENNA/PointingModel");
  	client->logout ();
  	delete client;

	//Sleep for 3 sec to allow everytihng to cleanup and stabilize
	ACE_OS::sleep (3);

	return 0;
}
