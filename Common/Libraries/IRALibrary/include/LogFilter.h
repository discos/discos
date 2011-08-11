#ifndef LOGFILTER_H_
#define LOGFILTER_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: LogFilter.h,v 1.6 2011-03-14 13:18:54 a.orlati Exp $                                         													    */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 25/01/2007      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 01/09/2007      Added a thread that caches the repeated logs      */
/* Andrea Orlati(aorlati@ira.inaf.it) 17/03/2008      Fixed a bug in ErrorWrapper taht causes the component to crash      */
/* Andrea Orlati(aorlati@ira.inaf.it) 12/02/2010      Added the destroy method that should guarantee a smoother close up      */

#include "Definitions.h"
#include "String.h"
#include <vector>
#include <acstimeEpochHelper.h>
#include <acstimeDurationHelper.h>
#include <baciThread.h>
#include <acserr.h>

/**
 * This macro makes available (to the component or to the server) the filter. The visibility scope of this declaration should be global.  
*/
#define _IRA_LOGFILTER_DECLARE IRA::CLogFilter __autoLogFilter_

/**
 *  This macros calls the <i>activate()</i> method to start the filter.
 *  @param CACHE time (microseconds) during which a log already sent to the central logger is stored in the filter cache
 *  @param EXPIRE time (microseconds) of persistency of a log inside the filter cache. 
*/
#define _IRA_LOGFILTER_ACTIVATE(CACHE,EXPIRE) __autoLogFilter_.activate(CACHE,EXPIRE)

/**
 * This macro is required to import the filter if this is declared in a module different form the current one. 
*/
#define _IRA_LOGFILTER_IMPORT extern IRA::CLogFilter __autoLogFilter_

/**
 * This macro calls the <i>flush()</i> method to send all the pending logs.
*/
#define _IRA_LOGFILTER_FLUSH __autoLogFilter_.flush()

/**
 * This macro calls the <i>destroy()</i> method to close the LofFilter object
 */
#define _IRA_LOGFILTER_DESTROY __autoLogFilter_.destroy()

/**
 * This sends a log.
 * @param PRIORITY this is the priority associated to the log
 * @param ROUTINE the name of the routine form whihc the log is launched
 * @param MESSAGE the message of the log
*/
#define _IRA_LOGFILTER_LOG(PRIORITY,ROUTINE,MESSAGE) { \
	Logging::Logger::LoggerSmartPtr __logger_=getLogger(); \
	__autoLogFilter_.log(__logger_,Logging::ace2acsPriority(PRIORITY),MESSAGE,__FILE__,__LINE__,ROUTINE,LM_FULL_INFO); \
}

/** 
 * This is the static(to be used in static functions) counter part of the previous macro.
 * @param PRIORITY this is the priority associated to the log
 * @param ROUTINE the name of the routine form whihc the log is launched
 * @param MESSAGE the message of the log
*/ 
#define _IRA_LOGFILTER_STATIC_LOG(PRIORITY,ROUTINE,MESSAGE) { \
	Logging::Logger::LoggerSmartPtr __logger_=getNamedLogger(Logging::BaseLog::STATIC_LOGGER_NAME); \
	__autoLogFilter_.log(__logger_,Logging::ace2acsPriority(PRIORITY),MESSAGE,__FILE__,__LINE__,ROUTINE,LM_FULL_INFO); \
}

/**
 * This logs a completion.
 * @param CMP the ACS completion object 
 * @param PRIORITY this is the priority associated to the completion
 * 
*/
#define _IRA_LOGFILTER_LOG_COMPLETION(CMP,PRIORITY) __autoLogFilter_.log(*CMP.getErrorTraceHelper(),__FILE__,__LINE__,PRIORITY)

/**
 * This logs an exception.
 * @param EX the ACS exception object. It does not work for all other kind of exception included CORBA exception. 
 * @param PRIORITY this is the priority associated to the completion
 * 
*/
#define _IRA_LOGFILTER_LOG_EXCEPTION(EX,PRIORITY) __autoLogFilter_.log(EX,__FILE__,__LINE__,PRIORITY)

