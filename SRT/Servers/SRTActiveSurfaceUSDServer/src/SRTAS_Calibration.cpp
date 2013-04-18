#include <maciSimpleClient.h>
#include <IRA>
//#include <usdC.h>
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
#include <SRTActiveSurfaceBossC.h>

#define MAINTHREADSLEEPTIME 15000
#define CIRCLES 17
#define ACTUATORS 96
#define firstUSD 1
#define lastUSD 1116
#define USDTABLE "/CDB/alma/AS/tab_convUSD.txt\0"
#define ASCALIBRATION "/CDB/alma/AS/AScalibration.txt\0"

int actuatorsInCircle[] = {0,24,24,48,48,48,48,96,96,96,96,96,96,96,96,96,8,4};

using namespace std;
using namespace maci;
using namespace ASErrors;
using namespace IRA;

void checkAScompletionerrors (char *str, int circle, int actuator,
			      CompletionImpl comp);
void checkASerrors (char *str, int circle, int actuator,
		    ASErrors::ASErrorsEx Ex);
void setradius(int radius, int &actuatorsradius, int &jumpradius);

int main (int argc, char *argv[])
{
	SimpleClient *client = new SimpleClient;
  	ACSErr::Completion_var completion;
  	bool loggedIn = false;
  	char serial_usd[23];
  	char graf[6], mecc[4];
  	int lanIndex, circleIndex, usdCircleIndex;
  	//SRTActiveSurface::USD_var usd[CIRCLES+1][ACTUATORS+1];
	//SRTActiveSurface::USD_var lanradius[CIRCLES+1][ACTUATORS+1];
	int usdinitialized[CIRCLES+1][ACTUATORS+1];
  	int i, l, m;
	long theCircle;
	long theActuator;
	long theRadius;
	char *endptr1, *str1, *endptr2, *str2, *endptr3, *str3;
	int base;
	char *s_usdTable;
	SRTActiveSurface::SRTActiveSurfaceBoss_var ASBoss;

	ACE_Time_Value tv(1);
/*
        char *s_AScalibration;
	TIMEVALUE tS;
	TIMEVALUE time;
	IRA::CString out;

	s_AScalibration = getenv ("ACS_CDB");
	strcat(s_AScalibration,ASCALIBRATION);
	fstream ASCalibration;
        ASCalibration.open (s_AScalibration, ios_base::out | ios_base::app);
	if (!ASCalibration.is_open()) {
                printf("not found\n");
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", s_AScalibration));
		exit(-1);
	}
	IRA::CIRATools::getTime (time);
	tS.value(time.value().value);
	out.Format("SRT-AS-CALIBRATION-%04d.%03d.%02d:%02d:%02d.%02d\n", tS.year (), tS.dayOfYear (), tS.hour (), tS.minute (), tS.second (),(long)(tS.microSecond () / 10000.));
        std::cout << out;
	ASCalibration << 100 << 200 << std::endl;
        exit(-1);
*/
/*
	s_usdTable = getenv ("ACS_CDB");
	strcat(s_usdTable,USDTABLE);
	ifstream usdTable(s_usdTable);
	if (!usdTable) {
		ACS_SHORT_LOG ((LM_INFO, "File %s not found", s_usdTable));
		exit(-1);
	}

	for (l = 1; l <= CIRCLES; l++)
	{
		for (m = 1; m <= actuatorsInCircle[l]; m++)
		{
			usdinitialized[l][m] = -1;
		}
	}
*/
	base = 10;
	str1 = argv[1];
	str2 = argv[2];
	str3 = argv[3];
	theCircle = strtol(str1, &endptr1, base);
	theActuator = strtol(str2, &endptr2, base);
	theRadius = strtol(str3, &endptr3, base);

  	try {
		if (client->init (argc, argv) == 0) {
          		_EXCPT (ClientErrors::CouldntInitExImpl, impl, "SRTAS_Calibration::Main()");
          		impl.log ();
          		delete client;
      		}
      		if (client->login () == 0) {
        		_EXCPT (ClientErrors::CouldntLoginExImpl, impl, "SRTAS_Calibration::Main()");
          		impl.log ();
          		delete client;
      		}
      		ACS_SHORT_LOG ((LM_INFO, "client %s activated", "SRTAS_Calibration"));
      		loggedIn = true;
  	}
  	catch (...) {
      		_EXCPT (ClientErrors::UnknownExImpl, impl, "SRTAS_Calibration::Main()");
      		impl.log ();
      		delete client;
  	}
  	ACS_SHORT_LOG ((LM_INFO, "OK activation SRTAS_Calibration: %s", "SRTAS_Calibration"));

	// Get reference to ASBoss component
      		try {
			ASBoss = client->getComponent < SRTActiveSurface::SRTActiveSurfaceBoss > ("AS/Boss", 0, true);
			if (CORBA::is_nil (ASBoss.in ()) == true) {
				_EXCPT (ClientErrors::CouldntAccessComponentExImpl, impl, "SRTAS_Calibration::Main()");
				impl.setComponentName ("AS/Boss");
				impl.log ();
			}
		}
		catch (maciErrType::CannotGetComponentExImpl & E) {
			_EXCPT (ClientErrors::CouldntAccessComponentExImpl, impl, "SRTAS_Calibration::Main()");
			impl.log ();
		}
		catch (CORBA::SystemException & E) {
			_EXCPT (ClientErrors::CORBAProblemExImpl, impl, "SRTAS_Calibration::Main()");
			impl.setName (E._name ());
			impl.setMinor (E.minor ());
			impl.log ();
		}
		catch (...) {
			_EXCPT (ClientErrors::UnknownExImpl, impl, "SRTAS_Calibration::Main()");
			impl.log ();
		}
/*
	// Get reference to usd components
	for (i = firstUSD; i <= lastUSD; i++) {
		usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc;
		usdinitialized[circleIndex][usdCircleIndex] = -1; // 0 ok, -1 fault
      		try {
			usd[circleIndex][usdCircleIndex] = client->getComponent < SRTActiveSurface::USD > (serial_usd, 0, true);
			lanradius[circleIndex][lanIndex] = usd[circleIndex][usdCircleIndex];
			if (CORBA::is_nil (usd[circleIndex][usdCircleIndex].in ()) == true) {
				_EXCPT (ClientErrors::CouldntAccessComponentExImpl, impl, "SRTActiveSurfaceTextClient::Main()");
				impl.setComponentName (serial_usd);
				impl.log ();
				usdinitialized[circleIndex][usdCircleIndex] = -1;
			}
			else {
				usdinitialized[circleIndex][usdCircleIndex] = 0;
			}
		}
		catch (maciErrType::CannotGetComponentExImpl & E) {
			_EXCPT (ClientErrors::CouldntAccessComponentExImpl, impl, "SRTAS_Calibration::Main()");
			impl.log ();
		}
		catch (CORBA::SystemException & E) {
			_EXCPT (ClientErrors::CORBAProblemExImpl, impl, "SRTAS_Calibration::Main()");
			impl.setName (E._name ());
			impl.setMinor (E.minor ());
			impl.log ();
		}
		catch (...) {
			_EXCPT (ClientErrors::UnknownExImpl, impl, "SRTAS_Calibration::Main()");
			impl.log ();
		}
	}

	if (theCircle == 0 && theActuator == 0 && theRadius == 0) { // ALL
		printf("up.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (usdinitialized[i][l] == 0) {
					try {
						usd[i][l]->up();
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (90);

		printf("calibration.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (usdinitialized[i][l] == 0) {
					try {
						CIRATools::Wait(0,250000);	// 0.25 secs per usd = 279 secs
						usd[i][l]->calibrate ();
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
		}
		ACE_OS::sleep (60);

		printf ("calibration verification.....\n");
		for (i = 1; i <= CIRCLES; i++) {
			for (l = 1; l <= actuatorsInCircle[i]; l++) {
				if (usdinitialized[i][l] == 0) {
					try {
						CIRATools::Wait(0,250000);	// 0.25 secs per usd = 279 secs
						usd[i][l]->calVer ();
					}
					catch (ASErrors::ASErrorsEx & Ex) {
						checkASerrors ("actuator", i, l, Ex);
					}
				}
			}
        	}
		ACE_OS::sleep (240);
	}
	else if (theCircle != 0 && theActuator == 0 && theRadius == 0) { // CIRCLE
		printf("up.....\n");
		for (l = 1; l <= actuatorsInCircle[theCircle]; l++) {
            		if (usdinitialized[theCircle][l] == 0) {
                		try {
					usd[theCircle][l]->up();
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", theCircle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (90);
					
		printf("calibration.....\n");
		for (l = 1; l <= actuatorsInCircle[theCircle]; l++) {
            		if (usdinitialized[theCircle][l] == 0) {
                		try {
					CIRATools::Wait(0,250000);	// 0.25 secs per usd = 279 secs
					usd[theCircle][l]->calibrate ();
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", theCircle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (60);

		printf ("calibration verification.....\n");
		for (l = 1; l <= actuatorsInCircle[theCircle]; l++) {
            		if (usdinitialized[theCircle][l] == 0) {
                		try {
					CIRATools::Wait(0,250000);	// 0.25 secs per usd = 279 secs
					usd[theCircle][l]->calVer ();
				}
				catch (ASErrors::ASErrorsEx & Ex) {
					checkASerrors ("actuator", theCircle, l, Ex);
				}
			}
		}
		ACE_OS::sleep (240);
	}
	else if (theCircle == 0 && theActuator == 0 && theRadius != 0) { // RADIUS
		int actuatorsradius;
        	int jumpradius;
        	setradius(theRadius, actuatorsradius, jumpradius);
        	int l;
		printf("up.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((theRadius == 13 || theRadius == 37 || theRadius == 61 || theRadius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][theRadius])) {
                		try {
					lanradius[l+jumpradius][theRadius]->up();
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, theRadius, Ex);
				}
			}
		}
		ACE_OS::sleep (90);
					
		printf("calibration.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((theRadius == 13 || theRadius == 37 || theRadius == 61 || theRadius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][theRadius])) {
                		try {
					CIRATools::Wait(0,250000); // 0.25 secs per usd = 279 secs
					lanradius[l+jumpradius][theRadius]->calibrate();
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, theRadius, Ex);
				}
			}
		}
		ACE_OS::sleep (60);

		printf ("calibration verification.....\n");
		for (l = 1; l <= actuatorsradius; l++) {
            		if ((theRadius == 13 || theRadius == 37 || theRadius == 61 || theRadius == 85) && l == 14)
                		jumpradius++;  // 17 circle
            		if (!CORBA::is_nil(lanradius[l+jumpradius][theRadius])) {
                		try {
					CIRATools::Wait(0,250000); // 0.25 secs per usd = 279 secs
					lanradius[l+jumpradius][theRadius]->calVer();
                		}
				catch (ASErrors::ASErrorsEx &Ex) {
					checkASerrors("actuator", l+jumpradius, theRadius, Ex);
				}
			}
		}
		ACE_OS::sleep (240);
	}
	else {
		if (!CORBA::is_nil(usd[theCircle][theActuator])) { // SINGLE ACTUATOR
			printf("up.....\n");
			try {
				usd[theCircle][theActuator]->up();
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", theCircle, theActuator, Ex);
			}
			ACE_OS::sleep (90);
			printf("calibration.....\n");
			try {
				usd[theCircle][theActuator]->calibrate();
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", theCircle, theActuator, Ex);
			}
			ACE_OS::sleep (10);
			printf ("calibration verification.....\n");
			try {
				usd[theCircle][theActuator]->calVer();
			}
			catch (ASErrors::ASErrorsEx &Ex) {
				checkASerrors("actuator", theCircle, theActuator, Ex);
			}
			ACE_OS::sleep (180);
		}
	}
*/
	try {
       		ASBoss->calibrate(theCircle, theActuator, theRadius);
		
    	}
    	catch (ComponentErrors::ComponentErrorsExImpl& ex) {
		ex.log(LM_DEBUG);
    	}
    	catch (CORBA::SystemException& sysEx) {
        	_EXCPT(ClientErrors::CORBAProblemExImpl,impl,"SRTAS_Calibration::main()");
        	impl.setName(sysEx._name());
        	impl.setMinor(sysEx.minor());
        	impl.log();
    	}
    	catch (...) {
        	_EXCPT(ClientErrors::UnknownExImpl,impl,"SRTAS_Calibration::main()");
        	impl.log();
    	}

	client->run(tv);
    	tv.set(0,MAINTHREADSLEEPTIME);

	// Releasing components TBD!!
	/*usdTable.seekg(0, ios::beg);
	for (i = firstUSD; i <= lastUSD; i++) {
		usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc;
      		if (usdinitialized[circleIndex][usdCircleIndex] == 0) {
			client->manager()->release_component(client->handle(),(const char*)serial_usd);
		}
	}*/
		
	// Releasing client
	if (loggedIn)
  	{
    		client->logout ();
    		delete client;
  	}

	//Sleep for 2 sec to allow everytihng to cleanup and stabilize
	ACE_OS::sleep (2);

	ACS_SHORT_LOG ((LM_INFO, "Done..."));

	return 0;
}

