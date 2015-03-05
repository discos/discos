#include "CustomLoggerProxy.h"
#include "AntennaBossProxy.h"

/**
 * As this Library is composed by only an header file
 * this test is necessary to try and compile it!! 
 * It is used only in local so that the library won't 
 * install if the compilation does not succeed
 */

int main()
{
    Management::CustomLogger_proxy m_customLogger;
    m_customLogger.setComponentName("CustomLogger");
    m_customLogger->emitLog("Proxy log!", Management::C_INFO);
    Antenna::AntennaBoss_proxy m_antennaBoss;
    return 0;
}