/**
 * This is a shortcut for the declaration of the guard.
 * @param NAME the CLogGuard object
 * @param TIME the time interval of the guard (microseconds) 
*/
#define _IRA_LOGGUARD_DECLARE(NAME,TIME) IRA::CLogGuard NAME(TIME)

/**
 * This macro will log a normal log if the guard permits this. 
 * @param NAME the CLogGuard object
 * @param PRIORITY this is the priority associated to the log
 * @param ROUTINE the name of the routine form whihc the log is launched
 * @param MESSAGE the message of the log
*/ 
#define _IRA_LOGGUARD_LOG(NAME,PRIORITY,ROUTINE,MESSAGE) { \
	Logging::Logger::LoggerSmartPtr __logger_=getLogger(); \
	NAME.log(__logger_,Logging::ace2acsPriority(PRIORITY),MESSAGE,__FILE__,__LINE__,ROUTINE,LM_FULL_INFO); \
}

/** 
 * This is the static(to be used in static functions) counter part of the previous macro.
 * @param NAME the CLogGuard object
 * @param PRIORITY this is the priority associated to the log
 * @param ROUTINE the name of the routine form whihc the log is launched
 * @param MESSAGE the message of the log
*/ 
#define _IRA_LOGGUARD_STATIC_LOG(NAME,PRIORITY,ROUTINE,MESSAGE) { \
	Logging::Logger::LoggerSmartPtr __logger_=getNamedLogger(Logging::BaseLog::STATIC_LOGGER_NAME); \
	NAME.log(__logger_,Logging::ace2acsPriority(PRIORITY),MESSAGE,__FILE__,__LINE__,ROUTINE,LM_FULL_INFO); \
}

/**
 * This logs a completion.
 * @param NAME the CLogGuard object
 * @param CMP the ACS completion object 
 * @param PRIORITY this is the priority associated to the completion
 * 
*/
#define _IRA_LOGGUARD_LOG_COMPLETION(NAME,CMP,PRIORITY) NAME.log(*CMP.getErrorTraceHelper(),__FILE__,__LINE__,PRIORITY)

/**
 * This logs an exception.
 * @param NAME the CLogGuard object
 * @param EX the ACS exception object. It does not work for all other kind of exception included CORBA exception. 
 * @param PRIORITY this is the priority associated to the completion
 * 
*/
#define _IRA_LOGGUARD_LOG_EXCEPTION(NAME,EX,PRIORITY) NAME.log(EX,__FILE__,__LINE__,PRIORITY)

	 
namespace IRA {

/**
 * This class allows to prevent log duplication based on time intervals. The working of the guard is very simple: the first time a log request
 * is issued the request is forwarded, then all other requests are ignored until the time interval is elapsed. in the latter case the log is 
 * forwared togetther with additional information.
 * The additional information consists for both errors and normal logs in two fields:
 *     @arg \c RepeatCounter reports the number of times that event was repeated in the given period.
 *     @arg \c Period this is the first and the last time the event occurred
 * In case the event is an error an extra field is added:
 *     @arg \c ErrorTraceSize reports the depth of the error trace of the error
 * The great concern about this policy can be explained with a practical example: let's suppose a log request is issued twice; the first log is
 * forwared regularly, if the second one is issued before the time interval is elapsed will be ignored. That causes a loss of information.
 * This class is NOT thread safe.
 * A typical call sequence could be:
 * <pre>
 * // declare the log guard with 1 sec of interval.
 * _IRA_LOGGUARD_DECALRE(guard,1000000);
 * ...................
 * for(int k=0;k<100;k++) {
 *    try {
 *       ....................
 *    }
 *    catch (ACSErr::ACSbaseExImpl& ex) {
 *       _IRA_LOGGUARD_LOG_EXCEPTION(guard,ex,LM_ERROR);
 *    } 
 * }
 * </pre>
 * This class can be used more generally to avoid code repetitions in general, for that purpose the method <i>check()</i> should be used:
 * <pre>
 * CLogGuard guard(1000000);
 * for(int k=0;k<100;k++) {
 *    //do something
 *    if (guard.check()) //do something else
 * }
 *</pre>
*/ 	
class CLogGuard {
public:
	/**
	 * Contrusctor.
	 * @param interval this parameter is the number of microseconds that the gaurd waits for a log to be posted to the logger again. If zero
	 * the guard is disabled and all the logs or errors are forwared to the logger. 
	*/ 
	CLogGuard(DDWORD interval) : m_counter(0), m_interval(interval), m_firstTime(true) { }
	
