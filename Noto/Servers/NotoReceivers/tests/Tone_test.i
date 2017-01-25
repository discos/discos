#include "Tone.h"
#include <stdlib.h>

namespace NotoReceiversTest {

//#define FASTQUEUE_SIZE 10000

class NotoReceivers_Tone : public ::testing::Test {
public:

	::testing::AssertionResult Tone_checkConnectionTimeOut() {
		RecordProperty("description","if the device is not available the connection should time out");
		TIMEVALUE start, stop;
		bool timeout=false;
		// wrong address, 2 seconds timeout
		CTone tone("10.10.10.10",5000,2000000);
		try {
			tone.init();
		}
		catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
			return ::testing::AssertionFailure() << " socket creation failed";
		}
		IRA::CIRATools::getTime(start);
		try {
			tone.turnOn();
		}
		catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
			timeout=true;
		}
		if (timeout) {
			IRA::CIRATools::getTime(stop);
			if ((stop.value().value-start.value().value)>55000000) { // difference greater than a safe limit
				return ::testing::AssertionFailure() << "the time out was longer than expected";
			}
			else {
				return ::testing::AssertionSuccess(); // the operation seems to have correctly timed out
			}
		}
		else {
			return ::testing::AssertionFailure() << "operation did not failed as expected";
		}
	}

	::testing::AssertionResult Tone_checkToneSwitchingOn() {
		RecordProperty("description","if the object can successfully turn on and off the phase cal tone");

		// wrong address, 2 seconds timeout

		CTone tone(IRA::CSocket::getAddrbyHostName(IRA::CSocket::getHostName()),simulatorPort,2000000); //local host
		try {
			tone.init();
		}
		catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
			return ::testing::AssertionFailure() << " socket creation failed";
		}
		try {
			tone.turnOn();
		}
		catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
			return ::testing::AssertionFailure() << " error turning on the tone";
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult Tone_checkToneSwitchingOff() {
		RecordProperty("description","if the object can successfully turn on and off the phase cal tone");

		// wrong address, 2 seconds timeout

		CTone tone(IRA::CSocket::getAddrbyHostName(IRA::CSocket::getHostName()),simulatorPort,2000000); //local host

		try {
			tone.init();
		}
		catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
			return ::testing::AssertionFailure() << " socket creation failed";
		}
		try {
			tone.turnOff();
		}
		catch (ComponentErrors::IRALibraryResourceExImpl& ex) {
			return ::testing::AssertionFailure() << " error turning off the tone";
		}
		return ::testing::AssertionSuccess();
	}

protected:

	static void TearDownTestCase()
	{
		system("tcpProt-sim stop"); // shout turn down all the simulators
	}

	static void SetUpTestCase()
	{
		simulatorPort=15000;
	}

	virtual void SetUp() {
		IRA::CString cmd,frm;
		simulatorPort++;
		frm="tcpProt-sim --command='A9' --answer='ack' --command='B9' --answer='ack' --default='error' --terminator=10 --port=%d start&";
		cmd.Format((const char *)frm,simulatorPort);
		// the answer set in the simulator are placeholders.
		system((const char *)cmd);
		IRA::CIRATools::Wait(1,0);
	}
	virtual void TearDown() {
	}

	static long simulatorPort;
};

long NotoReceivers_Tone::simulatorPort = 0;

}

