#include <maciSimpleClient.h>
#include <IRA>
#include <usdC.h>
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
#define MAINTHREADSLEEPTIME 150
using namespace std;
using namespace maci;
using namespace ASErrors;

void checkAScompletionerrors (char *str, int circle, int actuator,
			      CompletionImpl comp);
void checkASerrors (char *str, int circle, int actuator,
		    ASErrors::ASErrorsEx Ex);

int
main (int argc, char *argv[])
{
  SimpleClient *client = new SimpleClient;
  ACSErr::Completion_var completion;
  ACS::ROlong_var actPos_var;
  CORBA::Long actPos_val;
  bool loggedIn = false;
  char *value;
  char serial_usd[23];
  char graf[6], mecc[4];
  int lanIndex, circleIndex, usdCircleIndex;
  SRTActiveSurface::USD_var usd;
  int i, loop;
    ACE_Time_Value tv(1);
  value = "/home/almamgr/SRT/CDB/alma/AS/tab_convUSD.txt\0";
  ifstream usdTable (value);
  if (!usdTable) {
      ACS_SHORT_LOG ((LM_INFO, "File %s not found", value));
      exit (-1);
    }

  try {
      if (client->init (argc, argv) == 0) {
          _EXCPT (ClientErrors::CouldntInitExImpl, impl, "TestClient::Main()");
          impl.log ();
          delete client;
      }
      if (client->login () == 0) {
          _EXCPT (ClientErrors::CouldntLoginExImpl, impl, "TestClient::Main()");
          impl.log ();
          delete client;
      }
      ACS_SHORT_LOG ((LM_INFO, "client %s activated", "TestClient"));
      loggedIn = true;
  }
  catch (...) {
      _EXCPT (ClientErrors::UnknownExImpl, impl, "TestClient::Main()");
      impl.log ();
      delete client;
  }
  ACS_SHORT_LOG ((LM_INFO, "OK activation SRTAS_Calibration: %s", "SRTAS_Calibration"));

//  for (;;) {
// Get reference to usd components
//  for (i = 1; i <= 2; i++) {
//      usdTable >> lanIndex >> circleIndex >> usdCircleIndex >> serial_usd >> graf >> mecc;
      try {
          usd = client->getComponent < SRTActiveSurface::USD > ("AS/SECTOR01/LAN07/USD02", 0, true);
          if (CORBA::is_nil (usd.in ()) == true) {
              _EXCPT (ClientErrors::CouldntAccessComponentExImpl, impl, "SRTActiveSurfaceTextClient::Main()");
              impl.setComponentName (serial_usd);
              impl.log ();
          }
      }
      catch (maciErrType::CannotGetComponentExImpl & E) {
          _EXCPT (ClientErrors::CouldntAccessComponentExImpl, impl, "TestClient::Main()");
          impl.log ();
      }
      catch (CORBA::SystemException & E) {
          _EXCPT (ClientErrors::CORBAProblemExImpl, impl, "TestClient::Main()");
          impl.setName (E._name ());
          impl.setMinor (E.minor ());
          impl.log ();
      }
      catch (...) {
          _EXCPT (ClientErrors::UnknownExImpl, impl, "TestClient::Main()");
          impl.log ();
      }
      loop = 1;
      for (loop = 1; loop <= 1; loop++) {
          printf("measure n.: %d\n", loop);
        printf("wait 2 seconds before reset\n");
        ACE_OS::sleep (2);
        usd->reset ();
        printf("reset.......\n");
        try {
            printf("wait 2 seconds before top\n");
            ACE_OS::sleep (2);
            usd->top ();
            printf("top.......\n");
            printf("wait 45 seconds before calibration\n");
            ACE_OS::sleep (45);
            CompletionImpl comp = usd->calibrate ();
            printf("calibration.......\n");
            if (comp.isErrorFree () == false)
                checkAScompletionerrors ("usd", 2, 2, comp);
        }
        catch (ASErrors::ASErrorsEx & Ex) {
            checkASerrors ("actuator", 2, 2, Ex);
        }
        printf("wait 5 seconds before calibration verification\n");
        ACE_OS::sleep (5);
        printf ("calibration verification......\n");
        try {
            CompletionImpl comp = usd->calVer ();
            if (comp.isErrorFree () == false)
                checkAScompletionerrors ("usd", 2, 2, comp);
	    }
        catch (ASErrors::ASErrorsEx & Ex) {
            checkASerrors ("actuator", 2, 2, Ex);
        }
        }
    //}
    client->run(tv);
    tv.set(0,MAINTHREADSLEEPTIME*10);
    //}
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