void
checkAScompletionerrors (char *str, int circle, int actuator,
			 CompletionImpl comp)
{
  printf ("checkAScompletionerrors: ");
  printf ("%s %d_%d ", str, circle, actuator);

  switch (comp.getCode ())
    {
    case 0:
      printf ("NoError\n");
      break;
    case 1:
      printf ("USD calibrated\n");
      break;
    case 2:
      printf ("LibrarySocketError\n");
      break;
    case 3:
      printf ("SocketReconn\n");
      break;
    case 4:
      printf ("SocketFail\n");
      break;
    case 5:
      printf ("SocketTOut\n");
      break;
    case 6:
      printf ("SocketNotRdy\n");
      break;
    case 7:
      printf ("MemoryAllocation\n");
      break;
    case 8:
      printf ("LANConnectionError\n");
      break;
    case 9:
      printf ("LANUnavailable\n");
      break;
    case 10:
      printf ("sendCmdErr\n");
      break;
    case 11:
      printf ("Incomplete\n");
      break;
    case 12:
      printf ("InvalidResponse\n");
      break;
    case 13:
      printf ("Nak\n");
      break;
    case 14:
      printf ("CDBAccessError\n");
      break;
    case 15:
      printf ("USDConnectionError\n");
      break;
    case 16:
      printf ("USDTimeout\n");
      break;
    case 17:
      printf ("USDUnavailable\n");
      break;
    case 18:
      printf ("USDError\n");
      break;
    case 19:
      printf ("DevIOError\n");
      break;
    case 20:
      printf ("corbaError\n");
      break;
    case 21:
      printf ("USDStillRunning\n");
      break;
    case 22:
      printf ("USDunCalibrated\n");
      break;
    case 23:
      printf ("CannotGetUSD\n");
      break;
    }
}

