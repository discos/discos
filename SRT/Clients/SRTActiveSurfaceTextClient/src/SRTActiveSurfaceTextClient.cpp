#include <maciSimpleClient.h>
#include <IRA>
#include <usdC.h>
#include <lanC.h>
#include <iostream>
#include <stdio.h>
#include <kbhit.h>
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
	MOD_USD::USD_var tusd;
  	int command;
  	char underscore;
	//int actuatorcounter = 1, circlecounter = 1, totacts = 1;
	char anothersector;
  	int circle, actuator, radius, sector, sectorradius;
	int sectornumber;
	int firstUSD = 0, lastUSD = 0;
    char *value;
	MOD_USD::USD_var usd[CIRCLES+1][ACTUATORS+1];
//	MOD_LAN::lan_var lan[9][13];
	MOD_USD::USD_var lanradius[18][97];
  	char serial_usd[23];
	char graf[6], mecc[4];
	int usdCounter = 0;
	int usdinitialized[CIRCLES+1][ACTUATORS+1];
	int laninitialized[9][13];
	int lanradiusinitialized[18][97];
	int lanIndex, circleIndex, usdCircleIndex;
	IRA::CString lanCobName;

	ACE_Time_Value tv(1);

	int l, m;
	for (l = 1; l <= CIRCLES; l++)
	{
		for (m = 1; m <= actuatorsInCircle[l]; m++)
		{
			usdinitialized[l][m] = -1;
		}
	}
	for (l = 1; l <= 18; l++)
	{
		for (m = 1; m <= 97; m++)
		{
			lanradiusinitialized[l][m] = -1;
		}
	}
	std::cout << std::endl;
	std::cout << "************************************" << std::endl;
	std::cout << "NURAGHE - SRT Active Surface Control" << std::endl;
	std::cout << "************************************" << std::endl;
	std::cout << std::endl;

