
#include "Offset.h"

namespace AntennaBossTest {

class AntennaBoss_Offset : public ::testing::Test {
public:

	::testing::AssertionResult offset_scanUserCombination() {
		RecordProperty("description","check if the resulting offset are handle correctly in case both user and scna offset are defined");
		COffset test;
		TOffset user,scan,out;
		bool override;
		double lat,lon;

		scan.lon=0.1; scan.lat=1.55; scan.frame=Antenna::ANT_EQUATORIAL;
		user.lon=-0.5; user.lat=0.34; user.frame=Antenna::ANT_EQUATORIAL;

		test.reset();

		out=test.ephemGeneratorOffsets(override);
		if (override) return ::testing::AssertionFailure() << " override flag after a reset";
		if (out.isSet) return ::testing::AssertionFailure() << " is set flag after a reset";

		// set user offsets
		test.setUserOffset(user);
		out=test.ephemGeneratorOffsets(override);
		if (override) return ::testing::AssertionFailure() << " override flag after a user offset";
		if (!out.isSet) return ::testing::AssertionFailure() << " is set flag after a user offset";

		// set scan offsets with coherent frame
		test.setScanOffset(scan);
		out=test.ephemGeneratorOffsets(override);
		if (override) return ::testing::AssertionFailure() << " override flag after a coherent scan offset";
		if (!out.isSet) return ::testing::AssertionFailure() << " is set flag after a coherent scan offset";
		lon=scan.lon+user.lon;
		lat=scan.lat+user.lat;
		EXPECT_DOUBLE_EQ(lon,out.lon) << " longitude value after a coherent scan offset";;
		EXPECT_DOUBLE_EQ(lat,out.lat) << " latitude value after a coherent scan offset";
		if ((out.frame!=user.frame) || (out.frame!=scan.frame)) return ::testing::AssertionFailure() << " frame value after a coherent scan offset";

		// set scan offsets with not coherent frame
		test.resetScan();
		scan.frame=Antenna::ANT_GALACTIC;
		test.setScanOffset(scan);
		out=test.ephemGeneratorOffsets(override);
		if (!override) return ::testing::AssertionFailure() << " override flag after a not coherent scan offset";
		if (!out.isSet) return ::testing::AssertionFailure() << " is set flag after a not coherent scan offset";

		EXPECT_DOUBLE_EQ(out.lon,scan.lon) << " longitude value after a not coherent scan offset";
		EXPECT_DOUBLE_EQ(out.lat,scan.lat) << " latitude value after a not coherent scan offset";
		if (out.frame!=scan.frame)  return ::testing::AssertionFailure() << " frame value after a not coherent scan offset";

		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult offset_isSetCheck() {
		RecordProperty("description","check if the offsets are correctly flagged");
		COffset test;
		TOffset in;

		in.lon=0.1; in.lat=1.55; in.frame=Antenna::ANT_EQUATORIAL;

		if (test.isScanSet()) return ::testing::AssertionFailure() << " initial scan flag should be false";
		else if (test.isUserSet()) return ::testing::AssertionFailure() << " initial user flag should be false";
		else if (test.isSystemSet()) return ::testing::AssertionFailure() << " initial system flag should be false";

		// now set the offsets;
		test.setScanOffset(in);
		test.setUserOffset(in);
		test.setSystemOffset(in.lon,in.lat);

		if (!test.isScanSet()) return ::testing::AssertionFailure() << " scan flag should be set";
		else if (!test.isUserSet()) return ::testing::AssertionFailure() << " user flag should be set";
		else if (!test.isSystemSet()) return ::testing::AssertionFailure() << " system flag should be set";

		test.reset();

		if (test.isScanSet()) return ::testing::AssertionFailure() << " after a reset, the scan flag should be false";
		else if (test.isUserSet()) return ::testing::AssertionFailure() << " after reset, the user flag should be false";
		else if (test.isSystemSet()) return ::testing::AssertionFailure() << " after reset, the system flag should be false";
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult offset_AssignAndReadout() {
		RecordProperty("description","check if the data consistency is maintained");
		COffset test;
		TOffset out;
		TOffset scan,user;

		scan.lon=0.1; scan.lat=1.55; scan.frame=Antenna::ANT_EQUATORIAL;
		user.lon=-0.5; user.lat=0.34; user.frame=Antenna::ANT_HORIZONTAL;
		test.setScanOffset(scan.lon,scan.lat,scan.frame);
		test.setUserOffset(user.lon,user.lat,user.frame);

		// check user offsets
		out=test.getUserOffset();
		if (out.lon!=user.lon) return ::testing::AssertionFailure() << " user longitude offset does not match";
		else if (out.lat!=user.lat) return ::testing::AssertionFailure() << " user latitude offset does not match";
		if (out.frame!=user.frame) return ::testing::AssertionFailure() << " user longitude offset does not match";

		scan.lon=100;
		test.setScanOffset(scan);

		// check scan offsets
		out=test.getScanOffset();
		if (out.lon!=scan.lon) return ::testing::AssertionFailure() << " scan longitude offset does not match";
		else if (out.lat!=scan.lat) return ::testing::AssertionFailure() << " scan latitude offset does not match";
		if (out.frame!=scan.frame) return ::testing::AssertionFailure() << " scan longitude offset does not match";

		// check again user offsets
		out=test.getUserOffset();
		if (out.lon!=user.lon) return ::testing::AssertionFailure() << " new user longitude offset does not match";
		else if (out.lat!=user.lat) return ::testing::AssertionFailure() << " new user latitude offset does not match";
		if (out.frame!=user.frame) return ::testing::AssertionFailure() << " new user longitude offset does not match";

		test.resetUser();
		if (test.isSystemSet()) return ::testing::AssertionFailure() << " user offsets should not be set";

		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult offset_LValueAndRValue() {
		RecordProperty("description","check if the lvalue of get accesor works");
		COffset test;
		TOffset out,out1;
		TOffset scan;

		scan.lon=0.1; scan.lat=1.55; scan.frame=Antenna::ANT_EQUATORIAL;
		test.setScanOffset(scan);

		//RValue
		out=test.getUserOffset();
		out.lon=100;
		out.lat=300;
		out1=test.getUserOffset();

		//check the internal values are unchanged
		if (out1.lon==out.lon) return ::testing::AssertionFailure() << " Rvalue test, the object values should have not be changed";
		else if (out1.lat==out.lat) return ::testing::AssertionFailure() << " Rvalue test, object values should have not be changed";

		//LVALUE
		out.lon=100;
		out.lat=300;
		changeValue(test.getUserOffset(),out.lon,out.lat);
		out1=test.getUserOffset();
		//cout << out1.lon << " " << out1.lat << endl;
		//cout << out.lon << " " << out.lat << endl;

		//check the internal values match
		if (out1.lon!=out.lon) return ::testing::AssertionFailure() << " LValue test, object value are not changed accordingly";
		else if (out1.lat!=out.lat) return ::testing::AssertionFailure() << " LValue test, object value are not changed accordingly";

		return ::testing::AssertionSuccess();
	}


protected:
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}

	void changeValue(TOffset& off,const double& lon,const double& lat) {
		off.lon=lon;
		off.lat=lat;
	}
};

}


