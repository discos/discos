#include "gtest/gtest.h"
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>
#include <IRA>

#include "SP_parser.h"

using namespace SimpleParser;

class TestObject
{
    public:
        TestObject() : test_value(0){};
        void empty_command();
        void unary_command(const char* parameter);
        IRA::CString result;
        int test_value;
};

void 
TestObject::empty_command()
{
    test_value++;
    return;
}

void
TestObject::unary_command(const char* parameter)
{
    result = IRA::CString(parameter);
    return;
}

class TestProcedureParsing : public ::testing::Test
{
    public:
        TestProcedureParsing();
        virtual ~TestProcedureParsing();
        TestObject to;
        CParser<TestObject> *m_parser;
};

TestProcedureParsing::TestProcedureParsing()
{
    m_parser = new CParser<TestObject>(&to, 8, true);
    m_parser->add("empty_command",
                  new function0<TestObject, non_constant, void_type>(
                      &to,
                      &TestObject::empty_command), 
                  0);
    m_parser->add("unary_command",
                  new function1<TestObject, non_constant, void_type, I<string_type> >(
                      &to,
                      &TestObject::unary_command), 
                  1);
    ACS::stringSeq noarg_procedure, arg_procedure;
    noarg_procedure.length(1);
    noarg_procedure[0] = "empty_command";
    m_parser->addExtraProcedure("empty_procedure", "testprocedure", noarg_procedure, 0);
    arg_procedure.length(1);
    arg_procedure[0] = "unary_command=$1";
    m_parser->addExtraProcedure("unary_procedure", "testprocedure", arg_procedure, 1);
}

TestProcedureParsing::~TestProcedureParsing()
{
    delete m_parser;
}

TEST_F(TestProcedureParsing, test_setup)
{
    ASSERT_TRUE(true);
}

TEST_F(TestProcedureParsing, run_command_without_argument)
{
    IRA::CString procedure = "empty_command";
    IRA::CString output;
    int test_value = to.test_value;
    m_parser->run(procedure, output);
    //This is a google test macro
    RecordProperty("run_output", (const char*) output);
    ASSERT_EQ(test_value + 1, to.test_value);
}

TEST_F(TestProcedureParsing, run_procedure_without_argument)
{
    IRA::CString procedure = "empty_procedure";
    IRA::CString output;
    int test_value = to.test_value;
    m_parser->run(procedure, output);
    //This is a google test macro
    RecordProperty("run_output", (const char*) output);
    sleep(2);
    ASSERT_EQ(test_value + 1, to.test_value);
}

TEST_F(TestProcedureParsing, run_command_with_argument)
{
    IRA::CString procedure = "unary_command=ciao";
    IRA::CString output;
    m_parser->run(procedure, output);
    //This is a google test macro
    RecordProperty("run_output", (const char*) output);
    ASSERT_EQ(IRA::CString("ciao"), to.result);
}

TEST_F(TestProcedureParsing, run_procedure_with_argument)
{
    IRA::CString procedure = "unary_procedure=ciao";
    IRA::CString output;
    m_parser->run(procedure, output);
    //This is a google test macro
    RecordProperty("run_output", (const char*) output);
    sleep(2);
    ASSERT_EQ(IRA::CString("ciao"), to.result);
}

