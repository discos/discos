#ifndef CUSTOM_LOGGER_UTILS_H
#define CUSTOM_LOGGER_UTILS_H

#include <logging.h>
#include <ManagmentDefinitionsS.h>

#define CUSTOM_LOGGING_DATA_NAME "source"
#define CUSTOM_LOGGING_DATA_VALUE "custom"
#define CUSTOM_LOGGING_CHANNEL acscommon::LOGGING_CHANNEL_XML_NAME
#define CUSTOM_LOGGING_EXTENSION ".log"
#define FULL_LOGGING_EXTENSION ".fullog"

#define CUSTOM_LOG(flags, routine, X) \
{ \
    ACS_CHECK_LOGGER; \
    LoggingProxy::AddData(CUSTOM_LOGGING_DATA_NAME, CUSTOM_LOGGING_DATA_VALUE); \
    Logging::LogSvcHandler::DeprecatedLogInfo tStruct; \
    tStruct = Logging::LogSvcHandler::unformatted2formatted X; \
    LOG(tStruct.priority, routine, tStruct.message); \
} 

/********************************************/
/*              EXAMPLE USAGE               */
/********************************************/
/*
*   CUSTOM_LOG(LM_FULL_INFO, "MyModule::MyClass::MyMethod", (LM_ALERT, "My log message"));
*/

namespace IRA{

class CustomLoggerUtils
{
    public:
	static Management::LogLevel string2customLogLevel(const char *str);
	static ACE_Log_Priority custom2aceLogLevel(Management::LogLevel level);
    private:
        CustomLoggerUtils();
        CustomLoggerUtils(const CustomLoggerUtils&);
        virtual ~CustomLoggerUtils();
};

};
#endif
