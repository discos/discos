/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Definitions.h,v 1.4 2008-12-05 15:25:02 a.orlati Exp $											*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.cnr.it)  21/07/2004      Creation                                          */
/* Andrea Orlati(aorlati@ira.cnr.it)  24/08/2005      revisited error-handling macro definition         */
/* Andrea Orlati(aorlati@ira.cnr.it)  21/07/2008      Corrected _EXCPT_TO_STRING macro due to a memory leak problem    */
/* Andrea Orlati(aorlati@ira.cnr.it)  03/12/2008      Added ABS macro    */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#ifndef NULL
#define NULL 0
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned int
#endif

#ifndef DDWORD
#define DDWORD unsigned long long 
#endif 

#ifndef TIMEVALUE
#define TIMEVALUE EpochHelper
#endif

#ifndef TIMEDIFFERENCE
#define TIMEDIFFERENCE DurationHelper
#endif 

#define GETMIN(X,Y) (X<Y)?X:Y

#define GETMAX(X,Y) (X>=Y)?X:Y

#define GETMAX3(X,Y,Z) (X>=Y)?GETMAX(X,Z):GETMAX(Y,Z)

#define ABS(X,Y) (X>=Y)?X-Y:Y-X

#define LIGHTSPEED_MS 299792458.0

/** 
 * Creates an exception
 * @param TYPE exception type class
 * @param EXCPT exception object
 * @param ROUTINE routine name where the exception is created
*/
#define _EXCPT(TYPE,EXCPT,ROUTINE) TYPE EXCPT(__FILE__,__LINE__,ROUTINE)

/**
 * Creates a completion 
 * @param TYPE completion type class
 * @param COMPL completion object
 * @param ROUTINE routine name where the completion is created
*/ 
#define _COMPL(TYPE,COMPL,ROUTINE) TYPE COMPL(__FILE__,__LINE__,ROUTINE)

/**
 * Appends extra information(parameter,value) to a completion or exception
 * @param OBJ object (completion or exception) to which extra data must be appended
 * @param NAME parameter name
 * @param VALUE parameter value
*/
#define _ADD_EXTRA(OBJ,NAME,VALUE) OBJ.addData(NAME,VALUE)


/**
 * Used to read extra information from a completion or exception
 * @param OBJ object (completion or exception) to which extra data is appended
 * @param NAME para,eter name
 * @return the value of the parameter
*/
#define _GET_EXTRA(OBJ,NAME) OBJ.getData(NAME);

/** 
 * Creates and throws an exception
 * @param TYPE exception type class
 * @param ROUTINE routine name where the exception is created
*/
#define _THROW_EXCPT(TYPE,ROUTINE) { \
	_EXCPT(TYPE,__dummy,ROUTINE); \
	throw __dummy; \
}

/**
 * This macro can be used to log the whole error trace associated to a completion or exception object
 * @param OBJ completion or exception object
*/
#define _LOG_TRACE(OBJ) { \
	OBJ.log() \
}

/**
 * This macro creates a completion from an exception. The excpetion error backtrace is enqueued to the new completion.
 * <pre> 
 * catch (FileNotFoundExImpl &E) {
 * 	_COMPL_FROM_EXCPT(ArchiveNotObtainedCompletion,comp,E,"Server::getArchive()");
 * 	comp.log(); 
 * }
 * </pre>
 * @param TYPE completion class, the type of the newly created object
 * @param COMPL new completion object
 * @param EXCPT exception object
 * @param ROUTINE routine name where the completion is created
*/
#define _COMPL_FROM_EXCPT(TYPE,COMPL,EXCPT,ROUTINE) TYPE COMPL(EXCPT,__FILE__,__LINE__,ROUTINE)

/**
 * @todo this macro must me verified
 * This macro converts a completion to an exception
 * <pre> 
 * ACSErr::completionImpl comp=getFile();
 * if (!compl.isErrorFree()) {
 * 	_EXCPT_FROM_COMPL(ArchiveNotObtainedCompletion,Ex,comp,"Server::getArchive()");
 * 	throw Ex; 
 * }
 * </pre>
 * @param TYPE exception class, the type of the newly created object
 * @param EXCPT new exception object
 * @param COMPL completion object
*/
//#define _EXCPT_FROM_COMPL(TYPE,EXCPT,COMPL,ROUTINE) TYPE EXCPT(COMPL,__FILE__,__LINE__,ROUTINE)
#define _EXCPT_FROM_COMPL(TYPE,EXCPT,COMPL) TYPE EXCPT(COMPL.getErrorTraceHelper().getErrorTrace());

/**
 * This macro creates a new object (completion or exception) adding to its current error backtrace the previous error backtrace.
 * @param TYPE class of the newly created object
 * @param NEWOBJ new object
 * @param OLDOBJ old object (which will be enqueued to the error backtrace of the object)
 * @param ROUTINE routine name where the new object is created
*/
#define _ADD_BACKTRACE(TYPE,NEWOBJ,OLDOBJ,ROUTINE) TYPE NEWOBJ(OLDOBJ,__FILE__,__LINE__,ROUTINE)

/**
 * This macro creates a new TYPE1 exception object, starting from the error backtrace of the exception EXCPT2. Then the object is
 * thrown.
 * @param TYPE1 excpetion class of the newly created object
 * @param EXCPT2 exception object that must be enqueued to the error backtrace
 * @param ROUTINE routine name where the exception is created
*/
#define _THROW_EXCPT_FROM_EXCPT(TYPE1,EXCPT2,ROUTINE) { \
	_ADD_BACKTRACE(TYPE1,__dummy,EXCPT2,ROUTINE); \
	throw __dummy; \
}

