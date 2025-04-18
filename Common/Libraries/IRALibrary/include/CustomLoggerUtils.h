#ifndef CUSTOM_LOGGER_UTILS_H
#define CUSTOM_LOGGER_UTILS_H

#include <Cplusplus11Helper.h>

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
C11_IGNORE_WARNING("-Wmisleading-indentation")
C11_IGNORE_WARNING("-Wcatch-value=")
#include <logging.h>
C11_IGNORE_WARNING_POP
#include <ManagmentDefinitionsS.h>

#define CUSTOM_LOGGING_DATA_NAME "source"
#define CUSTOM_LOGGING_DATA_VALUE "custom"
#define CUSTOM_PYTHON_LOGGING_PROCESS "IRA_CUSTOM_LOGGER"
#define CUSTOM_LOGGING_CHANNEL acscommon::LOGGING_CHANNEL_XML_NAME

/**
 * Use this macro to make a logging event interceptable by our custom logging system.
 * the synthax is equal to ACS LOG macro but it automatically adds a Key-Value pair to
 * the log record.
 */
#define CUSTOM_LOG(flags, routine, X) \
{ \
    ACS_CHECK_LOGGER; \
    LoggingProxy::AddData(CUSTOM_LOGGING_DATA_NAME, CUSTOM_LOGGING_DATA_VALUE); \
    Logging::LogSvcHandler::DeprecatedLogInfo tStruct; \
    tStruct = Logging::LogSvcHandler::unformatted2formatted X; \
    LOG(tStruct.priority, routine, tStruct.message); \
} 

/**
 * @TODO: test this function!!!
 */
#define CUSTOM_STATIC_LOG(priority, routine, text) \
{ \
    LoggingProxy::AddData(CUSTOM_LOGGING_DATA_NAME, CUSTOM_LOGGING_DATA_VALUE); \
    Logging::Logger::getStaticLogger()->log(priority, text, __FILE__, __LINE__, routine); \
}

/**
 * Use this macro to log an exception in the custom logging system.
 * @param EX: an Exception
 * @param priority: The priority of the exception LOG, used to set DEBUG instead of ERROR.
 */
#define CUSTOM_EXCPT_LOG(EX, priority) \
{ \
    _ADD_EXTRA(EX, CUSTOM_LOGGING_DATA_NAME, CUSTOM_LOGGING_DATA_VALUE); \
    EX.log(priority); \
}

/********************************************/
/*              EXAMPLE USAGE               */
/********************************************/
/*
*   CUSTOM_LOG(LM_FULL_INFO, "MyModule::MyClass::MyMethod", (LM_ALERT, "My log message"));
*/

namespace IRA{

/**
 * Static utilities to manipulate Custom Logging Events.
 */
class CustomLoggerUtils
{
    public:
        /**
         * Convert a capitalized string to the corresponding log level as defined in Management interface.
         * i.e. "Trace" is converted to Management::C_TRACE , and so on.
         * The method does not throw exceptions and use C_INFO as default Logging Level for what it cannot interpret.
         * @param str: the capitalized string representing a log leve
         * @return a Management log level 
         */
	static Management::LogLevel string2customLogLevel(const char *str);
        /**
         * Converts a Custom log level to its character representation
         * C_TRACE -> "Trace" ...
         * @param level the custom log level
         */
        static const char* customLogLevel2string(Management::LogLevel level);
        /**
         * Conversion function between our Logging levels and the ones defined in ACE
         * This method does not throw an exception but uses default conversion to LM_INFO level.
         * @param level: a log level as defined in Management
         * @return: the ACE log level
         */
	static ACE_Log_Priority custom2aceLogLevel(Management::LogLevel level);
    private:
        CustomLoggerUtils();
        CustomLoggerUtils(const CustomLoggerUtils&);
        virtual ~CustomLoggerUtils();
};

};
#endif
