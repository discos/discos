/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                			    when                   What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  01/08/2008     Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  07/04/2009     Added a simple support for the jolly character   */
/* Andrea Orlati(aorlati@ira.inaf.it)  20/11/2009     Added the support for package   */
/* Andrea Orlati(aorlati@ira.inaf.it)  29/01/2010     added support for procedure and remote calls   */
/* Andrea Orlati(aorlati@ira.inaf.it)  16/04/2010     added support for background, time tagged operations   */
/* Andrea Orlati(aorlati@ira.inaf.it)  29/04/2010     added support for call backs in injected procedures   */
/* Andrea Orlati(aorlati@ira.inaf.it)  14/05/2010     async commands are now to be explicitly allowed at construction time */
/* Andrea Orlati(aorlati@ira.inaf.it)  05/02/2013     added support for key commands and system calls, reviewed the error handling */

#ifndef _SP_PARSER_H_
#define _SP_PARSER_H_

/**
 * @mainpage SimpleParser Documentation
 * @date 05/02/2013
 * @version 1.4.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 8.2.0
 * @remarks compiler version is 4.1.2
 * 
 * This is a collection of classes that allow an application to interpret strings as commands. In order to execute the command it is necessary to associate the command 
 * itself to a member function of a specific object (hosted object). This association is called execution rule and must be configured during initialization stage. Once the command has been 
 * parsed and the syntax checked the command itself is executed by calling the respective associated function. If the function provides output parameters, they are returned.
 * This library provides support for five kinds of commands:
 * @arg \c simple commands, the parsing and the execution of the function is completely done by this library. The execution can be synch or async.
 * @arg \c remote commands, this library just performs a pre-parsing but the arguments interpretation and the execution is completely delegated to the associated function. The execution can be synch or async.
 * @arg \c key commands, the command are implemented directly inside the parser
 * @arg \c system calls, the command will result in an execution of an external program or Operative System feature
 * @arg \c procedures, they are complex commands and can be composed by a variable number of the first two commands. Since a procedure could be very complex (any command could have an output) the final
 * result of the procedure execution is not propagated. The procedures are always executed asynchronously (the procedure commands become asynchronous themselves) but the command order is always respected.
 * All these commands are setup (@sa <i>CParser::add()</i> before staring the parser and could not be changed at runtime, anyway there is the possibility to add run-once procedure commands. 
 * This special commands are treated exactly as procedures but they are known by the parser only for they execution time.    
 * There is no prevention of name clashes, in case a command name is duplicated the conflict is resolved by calling first one that has was added, independently if it is a simple command or a procedure.
 * Commands are synchronous or asynchronous. In the asynchronous case it is possible to provide a timestamp or not, in the former case the command is executed at the given time, in the latter the
 * execution is done as soon as possible. The asynchronous command never return the execution result.
 * The grammar is very simple and can be customized a bit by changing the command delimiters. Using the defaults the grammar may sound like this:
 * <pre>
 * <char>:-'a'|'b'|'c'|.........|'A'|'B'|'C'.....|'Z'
 * <digit>:-0|1|2|..|9
 * <string>:-<char>|<digit>|<char><string>|<digit><string>
 * <jolly>:-'*'
 * <commanddelimiter>:-'='
 * <paramdelimiter>:-','
 * <timemarker>:-'@'
 * <year>:-{<digit>}*4
 * <doy>:-{< digit >}*3
 * <hour>:-{< digit >}*2
 * <minute>:-{< digit >}*2
 * <second>:-{< digit >}*2
 * <timestamp>:-[<year>]'-'[<doy>]'-'<hour>':'<minute>':'<second>
 * <timeinterval>:-'!' <hour>':'<minute>':'<second>
 * <time>:-<timestamp>|<timeinterval>
 * <commandname>:-<string>
 * <parameters>:-<string>|<string><paramdelimiter><parameters>
 * <command>:-<commandname>[<commanddelimiter><parameters>][<timemarker>|<timemarker><time>] 
 * </pre>
 * The parser also allows for answers to the command line. The answer format may be:
 * <pre>
 * <answerdelimiter>:-'\'
 * <errordelimiter>:-'?'
 * <errormessage>:-<string>
 * <outputparameters>:-<string>|<string><answerdelimiter><outputparameters>
 * <parsingerror>:-<errormessage>
 * <executionerror>:-<commandname><errorddelimiter><errormessage>
 * <successanswer>:-<commandname><answerdelimiter>[<outputparameters>]
 * <answer>:-<parsingerror>|<executionerror>|<successanswer>
 * </pre>
*/

