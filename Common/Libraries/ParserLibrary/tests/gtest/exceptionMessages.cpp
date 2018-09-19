#include <ParserErrors.h>
#include <IRA>
#include <SP_parser.h>

class TestExceptionMessages : public ::testing::Test
{
    public:
        TestExceptionMessages();
        virtual ~TestExceptionMessages();
        
};


TestExceptionMessages::TestExceptionMessages()
{
}

TestExceptionMessages::~TestExceptionMessages()
{
}

TEST_F(TestExceptionMessages,single_exception_description)
{
	ParserErrors::NotSupportedErrorExImpl ex(__FILE__,__LINE__,"single_exception_description");
	IRA::CString output;
	_EXCPT_TO_CSTRING(output,ex);
   RecordProperty("output_message", (const char*) output);
   ASSERT_TRUE(output.GetLength()>1);
}

TEST_F(TestExceptionMessages,double_exception_description)
{
	IRA::CString output;
	ParserErrors::NotSupportedErrorExImpl ex(__FILE__,__LINE__,"double_exception_description");
	_ADD_BACKTRACE(ParserErrors::ConversionErrorExImpl,ex1,ex,"double_exception_description");
	_EXCPT_TO_CSTRING(output,ex1);
   RecordProperty("output_message", (const char*) output);
   ASSERT_TRUE(output.GetLength()>1);
}

TEST_F(TestExceptionMessages,message_to_user)
{
	IRA::CString output;
	ParserErrors::NotSupportedErrorExImpl ex(__FILE__,__LINE__,"message_to_user");
	_ADD_BACKTRACE(ParserErrors::ConversionErrorExImpl,ex1,ex,"message_to_user");
	SimpleParser::CFormatter<ParserErrors::ConversionErrorExImpl>::exceptionToUser(ex1,output);
   RecordProperty("output_message", (const char*) output);
   ASSERT_TRUE(output.GetLength()>1);
}

TEST_F(TestExceptionMessages,adhoc_message_to_user)
{
	IRA::CString output;
	ParserErrors::NotSupportedErrorExImpl ex(__FILE__,__LINE__,"adhoc_message_to_user");
	_ADD_USER_MESSAGE(ex,"This is a unit test");
	_ADD_BACKTRACE(ParserErrors::ConversionErrorExImpl,ex1,ex,"adhoc_message_to_user");
	SimpleParser::CFormatter<ParserErrors::ConversionErrorExImpl>::exceptionToUser(ex1,output);
	RecordProperty("output_message", (const char*) output);
	//cout << (const char *)output << endl;
	ASSERT_TRUE(output.GetLength()>1);
}

	