void
checkASerrors (char *str, int circle, int actuator, ASErrors::ASErrorsEx Ex)
{
  ASErrors::ASErrorsExImpl exImpl (Ex);

  printf ("checkASerrors: ");
  printf ("%s %d_%d: ", str, circle, actuator);

  switch (exImpl.getErrorCode ())
    {
    case ASErrors::NoError:
      printf ("NoError\n");
      break;
    case ASErrors::LibrarySocketError:
      printf ("LibrarySocketError\n");
      break;
    case ASErrors::SocketReconn:
      printf ("SocketReconn\n");
      break;
    case ASErrors::SocketFail:
      printf ("SocketFail\n");
      break;
    case ASErrors::SocketTOut:
      printf ("SocketTOut\n");
      break;
    case ASErrors::SocketNotRdy:
      printf ("SocketNotRdy\n");
      break;
    case ASErrors::MemoryAllocation:
      printf ("MemoryAllocation\n");
      break;
    case ASErrors::Incomplete:
      printf ("Incomplete\n");
      break;
    case ASErrors::InvalidResponse:
      printf ("InvalideResponse\n");
      break;
    case ASErrors::Nak:
      printf ("Nak\n");
      break;
    case ASErrors::CDBAccessError:
      printf ("CDBAccessError\n");
      break;
    case ASErrors::USDConnectionError:
      printf ("USDConnectionError\n");
      break;
    case ASErrors::USDTimeout:
      printf ("USDTimeout\n");
      break;
    case ASErrors::LANConnectionError:
      printf ("LANConnectionError\n");
      break;
    case ASErrors::LAN_Unavailable:
      printf ("LANUnavailable\n");
      break;
    case ASErrors::sendCmdErr:
      printf ("sendCmdErr\n");
      break;
    case ASErrors::USDUnavailable:
      printf ("USDUnavailable\n");
      break;
    case ASErrors::USDError:
      printf ("USDError\n");
      break;
    case ASErrors::DevIOError:
      printf ("DevIOError\n");
      break;
    case ASErrors::corbaError:
      printf ("corbaError\n");
      break;
    case ASErrors::USDStillRunning:
      printf ("USDStillRunning\n");
      break;
    case ASErrors::USDunCalibrated:
      printf ("USDunCalibrated\n");
      break;
    case ASErrors::CannotGetUSD:
      printf ("CannotGetUSD\n");
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
