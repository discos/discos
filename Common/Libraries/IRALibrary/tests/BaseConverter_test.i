#include "BaseConverter.h"

#include <stdlib.h>

namespace IRALibraryTest {

class IRALibrary_BaseConverter : public ::testing::Test {

public:

::testing::AssertionResult decToBin_checkConversion() {
	::testing::Test::RecordProperty("description","check if straight decimal to binary convertion works correctly");
	char onebyte=1;
	if (IRA::CBaseConverter::decToBin(onebyte)!="00000001") return ::testing::AssertionFailure() << "Convertion of 1 (byte) is not correct";
	if (IRA::CBaseConverter::decToBin(1234)!="00000000000000000000010011010010") return ::testing::AssertionFailure() 
	  << "Convertion of 1234 (int) is not correct";
	if (IRA::CBaseConverter::decToBin(-1234)!="11111111111111111111101100101110") return ::testing::AssertionFailure() 
	  << "Convertion of -1234 (int) is not correct";	
	if (IRA::CBaseConverter::decToBin(-1234l)!="1111111111111111111111111111111111111111111111111111101100101110") 
	  return ::testing::AssertionFailure() << "Convertion of -1234 (long) is not correct";
	return ::testing::AssertionSuccess();
	

}

::testing::AssertionResult decToHex_checkConversion() {
	::testing::Test::RecordProperty("description","check if straight decimal to hex convertion works correctly");
	//std::cout << IRA::CBaseConverter::decToHex(-2147483648) << endl;
	if (IRA::CBaseConverter::decToHex(0)!="00000000") return ::testing::AssertionFailure() << "Convertion of 0 is not correct";
	if (IRA::CBaseConverter::decToHex(2147483647)!="7FFFFFFF") return ::testing::AssertionFailure() << "Convertion of 2147483647 is not correct";
	if (IRA::CBaseConverter::decToHex(-1)!="FFFFFFFF") return ::testing::AssertionFailure() << "Convertion of -1 is not correct";
	if (IRA::CBaseConverter::decToHex(-2147483648l)!="FFFFFFFF80000000") return ::testing::AssertionFailure() << "Convertion of -2147483648 is not correct";	
	return ::testing::AssertionSuccess();
}

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

};