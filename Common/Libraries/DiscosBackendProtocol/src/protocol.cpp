#include "protocol.hpp"

using namespace DiscosBackend;

bool 
Message::validate()
{
    if(_type == REQUEST)
    {
        if(_code != NOCODE)
            throw BackendProtocolError("request message must not contain code");
    }else{ // type == REPLY
        if(_code == NOCODE)
            throw BackendProtocolError("reply message must contain a valid code");
    }
    return true;
}

Message::Message(string message, bool terminator)
{
    string message_code, message_argument;
    char message_type;
    if((message.length() < 2) && (!terminator))
        throw BackendProtocolError("messages must be composed of at least 2 characters");
    if((message.length() < 4) && (terminator))
        throw BackendProtocolError("messages must be composed of at least 2 characters");
    if(terminator)
    {
        //remove ending characters \r\n
        message.erase(message.length() - 1);
    }
    stringstream stream_message(message);
    //Get the message type
    stream_message.get(message_type);
    if(message_type == REPLY_CODE)
        _type = REPLY;
    else if(message_type == REQUEST_CODE)
        _type = REQUEST;
    else
        throw BackendProtocolError("invalid message code");
    //get the message name
    getline(stream_message, _name, SEPARATOR);
    //if reply get the reply-code
    if(_type == REPLY)
    {
        if(stream_message.eof())
            throw BackendProtocolError("reply message must contain a code");
        getline(stream_message, message_code, SEPARATOR);
        if(message_code == FAIL_CODE)
            _code = FAIL;
        else if(message_code == SUCCESS_CODE)
            _code = SUCCESS;
        else if(message_code == INVALID_CODE)
            _code = INVALID;
        else
            throw BackendProtocolError("invalid reply code");
    } // if _type == REPLY
    // get message arguments
    while(!stream_message.eof())
    {
        getline(stream_message, message_argument, SEPARATOR);
        if((terminator) && (message_argument != TERMINATOR))
            _arguments.push_back(message_argument);
    }
} //Message(string message, bool terminator)

Message::Message(const Message& other)
{
    _type = other._type;
    _name = other._name;
    _code = other._code;
    _arguments = other._arguments;
}

bool
Message::is_valid_reply_for(const Message& request)
{
    if(_type != REPLY)
        return false;
    if(request._type != REQUEST)
        return false;
    if(_name != request._name)
        return false;
    return true;
}

bool
Message::is_valid_request_for(const Message& reply)
{
    if(_type != REQUEST)
        return false;
    if(reply._type != REPLY)
        return false;
    if(_name != reply._name)
        return false;
    return true;
}

string
Message::toString(bool terminate)
{
    validate();
    stringstream message;
    if(_type == REQUEST)
        message << REQUEST_CODE;
    else
        message << REPLY_CODE;
    message << _name;
    if(_type == REPLY)
    {
        message << SEPARATOR;
        if(_code == SUCCESS)
            message << SUCCESS_CODE;
        if(_code == INVALID)
            message << INVALID_CODE;
        if(_code == FAIL)
            message << FAIL_CODE;
    }
    for(vector<string>::iterator i = _arguments.begin(); 
        i != _arguments.end();
        ++i)
        message << SEPARATOR << (string)(*i);
    if(terminate)
        message << TERMINATOR;
    return message.str();
}

