#include "Error.h"

#include <stdlib.h>

namespace IRALibraryTest {

class IRALibrary_CError : public ::testing::Test {
public:
	::testing::AssertionResult copyConstructor_segfault() {
		testSegFault();	
		return ::testing::AssertionSuccess();
	}

private:
	IRA::CError m_error;
	
	IRA::CError testSegFault() {
		// This SHOULD NOT generate a Segmentation fault after fix #185.
        m_error.setError(IRA::CError::SerialType,IRA::CError::SrlPortConfig);
		return m_error;
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
