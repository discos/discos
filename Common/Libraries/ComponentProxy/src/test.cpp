#include <iostream>
#include <ComponentProxy.h>
#include <CustomLoggerC.h>

/**
 * As this Library is composed by only an header file
 * this test is necessary to try and compile it!! 
 * It is used only in local so that it won't get install if
 * the compilation does not succeed
 */

using namespace Management;

int main()
{
    //ComponentProxy<Management::CustomLogger, Management::CustomLogger_var> m_customLogger("CustomLogger");
    PROXY_COMPONENT(CustomLogger);
    CustomLogger_proxy m_customLogger("CustomLogger");
    m_customLogger->emitLog("Proxy log!", C_INFO);
    return 0;
}

