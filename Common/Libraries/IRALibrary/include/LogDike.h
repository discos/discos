#ifndef _LOGDIKE_H_
#define _LOGDIKE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(a.orlati@ira.inaf.it) 15/06/2012      Creation                                          */

#include "CustomLoggerUtils.h"
#include "Definitions.h"
#include "String.h"
#include "IRATools.h"
#include <acserr.h>

/**
 * Return the reference to the logger object
 */
#define _IRA_LOGDIKE_GETLOGGER getLogger()

/**
 * Return the reference to the logger object to be used when acting inside a static method
 */
#define _IRA_LOGDIKE_GETSTATICLOGGER  getNamedLogger(Logging::BaseLog::STATIC_LOGGER_NAME)

/**
 * Call this macro inside the working thread to log all pending events
 * @param DIKE LogDike object
 */
#define _IRA_LOGDIKE_CHECK(DIKE) DIKE.openDike();

/**
 * This sends a log.
 * @param DIKE LogDike object
 * @param PRIORITY this is the priority associated to the log
 * @param ROUTINE the name of the routine form which the log is launched
 * @param MESSAGE the message of the log
*/
#define _IRA_LOGDIKE_LOG(DIKE,PRIORITY,ROUTINE,MESSAGE) 	DIKE.log(Logging::ace2acsPriority(PRIORITY),MESSAGE,__FILE__,__LINE__,ROUTINE,LM_FULL_INFO)

/**
 * This logs an exception.
 * @param DIKE LogDike object
 * @param EX the ACS exception object. It does not work for all other kind of exception included CORBA exception.
 * @param PRIORITY this is the priority associated to the completion
*/
#define _IRA_LOGDIKE_EXCEPTION(DIKE,EX,PRIORITY) DIKE.log(EX,__FILE__,__LINE__,PRIORITY)

/**
 * This logs a completion.
 * @param DIKE LogDike object
 * @param CMP the ACS completion object
 * @param PRIORITY this is the priority associated to the completion
*/
#define _IRA_LOGDIKE_COMPLETION(DIKE,CMP,PRIORITY) DIKE.log(*CMP.getErrorTraceHelper(),__FILE__,__LINE__,PRIORITY)

/**
 * Flushes the object cache and prepare for its destruction
 * @param DIKE LogDike object
 */
#define _IRA_LOGDIKE_DESTROY(DIKE) DIKE.destroy()

/*
 * allows the configuration of object parameters
 * @param DIKE LogDike object
 * @param CACHETIME caching time of the object (microseconds)
 * @param EXPIRETIME expire time of the object (microseconds)
 */
#define _IRA_LOGDIKE_CONFIGURE(DIKE,CACHETIME,EXPIRETIME) DIKE.changeTimeSettings(CACHETIME,EXPIRETIME)

namespace IRA {

/**
 * This class can be used to keep track of the number of logs published toward the ACS centralized log in order  to avoid to flood the service and the network.
 * The logs are identified by the pair (file name,line number) of the source code that wants to log a messages. This class is very primitive and allows to filter out log messages and errors based just on time.
 * The class is not thread safe.
 * Since the proper use of this object is inside thread run loop code, the use has to make sure that the method <i>openDike()</i> is called with a frequency f that is f<< cacheTime.
 * In order to ease the use of this class a set of macros are supplied; typically if we consider a single module a sequence of calls could
 * be:
 * <pre>
 * WorkingClass::WorkingClass(): dike(_IRA_LOGDIKE_GETLOGGER)
 * {
 * }
 * void WorkingClass::runLoop()
 * {
 * 	_IRA_LOGDIKE_CHECK(dike);
 *     //do something useful
 *     _IRA_LOGDIKE_LOG(dike,LM_NOTICE,"WorkingClass::runLoop()","OPERATION IN PROGRESS");
 *    try {
 *    	// do something more
 *    }
 *    catch (ComponentErrors::SocketErrorExImpl& ex) {
 *		_ADD_BACKTRACE(ComponentErrors::WatchDogErrorExImpl,impl,ex,"CWatchDog::runLoop()");
 *		impl.setReason("Cannot connect command socket");
 *		_IRA_LOGDIKE_EXCEPTION(dike,impl,LM_ERROR);
 * 	}
 * }
 * </pre>
 * The algorithm used by the dike is trivial. if an event arrives for the first time, the filter lets it pass. if a log, with its identification codes already known to the filter, arrives it is cached and the associated counter is incremented. if
 * an event occurs more than once in the "cache" period at the end of that period it is logged with additional information. If an event does not happen in last "expire" microseconds it is cleared from the dike, so that the next time
 * it is like it was the first time.
 * The additional information consists for both errors and logs in two fields:
 *     @arg \c RepeatCounter reports the number of times that event was repeated in the given period.
 *     @arg \c Period this is the first and the last time the event occurred
 *  In order to keep consistency with normal logs all the errors in the error trace,  at the end of the cache period, are logged using the current time as time stamp (not the time in which the error  occurred).
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
*/
class CLogDike  {
public:

	/**
	 * Constructor
	 * @param pointer to the logger object that has to be used
	 * @param cacheTime is  the time interval (microseconds) that a log message  is not published again after it has been sent the first time. After that period is elapsed, the log is posted with the additional
	 *                   information about how many times the same message should have been sent. Default is 4000000 (4 sec)
	 * @param expireTime this represents the number of microseconds the filter will remember that a certain message has already been posted. In order to avoid information loss the user must make
	 *                   sure that this value is greater than the other parameter. Default is 10000000 (10 sec)
	 */
	CLogDike(Logging::Logger::LoggerSmartPtr logger,const long& cacheTime=4000000,const long& expireTime=10000000);

	/**
	 * Destructor
	 */
	virtual ~CLogDike();

	/**
	 * This function checks if there are log events that could be or discarded or logged with its own statistics (computed along the caching period).
	 * The user must take care of calling it with appropriate frequency.
	 */
	void openDike();

	/**
	 * This method allows to reset the cache and the expire time in order to configure the filtering policy of the class.
	 * @param cacheTime new cache time in microseconds
	 * @param expireTime new expire time in microseconds
	 */
	void changeTimeSettings(const long& cacheTime,const long& expireTime);

	/**
	 * This function should be used to log a message with a preferred logger. The message will be effectively logged only if the filter conditions are matched, otherwise
	 * it will be cached and logged later, according the filter policies.
	 * @param logger that's the logger object that should be used to log the message
	 * @param prior the log priority
	 * @param msg the log text
	 * @param fn the file name of the source file that generates the log message
	 * @param ln the line number inside the file name given before. The pair (filename,line number) will be the way the filter recognizes the log.
	 * @param rtn the name of the routine that generates the log message
	 * @param flg sets the flags that will apply to the log entry that will be submitted
	 */
	void log(Logging::Logger::LoggerSmartPtr& logger,const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fn,
	  const DDWORD& ln,const CString& rtn,const unsigned flg);

	/**
	 * This function should be used to log a message. The message will be effectively logged only if the filter conditions are matched, otherwise
	 * it will be cached and logged later, according the filter policies.
	 * @param prior the log priority
	 * @param msg the log text
	 * @param fn the file name of the source file that generates the log message
	 * @param ln the line number inside the file name given before. The pair (filename,line number) will be the way the filter recognizes the log.
	 * @param rtn the name of the routine that generates the log message
	 * @param flg sets the flags that will apply to the log entry that will be submitted
	 */
	void log(const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fn,const DDWORD& ln,const CString& rtn,const unsigned flg);

	/**
	 * This function should be used to log a exception. The message will be effectively logged only if the filter conditions are matched, otherwise
	 * it will be cached and logged later, according the filter policies.
	 * @param et this is the reference to the error trace helper that contains the error stack to be logged.
	 * @param fn the file name of the source file that wants to log the error
	 * @param ln the line number inside the file name given before
	 * @param priority the error will be logged with this priority
	 */
	void log(ACSErr::ErrorTraceHelper& et,const CString& fn,const DDWORD& ln,const ACE_Log_Priority& priority);

	/**
	 * Flushes all the event in the list
	 */
	void flush();

	/**
	 * Destroys the event list and frees all the allocated memory. It also calls <i>flush()</i>
	 */
	void destroy();

private:

	/**
	 * Default constructor. Disabled.
	 */
	CLogDike();

	/**
	 * Copy constructor. Disabled
	*/
	CLogDike(const CLogDike& src);

	/**
	 * Copy operator. Disabled.
	*/
	const CLogDike& operator=(const CLogDike& src);

	/**
	 * This is the interface class of the managed events of the ClogDikeClass
	 */
	class CEvent {
	public:
		enum TEventType {
			LOGEVENT,
			ERROREVENT
		};
		/**
		 * Constructor.
		 * @param type identifies the type of the event
		 * @param file source file name
		 * @param line number of line (inside the source file)
		 */
		CEvent(const TEventType& type,const IRA::CString& file,const DDWORD& line);
		/**
		 * Destructor
		 */
		virtual ~CEvent();

		/**
		 * compare the event with a given identification couple (FileName,LineNumber)
		 * @param file Name of the source file
		 * @param line line number inside the source file
		 * @return true if the couple (FileName,LineNumber) match with the couple of the event
		 */
		bool match(const IRA::CString& file,const DDWORD& line) const;