sectorsLabel:
	std::cout << "insert sector number (1-8) or 0 for all and press enter" << std::endl;
	std::cin >> sectornumber;
	switch (sectornumber) {
		case 0:
			firstUSD = 1;
			lastUSD = 1116;
			value = "/home/almamgr/SRT/CDB/tab_convUSD.txt\0";
			break;
		case 1:
			firstUSD = 1;
			lastUSD = 139;
			value = "/home/almamgr/SRT/CDB/tab_convUSD_S1.txt\0";
			break;
		case 2:
			firstUSD = 1;
			lastUSD = 140;
			value = "/home/almamgr/SRT/CDB/tab_convUSD_S2.txt\0";
			break;
		case 3:
			firstUSD = 1;
			lastUSD = 139;
			value = "/home/almamgr/SRT/CDB/tab_convUSD_S3.txt\0";
			break;
		case 4:
			firstUSD = 1;
			lastUSD = 140;
			value = "/home/almamgr/SRT/CDB/tab_convUSD_S4.txt\0";
			break;
		case 5:
			firstUSD = 1;
			lastUSD = 139;
			value = "/home/almamgr/SRT/CDB/tab_convUSD_S5.txt\0";
			break;
		case 6:
			firstUSD = 1;
			lastUSD = 140;
			value = "/home/almamgr/SRT/CDB/tab_convUSD_S6.txt\0";
			break;
		case 7:
			firstUSD = 1;
			lastUSD = 139;
			value = "/home/almamgr/SRT/CDB/tab_convUSD_S7.txt\0";
			break;
		case 8:
			firstUSD = 1;
			lastUSD = 140;
			value = "/home/almamgr/SRT/CDB/tab_convUSD_S8.txt\0";
			break;
	}
	ifstream usdTable(value);
	if (!usdTable) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
		exit(-1);
	}

	if (!loggedIn) {
		try {
      			if (client->init(argc, argv)==0) {
				_EXCPT(ClientErrors::CouldntInitExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
				impl.log();
				delete client;
				goto CloseLabel;
			}
			if (client->login()==0) {
				_EXCPT(ClientErrors::CouldntLoginExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
				impl.log();
				delete client;
				goto CloseLabel;
			}
			ACS_SHORT_LOG((LM_INFO, "client %s activated","SRTActiveSurfaceTextClient"));
			loggedIn = true;
		}
		catch(...)
		{
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			delete client;
			goto CloseLabel;
		}
		ACS_SHORT_LOG ((LM_INFO, "OK activation client: %s", "SRTActiveSurfaceTextClient"));
	}

/*	// Activate the LAN components
	for (int s = 1; s <= 8; s++)
	{
		laninitialized[s][l] = -1;
		for (int l = 1; l <= 12; l++)
		{
			// Use container to activate the object
			lanCobName.Format("AS/SECTOR%02d/LAN%02d",s, l);
			ACS_SHORT_LOG((LM_INFO, "Getting component: %s", (const char*)lanCobName) );
			try {
				lan[s][l] = client->getComponent < MOD_LAN::lan> ((const char *)lanCobName, 0, true);
				if (CORBA::is_nil (lan[s][l].in ()) == true)
				{
					_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
					impl.setComponentName((const char *)lanCobName);
					impl.log();
					laninitialized[s][l] = -1;
					//goto CloseLabel; // DA VERIFICARE
				}
				else {
					laninitialized[s][l] = 0;
					std::cout << std::endl << "activated lan " << s << " " << l << std::endl;
				}
			}
			catch (maciErrType::CannotGetComponentExImpl &E) {
				_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
				impl.log();
				//goto CloseLabel; // DA VERIFICARE
			}
			catch(CORBA::SystemException &E) {
				_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
				impl.setName(E._name());
				impl.setMinor(E.minor());
				impl.log();
				goto CloseLabel;
			}
			catch(...) {
				_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
				impl.log();
				goto CloseLabel;
   			}
		}
	}
*/
	// Get reference to usd components
	for (int i = firstUSD; i <= lastUSD; i++) {
		usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc;
		usdinitialized[circleIndex][usdCircleIndex] = -1; // 0 ok, -1 fault
		try {
			usd[circleIndex][usdCircleIndex] = client->getComponent < MOD_USD::USD > (serial_usd, 0, true);
			if (CORBA::is_nil (usd[circleIndex][usdCircleIndex].in ()) == true)
			{
				_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
				impl.setComponentName(serial_usd);
				impl.log();
				usdinitialized[circleIndex][usdCircleIndex] = -1;
				//goto CloseLabel; // DA VERIFICARE
	  		}
			else {
				usdinitialized[circleIndex][usdCircleIndex] = 0;
				lanradiusinitialized[circleIndex][lanIndex] = 0;
				lanradius[circleIndex][lanIndex] = usd[circleIndex][usdCircleIndex];
				usdCounter++;
			}
		}
		catch (maciErrType::CannotGetComponentExImpl &E) {
			_EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel; // DA VERIFICARE
		}
		catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			goto CloseLabel;
   		}
	}
	ACS_SHORT_LOG ((LM_INFO, "activated %d usd", usdCounter));
	std::cout << std::endl << "activated " << usdCounter << " usd" << std::endl;
	ACE_OS::sleep (2);

	std::cout << std::endl << "another sector ? type (y/n) and press enter" << std::endl;
	std::cin >> anothersector;
	if (anothersector == 'y')
		goto sectorsLabel;

  	std::cout << std::endl << "type l (command list) or x (exit)" << std::endl;
  	init_keyboard ();
  	//do
	for (;;)
    	{
		/*int i = circlecounter;
		int l = actuatorcounter;
		int totalactuators = totacts;

		if (totalactuators >= 1 && totalactuators <= 24) // 1 circle
			i= 1;
		if (totalactuators >= 25 && totalactuators <= 48)  // 2 circle
			i= 2;
		if (totalactuators >= 49 && totalactuators <= 96)  // 3 circle
			i= 3;
		if (totalactuators >= 97 && totalactuators <= 144) // 4 circle
			i= 4;
		if (totalactuators >= 145 && totalactuators <= 192) // 5 circle
			i= 5;
		if (totalactuators >= 193 && totalactuators <= 240) // 6 circle
			i= 6;
		if (totalactuators >= 241 && totalactuators <= 336) // 7 circle
				i= 7;
		if (totalactuators >= 337 && totalactuators <= 432) // 8 circle
			i= 8;
		if (totalactuators >= 433 && totalactuators <= 528) // 9 circle
			i= 9;
		if (totalactuators >= 529 && totalactuators <= 624) // 10 circle
			i= 10;
		if (totalactuators >= 625 && totalactuators <= 720) // 11 circle
			i= 11;
		if (totalactuators >= 721 && totalactuators <= 816) // 12 circle
			i= 12;
		if (totalactuators >= 817 && totalactuators <= 912) // 13 circle
			i= 13;
		if (totalactuators >= 913 && totalactuators <= 1008) // 14 circle
			i= 14;
		if (totalactuators >= 1009 && totalactuators <= 1104) // 15 circle
			i= 15;
		if (totalactuators >= 1105 && totalactuators <= 1112) // 16 circle
			i= 16;
		if (totalactuators >= 1113 && totalactuators <= 1116) // 17 circle
			i= 17;
		if (totalactuators == 1 || totalactuators == 25 || totalactuators == 49 ||
		    totalactuators == 97 || totalactuators == 145 || totalactuators == 193 ||
		    totalactuators == 241 || totalactuators == 337 || totalactuators == 433 ||
		    totalactuators == 529 || totalactuators == 625 || totalactuators == 721 ||
		    totalactuators == 817 || totalactuators == 913 || totalactuators == 1009 ||
		    totalactuators == 1105 || totalactuators == 1113 || totalactuators == 1117)
			l = 1;
		*/
		if (kbhit ())
		{
	  		command = readch ();
	  		close_keyboard ();
	  		switch (command)
	    		{
	    			case 'x':
	      			std::cout << "Exit program" << std::endl;
				goto CloseLabel;
	      			break;

	    			case 'l':
	      			std::cout << std::endl;
	      			std::cout << "************ MENU ************" << std::endl;
	      			std::cout << "0 - select actuator" << std::endl;
	      			std::cout << "1 - delay" << std::endl;
	      			std::cout << "2 - commanded position" << std::endl;
	      			std::cout << "3 - minimum frequency" << std::endl;
	      			std::cout << "4 - maximum frequency" << std::endl;
	      			std::cout << "5 - acceleration" << std::endl;
	      			std::cout << "6 - users bits" << std::endl;
	      			std::cout << "7 - step resolution" << std::endl;
	      			std::cout << "8 - last minute correction" << std::endl;
	      			std::cout << "9 - actual position" << std::endl;
	      			std::cout << "a - status" << std::endl;
	      			std::cout << "b - software version" << std::endl;
	      			std::cout << "c - type" << std::endl;
	      			std::cout << "d - gravity correction" << std::endl;
	      			std::cout << "e - user offset" << std::endl;
	      			std::cout << "f - camma length" << std::endl;
	      			std::cout << "g - camma position" << std::endl;
	      			std::cout << "h - stop" << std::endl;
	      			std::cout << "i - reset" << std::endl;
	      			std::cout << "j - up" << std::endl;
	      			std::cout << "k - down" << std::endl;
	      			std::cout << "l - command list" << std::endl;
	      			std::cout << "m - move" << std::endl;
	      			std::cout << "n - calibrate" << std::endl;
	      			std::cout << "o - scale verification" << std::endl;
	      			std::cout << "p - reference position" << std::endl;
	      			std::cout << "q - set profile " << std::endl;
	      			std::cout << "r - correction" << std::endl;
	      			std::cout << "s - update" << std::endl;
	      			std::cout << "t - stow" << std::endl;
	      			std::cout << "u - setup" << std::endl;
	      			std::cout << "v - top most position" << std::endl;
	      			std::cout << "w - bottom most position" << std::endl;
	      			std::cout << "x - exit" << std::endl;
	      			std::cout << "******************************" << std::endl;
	      			break;

	    			case '0':
	      			printf ("\ninsert actuator number (circle_actuator_radius):\n");
	      			printf ("nn_nn_0 for single actuator\n");
	      			printf ("0_0_0 for all actuators\n");
	      			printf ("nn_0_0 for all actuators in nn circle\n");
	      			printf ("0_0_nn for all actuators in nn radius\n");
	      			printf ("press enter and choose a command from the menu list\n\n");
	      			scanf ("%d%c%d%c%d", &circle, &underscore, &actuator, &underscore, &radius);
				if ((circle == 0 && actuator == 0 && radius == 0) ||
				    (circle !=0 && actuator == 0 && radius == 0)  ||
				    (circle == 0 && actuator == 0 && radius != 0))
					usdinitialized[circle][actuator] = 0;
				else
					tusd = usd[circle][actuator];
	      			break;

	    			default:
	      			if (usdinitialized[circle][actuator] == 0)
				{
		  			switch (command)
		    			{
		    				case '1':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											delay_var = usd[i][l]->delay ();
		      									delay_val = delay_var->get_sync (completion.out ());
											double delay = delay_val*(512./1000.);
		      									printf ("actuator n.%d_%d, delay = %f msec\n", i, l, delay);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										delay_var = usd[circle][l]->delay ();
		      								delay_val = delay_var->get_sync (completion.out ());
										double delay = delay_val*(512./1000.);
		      								printf ("actuator n.%d_%d, delay = %f msec\n", circle, l, delay);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										delay_var = lanradius[l+jumpradius][radius]->delay();
										delay_val = delay_var->get_sync (completion.out ());
										double delay = delay_val*(512./1000.);
		      								printf ("actuator n.%d_%d, delay = %f msec\n", l+jumpradius, radius, delay);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									delay_var = usd[circle][actuator]->delay ();
		      							delay_val = delay_var->get_sync (completion.out ());
									double delay = delay_val*(512./1000.);
		      							printf ("actuator n.%d_%d, delay = %f msec\n", circle, actuator, delay);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
		      				break;

		    				case '2':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											cmdPos_var = usd[i][l]->cmdPos ();
		      									cmdPos_val = cmdPos_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, commanded position = %d\n", i, l, cmdPos_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										cmdPos_var = usd[circle][l]->cmdPos ();
		      								cmdPos_val = cmdPos_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, commanded position = %d\n", circle, l, cmdPos_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										cmdPos_var = lanradius[l+jumpradius][radius]->cmdPos ();
		      								cmdPos_val = cmdPos_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, commanded position = %d\n", l+jumpradius, radius, cmdPos_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									cmdPos_var = usd[circle][actuator]->cmdPos ();
		      							cmdPos_val = cmdPos_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, commanded position = %d\n", circle, actuator, cmdPos_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
		      				break;

		    				case '3':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											Fmin_var = usd[i][l]->Fmin ();
		      									Fmin_val = Fmin_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, minimun frequency = %d\n", i, l, Fmin_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										Fmin_var = usd[circle][l]->Fmin ();
		      								Fmin_val = Fmin_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, minimun frequency = %d\n", circle, l, Fmin_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										Fmin_var = lanradius[l+jumpradius][radius]->Fmin ();
		      								Fmin_val = Fmin_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, minimun frequency = %d\n", l+jumpradius, radius, Fmin_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									Fmin_var = usd[circle][actuator]->Fmin ();
		      							Fmin_val = Fmin_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, minimun frequency = %d\n", circle, actuator, Fmin_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
		      				break;

		    				case '4':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											Fmax_var = usd[i][l]->Fmax ();
		      									Fmax_val = Fmax_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, maximum frequency = %d\n", i, l, Fmax_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										Fmax_var = usd[circle][l]->Fmax ();
		      								Fmax_val = Fmax_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, maximum frequency = %d\n", circle, l, Fmax_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										Fmax_var = lanradius[l+jumpradius][radius]->Fmax ();
		      								Fmax_val = Fmax_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, maximum frequency = %d\n", l+jumpradius, radius, Fmax_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									Fmax_var = usd[circle][actuator]->Fmax ();
		      							Fmax_val = Fmax_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, maximum frequency = %d\n", circle, actuator, Fmax_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case '5':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											acc_var = usd[i][l]->acc ();
		      									acc_val = acc_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, acceleration = %d\n", i, l, acc_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										acc_var = usd[circle][l]->acc ();
		      								acc_val = acc_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, acceleration = %d\n", circle, l, acc_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										acc_var = lanradius[l+jumpradius][radius]->acc ();
		      								acc_val = acc_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, acceleration = %d\n", l+jumpradius, radius, acc_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									acc_var = usd[circle][actuator]->acc ();
		      							acc_val = acc_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, acceleration = %d\n", circle, actuator, acc_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case '6':
		      				//uBits_var = tusd->uBits ();
		      				//uBits_val = uBits_var->get_sync (completion.out ());
		      				//printf ("users bits = %d\n", uBits_val);
		      				printf ("users bits TBD\n");
		      				break;

		    				case '7':
		      				//resol_var = tusd->resol ();
		      				//resol_val = resol_var->get_sync (completion.out ());
		      				//printf ("step resolution = %ld\n", resol_val);
		      				printf ("step resolution TBD\n");
		      				break;

		    				case '8':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											lmCorr_var = usd[i][l]->lmCorr ();
		      									lmCorr_val = lmCorr_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, last minute correction = %f\n", i, l, lmCorr_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										lmCorr_var = usd[circle][l]->lmCorr ();
		      								lmCorr_val = lmCorr_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, last minute correction = %f\n", circle, l, lmCorr_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										lmCorr_var = lanradius[l+jumpradius][radius]->lmCorr ();
		      								lmCorr_val = lmCorr_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, last minute correction = %f\n", l+jumpradius, radius, lmCorr_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									lmCorr_var = usd[circle][actuator]->lmCorr ();
		      							lmCorr_val = lmCorr_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, last minute correction = %f\n", circle, actuator, lmCorr_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case '9':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											actPos_var = usd[i][l]->actPos ();
		      									actPos_val = actPos_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, actual position = %d\n", i, l, actPos_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										actPos_var = usd[circle][l]->actPos ();
		      								actPos_val = actPos_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, actual position = %d\n", circle, l, actPos_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										actPos_var = lanradius[l+jumpradius][radius]->actPos ();
		      								actPos_val = actPos_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, actual position = %d\n", l+jumpradius, radius, actPos_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									actPos_var = usd[circle][actuator]->actPos ();
		      							actPos_val = actPos_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, actual position = %d\n", circle, actuator, actPos_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 'a':
						bool run, camm, enbl, paut, cal;
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											status_var = usd[i][l]->status ();
		      									status_val = status_var->get_sync (completion.out ());
											run  = status_val & MRUN; camm = status_val & CAMM;
											enbl = status_val & ENBL; paut = status_val & PAUT;
											cal  = status_val & CAL;
		      									printf ("actuator n.%d_%d, status -> run = %d, camm = %d, enbl = %d, paut = %d, cal = %d\n", i, l, run, camm, enbl, paut, cal);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
                                            printf("eccezione raccolta\n");
											checkASerrors("actuator", i, l, Ex);
										}
                                        catch(CORBA::SystemException &E) {
                                            printf("system eccezione raccolta\n");
			                                _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			                                impl.setName(E._name());
			                                impl.setMinor(E.minor());
			                                impl.log();
			                                //goto CloseLabel;
		                                }
		                                catch(...) {
                                            printf("unknown eccezione raccolta\n");
			                                _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			                                impl.log();
			                                //goto CloseLabel;
   		                                }
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										status_var = usd[circle][l]->status ();
		      								status_val = status_var->get_sync (completion.out ());
										run  = status_val & MRUN; camm = status_val & CAMM;
										enbl = status_val & ENBL; paut = status_val & PAUT;
										cal  = status_val & CAL;
		      								printf ("actuator n.%d_%d, status -> run = %d, camm = %d, enbl = %d, paut = %d, cal = %d\n", circle, l, run, camm, enbl, paut, cal);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										status_var = lanradius[l+jumpradius][radius]->status ();
		      								status_val = status_var->get_sync (completion.out ());
										run  = status_val & MRUN; camm = status_val & CAMM;
										enbl = status_val & ENBL; paut = status_val & PAUT;
										cal  = status_val & CAL;
		      								printf ("actuator n.%d_%d, status -> run = %d, camm = %d, enbl = %d, paut = %d, cal = %d\n", l+jumpradius, radius, run, camm, enbl, paut, cal);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									status_var = usd[circle][actuator]->status ();
	      								status_val = status_var->get_sync (completion.out ());
									run  = status_val & MRUN; camm = status_val & CAMM;
									enbl = status_val & ENBL; paut = status_val & PAUT;
									cal  = status_val & CAL;
	      								printf ("actuator n.%d_%d, status -> run = %d, camm = %d, enbl = %d, paut = %d, cal = %d\n", circle, actuator, run, camm, enbl, paut, cal);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 'b':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											softVer_var = usd[i][l]->softVer ();
		      									softVer_val = softVer_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, software version = %02x\n", i, l, softVer_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										softVer_var = usd[circle][l]->softVer ();
		      								softVer_val = softVer_var->get_sync (completion.out ());
										printf ("actuator n.%d_%d, software version = %02x\n", circle, l, softVer_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										softVer_var = lanradius[l+jumpradius][radius]->softVer ();
		      								softVer_val = softVer_var->get_sync (completion.out ());
										printf ("actuator n.%d_%d, software version = %02x\n", l+jumpradius, radius, softVer_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									softVer_var = usd[circle][actuator]->softVer ();
		      							softVer_val = softVer_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, software version = %02x\n", circle, actuator, softVer_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
		      				break;

		    				case 'c':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											type_var = usd[i][l]->type ();
		      									type_val = type_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, software version = %02x\n", i, l, type_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										type_var = usd[circle][l]->type ();
		      								type_val = type_var->get_sync (completion.out ());
										printf ("actuator n.%d_%d, software version = %02x\n", circle, l, type_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										type_var = lanradius[l+jumpradius][radius]->type ();
		      								type_val = type_var->get_sync (completion.out ());
										printf ("actuator n.%d_%d, software version = %02x\n", l+jumpradius, radius, type_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									type_var = usd[circle][actuator]->type ();
		      							type_val = type_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, software version = %02x\n", circle, actuator, type_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
		      				break;

		    				case 'd':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											gravCorr_var = usd[i][l]->gravCorr ();
		      									gravCorr_val = gravCorr_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, gravity correction = %f\n", i, l, gravCorr_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										gravCorr_var = usd[circle][l]->gravCorr ();
		      								gravCorr_val = gravCorr_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, gravity correction = %f\n", circle, l, gravCorr_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										gravCorr_var = lanradius[l+jumpradius][radius]->gravCorr ();
		      								gravCorr_val = gravCorr_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, gravity correction = %f\n", l+jumpradius, radius, gravCorr_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									gravCorr_var = usd[circle][actuator]->gravCorr ();
		      							gravCorr_val = gravCorr_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, gravity correction = %f\n", circle, actuator, gravCorr_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 'e':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											userOffset_var = usd[i][l]->userOffset ();
		      									userOffset_val = userOffset_var->get_sync (completion.out ());
		      									printf ("actuator n.%d_%d, user offset = %f\n", i, l, userOffset_val);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										userOffset_var = usd[circle][l]->userOffset ();
		      								userOffset_val = userOffset_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, user offset = %f\n", circle, l, userOffset_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										userOffset_var = lanradius[l+jumpradius][radius]->userOffset ();
		      								userOffset_val = userOffset_var->get_sync (completion.out ());
		      								printf ("actuator n.%d_%d, user offset = %f\n", l+jumpradius, radius, userOffset_val);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									userOffset_var = usd[circle][actuator]->userOffset ();
		      							userOffset_val = userOffset_var->get_sync (completion.out ());
		      							printf ("actuator n.%d_%d, user offset = %f\n", circle, actuator, userOffset_val);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 'f': // retry
		      				//cammaLen_var = tusd->cammaLen ();
		      				//cammaLen_val = cammaLen_var->get_sync (completion.out ());
		     	 			//printf ("camma length = %ld\n", cammaLen_val);
		     	 			//printf ("camma length TBD\n");
                            try {
                                setserial (circle, actuator, lanIndex, serial_usd);
                                printf("serial_usd = %s\n", serial_usd);
			                    usd[circle][actuator] = client->getComponent < MOD_USD::USD > (serial_usd, 0, true);
			                    if (CORBA::is_nil (usd[circle][actuator].in ()) == true)
			                    {
				                    _EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
				                    impl.setComponentName(serial_usd);
				                    impl.log();
				                    usdinitialized[circle][actuator] = -1;
				                    //goto CloseLabel; // DA VERIFICARE
	  		                    }
			                    else {
				                    usdinitialized[circle][actuator] = 0;
				                    lanradiusinitialized[circle][lanIndex] = 0;
				                    lanradius[circle][lanIndex] = usd[circle][actuator];
			                    }
		                    }
		                    catch (maciErrType::CannotGetComponentExImpl &E) {
			                _EXCPT(ClientErrors::CouldntAccessComponentExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			                impl.log();
			                //goto CloseLabel; // DA VERIFICARE
		                    }
		                catch(CORBA::SystemException &E) {
			                _EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			                impl.setName(E._name());
			                impl.setMinor(E.minor());
			                impl.log();
			                goto CloseLabel;
		                }
		                catch(...) {
			                _EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			                impl.log();
			                goto CloseLabel;
   		                }
		      			break;

		    				case 'g':
		      				//cammaPos_var = tusd->cammaPos ();
		      				//cammaPos_val = cammaPos_var->get_sync (completion.out ());
		      				//printf ("camma position = %ld\n", cammaPos_val);
		      				printf ("camma position TBD\n");
		      				break;

		    				case 'h':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->stop();
		      									printf ("actuator n.%d_%d, stop\n", i, l);
											CIRATools::Wait(LOOPTIME);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
										catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->stop();
		      								printf ("actuator n.%d_%d, stop\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										lanradius[l+jumpradius][radius]->stop();
		      								printf ("actuator n.%d_%d, stop\n", l+jumpradius, radius);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->stop();
		      							printf ("actuator n.%d_%d, stop\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
								catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						/*if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int s, l;
							for (s = 1; s <= 8; s++)
							{
								for (l = 1; l <= 12; l++)
								{
									if (laninitialized[s][l] == 0)
									{
										try {
                                            						CompletionImpl comp = lan[s][l]->sendUSDCmd(STOP,0, 0, 0);
											if (comp.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, comp);
											printf ("lan n.%d_%d, stop\n", s, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("lan", s, l, Ex);
										}
									}
									else 
										printf ("lan n.%d_%d not initialized!\n", s, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->stop ();
		      								printf ("actuator n.%d_%d, stop\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							setradius2(radius, sector, sectorradius);
							if (laninitialized[sector][sectorradius] == 0)
							{
								try {
									CompletionImpl comp = lan[sector][sectorradius]->sendUSDCmd(STOP,0, 0, 0);
									if (comp.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, comp);
										printf ("lan n.%d_%d, stop\n", sector, sectorradius);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("lan", sector, sectorradius, Ex);
									}
							}
							else 
								printf ("lan n.%d_%d not initialized!\n", sector, sectorradius);
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->stop ();
		      							printf ("actuator n.%d_%d, stop\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}*/
						break;

		    			case 'i':
		      			if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
						    int i, l;
							for (i = 1; i <= CIRCLES; i++)
    						{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->reset();
		      									printf ("actuator n.%d_%d, reset\n", i, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->reset ();
		      								printf ("actuator n.%d_%d, reset\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										lanradius[l+jumpradius][radius]->reset ();
		      								printf ("actuator n.%d_%d, reset\n", l+jumpradius, radius);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->reset ();
		      							printf ("actuator n.%d_%d, reset\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 'j':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->up();
		      									printf ("actuator n.%d_%d, up\n", i, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									//try {
										usd[circle][l]->up();
		      								printf ("actuator n.%d_%d, up\n", circle, l);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", circle, l, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									//try {
										lanradius[l+jumpradius][radius]->up();
		      								printf ("actuator n.%d_%d, up\n", l+jumpradius, radius);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", l+jumpradius, radius, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								//try {
									usd[circle][actuator]->up();
		      							printf ("actuator n.%d_%d, up\n", circle, actuator);
								//}
								//catch (ASErrors::ASErrorsEx &Ex) {
								//	checkASerrors("actuator", circle, actuator, Ex);
								//}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						/*if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int s, l;
							for (s = 1; s <= 8; s++)
							{
								for (l = 1; l <= 12; l++)
								{
									if (laninitialized[s][l] == 0)
									{
										try {
											CompletionImpl compStop = lan[s][l]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
											if (compStop.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compStop);
											printf ("lan n.%d_%d, stop before up\n", s, l);
											CompletionImpl compUp = lan[s][l]->sendUSDCmd(GO,0, 1, 1); // go up
											if (compUp.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compUp);
											printf ("lan n.%d_%d, up\n", s, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("lan", s, l, Ex);
										}
									}
									else 
										printf ("lan n.%d_%d not initialized!\n", s, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->up ();
		      								printf ("actuator n.%d_%d, up\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							setradius2(radius, sector, sectorradius);
							if (laninitialized[sector][sectorradius] == 0)
							{
								try {
									CompletionImpl compStop = lan[sector][sectorradius]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
									if (compStop.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compStop);
									printf ("lan n.%d_%d, stop before up\n", sector, sectorradius);
									CompletionImpl compUp = lan[sector][sectorradius]->sendUSDCmd(GO, 0, 1, 1);
									if (compUp.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compUp);
									printf ("lan n.%d_%d, up\n", sector, sectorradius);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("lan", sector, sectorradius, Ex);
								}
							}
							else 
								printf ("lan n.%d_%d not initialized!\n", sector, sectorradius);
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->up ();
		      							printf ("actuator n.%d_%d, up\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}*/
						break;

		    				case 'k':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->down();
		      									printf ("actuator n.%d_%d, down\n", i, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									//try {
										usd[circle][l]->down();
		      								printf ("actuator n.%d_%d, down\n", circle, l);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", circle, l, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									//try {
										lanradius[l+jumpradius][radius]->down();
		      								printf ("actuator n.%d_%d, down\n", l+jumpradius, radius);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", l+jumpradius, radius, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								//try {
									usd[circle][actuator]->down();
		      							printf ("actuator n.%d_%d, down\n", circle, actuator);
								//}
								//catch (ASErrors::ASErrorsEx &Ex) {
								//	checkASerrors("actuator", circle, actuator, Ex);
								//}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						/*if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int s, l;
							for (s = 1; s <= 8; s++)
							{
								for (l = 1; l <= 12; l++)
								{
									if (laninitialized[s][l] == 0)
									{
										try {
											CompletionImpl compStop = lan[s][l]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
											if (compStop.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compStop);
											printf ("lan n.%d_%d, stop before down\n", s, l);
											CompletionImpl compDown = lan[s][l]->sendUSDCmd(GO, 0, -1, 1); // down
											if (compDown.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compDown);
											printf ("lan n.%d_%d, down\n", s, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("lan", s, l, Ex);
										}
									}
									else 
										printf ("lan n.%d_%d not initialized!\n", s, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->down ();
		      								printf ("actuator n.%d_%d, down\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							setradius2(radius, sector, sectorradius);
							if (laninitialized[sector][sectorradius] == 0)
							{
								try {
									CompletionImpl compStop = lan[sector][sectorradius]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
									if (compStop.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compStop);
									printf ("lan n.%d_%d, stop before down\n", sector, sectorradius);
									CompletionImpl compDown = lan[sector][sectorradius]->sendUSDCmd(GO, 0, -1, 1);
									if (compDown.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compDown);
									printf ("lan n.%d_%d, down\n", sector, sectorradius);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("lan", sector, sectorradius, Ex);
								}
							}
							else 
								printf ("lan n.%d_%d not initialized!\n", sector, sectorradius);
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->down ();
		      							printf ("actuator n.%d_%d, down\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}*/
						break;

		    				case 'm':
		      				printf ("move: set increment\n");
		      				long increment;
		      				std::cin >> increment;
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->move(increment);
		      									printf ("actuator n.%d_%d, move with %ld increment\n", i, l, increment);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->move (increment);
		      								printf ("actuator n.%d_%d, move with %ld increment\n", circle, l, increment);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										lanradius[l+jumpradius][radius]->move (increment);
		      								printf ("actuator n.%d_%d, move with %ld increment\n", l+jumpradius, radius, increment);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->move (increment);
		      							printf ("actuator n.%d_%d, move with %ld increment\n", circle, actuator, increment);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 'n':
		      				if (circle == 0 && actuator == 0 && radius == 0) { // ALL
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++)
								{
									if (usdinitialized[i][l] == 0) {
										try {
											usd[i][l]->top();
											actPos_var = usd[i][l]->actPos ();
											while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000) {
												printf("actuator n.%d_%d waiting to reach top position...\n", i, l);
											}
											CompletionImpl comp = usd[i][l]->calibrate();
											if (comp.isErrorFree() == false)
												checkAScompletionerrors("usd", i, l, comp);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0) {
									try {
										usd[circle][l]->top();
										actPos_var = usd[circle][l]->actPos ();
										while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000) {
											printf("actuator n.%d_%d waiting to reach top position...\n", circle, l);
										}
										CompletionImpl comp = usd[circle][l]->calibrate();
										if (comp.isErrorFree() == false)
											checkAScompletionerrors("usd", circle, l, comp);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++)  {
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0) {
									try {
										lanradius[l+jumpradius][radius]->top();
										actPos_var = lanradius[l+jumpradius][radius]->actPos ();
										while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000) {
											printf("actuator n.%d_%d waiting to reach top position...\n", l+jumpradius, radius);
										}
										lanradius[l+jumpradius][radius]->top();
										CompletionImpl comp = lanradius[l+jumpradius][radius]->calibrate();
										if (comp.isErrorFree() == false)
											checkAScompletionerrors("lanradius", l+jumpradius, radius, comp);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->top();
									actPos_var = usd[circle][actuator]->actPos ();
									while ((actPos_val = actPos_var->get_sync (completion.out ())) != 21000) {
										ACE_OS::sleep (1);
										printf("actuator n.%d_%d waiting to reach top position...\n", circle, actuator);
									}
									CompletionImpl comp = usd[circle][actuator]->calibrate();
									if (comp.isErrorFree() == false)
										checkAScompletionerrors("usd", circle, actuator, comp);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
		      				break;

		    				case 'o':
		      				if (circle == 0 && actuator == 0 && radius == 0) { // ALL
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++)
								{
									if (usdinitialized[i][l] == 0) {
										try {
											printf("actuator n.%d_%d verification of calibration\n", i, l);
											CompletionImpl comp = usd[i][l]->calVer();
											if (comp.isErrorFree() == false)
												checkAScompletionerrors("usd", i, l, comp);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0) {
									try {
										printf("actuator n.%d_%d verification of calibration\n", circle, l);
										CompletionImpl comp = usd[circle][l]->calVer();
										if (comp.isErrorFree() == false)
											checkAScompletionerrors("usd", circle, l, comp);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++)  {
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0) {
									try {
										printf("actuator n.%d_%d verification of calibration\n", l+jumpradius, radius);
										CompletionImpl comp = lanradius[l+jumpradius][radius]->calVer();
										if (comp.isErrorFree() == false)
											checkAScompletionerrors("lanradius", l+jumpradius, radius, comp);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									printf("actuator n.%d_%d verification of calibration\n", circle, actuator);
									CompletionImpl comp = usd[circle][actuator]->calVer();
									if (comp.isErrorFree() == false)
										checkAScompletionerrors("usd", circle, actuator, comp);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
		      				break;

		    				case 'p':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->refPos();
		      									printf ("actuator n.%d_%d, refPos\n", i, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									//try {
										usd[circle][l]->refPos();
		      								printf ("actuator n.%d_%d, refPos\n", circle, l);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", circle, l, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									//try {
										lanradius[l+jumpradius][radius]->refPos();
		      								printf ("actuator n.%d_%d, refPos\n", l+jumpradius, radius);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", l+jumpradius, radius, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								//try {
									usd[circle][actuator]->refPos();
		      							printf ("actuator n.%d_%d, refPos\n", circle, actuator);
								//}
								//catch (ASErrors::ASErrorsEx &Ex) {
								//	checkASerrors("actuator", circle, actuator, Ex);
								//}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						/*if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int s, l;
							for (s = 1; s <= 8; s++)
							{
								for (l = 1; l <= 12; l++)
								{
									if (laninitialized[s][l] == 0)
									{
										try {
											CompletionImpl compStop = lan[s][l]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
											if (compStop.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compStop);
											printf ("lan n.%d_%d, stop before refPos\n", s, l);
											CompletionImpl compRefPos = lan[s][l]->sendUSDCmd(CPOS, 0, 0, 4);
											if (compRefPos.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compRefPos);
											printf ("lan n.%d_%d, refPos\n", s, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("lan", s, l, Ex);
										}
									}
									else 
										printf ("lan n.%d_%d not initialized!\n", s, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->refPos ();
		      								printf ("actuator n.%d_%d, refPos\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							setradius2(radius, sector, sectorradius);
							if (laninitialized[sector][sectorradius] == 0)
							{
								try {
									CompletionImpl compStop = lan[sector][sectorradius]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
									if (compStop.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compStop);
									printf ("lan n.%d_%d, stop before refPos\n", sector, sectorradius);
									CompletionImpl compRefPos = lan[sector][sectorradius]->sendUSDCmd(CPOS, 0, 0, 4);
									if (compRefPos.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compRefPos);
									printf ("lan n.%d_%d, refPos\n", sector, sectorradius);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("lan", sector, sectorradius, Ex);
								}
							}
							else 
								printf ("lan n.%d_%d not initialized!\n", sector, sectorradius);
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->refPos ();
		      							printf ("actuator n.%d_%d, refPos\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}*/
		      				break;

		    				case 'q':
		      				printf ("set profile: (0 shaped, 1 parabolic)\n");
		      				long profile;
		      				std::cin >> profile;
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->setProfile(profile);
		      									printf ("actuator n.%d_%d, setProfile with %ld profile\n", i, l, profile);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->setProfile (profile);
		      								printf ("actuator n.%d_%d, setProfile with %ld profile\n", circle, l, profile);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										lanradius[l+jumpradius][radius]->setProfile (profile);
		      								printf ("actuator n.%d_%d, setProfile with %ld profile\n", l+jumpradius, radius, profile);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->setProfile (profile);
		      							printf ("actuator n.%d_%d, setProfile with %ld profile\n", circle, actuator, profile);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 'r':
		      				printf ("set correction: ?\n");
		      				double correction;
		      				std::cin >> correction;
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->correction(correction);
		      									printf ("actuator n.%d_%d, correction with %f correction\n", i, l, correction);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->correction (correction);
		      								printf ("actuator n.%d_%d, correction with %f correction\n", circle, l, correction);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										lanradius[l+jumpradius][radius]->correction (correction);
		      								printf ("actuator n.%d_%d, correction with %f correction\n", l+jumpradius, radius, correction);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->correction (correction);
		      							printf ("actuator n.%d_%d, correction with %f correction\n", circle, actuator, correction);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 's':
		      				printf ("update: insert elevation value\n");
		      				double elevation;
		      				std::cin >> elevation;
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->update(elevation);
		      									printf ("actuator n.%d_%d, update with %f elevation\n", i, l, elevation);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->update (elevation);
		      								printf ("actuator n.%d_%d, update with %f elevation\n", circle, l, elevation);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									try {
										lanradius[l+jumpradius][radius]->update (elevation);
		      								printf ("actuator n.%d_%d, update with %f elevation\n", l+jumpradius, radius, elevation);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", l+jumpradius, radius, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->update (elevation);
		      							printf ("actuator n.%d_%d, update with %f elevation\n", circle, actuator, elevation);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						break;

		    				case 't':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->stow();
		      									printf ("actuator n.%d_%d, stow\n", i, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									//try {
										usd[circle][l]->stow();
		      								printf ("actuator n.%d_%d, stow\n", circle, l);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", circle, l, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									//try {
										lanradius[l+jumpradius][radius]->stow();
		      								printf ("actuator n.%d_%d, stow\n", l+jumpradius, radius);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", l+jumpradius, radius, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								//try {
									usd[circle][actuator]->stow();
		      							printf ("actuator n.%d_%d, stow\n", circle, actuator);
								//}
								//catch (ASErrors::ASErrorsEx &Ex) {
								//	checkASerrors("actuator", circle, actuator, Ex);
								//}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						/*if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int s, l;
							for (s = 1; s <= 8; s++)
							{
								for (l = 1; l <= 12; l++)
								{
									if (laninitialized[s][l] == 0)
									{
										try {
											CompletionImpl compStop = lan[s][l]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
											if (compStop.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compStop);
											printf ("lan n.%d_%d, stop before stow\n", s, l);
											CompletionImpl compStow = lan[s][l]->sendUSDCmd(STOW, 0, -10000, 4); 
											if (compStow.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compStow);
											printf ("lan n.%d_%d, stow\n", s, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("lan", s, l, Ex);
										}
									}
									else 
										printf ("lan n.%d_%d not initialized!\n", s, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->stow ();
		      								printf ("actuator n.%d_%d, stow\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							setradius2(radius, sector, sectorradius);
							if (laninitialized[sector][sectorradius] == 0)
							{
								try {
									CompletionImpl compStop = lan[sector][sectorradius]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
									if (compStop.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compStop);
									printf ("lan n.%d_%d, stop before stow\n", sector, sectorradius);
									CompletionImpl compStow = lan[sector][sectorradius]->sendUSDCmd(STOW, 0, -10000, 4);
									if (compStow.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compStow);
									printf ("lan n.%d_%d, stow\n", sector, sectorradius);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("lan", sector, sectorradius, Ex);
								}
							}
							else 
								printf ("lan n.%d_%d not initialized!\n", sector, sectorradius);
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->stow ();
		      							printf ("actuator n.%d_%d, stow\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}*/
						break;

		    				case 'u':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->setup();
		      									printf ("actuator n.%d_%d, setup\n", i, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									//try {
										usd[circle][l]->setup();
		      								printf ("actuator n.%d_%d, setup\n", circle, l);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", circle, l, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									//try {
										lanradius[l+jumpradius][radius]->setup();
		      								printf ("actuator n.%d_%d, setup\n", l+jumpradius, radius);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", l+jumpradius, radius, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								//try {
									usd[circle][actuator]->setup();
		      							printf ("actuator n.%d_%d, setup\n", circle, actuator);
								//}
								//catch (ASErrors::ASErrorsEx &Ex) {
								//	checkASerrors("actuator", circle, actuator, Ex);
								//}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						/*if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int s, l;
							for (s = 1; s <= 8; s++)
							{
								for (l = 1; l <= 12; l++)
								{
									if (laninitialized[s][l] == 0)
									{
										try {
											CompletionImpl compStop = lan[s][l]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
											if (compStop.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compStop);
											printf ("lan n.%d_%d, stop before setup\n", s, l);
											CompletionImpl compSetup = lan[s][l]->sendUSDCmd(SETUP, 0, 0, 4); 
											if (compSetup.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compSetup);
											printf ("lan n.%d_%d, setup\n", s, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("lan", s, l, Ex);
										}
									}
									else 
										printf ("lan n.%d_%d not initialized!\n", s, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->setup ();
		      								printf ("actuator n.%d_%d, setup\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							setradius2(radius, sector, sectorradius);
							if (laninitialized[sector][sectorradius] == 0)
							{
								try {
									CompletionImpl compStop = lan[sector][sectorradius]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
									if (compStop.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compStop);
									printf ("lan n.%d_%d, stop before setup\n", sector, sectorradius);
									CompletionImpl compSetup = lan[sector][sectorradius]->sendUSDCmd(SETUP, 0, 0, 4);
									if (compSetup.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compSetup);
									printf ("lan n.%d_%d, setup\n", sector, sectorradius);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("lan", sector, sectorradius, Ex);
								}
							}
							else 
								printf ("lan n.%d_%d not initialized!\n", sector, sectorradius);
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->setup ();
		      							printf ("actuator n.%d_%d, setup\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}*/
						break;

		    				case 'v':
		      				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->top();
		      									printf ("actuator n.%d_%d, top\n", i, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									//try {
										usd[circle][l]->top();
		      								printf ("actuator n.%d_%d, top\n", circle, l);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", circle, l, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									//try {
										lanradius[l+jumpradius][radius]->top();
		      								printf ("actuator n.%d_%d, top\n", l+jumpradius, radius);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", l+jumpradius, radius, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								//try {
									usd[circle][actuator]->top();
		      							printf ("actuator n.%d_%d, top\n", circle, actuator);
								//}
								//catch (ASErrors::ASErrorsEx &Ex) {
								//	checkASerrors("actuator", circle, actuator, Ex);
								//}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						/*if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int s, l;
							for (s = 1; s <= 8; s++)
							{
								for (l = 1; l <= 12; l++)
								{
									if (laninitialized[s][l] == 0)
									{
										try {
											CompletionImpl compStop = lan[s][l]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
											if (compStop.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compStop);
											printf ("lan n.%d_%d, stop before top\n", s, l);
											CompletionImpl compTop = lan[s][l]->sendUSDCmd(CPOS, 0, TOP<<USxS, 4);
											if (compTop.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compTop);
											printf ("lan n.%d_%d, top\n", s, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("lan", s, l, Ex);
										}
									}
									else 
										printf ("lan n.%d_%d not initialized!\n", s, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->top ();
		      								printf ("actuator n.%d_%d, top\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							setradius2(radius, sector, sectorradius);
							if (laninitialized[sector][sectorradius] == 0)
							{
								try {
									CompletionImpl compStop = lan[sector][sectorradius]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
									if (compStop.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compStop);
									printf ("lan n.%d_%d, stop before top\n", sector, sectorradius);
									CompletionImpl compTop = lan[sector][sectorradius]->sendUSDCmd(CPOS, 0, TOP<<USxS, 4);
									if (compTop.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compTop);
									printf ("lan n.%d_%d, top\n", sector, sectorradius);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("lan", sector, sectorradius, Ex);
								}
							}
							else 
								printf ("lan n.%d_%d not initialized!\n", sector, sectorradius);
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->top ();
		      							printf ("actuator n.%d_%d, top\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}*/
						break;

		    				case 'w':
		     				if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
                            				int i, l;
							for (i = 1; i <= CIRCLES; i++)
    							{
								for (l = 1; l <= actuatorsInCircle[i]; l++) 
								{
									if (usdinitialized[i][l] == 0)
									{
										try {
											usd[i][l]->bottom();
		      									printf ("actuator n.%d_%d, bottom\n", i, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("actuator", i, l, Ex);
										}
									catch(CORBA::SystemException &E) {
			_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.setName(E._name());
			impl.setMinor(E.minor());
			impl.log();
			//goto CloseLabel;
		}
		catch(...) {
			_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTActiveSurfaceTextClient::Main()");
			impl.log();
			//goto CloseLabel;
   		}}
									else 
										printf ("actuator n.%d_%d not initialized!\n", i, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									//try {
										usd[circle][l]->bottom();
		      								printf ("actuator n.%d_%d, bottom\n", circle, l);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", circle, l, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							int actuatorsradius;
							int jumpradius;
							setradius(radius, actuatorsradius, jumpradius);
							int l;
							for (l = 1; l <= actuatorsradius; l++) 
							{
								if ((radius == 13 || radius == 37 || radius == 61 || radius == 85) && l == 14)
									jumpradius++;  // 17 circle
								if (lanradiusinitialized[l+jumpradius][radius] == 0)
								{
									//try {
										lanradius[l+jumpradius][radius]->bottom();
		      								printf ("actuator n.%d_%d, bottom\n", l+jumpradius, radius);
									//}
									//catch (ASErrors::ASErrorsEx &Ex) {
									//	checkASerrors("actuator", l+jumpradius, radius, Ex);
									//}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", l+jumpradius, radius);
							}
						}
						else {
                            				if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								//try {
									usd[circle][actuator]->bottom();
		      							printf ("actuator n.%d_%d, bottom\n", circle, actuator);
								//}
								//catch (ASErrors::ASErrorsEx &Ex) {
								//	checkASerrors("actuator", circle, actuator, Ex);
								//}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}
						/*if (circle == 0 && actuator == 0 &&radius == 0) // ALL
						{
							int s, l;
							for (s = 1; s <= 8; s++)
							{
								for (l = 1; l <= 12; l++)
								{
									if (laninitialized[s][l] == 0)
									{
										try {
											CompletionImpl compStop = lan[s][l]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
											if (compStop.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compStop);
											printf ("lan n.%d_%d, stop before bottom\n", s, l);
											CompletionImpl compBottom = lan[s][l]->sendUSDCmd(CPOS, 0, BOTTOM<<USxS, 4);
											if (compBottom.isErrorFree() == false)
												checkAScompletionerrors("lan", s, l, compBottom);
											printf ("lan n.%d_%d, bottom\n", s, l);
										}
										catch (ASErrors::ASErrorsEx &Ex) {
											checkASerrors("lan", s, l, Ex);
										}
									}
									else 
										printf ("lan n.%d_%d not initialized!\n", s, l);
								}
							}
						}
						else if (circle != 0 && actuator == 0 && radius == 0) { // CIRCLE
							int l;
							for (l = 1; l <= actuatorsInCircle[circle]; l++)
							{
								if (usdinitialized[circle][l] == 0)
								{
									try {
										usd[circle][l]->bottom ();
		      								printf ("actuator n.%d_%d, bottom\n", circle, l);
									}
									catch (ASErrors::ASErrorsEx &Ex) {
										checkASerrors("actuator", circle, l, Ex);
									}
								}
								else
									printf ("actuator n.%d_%d not initialized!\n", circle, l);
							}
						}
						else if (circle == 0 && actuator == 0 && radius != 0) { // RADIUS
							setradius2(radius, sector, sectorradius);
							if (laninitialized[sector][sectorradius] == 0)
							{
								try {
									CompletionImpl compStop = lan[sector][sectorradius]->sendUSDCmd(STOP, 0, 0, 0); // must stop the usd motor before
									if (compStop.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compStop);
									printf ("lan n.%d_%d, stop before bottom\n", sector, sectorradius);
									CompletionImpl compBottom = lan[sector][sectorradius]->sendUSDCmd(CPOS, 0, BOTTOM<<USxS, 4);
									if (compBottom.isErrorFree() == false)
										checkAScompletionerrors("lan", sector, sectorradius, compBottom);
									printf ("lan n.%d_%d, bottom\n", sector, sectorradius);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("lan", sector, sectorradius, Ex);
								}
							}
							else 
								printf ("lan n.%d_%d not initialized!\n", sector, sectorradius);
						}
						else {
							if (usdinitialized[circle][actuator] == 0) { // SINGLE ACTUATOR
								try {
									usd[circle][actuator]->bottom ();
		      							printf ("actuator n.%d_%d, bottom\n", circle, actuator);
								}
								catch (ASErrors::ASErrorsEx &Ex) {
									checkASerrors("actuator", circle, actuator, Ex);
								}
							}
							else 
								printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
						}*/
		      				break;
		    			}
				}
	      			else
					printf ("actuator n.%d_%d not initialized!\n", circle, actuator);
	    		}
	  		init_keyboard ();
		}
		/*if (usdinitialized[i][l] == 0)
		{
			actPos_var = usd[i][l]->actPos ();
			actPos_val = actPos_var->get_sync (completion.out ());
			printf("looping actuator n.%d_%d: position = %ld\n", i, l, actPos_val);
		}
		totalactuators++;
		l++;
		if (totalactuators == 1117) {
			i = l = totalactuators = 1;
		}
		circlecounter = i;
		actuatorcounter = l;
		totacts = totalactuators;

      		ACE_OS::sleep (1); */
		client->run(tv);
		tv.set(0,MAINTHREADSLEEPTIME*1000);
	}
  	//while (command != 'x');
  	close_keyboard ();


CloseLabel:
/*
	// Releasing usd
	ACS_SHORT_LOG ((LM_INFO, "Releasing..."));
	printf("releasing usd....\n");
	usdTable.seekg(0, ios::beg);
	for (int i = firstUSD; i <= lastUSD; i++) {
		try {
			if (usdinitialized[circleIndex][usdCircleIndex] == 0) {
				usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc;
				client->releaseComponent (serial_usd);
				//printf("released %d usd %d_%d\n", i, circleIndex, usdCircleIndex);
				printf("released %d usd %s\n", i, (const char*)serial_usd);
			}
		}
		catch (maciErrType::CannotReleaseComponentExImpl &_ex)
		{
			_ex.log();
		}
		catch(...)
		{
			ACSErrTypeCommon::UnexpectedExceptionExImpl uex(__FILE__, __LINE__, "SRTActiveSurfaceTextClient::Main()");
			uex.log();
		}
	}

	// Releasing lan
	printf("releasing lan....\n");
	for (int s = 1; s <= 8; s++)
	{
		for (int l = 1; l <= 12; l++)
		{
			try {
				if (laninitialized[s][l] == 0) {
					lanCobName.Format("AS/SECTOR%02d/LAN%02d",s, l);
					client->releaseComponent ((const char*)lanCobName);
					printf("released lan %s\n", (const char*)lanCobName);
				}
			}
			catch (maciErrType::CannotReleaseComponentExImpl &_ex)
			{
				_ex.log();
			}
			catch(...)
			{
				ACSErrTypeCommon::UnexpectedExceptionExImpl uex(__FILE__, __LINE__, "SRTActiveSurfaceTextClient::Main()");
				uex.log();
			}
		}
	}
*/
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

void checkAScompletionerrors(char* str, int circle, int actuator, CompletionImpl comp)
{
	printf("checkAScompletionerrors: ");
	printf("%s %d_%d ", str, circle, actuator);

	switch(comp.getCode()) {
		case 0:
			printf("NoError\n");
			break;
		case 1:
			printf("LibrarySocketError\n");
			break;
		case 2:
			printf("SocketReconn\n");
			break;
		case 3: 
			printf("SocketFail\n");
			break;
		case 4:
			printf("SocketTOut\n");
			break;
		case 5: 
			printf("SocketNotRdy\n");
			break;
		case 6: 
			printf("MemoryAllocation\n");
			break;
		case 7: 
			printf("Incomplete\n");
			break;
		case 8: 
			printf("InvalideResponse\n");
			break;
		case 9: 
			printf("Nak\n");
			break;
		case 10: 
			printf("CDBAccessError\n");
			break;
		case 11: 
			printf("USDConnectionError\n");
			break;
		case 12: 
			printf("USDTimeout\n");
			break;
		case 13: 
			printf("LANConnectionError\n");
			break;
		case 14: 
			printf("LANUnavailable\n");
			break;
		case 15: 
			printf("sendCmdErr\n");
			break;
		case 16: 
			printf("USDUnavailable\n");
			break;
		case 17: 
			printf("USDError\n");
			break;
		default: 
			printf("Unknown Error\n");
			break;
	}
}

void checkASerrors(char* str, int circle, int actuator, ASErrors::ASErrorsEx Ex)
{
	ASErrors::ASErrorsExImpl exImpl(Ex);

	printf("checkASerrors: ");
	printf("%s %d_%d: ", str, circle, actuator);

	switch(exImpl.getErrorCode()) {
		case ASErrors::NoError:
			printf("NoError\n");
			break;
		case ASErrors::LibrarySocketError:
			printf("LibrarySocketError\n");
			break;
		case ASErrors::SocketReconn:
			printf("SocketReconn\n");
			break;
		case ASErrors::SocketFail: 
			printf("SocketFail\n");
			break;
		case ASErrors::SocketTOut:
			printf("SocketTOut\n");
			break;
		case ASErrors::SocketNotRdy: 
			printf("SocketNotRdy\n");
			break;
		case ASErrors::MemoryAllocation: 
			printf("MemoryAllocation\n");
			break;
		case ASErrors::Incomplete: 
			printf("Incomplete\n");
			break;
		case ASErrors::InvalidResponse: 
			printf("InvalideResponse\n");
			break;
		case ASErrors::Nak: 
			printf("Nak\n");
			break;
		case ASErrors::CDBAccessError: 
			printf("CDBAccessError\n");
			break;
		case ASErrors::USDConnectionError: 
			printf("USDConnectionError\n");
			break;
		case ASErrors::USDTimeout: 
			printf("USDTimeout\n");
			break;
		case ASErrors::LANConnectionError: 
			printf("LANConnectionError\n");
			break;
		case ASErrors::LAN_Unavailable: 
			printf("LANUnavailable\n");
			break;
		case ASErrors::sendCmdErr: 
			printf("sendCmdErr\n");
			break;
		case ASErrors::USDUnavailable: 
			printf("USDUnavailable\n");
			break;
		case ASErrors::USDError: 
			printf("USDError\n");
			break;
		default: 
			printf("Unknown Error\n");
			break;
	}
}

void setradius(int radius, int &actuatorsradius, int &jumpradius)
{
	if (radius ==  1 || radius ==  5 || radius ==  9 || radius == 13 ||
	    radius == 17 || radius == 21 || radius == 25 || radius == 29 ||
	    radius == 33 || radius == 37 || radius == 41 || radius == 45 ||
	    radius == 49 || radius == 53 || radius == 57 || radius == 61 ||
	    radius == 65 || radius == 69 || radius == 73 || radius == 77 ||
	    radius == 81 || radius == 85 || radius == 89 || radius == 93)
	{
		actuatorsradius = 13;
		if (radius == 13 || radius == 37 || radius == 61 || radius == 85)
			actuatorsradius++;
		jumpradius = 2;
	}
	else if (radius ==  3 || radius ==  7 || radius == 11 || radius == 15 ||
	    	 radius == 19 || radius == 23 || radius == 27 || radius == 31 ||
	    	 radius == 35 || radius == 39 || radius == 43 || radius == 47 ||
	    	 radius == 51 || radius == 55 || radius == 59 || radius == 63 ||
	    	 radius == 67 || radius == 71 || radius == 75 || radius == 79 ||
	    	 radius == 83 || radius == 87 || radius == 91 || radius == 95)
	{
		actuatorsradius = 15;
		if (radius == 11 || radius == 15 || radius == 35 || radius == 39 ||
		    radius == 59 || radius == 63 || radius == 83 || radius == 87)
			actuatorsradius++;
		jumpradius = 0;
	}
	else
	{
		actuatorsradius = 9;
		jumpradius = 6;
	}
}

void setradius2 (int radius, int &sector, int &sectorradius)
{
	if (radius >= 1 && radius <= 12)
		sector = 1;
	if (radius >= 13 && radius <= 24)
		sector = 2;
	if (radius >= 25 && radius <= 36)
		sector = 3;
	if (radius >= 37 && radius <= 48)
		sector = 4;
	if (radius >= 49 && radius <= 60)
		sector = 5;
	if (radius >= 61 && radius <= 72)
		sector = 6;
	if (radius >= 73 && radius <= 84)
		sector = 7;
	if (radius >= 85 && radius <= 96)
		sector = 8;

	sectorradius = radius-12*(sector-1);
}

void setserial (int circle, int actuator, int &lan, char* serial_usd)
{
    int sector = 0;

    if (circle >= 1 && circle <= 2) {
        if (actuator >= 1 && actuator <= 3)
                sector = 1;
        if (actuator >= 4 && actuator <= 6)
                sector = 2;
        if (actuator >= 7 && actuator <= 9)
                sector = 3;
        if (actuator >= 10 && actuator <= 12)
                sector = 4;
        if (actuator >= 13 && actuator <= 15)
                sector = 5;
        if (actuator >= 16 && actuator <= 18)
                sector = 6;
        if (actuator >= 19 && actuator <= 21)
                sector = 7;
        if (actuator >= 22 && actuator <= 24)
                sector = 8;
    }
    if (circle >= 3 && circle <= 6) {
        if (actuator >= 1 && actuator <= 6)
                sector = 1;
        if (actuator >= 7 && actuator <= 12)
                sector = 2;
        if (actuator >= 13 && actuator <= 18)
                sector = 3;
        if (actuator >= 19 && actuator <= 24)
                sector = 4;
        if (actuator >= 25 && actuator <= 30)
                sector = 5;
        if (actuator >= 31 && actuator <= 36)
                sector = 6;
        if (actuator >= 37 && actuator <= 42)
                sector = 7;
        if (actuator >= 43 && actuator <= 48)
                sector = 8;
    }
    if (circle >= 7 && circle <= 15) {
        if (actuator >= 1 && actuator <= 12)
                sector = 1;
        if (actuator >= 13 && actuator <= 24)
                sector = 2;
        if (actuator >= 25 && actuator <= 36)
                sector = 3;
        if (actuator >= 37 && actuator <= 48)
                sector = 4;
        if (actuator >= 49 && actuator <= 60)
                sector = 5;
        if (actuator >= 61 && actuator <= 72)
                sector = 6;
        if (actuator >= 73 && actuator <= 84)
                sector = 7;
        if (actuator >= 85 && actuator <= 96)
                sector = 8;
    }
    if (circle == 16)
        sector = actuator;
    if (circle == 17)
        sector = actuator*2;

    lan = actuator - 12*(sector-1);

    sprintf (serial_usd,"AS/SECTOR%02d/LAN%02d/USD%02d",sector, lan, circle);
}