#include "SP_typeConversion.h"
#include "SP_types.h"
#include "SP_functor.h"
#include "SP_function0.h"
#include "SP_function1.h"
#include "SP_function2.h"
#include "SP_function3.h"
#include "SP_function4.h"
#include "SP_function5.h"
#include "SP_function6.h"
#include "SP_function7.h"
#include <vector>
#include <list>
#include <acsThreadBase.h>
#include <ScheduleTimer.h>
#include <stdlib.h>


/**
 * @namespace SimpleParser
 * This namespace collects all the classes required by the parser in order to operate
*/
namespace SimpleParser {

/**
 * Prototype of a remote function, it will accept the full command, the extra parameter and the name of the package.
 * It could throw a remote package error
 */
#define _SP_REMOTECALL(X) bool (OBJ::*X)(const IRA::CString& command,const IRA::CString& package,const long& param,IRA::CString& out)  throw (ParserErrors::PackageErrorExImpl)
/**
 * Prototype of the callback function, invoked when a injected procedure has completed. If accepts the name of the procedure and the execution result
 */
#define _SP_CALLBACK(X) void (*X)(const void* param,const IRA::CString& procName,const bool& resultOk)
#define THREAD_NAME "SimpleParserExecutorThread"

#define _SP_TIMETAGGEDQUEUE "ti"
#define _SP_TIMETAGGEDQUEUE_PARAMS 0
#define _SP_TIMETAGGEDQUEUECLEAR "flush"
#define _SP_TIMETAGGEDQUEUECLEAR_PARAMS 1
#define _SP_TIMETAGGEDQUEUECLEARALL "flushAll"
#define _SP_TIMETAGGEDQUEUECLEARALL_PARAMS 0

template<class EX> class CFormatter {
public:
	static void exceptionToUser(EX& exObj,IRA::CString& output);
private:
   static IRA::CString getMessage(EX& exObj);
};

/**
 * This is the parser main class. It allows to add execution rules, to parse command line according to the supported grammar to execute 
 * the commands and, in case, to return back the values generated by the execution. The template parameter is the class that owns
 * the method that will be executed by the parser.
 * A correct sequence of calls is (provided there is a class named CTest that exposes the mod, date and rand methods): 
 * <pre>
  * CTest test;
 * CParser<CTest> parser(&test,5);
 *	parser.add<2>("modulo",new function2<CTest,double_type,I<double_type>,I<double_type> >(&test,&CTest::mod));
 *	parser.add<0>("ora",new function0<CTest,time_type >(&test,&CTest::date));
 * parser.add<0>("random",new function1<CTest,void_type,O<int_type> >(&test,&CTest::rand));
 *	for(;;) {
 * 		scanf("%s",input);
 * 		inStr=IRA::CString(input);
 * 		if (inStr=="exit") break;
 * 		printf("%s\n",(const char *)parser.run(inStr));
 * }
 * </pre>
 * For example a parser like this can successfully interpret and execute commands like :
 * <pre>
 * modulo=12,33
 * date
 * random
 * </pre>
 * On the other hand the following command generates errors:
 * <pre>
 * modulo=qw,33  //wrong parameter format
 * modulo=12       // not enough parameters
 * time				       // unknown command
 * </pre>
 * This class is partially thread safe, synchronization is done for the execution list on which the internal thread and the caller could operate simultaneously.
 * The standard  rules list is not serialized, but it can be considered safe provided that no calls to <i>add()</i> methods are done during normal parser execution. This should not be
 * a limitation since calls to <i>add()</i> are thought to be done during initialization stage.
 * The extra rule list instead, since it is possible to operate over it at runtime, is synchronized.
 * Of course there is no control over the called functions (associated to the execution rules), in this case any synchronization issue must be managed inside the hosted class.
 */
template <class OBJ> class CParser {
public: 
	
	/**
	 * Constructor.
	 * @param obj pointer to the hosted object 
	 * @param max indicates the maximum number of parameters allowed for each function.
	 * @param delim parameter separator character
	 * @param cmdDelimiter character that separates the command from the parameters list
	 * @param levelTwoSupport true if level two support has to be enabled, that means  asynchronous commands (time tagged or not), and key commands.
	 * @param answerDelimiter character that the parser uses to separate the various fields in the answer
	 * @param timeDelimiter character that mark a timestamp
	 * @param errorDelimiter character used to separate the command from the error message
	 */  
	CParser(OBJ *obj,const BYTE& max,bool levelTwoSupport=false,const char delim=',',const char cmdDelimiter='=',const char timeDelimiter='@',/*const char jolly='*',*/const char answerDelimiter='\\',const char errorDelimiter='?') {
		m_delimiter=delim;
		m_commandDelimiter=cmdDelimiter;
		m_timeDelimiter=timeDelimiter;
		m_answerDelimiter=IRA::CString(answerDelimiter);
		m_errorDelimiter=IRA::CString(errorDelimiter);
		/*m_jollyChar=IRA::CString(jolly);
		m_jollyCharReplacement="-1";*/
		m_maxArgs=max;
		m_ruleSet.clear();
		m_extraRuleSet.clear();
		// add the KEYCOMMAND set
		if (levelTwoSupport) {
			TRule *elem=new TRule(_SP_TIMETAGGEDQUEUE,_SP_TIMETAGGEDQUEUE_PARAMS);
			m_ruleSet.push_back(elem);
			elem=new TRule(_SP_TIMETAGGEDQUEUECLEAR,_SP_TIMETAGGEDQUEUECLEAR_PARAMS);
			m_ruleSet.push_back(elem);
			elem=new TRule(_SP_TIMETAGGEDQUEUECLEARALL,_SP_TIMETAGGEDQUEUECLEARALL_PARAMS);
			m_ruleSet.push_back(elem);
		}
		m_object=obj;
		m_levelTwoSupport=levelTwoSupport;
		if (m_levelTwoSupport) {
			m_pThread=m_threadManager.create(THREAD_NAME,(void *)worker,static_cast<void *>(this));
			if (m_pThread!=NULL) {
				m_pThread->resume();
			}
		}
		m_executionList.clear();
		m_timerInitialized=false;
	}
	
	/**
	 * Destructor.
	*/
	virtual ~CParser() {
		m_timer.cancelAll();
		if (m_levelTwoSupport) {
			if (m_pThread) {
				m_pThread->stop();
				while (m_pThread->isAlive()) {
					IRA::CIRATools::Wait(0,50000);
				}
				m_threadManager.terminate(THREAD_NAME);
			}
		}IRA::CString msg;
		typename TRuleSet::iterator i;
		for(i=m_ruleSet.begin(); i!=m_ruleSet.end(); i++) {
			if ((*i)!=NULL) delete *i;
		}
		m_ruleSet.clear();
		clearExtraProcedures();
		baci::ThreadSyncGuard guard(&m_exeListMutex);
		typename TExecutionList::iterator j;
		for (j=m_executionList.begin();j!=m_executionList.end();++j) {
			if ((*j)!=NULL) delete *j;
		}
		m_executionList.clear();
	}
	
	/**
	 * This template function adds an execution rule to the parser. In other word it associates a command to the respective local function that
	 * must be executed if the command is received. The template parameter is the number of arguments that are expected by this function. 
	 * @param fName name of the command, identifies the command that will be recognized by the parser.
	 * @param function function associated to the command, the number of input parameters must correspond to the number specified by the
	 *               template parameter.
	 */
	void add(const IRA::CString& fName,CBaseFunction *function,int N) {
		TRule *elem=new TRule(function,fName,N);
		m_ruleSet.push_back(elem);
	}

	/**
	 * This function adds an execution rule to the parser. It associates a command to an external function. This function must have three arguments  
	 * and return a string. The returned value is interpreted as the execution results. It is up to the function to format properly the this string. The first input argument 
	 * is used to pass the command that should be executed, the second is the package name that owns the command, while the third is an extra execution parameter that may o may not be used.
	 * @param name name of the command that will be associated to the remote function
	 * @param package extra string that can be used to compose the answer by the remote function, it can be the name of a collection or a library that the function belongs to.
	 * @param prm extra parameter passed to the called remote function.
	 * @remoteFunc pointer to the function to be called in case the command name is recognized
	 * */
	void add(const IRA::CString& name,const IRA::CString& package,const long& prm,_SP_REMOTECALL(remoteFunc)) {
		TRule *elem=new TRule(remoteFunc,name,package,prm);
		m_ruleSet.push_back(elem);
	}

	/**
	 * This function adds an execution rule to the parser. It associates a command to a procedure. The procedure is a collection of commands itself.  
	 * @param name name of the command associated to the procedure
	 * @param package extra string that can be used to compose the answer, it can be the name of a collection that the function belongs to.
	 * @param procedure list of commands that compose the procedure
	 * */	
	void add(const IRA::CString& name,const IRA::CString& package,const ACS::stringSeq& procedure,int N) {
		TRule *elem=new TRule(procedure,name,package,N);
		m_ruleSet.push_back(elem);
	}
	
	/**
	 * This template function adds an execution rule to the parser. In other word it associates a command to a system call  that
	 * must be executed if the command is received. The template parameter is the number of arguments that are expected by this call.
	 * @param name name of the command, identifies the command that will be recognized by the parser.
	 * @param syscall function associated to the command, the number of input parameters must correspond to the number specified by the
	 *               template parameter.
	 */
	void add(const IRA::CString& name,IRA::CString syscall,int N) {
		TRule *elem=new TRule(syscall,name,N);
		m_ruleSet.push_back(elem);
	}

	/**
	 * This function adds an execution rule to the parser. It associates a command to a procedure. The procedure is a collection of commands itself.
	 * This procedure is added in a separate set of commands so that they can be handled more simply, for example at run time to load a new procedures file.
	 * @param name name of the command associated to the procedure
	 * @param package extra string that can be used to compose the answer, it can be the name of a collection that the function belongs to.
	 * @param procedure list of commands that compose the procedure
	 * */
	void addExtraProcedure(const IRA::CString& name,const IRA::CString& package,const ACS::stringSeq& procedure,int N) {
		TRule *elem=new TRule(procedure,name,package,N);
		baci::ThreadSyncGuard guard(&m_extraRuleSetMutex);
		m_extraRuleSet.push_back(elem);
	}

	/**
	 * This method is in charge of deleting the extra rule set.
	 */
	void clearExtraProcedures() {
		baci::ThreadSyncGuard guard(&m_extraRuleSetMutex);
		typename TRuleSet::iterator i;
		for(i=m_extraRuleSet.begin(); i!=m_extraRuleSet.end(); i++) {
			if ((*i)!=NULL) delete *i;
		}
		m_extraRuleSet.clear();
	}
	
	/**
	 * This method is used to override the default replacement for the jolly character.
	 * @replace the string that will replace the jolly character in command before execution
	 */
	//void setJollyCharReplacement(const IRA::CString& replace) { m_jollyCharReplacement=replace; }
	
	/**
	 * This function is a wrapper of the function <i>executeCommand()</i> but taking into account the exceptions. 
	 * There are two possible error cases:  If the command is malformed (parsing time) or if an error was generated 
	 * by the called function (execution time error). In both cases the respective error message is expanded and 
	 * returned in a string; the name of the command and the  <i>errorDelimiter</i> character is always preponed.  
	 * If no errors were found the name of the command, the <i>answerDelimiter</i> and the returnd values (if any) are returned.
	 * In case of errors the parser will take also care of raising the proper exception.
	 * @throw ParserErrors::ParserErrorsExImpl
	 * @throw ACSErr::ACSbaseExImpl
	 * @param command string that stores the command to be executed.
	 * @param out  the result of the requested command
	 */
	void run(const IRA::CString& command,IRA::CString& out) throw (ParserErrors::ParserErrorsExImpl,ACSErr::ACSbaseExImpl);

	/**
	 * This function parses and executes a command asynchronously independently if the command is a simple procedure, a simple command with or without the time mark character.
	 *  In case when the operation completes a callback is also issued to inform the caller.
	 * @throw ParserErrors::ParserErrorsExImpl
	 * @param command properly formatted command to be executed
	 * @param callBack callback function to be invoked as soon as the operation completes
	 * @param callBackParam parameter to passed to the callback function as argument
	 */
	void runAsync(const IRA::CString& command,_SP_CALLBACK(callBack),const void * callBackParam)  throw (ParserErrors::ParserErrorsExImpl);

	/**
	 * Call in order to know the command delimiter char that the parser is using 
	*/
	const char& getCommandDelimiter() const { return m_commandDelimiter; }

	/**
	 * Call in order to know the parameters delimiter char that the parser is using 
	*/	
	const char& getParameterdelimiter() const { return m_delimiter; }

	/**
	 * Call in order to know the time marker char that the parser is using 
	*/	
	const char& getTimeDelimiter() const { return m_timeDelimiter; }
	
private:
	/**
	 * This enlists the various command that the parser is able to manage
	 */
	enum TCommandType {
		COMMAND,     				/*!< this is a pure command, it is executed locally  */ 
		SYSTEMCALL,                /*!< this will executes a  program available in the OS*/
		REMOTECOMMAND,     /*!< this command requires a remote invokation */
		PROCEDURE ,                /*!< this a complex command, usually composed by more than one command or procedure */
		KEYCOMMAND              /*!> this command is interpreted and executed directly by the scheduler */
	} ;
	class TRule {
	public:		
		TRule(CBaseFunction * const func,const IRA::CString& name,const long& prm): m_func(func),m_name(name),m_inputParametersNumber(prm),m_remoteFunc(NULL),m_package(""),
		m_param(0),m_type(COMMAND),m_syscall("")
		{
			m_procedure.length(0);
			m_parametersNumber=m_func->arity();
		}
		TRule(_SP_REMOTECALL(remotefunc),const IRA::CString& name,const IRA::CString package,const long& prm): m_func(NULL),m_name(name),m_inputParametersNumber(0),m_remoteFunc(remotefunc),m_package(package),
				m_param(prm),m_type(REMOTECOMMAND),m_syscall("")
		{
			m_procedure.length(0);
		}
		TRule(const ACS::stringSeq& procedure,const IRA::CString& name,const IRA::CString& package,const long& prm): m_func(NULL),m_name(name),m_inputParametersNumber(prm),m_remoteFunc(NULL),
				m_package(package),m_param(0),m_type(PROCEDURE),m_syscall("")
		{
			m_procedure.length(procedure.length());
			for (unsigned  i=0;i<procedure.length();i++) {
				m_procedure[i]=procedure[i];
			}
			m_parametersNumber=m_inputParametersNumber;
		}
		TRule(const IRA::CString& syscall,const IRA::CString& name, const long& prm): m_func(NULL),m_name(name),m_inputParametersNumber(prm),m_remoteFunc(NULL),m_package(""),
				m_param(0),m_type(SYSTEMCALL),m_syscall(syscall)
		{
			m_procedure.length(0);
			m_parametersNumber=m_inputParametersNumber;
		}

		TRule(const IRA::CString& name, const long& prm): m_func(NULL),m_name(name),m_inputParametersNumber(prm),m_remoteFunc(NULL),m_package(""),
				m_param(0),m_type(KEYCOMMAND),m_syscall("")
		{
			m_procedure.length(0);
			m_parametersNumber=m_inputParametersNumber;
		}

		virtual ~TRule() {
			if (m_func!=NULL) delete m_func;
		}
		CBaseFunction *m_func;
		IRA::CString m_name;
		long  m_inputParametersNumber;
		_SP_REMOTECALL(m_remoteFunc);
		IRA::CString m_package;
		long m_param;
		TCommandType m_type;
		IRA::CString m_syscall;
		ACS::stringSeq m_procedure;
		long m_parametersNumber;
	};
	/**
	 * This describes the single item of the execution list 
	 */
	class TExecutionUnit {
	public:
		/**
		 * Constructor
		 * @param command this is the full command with all the arguments if needed
		 * @param package extra qualifier of the command, if the command belongs to a procedure, for example, this is the procedure name
 		 * @param callBack pointer to the function called when the execution is completed
		 * @param parameter argument of the call back function 
		 */
		TExecutionUnit(const IRA::CString& command,const IRA::CString& name,_SP_CALLBACK(callBack)=NULL,const void* parameter=NULL): m_command(command), m_callBack(callBack), m_parameter(parameter), m_name(name) {
		}
		IRA::CString m_command;
		_SP_CALLBACK(m_callBack);
		const void *m_parameter;
		IRA::CString m_name;
	};
	typedef std::vector<TRule *> TRuleSet;
	typedef std::list<TExecutionUnit *> TExecutionList;
	
	/**
	 * Defines the timer job by storing its parameters
	 */
	class TTimerJob {
	public:
		TTimerJob(CParser* const parser,const IRA::CString& command): m_parser(parser), m_command(command) {
		}
		CParser *m_parser;
		IRA::CString m_command;
	};
	
	/**
	 * This function parses a command and if the respective execution rule was set it executes the command. If the command is postponed command (for example a procedure)
	 * it will be executed by the execution thread, in this case any answer are lost and the answer to the procedure is the name of the procedure plus the <i>answerDelimiter</i>
	 * character.. In case of errors  an exception is thrown. If the command has return values, they are returned as a list of values separeted by the
	 * <i>answerDelimiter</i> character.
	 * @throw ParserErrors::SyntaxErrorExImpl if the command does not fit the expected grammar (parsing time error)
	 * @throw ParserErrors::CommandNotFoundExImpl if the command has not been found (parsing time error)
	 * @thorw ParserErrors::NotEnoughParametersExImpl if the number of parameters is less than required by the command (parsing time error)
	 * @throw ParserErrors::SystemCommandErrorExImpl if an error was encountered during the execution of the command (execution time error). The
	 *                                                                             error stack of the calling sequence is enqueued.
	 * @thorw ParserErrors::ProcedureErrorExImpl if a procedure contains one or more command with syntax errors.
	 * @thorw ParserErrors::TimeFormatErrorExImpl if the time format of the timestamp for asynchronous commands is incorrect
	 * @throw ParserErrors::ParserTimerErrorExImpl if the internal timer could not be set up
	 * @throw ParserErrors::NotSupportedErrorExImpl if the command is async but the feature to execute it has not been installed
	 * @thorw ParserErrors::TooManyParametersExImpl if the command contains too many arguments
	 * @throw ParserErrors::ConversionErrorExImpl if at least one parameter has an unexpected format
	 * @throw ParserErrors::PackageErrorExImpl if the destination package of a remote invokation is not reachable
	 * @throw ParserErrors::RemoteCommandErrorExImpl if the command was executed "remotely" and results in an error
	 * @throw ACSErr::ACSbaseExImpl
	 * @param command properly formatted command to be executed
	 * @param instr after execution it stores the name of the just executed command.
	 * @return the output of the command, if more than one value has to be returned (for example in case of more than one output
	 *               arguments), the list is separated by the <i>answerDelimiter</i>. In case of exception the value is not meaningful.
	 */
	IRA::CString executeCommand(const IRA::CString& command,IRA::CString& instr) throw (ParserErrors::SyntaxErrorExImpl,ParserErrors::CommandNotFoundExImpl,
			ParserErrors::NotEnoughParametersExImpl,ParserErrors::SystemCommandErrorExImpl,ParserErrors::ProcedureErrorExImpl,ParserErrors::TimeFormatErrorExImpl,ParserErrors::ParserTimerErrorExImpl,
			ParserErrors::NotSupportedErrorExImpl,ParserErrors::TooManyParametersExImpl,ParserErrors::ConversionErrorExImpl,ParserErrors::PackageErrorExImpl,ParserErrors::RemoteCommandErrorExImpl,
			ACSErr::ACSbaseExImpl);
	
	/**
	 * Format an output string that summarizes the time based commands that are in the execution queue.
	 * @return the properly formatted string
	 */
	IRA::CString showTimeBasedCommands();

	/**
	 * Flushes the commands in the time base command queue.
	 * @param pos indicates which position in the queue must be flushed, if negative, all the events are flushed
	 */
	void flushTimeBasedCommands(const int& pos);

	/**
	 * Return the next token in a string. The string is scrolled starting from the given position. Tokens are delimited
	 * by the delimiter character.
	 * @param str string to analyze
	 * @param start position inside the string to start from
	 * @param delimiter character used as separations between tokens
	 * @param ret the token
	 * @return true if a token has been found
	 */
	bool getNextToken(const IRA::CString& str,int &start,char delimiter,IRA::CString &ret) const;
	
	/**
	 * Parses the command line and returns back the instruction and the list of arguments.
	 * @param line string containing the command line
	 * @param instr name of the instruction to be executed 
	 * @param pars list of the arguments as strings, a maximum of <i>maxPars</i> will be included
	 * @param timeTagged true if the command must be executed at a given time or asynchronously
	 * @param timeCommand if <i>timeMark</i> is true, it contains the command line  without the time mark.
	 * @param timeMark if <i>timeMark</i> is true, mark of time at which the command must be executed, if empty the command is executed as soon as possible
	 * @param maxPars the maximum allowed number of arguments 
	 * @return the number of parameters, if negative the parsing does not succeed.
	 */
	int parseCommand(const IRA::CString& line,IRA::CString& instr,IRA::CString* pars,bool& timeTagged,IRA::CString& timeCommand,IRA::CString& timeMark,const int& maxPars) const;
	
	/**
	 * Parses a string containing a time definitions into a numerical representation.
	 * @param timeMark time definitions,according the parser time definition
	 * @param executionTime numerical representation of the execution time
	 * @param interval if the time format represent a continuous execution time, this represents the execution interval. Otherwise is zero
	 * @return true if the string contains a valid time, false otherwise
	 */
	bool parseTime(const IRA::CString& timeMark,ACS::Time& executionTime,ACS::TimeInterval& interval) const;

	/**
	 * Check the correctness of the command against the command definitions given by the user.
	 * @throw ParserErrors::SyntaxErrorExImpl
	 * @throw ParserErrors::CommandNotFoundExImpl
	 * @throw ParserErrors::NotEnoughParametersExImpl
	 * @throw ParserErrors::TimeFormatErrorExImpl
	 * @throw ParserErrors::NotSupportedErrorExImpl
	 * @throw ParserErrors::TooManyParametersExImpl
	 * @param line command line to be checked
	 * @param the instruction of the command without arguments
	 * @param inParams filled with the list of input arguments.
	 * @param parNum number of elements of the list of arguments
	 * @param timeTagged true if the command is an asynchronous command 
	 * @param timeCommand command line without the time mark
	 * @param execTime execution time
	 * @param execInterval execution interval
	 * @return the pointer to the element that defines the command
	 */
	TRule *checkCommand(const IRA::CString& line,IRA::CString& instr, IRA::CString* inParams,WORD& parNum,bool& timeTagged,IRA::CString& timeCommand,ACS::Time& execTime,ACS::TimeInterval& execInterval)  throw (
			ParserErrors::SyntaxErrorExImpl,ParserErrors::CommandNotFoundExImpl,ParserErrors::NotEnoughParametersExImpl,ParserErrors::TimeFormatErrorExImpl,ParserErrors::NotSupportedErrorExImpl,
			ParserErrors::TooManyParametersExImpl);

	/**
	 * This function searches the execution rules to find if a command has been defined.
	 * @param instr name of the command to look for.
	 * @return the pointer to the execution rule, a NULL means that the rule was not located
	 */ 
	TRule *locate(const IRA::CString& instr);
	
	/**
	 * Push a command in the command list, the command list  will be executed by the thread according a FIFO
	 * @param line command line to be executed, with all the required arguments and extra symbol, time marks  for example.
	 * @param package name of the package that the command belongs to
 	 * @param callBack call back function that will be invoked when the command is executed
	 * @param parameter parameter passed as argument to the callback function 
	 */
	void pushCommand(const IRA::CString& line,const IRA::CString& package,_SP_CALLBACK(callBack)=NULL,const void* parameter=NULL);
	
	/**
	 * Push a procedure in the command list, it takes charge of expanding the procedure.
	 * @throw ParserErrors::ProcedureErrorExImpl
	 * @param name of the procedure
	 * @param procedure list of commands of the procedure
	 * @param callBack function to called when the procedure execution is finished
	 * @param parameter parameter passed as argument to the callback function
	 * @param procPrm list of procedure parameters
	 * @param parNumber number of parameters of the procedure
	*/
	void pushProcedure(const IRA::CString& name,const ACS::stringSeq& procedure,IRA::CString *procPrm=NULL,WORD& parNumber=0,_SP_CALLBACK(callBack)=NULL,const void* parameter=NULL) throw (
			ParserErrors::ProcedureErrorExImpl);
	
	/**
	 * Pop a command form the begging of the command list
	 */
	bool popCommand(TExecutionUnit *& cmd);
	
	//Copies are disabled
	CParser(const CParser<OBJ>& src);
	const CParser<OBJ>& operator = (const CParser<OBJ>& src);
	
	/**
	 * static function that implements the execution thread
	 */
	static void worker(void *threadParam);
	
	/**
	 * Static function that is called by the timer at every event
	 */
	static void timerHandler(const ACS::Time& time,const void * par);
	
	/**
	 * Static function called by the timer when the event expires in order to clean handler parameters
	 */
	static void timerCleanup(const void * par);
	
	OBJ *m_object;
	TRuleSet m_ruleSet;
	TRuleSet m_extraRuleSet;
	char m_delimiter;
	char m_commandDelimiter;	
	char m_timeDelimiter;
	IRA::CString m_answerDelimiter;
	IRA::CString m_errorDelimiter;
	/*RA::CString m_jollyChar;
	IRA::CString m_jollyCharReplacement;*/
	BYTE m_maxArgs;
	ACS::ThreadBase *m_pThread;
	ACS::ThreadManagerBase m_threadManager;
	TExecutionList  m_executionList;
	//runtime timer
	IRA::CScheduleTimer m_timer;
	// true if the timer has been initialized
	bool m_timerInitialized;
	bool m_levelTwoSupport;
	// allows to sync activities between thread and this class
	BACIMutex m_exeListMutex;
	BACIMutex m_extraRuleSetMutex;
};

#include "SP_parser.i"

};

#endif /*_SP_PARSER_H_*/
