#include <CustomLoggerUtils.h>

using namespace IRA;

Management::LogLevel
CustomLoggerUtils::string2customLogLevel(const char *str)
{
    if(strcmp(str, "Trace") == 0)
	return Management::C_TRACE;
    if(strcmp(str, "Debug") == 0)
	return Management::C_DEBUG;
    if(strcmp(str, "Info") == 0)
	return Management::C_INFO;
    if(strcmp(str, "Notice") == 0)
	return Management::C_NOTICE;
    if(strcmp(str, "Warning") == 0)
	return Management::C_WARNING;
    if(strcmp(str, "Error") == 0)
	return Management::C_ERROR;
    if(strcmp(str, "Critical") == 0)
	return Management::C_CRITICAL;
    if(strcmp(str, "Alert") == 0)
	return Management::C_ALERT;
    if(strcmp(str, "Emergency") == 0)
	return Management::C_EMERGENCY;
    return Management::C_INFO; //defaults to INFO
};

const char* 
CustomLoggerUtils::customLogLevel2string(Management::LogLevel level)
{
    switch(level)
    {
        case(Management::C_TRACE):
            return "Trace";
            break;
        case(Management::C_DEBUG):
            return "Debug";
            break;
        case(Management::C_INFO):
            return "Info";
            break;
        case(Management::C_NOTICE):
            return "Notice";
            break;
        case(Management::C_WARNING):
            return "Warning";
            break;
        case(Management::C_ERROR):
            return "Error";
            break;
        case(Management::C_CRITICAL):
            return "Critical";
            break;
        case(Management::C_ALERT):
            return "Alert";
            break;
        case(Management::C_EMERGENCY):
            return "Emergency";
            break;
        default:
            return "Info"; //defaults to Info
            break;
    }
}

ACE_Log_Priority 
CustomLoggerUtils::custom2aceLogLevel(Management::LogLevel level)
{
    switch(level)
    {
	case(Management::C_TRACE):
	    return LM_TRACE;
	    break;
	case(Management::C_DEBUG):
	    return LM_DEBUG;
	    break;
	case(Management::C_INFO):
	    return LM_INFO;
	    break;
	case(Management::C_NOTICE):
	    return LM_NOTICE;
	    break;
	case(Management::C_WARNING):
	    return LM_WARNING;
	    break;
	case(Management::C_ERROR):
	    return LM_ERROR;
	    break;
	case(Management::C_CRITICAL):
	    return LM_CRITICAL;
	    break;
	case(Management::C_ALERT):
	    return LM_ALERT;
	    break;
	case(Management::C_EMERGENCY):
	    return LM_EMERGENCY;
	    break;
	default:
	    return LM_INFO; //does this make sense?
	    break;
    }
};
