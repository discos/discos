#include <IRATools.h>
#include <maciSimpleClient.h>
#include "bulkDataZMQConfiguration.h"
#include <stdlib.h>

using namespace bulkdataZMQImpl; 

namespace BulkDataZMQTests {

class ZMQConfiguration_Configuration : public ::testing::Test 
{
public:
	::testing::AssertionResult InitializeConfiguration_ResultOk() {
		CZMQConfiguration<maci::SimpleClient> conf;
		if (!logged) return ::testing::AssertionFailure() << " ACS is not running or other related problem has occurred";
		try {
			conf.init(&client);
		}
		catch (ComponentErrors::CDBAccessExImpl &ex) {
			return ::testing::AssertionFailure() << " CDB Access error: " << ex.getFieldName();
		}
		catch (ComponentErrors::MemoryAllocationExImpl &ex) {
			return ::testing::AssertionFailure() << " Memory allocation error";
		}
		return ::testing::AssertionSuccess() <<" All configuration records properly retrieved";
	}

	::testing::AssertionResult ReadConfiguration() {
		CZMQConfiguration<maci::SimpleClient> conf;
		conf.init(&client);

		try {
			// Test retrieval by flow number (assuming flow 0 exists in CDB)
			TZMQConfig config = conf.getConfiguration(0);
			::testing::Test::RecordProperty("Flow0_Endpoint", (const char *)config.endPoint);
			::testing::Test::RecordProperty("Flow0_Backend", (const char *)config.backendName);
		}
		catch (...) {
			return ::testing::AssertionFailure() << " Failed to read configuration for Flow 0";
		}

		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult GetByComponentName() {
		CZMQConfiguration<maci::SimpleClient> conf;
		conf.init(&client);
		try {
			// Assuming 'BACKEND/TotalPower' is a valid entry in the CDB FlowTable
			TZMQConfig config = conf.getConfiguration("BACKENDS/TotalPower");
			if (config.backendName != "BACKENDS/TotalPower") {
				return ::testing::AssertionFailure() << " Returned backend name mismatch";
			}
		}
		catch (...) {
			return ::testing::AssertionFailure() << " Failed to read configuration by component name";
		}
		return ::testing::AssertionSuccess();
	}

protected:

	static void TearDownTestCase() {
	}

	static void SetUpTestCase() {
	}

	virtual void SetUp() {
		int argc=0;
		const char *argv[]={ "dummy", nullptr };
		logged=false;
		try {
			if (client.init(argc,const_cast<char**>(argv))==0) {
			}
			else {
				if (client.login()==0) {
				}
				logged=true;
			}	
		}
		catch(...) {
		}
	}

	virtual void TearDown() {
		if (logged) client.logout();
	}
	
	maci::SimpleClient client;
	bool logged;
	
};
};