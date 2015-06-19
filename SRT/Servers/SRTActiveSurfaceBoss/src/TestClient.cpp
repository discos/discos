#include <maciSimpleClient.h>
#include <IRA>
#include <usdC.h>
#include <lanC.h>
#include <SRTActiveSurfaceBossC.h>
#include <iostream>
#include <stdio.h>
#include <acserr.h>
#include <ASErrors.h>
#include <ClientErrors.h>
#include <ComponentErrors.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

// mask pattern for status 
#define MRUN	0x000080
#define CAMM	0x000100
#define ENBL	0x002000
#define PAUT	0x000800
#define CAL 	0x008000

#define STOP 	0x11
#define RESET 	0x01
#define GO      0x32
#define STOW	0x31
#define SETUP	0x31
#define CPOS	0x30
#define TOP     21000
#define BOTTOM  -21000
#define USxS 	7   /// ustep to step exp. 2 factor (2^7=128)
#define CIRCLES 17
#define ACTUATORS 96
#define TOTALACTUATORS 1116
#define MAINTHREADSLEEPTIME 150

#define LOOPTIME 1000 // 1 msec

using namespace std;
using namespace maci;
using namespace ASErrors;

int actuatorsInCircle[] = {0,24,24,48,48,48,48,96,96,96,96,96,96,96,96,96,8,4};

void checkAScompletionerrors(char* str, int circle, int actuator, CompletionImpl comp);
void checkASerrors(char* str, int circle, int actuator, ASErrors::ASErrorsEx Ex);
void setradius(int radius, int &actuatorsradius, int &jumpradius);
void setradius2(int radius, int &sector, int &sectorradius);
void setserial (int circle, int actuator, int &lanIndex, char *serial_usd);

int main (int argc, char *argv[])
{
	SimpleClient *client = new SimpleClient;
	bool loggedIn=false;
	ACSErr::Completion_var completion;
  	ACS::ROlong_var softVer_var; CORBA::Long softVer_val;
  	ACS::ROlong_var actPos_var; CORBA::Long actPos_val;
  	ACS::ROpattern_var status_var; CORBA::Long status_val;
	ACS::ROlong_var type_var; CORBA::Long type_val;
	ACS::RWlong_var Fmin_var; CORBA::Long Fmin_val;
	ACS::RWlong_var Fmax_var; CORBA::Long Fmax_val;
	ACS::RWlong_var acc_var; CORBA::Long acc_val;
	ACS::RWlong_var delay_var; CORBA::Long delay_val;
	ACS::RWlong_var cmdPos_var; CORBA::Long cmdPos_val;
	//ACS::RWlong_var uBits_var; CORBA::Long uBits_val;
	ACS::RWdouble_var lmCorr_var; CORBA::Double lmCorr_val;
	ACS::RWdouble_var gravCorr_var; CORBA::Double gravCorr_val;
	ACS::RWdouble_var userOffset_var; CORBA::Double userOffset_val;
	//MOD_USD::USD_var tusd;
  	int command;
  	char underscore;
	//int actuatorcounter = 1, circlecounter = 1, totacts = 1;
	char anothersector;
  	int circle, actuator, radius, sector, sectorradius;
	int sectornumber;
	int firstUSD = 0, lastUSD = 0;
    char *value;
	//MOD_USD::USD_var usd[CIRCLES+1][ACTUATORS+1];
//	MOD_LAN::lan_var lan[9][13];
	//MOD_USD::USD_var lanradius[18][97];
  	char serial_usd[23];
	char graf[6], mecc[4];
	int usdCounter = 0;
	int usdinitialized[CIRCLES+1][ACTUATORS+1];
	int laninitialized[9][13];
	int lanradiusinitialized[18][97];
	int lanIndex, circleIndex, usdCircleIndex;
	IRA::CString lanCobName;

    	SRTActiveSurface::SRTActiveSurfaceBoss_var ASBoss;

	ACE_Time_Value tv(1);

		try {
      			if (client->init(argc, argv)==0) {
				_EXCPT(ClientErrors::CouldntInitExImpl,impl,"TestClient::Main()");
				impl.log();
				delete client;
				//goto CloseLabel;
			}
			if (client->login()==0) {
				_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"TestClient::Main()");
				impl.log();
				delete client;
				//goto CloseLabel;
			}
			ACS_SHORT_LOG((LM_INFO, "client %s activated","TestClient"));
			loggedIn = true;
		}
		catch(...)
		{
			_EXCPT(ClientErrors::UnknownExImpl,impl,"TestClient::Main()");
			impl.log();
			delete client;
			//goto CloseLabel;
		}
		ACS_SHORT_LOG ((LM_INFO, "OK activation client: %s", "TestClient"));

    try {
			ASBoss = client->getComponent < SRTActiveSurface::SRTActiveSurfaceBoss > ("AS/Boss", 0, true);
			if (CORBA::is_nil (ASBoss.in ()) == true)
			{
				_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"TestClient::Main()");
				impl.setComponentName("AS/Boss");
				impl.log();
	  		}
			else {
				printf("ASBoss activated\n");
			}
		}
		catch (maciErrType::CannotGetComponentExImpl &E) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"TestClient::Main()");
			impl.log();
			//goto CloseLabel; // DA VERIFICARE
		}
		catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"TestClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"TestClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}
        ACE_OS::sleep (2);
        ASBoss->calibrate(1,7,0);
        ACE_OS::sleep (2);
        ASBoss->calVer(1,7,0);
        ACE_OS::sleep (2);
        for (;;) {
            client->run(tv);
            tv.set(0,MAINTHREADSLEEPTIME*10);
        }
	// Releasing client
	if (loggedIn) {
  		client->logout ();
  		delete client;
	}

  	//Sleep for 2 sec to allow everytihng to cleanup and stabilize
  	ACE_OS::sleep (2);

	ACS_SHORT_LOG ((LM_INFO, "Done..."));

  	return 0;
}