	/**
	 * Destructor.
	*/
	virtual ~CLogGuard() { }
	
	/** 
	 * This function should be used to log a message. The message will be effectively logged the first time or after hte time interval has elapsed. 
	 * @param logger that's the logger object that should be used to log the message
	 * @param prior the log priority
	 * @param msg the log text 
	 * @param fn the file name of the source file that generates the log message
	 * @param ln the line number inside the file name given before. The pair (filename,linenumber) will be the way the filter recognizes the log.
	 * @param rtn the name of the routine that generates the log message
	 * @param flg sets the flags that will aplly to the log entry that will be submitted
	 */
	virtual void log(Logging::Logger::LoggerSmartPtr& logger,const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fn,
	  const DDWORD& ln,const CString& rtn,const unsigned flg);
	  
	/** 
	 * This function should be used to log an error trace coming from either an exception or a completion. The error will be effectively logged 
	 * the first time or after the time interval has elapsed.  
	 * @param et this is the reference to the error trace helper that cointains the error stack to be logged.
	 * @param fn the file name of the source file that wants to log the error (not effectively uysed)
	 * @param ln the line number inside the file name given before (not effectively uysed)  
	 * @param priority the error will be logged with this priority
	 */	  
	virtual void log(ACSErr::ErrorTraceHelper& et,const CString& fn,const DDWORD& ln,const ACE_Log_Priority& priority);
	
	/**
	 * Checks if the action can be done or not.
	 * @return true if the log or a generic action can be done according to the guard policy, false otherwise
	*/
	virtual bool check();
	
	/**
	 * This method can be used to change the time interval for the guard on-the-fly. This method will not work if guard is not the first time
	 * that is enqueried. @sa <i>m_firstTime</i>.
	 * @param interval new interval in microseconds
	*/
	virtual void changeInterval(const DDWORD& interval);
	
protected:

	/**
	 * This class stores the data that are specific for normal log messages. 
	*/
	class CLogWrapper {
	public:
		/**
		 * It creates the log wrapper initializing all the members. 
		*/
		CLogWrapper(Logging::Logger::LoggerSmartPtr& lgg,const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fName,
		  const DDWORD& lNumber,const CString& rtn,const unsigned& flg): priority(prior), message(msg), fileName(fName), 
		  lineNumber(lNumber), routine(rtn), flags(flg)  { logger=lgg; }
		/** Destructor. */
		~CLogWrapper() { }
		/** pointer to the logger to be used to log the message */
		Logging::Logger::LoggerSmartPtr logger;
		/** the log will be forwarded to the central logger using this priority */
		Logging::BaseLog::Priority priority;
		/** the message to be logged */
		CString message;
		/** the file name of the source code where the log originates from */
		CString fileName;
		/** the line numeber of the code where the log originates from */
		DDWORD lineNumber;
		/** the name of the routine that issued the log operation */
		CString routine;
		/** extra flags for the log */
		unsigned flags;
	};
	
	/**
	 * This class stores the data that are specific for errors. 
	*/
	class CErrorWrapper : public ACSErr::ErrorTraceHelper {
	public:
		/**
		 * It creates the error wrapper initializing all the members. 
		*/
		CErrorWrapper(ACSErr::ErrorTrace& et,const ACE_Log_Priority& pr): ACSErr::ErrorTraceHelper(et),m_logPriority(pr),m_trace(&et) {
			m_delete=false;
		}
		/**
		 * It creates the error wrapper initializing all the members. At finalization the pointer to the ErrorTrace will be disposed. 
		*/
		CErrorWrapper(ACSErr::ErrorTrace* et,const ACE_Log_Priority& pr): ACSErr::ErrorTraceHelper(*et),m_logPriority(pr),m_trace(et) {
			m_delete=true;
		}
		/**
		 * Destructor.  
		*/
		~CErrorWrapper() { 
			if (m_delete) delete m_trace;
		}
	
