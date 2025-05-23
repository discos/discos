#ifndef EXPAT_PARSING_HPP
#define EXPAT_PARSING_HPP

#include <expat.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <cstring>
#include <string.h>
#include <ctype.h>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <loggingBaseLog.h>
#include <acstime.h>
#include <IRA>
#include <ManagmentDefinitionsS.h>
#include <CustomLoggerUtils.h>

#define START_TAG "<root>"
#define START_TAG_LEN 6
#define END_TAG "</root>"
#define END_TAG_LEN 7

class LogRecord; //fwd declaration
class LogRecordComparator; //fwd declaration

class MalformedXMLError : public std::runtime_error
{
    public:
        MalformedXMLError(const char *msg):
        std::runtime_error(std::string(msg)){};
};

/**
 * Smart pointer to a LogRecord, used in conjunction with STL and non-standard containers.
 */
typedef boost::shared_ptr<LogRecord> LogRecord_sp;
/** 
 * Key-Value string map, used to store extra data in the LogRecord
 */
typedef std::map<std::string, std::string> KVMap;
typedef std::map<std::string, std::string>::iterator KVIterator;
/**
 * Priority queue used to store LogRecords in timestamp order, as defined by LogReecordComparator.
 */
typedef std::priority_queue<LogRecord_sp, std::vector<LogRecord_sp>, LogRecordComparator> LogRecordQueue;
/** @name expat
 * These funtions implement the expat parsing logics.
 */
/**@{*/
void start_parsing_element(void* data, const char *el, const char **attr); 
void end_parsing_element(void *data, const char *el); 
void start_parsing_cdata_element(void *data); 
void end_parsing_cdata_element(void *data); 
void parsing_character_data(void* data, const char *chars, int len);
/**@}*/
/**
 * Creates the expat parser
 * @return a new expat xml parser
 */
XML_Parser init_log_parsing();
/**
 * Delete an expat xml parser, freeing the allocated memory.
 * @param log_parser an expat xml parser
 */
void free_log_parsing(XML_Parser log_parser);
/**
 * Main parsing functionality, implementing RAII pattern for LogRecord objects.
 * Extracts a LogRecord from an xml string.
 * @return the newly constructed LogRecord in a smart pointer
 */
LogRecord_sp get_log_record(XML_Parser log_parser, const char *xml_text);
/**
 * Programattically create a custom log record.
 * @param msg the log message
 * @param level the log level
 * @return the newly constructed LogRecrod in a smart pointer
 */
LogRecord_sp get_log_record(const char *msg, Management::LogLevel level);
ACS::Time parse_timestamp(const char* ts);
std::string trim_date(const std::string& str);
std::string log_to_string(const LogRecord& log_record);
ACS::TimeInterval log_age(const LogRecord& log_record);

/**
 * Checks if all characters contained in the string are printable characters
 * and ASCII conformant.
 **/
std::string sanitize_xml(const char* input_text, bool brackets=false);


/**
 * Class used to represent a log record and the necessary information
 * to be stored during the xml parsing. These two have been packed for conveniently
 * make use of expat parsing primitives which accept one only structure as user data. 
 * Attributes marked as _ are to be intended as XML accessories.
 * @TODO LogRecord informations should be decoupled from xml parsing structures. 
 */
class LogRecord
{
public:
    /**
     * Constructor
     */
    LogRecord();
    virtual ~LogRecord();
    /**
     * The log level of this record.
     */
    Management::LogLevel log_level;
    std::string log_level_name, message, process_name, _element, xml_text, _data_name;
    /**
     * the timestamp of the logging event, extracted from the ACSLogRecord.
     */
    ACS::Time timestamp;
    /**
     * Key-Value couples of extra data tied to the log record.
     */
    KVMap kwargs;
    bool _initialized, _parsing_message_cdata, _parsing_data_cdata, _finished;
    int _depth;
    /**
     * Adds a Key-Value pair to the log record.
     * @param key the key string.
     * @param val the value string.
     */
    void add_data(std::string key, std::string val);
    /**
     * Reads a Key-Value pair from the record. 
     * @param key the key string .
     * @return the associated value if key is found, NULL otherwise.
     */
    std::string get_data(std::string key);
    /**
     * Get a string representation of the extra data of this log record.
     * @return the string representation as "[key1=value1, key2=value2 .... ]"
     */
    std::string get_extra_data_string() const;
};

/**
 * Functor used to compare LogRecords relatively to their timestamp attribute.
 */
class LogRecordComparator
{
    public:
        bool operator()(const LogRecord_sp& lhr, const LogRecord_sp& rhr);
};
        

#endif
