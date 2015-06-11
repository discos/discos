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
        TestObject();
        void empty_command();
        void unary_command(const char* parameter);
        void two_params_command(const double& first, const double& second);
        IRA::CString result;
        int test_int;
        double test_double[2];
};

TestObject::TestObject() :
    test_int(0)
{
    test_double[0] = 0;
    test_double[1] = 0;
}

void 
TestObject::empty_command()
{
    test_int++;
    return;
}

void
TestObject::unary_command(const char* parameter)
{
    result = IRA::CString(parameter);
    return;
}

void 
TestObject::two_params_command(const double& first, const double& second)
{
    test_double[0] = first;
    test_double[1] = second;
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
    m_parser->add("two_params_command",
                  new function2<TestObject, non_constant, void_type, I<double_type>, I<double_type> >(
                      &to,
                      &TestObject::two_params_command), 
                  2);
    ACS::stringSeq noarg_procedure, arg_procedure, two_args_procedure;
    noarg_procedure.length(1);
    noarg_procedure[0] = "empty_command";
    m_parser->addExtraProcedure("empty_procedure", "testprocedure", noarg_procedure, 0);
    arg_procedure.length(1);
    arg_procedure[0] = "unary_command=$1";
    m_parser->addExtraProcedure("unary_procedure", "testprocedure", arg_procedure, 1);
    two_args_procedure.length(1);
    two_args_procedure[0] = "two_params_command=$1,$2";
    m_parser->addExtraProcedure("two_params_procedure", "testprocedure",
                                two_args_procedure, 2);
}

TestProcedureParsing::~TestProcedureParsing()
{
    delete m_parser;
}

TEST_F(TestProcedureParsing, test_empty_procedure)
{
    ACS::stringSeq empty_procedure;
    IRA::CString procedure = "empty_procedure";
    IRA::CString output;
    empty_procedure.length(0);
    ASSERT_NO_THROW({
        m_parser->addExtraProcedure("empty_procedure", 
                                    "testprocedure", 
                                    empty_procedure, 
                                    0);
        m_parser->run(procedure, output); 
    });
    RecordProperty("run_output", (const char*) output);
}

TEST_F(TestProcedureParsing, test_setup)
{
    ASSERT_TRUE(true);
}

TEST_F(TestProcedureParsing, run_command_without_argument)
{
    IRA::CString procedure = "empty_command";
    IRA::CString output;
    int test_int = to.test_int;
    m_parser->run(procedure, output);
    //This is a google test macro
    RecordProperty("run_output", (const char*) output);
    ASSERT_EQ(test_int + 1, to.test_int);
}

TEST_F(TestProcedureParsing, run_procedure_without_argument)
{
    IRA::CString procedure = "empty_procedure";
    IRA::CString output;
    int test_int = to.test_int;
    m_parser->run(procedure, output);
    //This is a google test macro
    RecordProperty("run_output", (const char*) output);
    sleep(2);
    ASSERT_EQ(test_int + 1, to.test_int);
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

TEST_F(TestProcedureParsing, run_command_with_two_arguments)
{
    IRA::CString procedure = "two_params_command=10,20";
    IRA::CString output;
    m_parser->run(procedure, output);
    //This is a google test macro
    RecordProperty("run_output", (const char*) output);
    ASSERT_EQ((double)10, to.test_double[0]);
    ASSERT_EQ((double)20, to.test_double[1]);
}

TEST_F(TestProcedureParsing, run_procedure_with_two_arguments)
{
    IRA::CString procedure = "two_params_procedure=50,60.2345";
    IRA::CString output;
    m_parser->run(procedure, output);
    //This is a google test macro
    RecordProperty("run_output", (const char*) output);
    sleep(2);
    ASSERT_EQ((double)50, to.test_double[0]);
    ASSERT_EQ((double)60.2345, to.test_double[1]);
}