		/**
		 * This method overrides the method of the base class defining a slightly different behaviour. The parent methods defined by
		 * the ACS team logs all the error trace; this method just logs the error at the top of the stack.
		 * @param priorty the priority of the log
		*/  
		void log(ACE_Log_Priority priorty=LM_ERROR);	// override. 
		
		/**
		 * @return the priority that is associated to the error
		*/
		ACE_Log_Priority getLogPriority() const { return  m_logPriority; }
			
	private:
		/** the error will be logged with this priority. */
		ACE_Log_Priority m_logPriority;
		
		ACSErr::ErrorTrace *m_trace;
		bool m_delete;
	};
	
	/*
	 * This method is used internally for logging a normal log message together with additional information:
	 *    @arg \c RepeatCounter reports the number of times that event was repeated in the given period.
 	 *    @arg \c Period this is the first and the last time the event occured
 	 * @param log this object contains all the information required for the log posting
 	 * @param startTime epoch of the first log request
 	 * @param stopTime epoch of the last log request
 	 * @param counter number of requests occurred in the interval bounded by the above parameters. 
 	*/ 
	void logLog(const CLogWrapper& log,TIMEVALUE& startTime,TIMEVALUE& stopTime,const DWORD& counter) const;
	
	/*
	 * This method is used internally for logging an error event together with additional information:
	 *    @arg \c RepeatCounter reports the number of times that event was repeated in the given period.
 	 *    @arg \c Period this is the first and the last time the event occured
 	 *    @arg \c ErrorTraceSize reports the depth of the error trace of the error
 	 * The last information is needed for completeness because just the error at the top of the stack is logged
 	 * @param log this object contains all the information required for the log posting
 	 * @param startTime epoch of the first log request
 	 * @param stopTime epoch of the last log request
 	 * @param counter number of requests occurred in the interval bounded by the above parameters. 
 	*/ 	
	void logError(CErrorWrapper& err,TIMEVALUE& startTime,TIMEVALUE& stopTime,const DWORD& counter) const;
	
private:

	/**
	 * Copy constructor. Disabled
	*/
	CLogGuard(const CLogGuard& src);
	
	/**
	 * Copy operator. Disabled.
	*/ 
	const CLogGuard& operator=(const CLogGuard& src);
	
	/** this is the counter that counts the number of times the event occurs in the interval */
	DWORD m_counter;
	/** time interval in microseconds */
	DDWORD m_interval;
	/** true if the event has not occured yet */
	bool m_firstTime;
	/** time when the event occured the first time */
	TIMEVALUE m_time;
};

/**
 * This class can be used to keep track of the number of logs published toward the ACS centralized log in order
 * to avoid to flood the service and the network. The logs are identified by the pair (file name,line number) of the
 * source code that wants to log a messages. This class is very rudimental and allows to filter out log messages and errors based just on time.
 * In order to ease the deplyment of this class a set of macros are supplied; typically if we consider a single module a sequence of calls could
 * be: 
 * <pre>
 * // declare the log filter. 
 * _IRA_LOGFILTER_DECALRE;
 * ...................
 * void CFooImpl::execute() {
 *   // activates the filter, 1 second of caching time, 3 seconds of expiration time
 *    _IRA_LOGFILTER_ACTIVATE(1000000,3000000);
 * }
 * 
 * void CFooImpl::cleanUp() {
 *    _IRA_LOGFILTER_FLUSH;
 *   _IRA_LOGFILTER_DESTROY;
 * }
 * ...................
 * void CFooImpl:doSomething() {
 *    for(int k=0;k<100;k++) {
 *       try {
 *            ....................
 *       }
 *       catch (ACSErr::ACSbaseExImpl& ex) {
 *           _IRA_LOGFILTER_LOG_EXCEPTION(ex,LM_ERROR);
 *       } 
 *       _IRA_LOGFILTER_LOG(LM_INFO,"doSomething()","iteration done!");
 *    }
 * }
 * </pre>
 * In case your software is composed by more than one module the _IRA_LOGFILTER_IMPORT directive must be used.
 * This class is thread safe. The algorithm used by the filter is trivial. if an event arrives for the first time, the filter lets it pass.
 * if a log, with its identification codes already known to the filter, arrives it is cached and the associated counter is incremented. if
 * an event occurs more than once in the "cache" period at the end of that period it is logged with additional information. If an event 
 * does not happen in last "expire" microseconds it is cleared from the filter, so that the next time it is like it was the first time.     
 * The additional information consists for both errors and logs in two fields:
 *     @arg \c RepeatCounter reports the number of times that event was repeated in the given period.
 *     @arg \c Period this is the first and the last time the event occured
 * In case the event is an error an extra field is added:
 *     @arg \c ErrorTraceSize reports the depth of the error trace of the error
 * This last field is required because not all the error trace is logged but just the error at the top of the stack. In order to keep consistency
 * with normal logs the errors at the end of the cache period are logged using the current time as timestamp (not the time in which the error
 * occurred).    
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CLogFilter : public CLogGuard {
public:

	/**
	 * Default constructor. The object is not activated yet. 
	*/
	CLogFilter();
	
