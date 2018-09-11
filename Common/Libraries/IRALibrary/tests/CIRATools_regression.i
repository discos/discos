#include "IRATools.h"
#include "String.h"

#include <stdlib.h>
#include <slamac.h>

#define DEC2000 "29:40:15.201"
#define RA2000 "04:37:04.370"


namespace IRALibraryTest {

class IRALibrary_CIRATools_regression : public ::testing::Test {
public:
	::testing::AssertionResult radToSexagesimalAngle_badconversion() {	
		double dec;
		IRA::CString outdec;
		RecordProperty("description","check the conversion of arc seconds which seems screwed up after 64 bit migration");
		IRA::CIRATools::sexagesimalAngleToRad(DEC2000,dec,true);		
		IRA::CIRATools::radToSexagesimalAngle(dec,outdec);	
		if (outdec!=DEC2000) {
			return ::testing::AssertionFailure() << " unexpected arc seconds conversion";
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult radToHourAngle_badconversion() {	
		double ra;
		IRA::CString outra;
		RecordProperty("description","check the conversion of arc seconds which seems screwed up after 64 bit migration");
		IRA::CIRATools::hourAngleToRad(RA2000,ra,true);
		IRA::CIRATools::radToHourAngle(ra,outra);		
		if (outra!=RA2000) {
			return ::testing::AssertionFailure() << " unexpected arc seconds conversion";
		}
		return ::testing::AssertionSuccess();
	}

protected:

	static void TearDownTestCase()
	{
	}

	static void SetUpTestCase()
	{
	}

	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};

}
