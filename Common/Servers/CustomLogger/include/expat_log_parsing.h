#ifndef EXPAT_PARSING_HPP
#define EXPAT_PARSING_HPP

#include <expat.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <string.h>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <loggingBaseLog.h>
#include <acstime.h>
#include <IRA>
#include <ManagmentDefinitionsS.h>

#define START_TAG "<root>"
#define START_TAG_LEN 6
#define END_TAG "</root>"
#define END_TAG_LEN 7

class LogRecord; //fwd dec
class LogRecordComparator; //fwd dec
typedef boost::shared_ptr<LogRecord> LogRecord_sp;
void start_parsing_element(void* data, const char *el, const char **attr); 
void end_parsing_element(void *data, const char *el); 
void start_parsing_cdata_element(void *data); 
void end_parsing_cdata_element(void *data); 
void parsing_character_data(void* data, const char *chars, int len);
XML_Parser init_log_parsing();
void free_log_parsing(XML_Parser log_parser);
LogRecord_sp get_log_record(XML_Parser log_parser, const char *xml_text);
ACS::Time parse_timestamp(const char* ts);
std::string trim_date(const std::string& str);
std::string log_to_string(const LogRecord& log_record);
ACS::Time log_age(const LogRecord& log_record);

// Used for "Data" xml field
typedef std::map<std::string, std::string> KVMap;
// Used for storing LogRecord events in batches
typedef std::priority_queue<LogRecord_sp, std::vector<LogRecord_sp>, LogRecordComparator> LogRecordQueue;

/*
Class used to representing a log record and the necessary information
to be stored during the xml parsing. 
Attributes marked as _ are to be intended as XML accessories.
*/
class LogRecord
{
public:
    LogRecord();
    virtual ~LogRecord();
    Management::LogLevel log_level;
    std::string log_level_name, message, _element, xml_text, _data_name;
    ACS::Time timestamp;
    KVMap kwargs;
    bool _initialized, _parsing_message_cdata, _parsing_data_cdata, _finished;
    int _depth;
    void add_data(std::string key, std::string val);
    std::string get_data(std::string key);
};

class LogRecordComparator
{
    public:
        bool operator()(const LogRecord_sp& lhr, const LogRecord_sp& rhr);
};
        

#endif
