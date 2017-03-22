#ifndef DISCOS_BACKEND_PROTOCOL_HPP
#define DISCOS_BACKEND_PROTOCOL_HPP

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

#define FAIL_CODE "fail"
#define SUCCESS_CODE "ok"
#define INVALID_CODE "invalid"
#define TERMINATOR "\r\n"
#define SEPARATOR ','
#define REQUEST_CODE '?'
#define REPLY_CODE '!'

using namespace std;

namespace DiscosBackend{

typedef enum{REQUEST, REPLY} MESSAGE_TYPE;
typedef enum{SUCCESS, INVALID, FAIL, NOCODE} MESSAGE_CODE;

class BackendProtocolError : public runtime_error
{
    public:
        BackendProtocolError(const char* msg) : runtime_error(string(msg)){};
};

class Message
{
    public:
        Message(
            MESSAGE_TYPE type,
            string name,
            MESSAGE_CODE code = NOCODE,
            vector<string> arguments = vector<string>()
            ) : _type(type),
                _name(name),
                _code(code),
                _arguments(arguments){};
       Message(string message, bool terminator = false);
       Message(const Message&);
       virtual ~Message(){};
       MESSAGE_TYPE get_type(){return _type;};
       string get_name(){return _name;};
       MESSAGE_CODE get_code(){return _code;};
       vector<string> get_arguments(){return _arguments;};
       template <typename T>
       void add_argument(T argument){
            stringstream ss;
            ss << argument;
            _arguments.push_back(ss.str());
       };
       template <typename T>
       T get_argument(const unsigned int& position)
       {
           if(position >= _arguments.size())
               throw BackendProtocolError("cannot access command argument");
           stringstream ss;
           ss << _arguments[position];
           T result; 
           try{
               ss >> result;
           }catch(...){
               throw BackendProtocolError("cannot convert argument to given type");
           }
           return result;
       };
       void add_argument(string);
       void add_argument(float);
       void add_argument(int);
       string toString(bool terminate = false);
       bool validate();
       bool is_valid_reply_for(const Message&);
       bool is_valid_request_for(const Message&);
       bool is_success_reply(){return (_code == SUCCESS);};
    private:
       MESSAGE_TYPE _type;
       string _name;
       MESSAGE_CODE _code;
       vector<string> _arguments;
}; //class Message

}; //namespace DiscosBackend

#endif 

