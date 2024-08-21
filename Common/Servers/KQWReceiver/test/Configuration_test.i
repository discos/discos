#include <IRATools.h>
#include <maciSimpleClient.h>
#include "Configuration.h"
#include <stdlib.h>

#define COMPONENT_NAME "RECEIVERS/KQWBandReceiver"

namespace KQWComponentTests {

class KQWComponent_Configuration : public ::testing::Test 
{
public:
	::testing::AssertionResult InitializeConfiguration_ResultOk() {
		CConfiguration<maci::SimpleClient> conf;
		if (!logged) return ::testing::AssertionFailure() << " ACS is not running or other related problem has occurred";
		try {
			conf.init(&client,COMPONENT_NAME);
		}
		catch (...) {
			return ::testing::AssertionFailure() << " An exception was thrown while reading the configuration";
		}
		return ::testing::AssertionSuccess() <<" All configuration records properly retrieved";
}
	::testing::AssertionResult ReadConfiguration() {
		double *loFreq,*loPower;
		double *freq,*taper;
		CConfiguration<maci::SimpleClient> conf;		
		long feed_i, if_i;
		IRA::CString formatter;

		conf.init(&client,COMPONENT_NAME);

		::testing::Test::RecordProperty("Default conf",(const char *)conf.getDefaultMode());	
		::testing::Test::RecordProperty("Actual conf",(const char *)conf.getActualMode());
		::testing::Test::RecordProperty("Rep. cache time",conf.getRepetitionCacheTime());
		::testing::Test::RecordProperty("Rep. expire time",conf.getRepetitionExpireTime());
		::testing::Test::RecordProperty("LNA Board IP",(const char *)conf.getLNAIPAddress());
		
		::testing::Test::RecordProperty("Total Feeds",conf.getFeeds());
		::testing::Test::RecordProperty("Number of IFs",conf.getIFs());
		
		::testing::Test::RecordProperty("Records number of the Noise Mark Table",conf.getMarkVectorLen());

		::testing::Test::RecordProperty("Records number of the Synth Table",conf.getSynthesizerTable(loFreq,loPower));
		::testing::Test::RecordProperty("Synth[0].freq",loFreq[0]);
		::testing::Test::RecordProperty("Synth[0].power",loPower[0]);
		delete [] loFreq;
		delete [] loPower;

		::testing::Test::RecordProperty("Records number of the Taper Table (feed=1)",conf.getTaperTable(freq,taper,1));
		::testing::Test::RecordProperty("Taper[feed=1,i=0].freq",freq[0]);
		::testing::Test::RecordProperty("Taper[feed=1,i=0].power",taper[0]);
		delete [] freq;
		delete [] taper;

		feed_i=1;
		if_i=1;
		if (conf.getBandPolarizations()[conf.getArrayIndex(feed_i,if_i)]==Receivers::RCV_LCP) {
			::testing::Test::RecordProperty("IF polarization (feed=1,IF=1)","LCP");
		}
		else {
			::testing::Test::RecordProperty("IF polarization (feed=1,IF=1)","RCP");
		}

		feed_i=3;
		if_i=0;
		if (conf.getBandPolarizations()[conf.getArrayIndex(feed_i,if_i)]==Receivers::RCV_LCP) {
			::testing::Test::RecordProperty("IF polarization (feed=3,IF=0)","LCP");
		}
		else {
			::testing::Test::RecordProperty("IF polarization (feed=3,IF=0)","RCP");
		}
		formatter.Format("%.1lf %.1lf",conf.getBandRFMin()[conf.getArrayIndex(feed_i,if_i)],
		  conf.getBandRFMax()[conf.getArrayIndex(feed_i,if_i)]);
		::testing::Test::RecordProperty("RF ranges (feed=2,IF=0)",(const char *)formatter);
		
		formatter.Format("%.1lf %.1lf",conf.getBandIFMin()[conf.getArrayIndex(feed_i,if_i)],
		  conf.getBandIFBandwidth()[conf.getArrayIndex(feed_i,if_i)]);
		::testing::Test::RecordProperty("IF start and bandwidth (feed=2,IF=0)",(const char *)formatter);
		
		return ::testing::AssertionSuccess();
	}
	::testing::AssertionResult setMode() {
		CConfiguration<maci::SimpleClient> conf;	
		CString defaultMode,actualMode,mode("Sun");
		conf.init(&client,COMPONENT_NAME);
		defaultMode=conf.getDefaultMode();
		actualMode=conf.getActualMode();
		if (actualMode!=defaultMode) {
			return ::testing::AssertionFailure() << " default mode was not properly loaded";
		}
		try {
			conf.setMode(mode);
		}
		catch (...) {
			return ::testing::AssertionFailure() << " An exception was thrown while changing mode";
		}
		actualMode=conf.getActualMode();
		if (actualMode!=mode) {
			return ::testing::AssertionFailure() << " change mode was not properly completely";
		}
		else {
			::testing::Test::RecordProperty("New mode loaded",(const char *)mode);
		}
		mode="*";
		try {
			conf.setMode(mode);
		}
		catch (...) {
			return ::testing::AssertionFailure() << " An exception was thrown while changing mode";
		}
		if (actualMode!=conf.getActualMode()) {
			return ::testing::AssertionFailure() << " Jolly char not supported or properly handled";
		}
		else {
			::testing::Test::RecordProperty("Jolly char is supported, preserved current configuration",(const char *)actualMode);
		}
		return ::testing::AssertionSuccess();	
	}
	
	
	
	::testing::AssertionResult LNAbypass_check() {
		CConfiguration<maci::SimpleClient> conf;	
		CString mode("Sun");
		conf.init(&client,COMPONENT_NAME);	
		::testing::Test::RecordProperty("Current receiver mode is",(const char *)conf.getActualMode());
		
		if (conf.getLNABypass())
			::testing::Test::RecordProperty("LNA bypass is","enabled");
		else 
			::testing::Test::RecordProperty("LNA bypass is","disabled");
	
		try {
			conf.setMode(mode);
		}
		catch (...) {
			return ::testing::AssertionFailure() << " An exception was thrown while changing mode";
		}
		
		::testing::Test::RecordProperty("New receiver mode is",(const char *)conf.getActualMode());

		if (conf.getLNABypass())
			::testing::Test::RecordProperty("LNA bypass now is","enabled");
		else 
			::testing::Test::RecordProperty("LNA bypass now is","disabled");

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