/**
 * This macro is used to populate fields of an error object inside the IRALibrary.
 * @param ERROR error object
 * @param TYPE error type 
 * @param CODE error code
 * @param ROUTINE routine name where the error occured
 * @sa <i>CError</i>
*/
#define _SET_ERROR(ERROR,TYPE,CODE,ROUTINE) \
	if (ERROR.isNoError()) { \
		ERROR.setError(TYPE,CODE,__FILE__,ROUTINE,__LINE__); \
}

/**
 * This macro is used to populate fields of an error object inside the IRALibrary, system information is
 * also included
 * @param ERROR error object
 * @param TYPE error type 
 * @param CODE error code
 * @param ROUTINE routine name where the error occured
 * @param ERRNO system error code (typically errno from errno.h)
*/
#define _SET_SYSTEM_ERROR(ERROR,TYPE,CODE,ROUTINE,ERRNO) \
	if (ERROR.isNoError()) { \
		ERROR.setError(TYPE,CODE,__FILE__,ROUTINE,__LINE__); \
		ERROR.setExtra(strerror(ERRNO),ERRNO); \
	}

/**
 * This macro creates a new exception from an CError object. It also reset the error object.
 * @param TYPE exception class 
 * @param EXCPT exception object
 * @param ERROR CError object
 * @sa <i>CError</i>
*/
#define _EXCPT_FROM_ERROR(TYPE,EXCPT,ERROR)  \
	IRA::CString __Name; \
	DWORD __Value; \
	TYPE EXCPT((const char*)ERROR.getFile(),ERROR.getLine(),(const char*)ERROR.getRoutine()); \
	while(ERROR.getExtraInfo(__Name,__Value)) _ADD_EXTRA(EXCPT,(const char*)__Name,__Value); 

/**
 * This macro throws a new exception created from an CError object. It also reset the error object.
 * @param TYPE exception class 
 * @param ERROR CError object
 * @sa <i>CError</i>
*/
#define _THROW_EXCPT_FROM_ERROR(TYPE,ERROR) { \
	_EXCPT_FROM_ERROR(TYPE,__dummy,ERROR); \
	throw __dummy; \
}

/**
 * This macro produce a string containing some brief information about an exception and all the error backtrace. Please notice that the
 * excpetion is returned with the first error pointed in the backtrace.
 * @param STRING string (CString) that will be composed
 * @param EXCPT exception object 
*/
#define _EXCPT_TO_CSTRING(STRING,EXCPT) { \
	EXCPT.top(); IRA::CString __Tmp; char *descr; \
	descr=EXCPT.getDescription(); \
	__Tmp.Format("%s (type:%ld code:%ld)",descr,EXCPT.getErrorType(),EXCPT.getErrorCode()); \
	CORBA::string_free(descr); \
	STRING=__Tmp; \
	while (EXCPT.getNext()!=NULL) { \
		descr=EXCPT.getDescription(); \
		__Tmp.Format(" %s (type:%ld code:%ld)",descr,EXCPT.getErrorType(),EXCPT.getErrorCode()); \
		CORBA::string_free(descr); \
		STRING+=__Tmp; \
	} \
	EXCPT.top(); \
}

/**
 * This macro produce a string containing some brief information about a completion and all the error backtrace. Please notice that the
 * completion is returned with the first error pointed in the backtrace.
 * @param STRING string (CString) that will be composed.
 * @param COMPL completion object. 
*/
#define _COMPL_TO_CSTRING(STRING,COMPL) { \
	COMPL.getErrorTraceHelper()->top(); IRA::CString __Tmp; char *descr; \
	descr = COMPL.getErrorTraceHelper()->getDescription(); \
	__Tmp.Format("%s (type:%ld code:%ld)",descr,COMPL.getErrorTraceHelper()->getErrorType(), \
	  COMPL.getErrorTraceHelper()->getErrorCode()); \
	CORBA::string_free(descr); \
	STRING=__Tmp; \
	while (COMPL.getErrorTraceHelper()->getNext()!=NULL) { \
		descr = COMPL.getErrorTraceHelper()->getDescription(); \
		__Tmp.Format(" %s (type:%ld code:%ld)",descr,COMPL.getErrorTraceHelper()->getErrorType(), \
		  COMPL.getErrorTraceHelper()->getErrorCode()); \
		CORBA::string_free(descr); \
		STRING+=__Tmp; \
	} \
	COMPL.getErrorTraceHelper()->top(); \
}

/**
 * This macro is a shortcut. This is the implementation of the function that will returns the reference of a component
 * property.
 * @param IMPLCLASS the name of the c++ class that implements the IDL interface of the component.
 * @param TYPE the type of the ACS property that will be returned as a reference
 * @param PROPERTY the c++ object that implmemnts the ACS property
 * @param PROPERTYNAME the name of the property as reported in the idl file that defines the interface
*/
#define _PROPERTY_REFERENCE_CPP(IMPLCLASS,TYPE,PROPERTY,PROPERTYNAME) TYPE##_ptr IMPLCLASS::PROPERTYNAME() throw (CORBA::SystemException) \
{ \
	if (PROPERTY==0) return TYPE::_nil(); \
	TYPE##_var tmp=TYPE::_narrow(PROPERTY->getCORBAReference()); \
	return tmp._retn(); \
}

#endif