	/**
	 * Constructor. It creates an already activated filter object.
	 * @param cacheTime represents the persistency of a log message, that means the time interval (microseconds) that a log message
	 *        is not published again after it has been sent the first time. After that period is elapsed, the log is posted with the additional 
	 *        information about how many times the same message should have been sent. 
	 * @param expirationTime this represents the number of microseconds the filter will remember that a certain message has already been posted.
	 * 		  In order to avoid information loss the user must make sure that this value is greater than the other parameter. If both parameters
	 *        the filter is not activated. that means that all logs are not filtered. 
	*/ 
	CLogFilter(DDWORD cacheTime,DDWORD expirationTime);
	
	/*
	 * Destructor
	*/
	~CLogFilter();

	/**
	 * This method must be used to activate the filter. It's important to keep in mind that this method is not thread safe, so it's up to the
	 * programmer to make sure that this is called before any other operation.
	 * @param cacheTime represents the persistency of a log message, that means the time interval (microseconds) that a log message
	 *        is not published again after it has been sent the first time. After that period is elapsed, the log is posted with the additional 
	 *        information about how many times the same message should have been sent. 
	 * @param expirationTime this represents the number of microseconds the filter will remember that a certain message has already been posted.
	 * 		  In order to avoid information loss the user must make sure that this value is greater than the other parameter. If both parameters
	 *        the filter is not activated. that means that all logs are not filtered. 
	*/ 
	void activate(DDWORD cacheTime,DDWORD expirationTime);

	/**
	 * It flushes (it sends to the logger) all the logs stored in the filter cache. All logs are sent together with the additional information.
	*/
	void flush();
	
	/**
	 * It forses the current object to be freed.
	 */
	void destroy();
	
	/** 
	 * This function should be used to log a message. The message will be effectively logged only if the filter conditions are matched, otherwise
	 * it will be cached and logged later, according the filter policies. 
	 * @param logger that's the logger object that should be used to log the message
	 * @param prior the log priority
	 * @param msg the log text 
	 * @param fn the file name of the source file that generates the log message
	 * @param ln the line number inside the file name given before. The pair (filename,linenumber) will be the way the filter recognizes the log.
	 * @param rtn the name of the routine that generates the log message
	 * @param flg sets the flags that will aplly to the log entry that will be submitted
	 */
	void log(Logging::Logger::LoggerSmartPtr& logger,const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fn,
	  const DDWORD& ln,const CString& rtn,const unsigned flg); //override