		/**
		 * Check it the event if it has to be logged or cleared
		 * @param expireTime time to live of the event before disposing it
		 * @param cacheTime  period of time the event is cached before sending it to the logger
		 * @param logit the event has to be logged
		 * @param removeit the event has to be disposed
		 */
		void check(const ACS::TimeInterval& expireTime,const ACS::TimeInterval& cacheTime,bool& logit, bool& removeit) const;

		/**
		 * This method updates the event
		 */
		void updateEvent();

		/**
		 * After a logging the event counters have to be reset
		 */
		void reset();

		/**
		 * Called to log the event.
		 * @param logger logger object reference
		 * @param addExtraInfo controls if the extra info has to be added to the log
		 */
		virtual void logMe(Logging::Logger::LoggerSmartPtr& logger,bool addExtraInfo)=0;

		/**
		 * @return the number of times the event has occurred in the period
		 */
		inline DDWORD getCounter() const { return m_counter; }

		/**
		 * @return the initial time of the period
		 */
		inline ACS::Time getFirstEpoch() const { return m_firstEpoch; }

		/**
		 * @return the final time if the period
	     */
		inline ACS::Time getLastEpoch() const { return m_lastEpoch; }

	private:
		/**
		 * The type of the event
		 */
		TEventType m_type;
		/**
		 * the event is associated to a line number inside a source file
		 */
		DDWORD m_lineNumber;
		/**
		 * The event is associated with a file name
		 */
		IRA::CString m_file;
		/** This is the first time the event has been found */
		ACS::Time m_firstEpoch;
		/** This is the last  time the event has been found */
		ACS::Time m_lastEpoch;
		/**
		 * Counts how many times the event has occurred in the period
		 */
		DDWORD m_counter;
	};

	/**
	 * This class stores the data that are specific for errors.
	*/
	class CErrorWrapper : public ACSErr::ErrorTraceHelper, public CEvent {
	public:
		/**
		 * It creates the error wrapper initializing all the members.
		*/
		CErrorWrapper(ACSErr::ErrorTrace* et,const ACE_Log_Priority& pr,const CString& fName, const DDWORD& lNumber);

		/**
		 * Destructor.
		*/
		virtual ~CErrorWrapper();

		/**
		 * Implementation of the pure virtual method of the parent class
		 */
		void logMe(Logging::Logger::LoggerSmartPtr& logger,bool addExtraInfo);
	private:
		/** the error will be logged with this priority. */
		ACE_Log_Priority m_logPriority;
		ACSErr::ErrorTrace *m_trace;
	};

	/**
	 * This class stores the data that are specific for normal log messages.
	*/
	class CLogWrapper : public CEvent {
	public:
		/**
		 * It creates the log wrapper initializing all the members.
		*/
		CLogWrapper(const Logging::BaseLog::Priority& prior,const CString& msg,const CString& fName,const DDWORD& lNumber,const CString& rtn,const unsigned& flg);

		/** Destructor. */
		virtual ~CLogWrapper();

		/**
		 * Implementation of the pure virtual method of the parent class
		 * @param logger pointer to the logger object
		 * @param addExtraInfo add event tracking information
		 */
		void logMe(Logging::Logger::LoggerSmartPtr& logger,bool addExtraInfo);

	private:
		/** the log will be forwarded to the central logger using this priority */
		Logging::BaseLog::Priority m_priority;
		/** the message to be logged */
		CString m_message;
		/** the file name of the source code where the log originates from */
		CString m_fileName;
		/** the line number of the code where the log originates from */
		DDWORD m_lineNumber;
		/** the name of the routine that issued the log operation */
		CString m_routine;
		/** extra flags for the log */
		unsigned m_flags;
	};

	/**
	 * Events list
	 */
	std::vector<CEvent * > m_events;

	typedef std::vector<CEvent * >::iterator TIterator;

	/**
	 * Find a log event based on identification criteria: source file name and line number
	 * @return the pointer to the event or NULL if the event is not present
	 */
	CEvent *findEvent(const CString& fileName,const DDWORD& lineNumber);

	/**
	 * Add and event to the event list
	 * @param event new event (memory will be release automatically)
	*/
	void addEvent(CEvent* const event);

	/**
	 * Logging object
	 */
	Logging::Logger::LoggerSmartPtr m_logger;

	/**
	 * logging object has been initialized
	 */
	bool m_loggerReady;

	/**
	 * cache period
	 */
	ACS::TimeInterval m_cacheTime;

	/**
	 * Expire period
	 */
	ACS::TimeInterval m_expireTime;

};

};

#endif /* LOGDIKE_H_ */
