#include <Definitions.h>
#include <iostream>

// if this value is changed the modification should be reflected also in python implementation
#ifndef USER_MESSAGE_FIELDNAME 
#define USER_MESSAGE_FIELDNAME "Message-to-User"
#endif

/**
 * Appends the message to be returned to the user for a completion or an exception.
 * The message i shown in the operator input by consequence of a user command
 * @param OBJ object (completion or exception) to which extra data must be appended
 * @param VALUE message to the user
*/
#define _ADD_USER_MESSAGE(OBJ,VALUE) _ADD_EXTRA(OBJ,USER_MESSAGE_FIELDNAME,VALUE)

/**
 * Appends the message to be returned to the user for a completion or an exception.
 * The message i shown in the operator input by consequence of a user command.
 * This version will also send a message to the user from a stand-alone program.
 * @param OBJ object (completion or exception) to which extra data must be appended
 * @param VALUE message to the user
*/
#define _ADD_USER_MESSAGE_DIRECT(OBJ,VALUE) { \
	IRA::CString out; \
	_ADD_EXTRA(OBJ,USER_MESSAGE_FIELDNAME,VALUE); \
	exceptionToUser(OBJ,out); \
	std::cerr << "error " << (const char *)out; \
}											 

namespace SimpleParser {
											 										  
template<class EX> class CFormatter {
public:
	static void exceptionToUser(EX& exObj,IRA::CString& output);
private:
	static IRA::CString getMessage(EX& exObj);
};

#include "ParserConnector.i"

}



