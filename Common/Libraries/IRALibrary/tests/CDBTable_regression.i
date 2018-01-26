#include "DataField.h"
#include "String.h"


#include <stdlib.h>

namespace IRALibraryTest {

class IRALibrary_CDataField : public ::testing::Test {
public:
	// this is the test that identifies issue #200.
	::testing::AssertionResult typeConversion_fail() {	
		CDBTestField fieldS("string",IRA::CDataField::STRING);
		CDBTestField fieldL("long",IRA::CDataField::LONGLONG);
		CDBTestField fieldD("double",IRA::CDataField::DOUBLE);
		CDBTestField fieldW("word",IRA::CDataField::DOUBLEDOUBLEWORD);
		fieldS.addValue();
		fieldL.addValue();
		fieldD.addValue();
		fieldW.addValue();			
		fieldS.setValue("dummy");
		fieldL.setValue("-456789");
		fieldD.setValue("12345.678");
		fieldW.setValue("9000");
		fieldS.First();
		fieldL.First();
		fieldD.First();;
		fieldW.First();				
		if (fieldS.asString()!="dummy") return ::testing::AssertionFailure() << " string not properly converted";
		if (fieldD.asDouble()!=12345.678) return ::testing::AssertionFailure() << " double not properly converted";
		if (fieldW.asDoubleDoubleWord()!=9000) return ::testing::AssertionFailure() << " unsigned long not properly converted";
		if (fieldL.asLongLong()!=-456789) return ::testing::AssertionFailure() << " signed long not properly converted";
		return ::testing::AssertionSuccess();
	}

private:
	// this is a fake class in order to manipulate the CDataField protected methods
	class CDBTestField: public IRA::CDataField {
	public:
		CDBTestField(const IRA::CString& name,const TFieldType& type): IRA::CDataField(name,type) {
		}
		void addValue() {
			IRA::CDataField::addValue();
		}
		void setValue(const IRA::CString& value) {
			IRA::CDataField::setValue(value);
		}
		void First() {
			IRA::CDataField::First();
		}
		void Next() {
			IRA::CDataField::Next();
		}
	};
		
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