	/** 
	 * This function should be used to log an error trace coming from either an exception or a completion. The error will be effectively logged 
	 * only if the filter conditions are matched, otherwise it will be cached and logged later, according the filter policies.  
	 * @param et this is the reference to the error trace helper that cointains the error stack to be logged.
	 * @param fn the file name of the source file that wants to log the error
	 * @param ln the line number inside the file name given before. The pair (filename,linenumber) will be the way the filter recognizes the error.  
	 * @param priority the error will be logged with this priority
	 */
	void log(ACSErr::ErrorTraceHelper& et,const CString& fn,const DDWORD& ln,const ACE_Log_Priority& priority=LM_ERROR); //override
	
	/**
	 * @return true if the filter has been activated. 
	*/
	inline bool isActive() { return m_active; }
	   
private:
	/** 
	 * This class is the entry in the filter cache that stores log or error information.
	*/
	class CDataRecord {
		public:
			enum TLogType {
				ERROR,         /*!< the entry stores an error  */     
				LOG            /*!< the entry stored a normal log message  */     
			};
			/** This the exact timestamp the event was issued the first time */
			TIMEVALUE firstTime;
			/** This is the exact timestamp the event occured the last time */
			TIMEVALUE lastTime;
			/** identfiers of the log */
			DWORD lineNumber;
			CString fileName;
			/** pointer to the appropriate event wrapper */
			void *logPointer;
			/** this identifies the type of the entry: a normal log message or an error */
			TLogType logType;
			/** this is the counter that counts the number of times the event occurs in the period <i>firstTime</i> <i>lastTime</i> */
			DWORD counter;
			/**
			 * Constructor. 
			*/
			CDataRecord(const DWORD& line,const CString& file,void * const data,const TLogType& type);
			/**
			 * Destructor.
			*/
			~CDataRecord(); 
			/**
	 		 * It increases the counter by one.  
    		*/
			inline void incCounter() { counter++; }
			/**
	 		 * @return the value of the counter.
			*/
			inline DDWORD getCounter() const { return counter; }
			/** 
			 * This method is used internally when an already known log is requested again. It updates the <i>lastTime</i> timestamp. 
			*/
			void updateTimer();
			/**
			 * This method is used internally when an event is logged because the cache time is elapsed. It zeros the counter and updates the 
			 * timers. 
			*/
			void reset();
	};
	typedef std::vector<CDataRecord *>::iterator TIterator;
	
	/**
	 * Copy constructor. Disabled
	*/
	CLogFilter(const CLogFilter& src);
	
	/**
	 * Copy operator. Disabled.
	*/ 
	const CLogFilter& operator=(const CLogFilter& src);
	
	/** Filter cache */
	std::vector<CDataRecord *> m_logs;
	/** the cache time */
	DDWORD m_cache;
	/** the expire time */
	DDWORD m_expire;
	/** if true the filter is active, that also means the working thread is started */
	bool m_active;
	/** Indicated if the object has already been destroyed during clean up stage */
	bool m_destroyed;	
	/**
	 * mutex used to synchronize the operation over the filter. In particular to perform mutual exclusion between the inner working thread and the
	 * threads which wants to log messages.
	*/ 
	BACIMutex m_localMutex;
	/** the working thread */
	ACS::ThreadBase *m_pThread;
	/** the ACS thread manager */
	ACS::ThreadManagerBase m_threadManager;
	/** 
	 * It searches for a record, if it is found the pointer is returned, otherwise a NULL
	*/
	CDataRecord *findRecord(const CString& fileName,const DDWORD& lineNumber) const;
	/** 
	 * It adds a new record (an event with all the other information) to the cache.
	*/ 
	void addRecord(CDataRecord* const record);
	/** 
	 * This method is used directly by the working thread. It removes the log entries that are expired. Expired entries have the counter 
	 * equal to zero and the "expire" time is already elapsed. It also logs the entries with the "cache" time elapsed. This method is thread safe.
	*/ 
	void processLogs();
	/**
	 * It posts the cached events to the central logger with the required additional information.
	*/
	void logRecord(const TIterator& pos);
	/**
	 * This function is the job that the working thread continuosly performs.
	*/
	static void worker(void *threadParam);
	
	using CLogGuard::check;
	using CLogGuard::changeInterval;
	
};
	
}

#endif /*LOGFILTER_H_*/
