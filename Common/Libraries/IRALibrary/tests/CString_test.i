#include "String.h"
#include <stdlib.h>

namespace IRALibraryTest {

class IRALibrary_String : public ::testing::Test {
public:
	::testing::AssertionResult string_checkLeft() {
		RecordProperty("description","check the CString Left method");
		IRA::CString base("error and error message");		
		IRA::CString token("error ");
		IRA::CString comp("and error message");
		IRA::CString res;		
		res=base.Left(token.GetLength());
		if (res.GetLength()+comp.GetLength()!=base.GetLength()) {
			return ::testing::AssertionFailure() << " the length of the strings does not match";
		}
		if (res!=token) {
			return ::testing::AssertionFailure() << " unexpected function output, the resulting string is'"
			  << (const char *)res << "' instead of '" << (const char *)token << "'";
		}		
		return ::testing::AssertionSuccess();
	}
	
	::testing::AssertionResult string_checkSplit() {
		RecordProperty("description","check the how CString handle string splitting");
		IRA::CString base("That is a test string that must be split");		
		IRA::CString token("is");
		IRA::CString split1,split2;		
		int pos=base.Find((const char *)token);
		split1=base.Left(pos+token.GetLength());
		split2=base.Right(base.GetLength()-(pos+token.GetLength()));
		if (split1.GetLength()+split2.GetLength()!=base.GetLength()) {
			return ::testing::AssertionFailure() << " the length of the strings does not match";
		}
		if (split1!="That is") {
			return ::testing::AssertionFailure() << " unexpected function output, the resulting string is '"
			  << (const char *)split1 << "' instead of 'This is'";
		}
		if (split2!=" a test string that must be split") {
			return ::testing::AssertionFailure() << " unexpected function output, the resulting string is '"
			  << (const char *)split2 << "' instead of ' a test string that must be split'";
		}		
		return ::testing::AssertionSuccess();
	}	

protected:
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};

}