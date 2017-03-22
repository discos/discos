// note, the blank between the macro and the parenthesis is mandatory
#define BOOST_PP_LOCAL_LIMITS (1, SCHEMA_HEADER_ENTRY_NUMBER)
#define BOOST_PP_LOCAL_MACRO(n) _FILE_CREATEHEADER(n)
#include BOOST_PP_LOCAL_ITERATE()

//#define BOOST_PP_LOCAL_LIMITS (1, SCHEMA_TABLE_NUMBER)
//#define BOOST_PP_LOCAL_MACRO(n) _FILE_CREATETABLE(n)
//#include BOOST_PP_LOCAL_ITERATE()
