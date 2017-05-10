/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: ScheduleTimer.h,v 1.4 2010-04-30 10:08:45 a.orlati Exp $										                                                            */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when           What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)  19/12/2008     Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  29/04/2010     Modified the semantics of argument 'time' inside schedule function, now a zero represents now, immediate execution    */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/04/2010     Added the support for cleanup function   */

#ifndef SCHEDULETIMER_H_
#define SCHEDULETIMER_H_

#include <ace/Timer_Heap_T.h>
#include <ace/Timer_Queue_Adapters.h>
#include <Event_Handler_Handle_Timeout_Upcall.h>
#include <acstimeTimeUtil.h>
#include <vector>

namespace IRA {

/**
 * This class implements a timer that can be used to schedule events. Events could be one shot events or
 * continuous events. In the former case an absolut time stamp is given, in the latter a time stamp and then
 * an interval that must be between two separate events. 
 * One shot events are removed and freed automatically after they have happened, continuous ones must 
 * be esplicitally cancelled (<i>cancel()</i>). in any case all the events are freed at the timer distruction.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CScheduleTimer {
public:
	/**
	 * Defines the standard for call back functions
	 */
	typedef void (*TCallBack)(const ACS::Time&,const void *);
	/**
	 * Defines the clean up function that is eventually called if the event handler parameter must be freed
	 */
	typedef void (*TCleanupFunction)(const void *);
	/**
	 * Constructor
	 */
	CScheduleTimer();
	
	/**
	 * Destructor
	 */
	~CScheduleTimer();
	
	/**
	 * It must be called to initialize the timer.
	 * @return false if the timer fails to initialize
	 */
	bool init();
	
	/**
	 * This command will schedule a new timeout event based on a absolute time (one-shot) or based an absolute time
	 * and an interval.
	 * @param callBack pointer to the function that should be called back to handle the event
	 * @param timeMark absolute time mark, if zero and interval is zero a failure is returned, if interval is not zero it is considered to be "now".
	 * @param interval if non zero the event, after the first time is re-scheduled with this interval
	 * @param param parameter that is passed to the event handler
	 * @param cleanup if not null the function will be called when the parameter is not needed any more. The argument of the function is the pointer to the parameter. This allows the user to free
	 *        the parameter if required. Default is NULL. 
	 * @return false if the event could not be scheduled
	 */
	bool schedule(TCallBack callBack,const ACS::Time& timeMark,const ACS::TimeInterval& interval,const void* param,TCleanupFunction cleanup=NULL);
	
	/**
	 * This command cancel an event based on its start time. If two or more events have the same time
	 * it will be canceled the first scheduled. 
	 * @param time time stamp that describes the event.
	 * @return true if the operation was successful
	 */
	bool cancel(const ACS::Time& time);

	/**
	 * This command cancel an event based on its position inside the scheduled events queue.
	 * @param pos position of the event inside the queue, first element is the zero
	 * @return true if the operation was successful
	 */
	bool cancel(const unsigned& pos);
		
	/**
	 * This command cancel all the events.
	 * @return true if the operation was successful
	 */
	bool cancelAll();
	
	/**
	 * This command allows to scroll the scheduled events.
	 * @param index indicates the position of the event that must be returned, on exit it will point to the next element
	 * @param time  epoch of the event
	 * @param interval gap between two execution of the event
	 * @param parameter information of the event
	 * @return true if the event could be found
	 */
	bool getNextEvent(unsigned &index,ACS::Time& time,ACS::TimeInterval &interval,const void *&parameter);

private:
	/**
	 * This class implements the method <i>handle_timeout()</i> that will be called by the timer when the
	 * events is issued.
	 */
	class EventHandler:  public ACE_Event_Handler {
	public:
		/**
		 * Constructor
		 * @param callBack pointer to the function that has to be invoked when a certain events is fired
		 * @param oneShot true if the event is one shot
		 * @param timer pointer to the upper class
		 */
		EventHandler(TCallBack callBack,bool oneshot,CScheduleTimer *timer);
		/**
		 * Destructor
		 */
		~EventHandler();
		/**
		 * Called when a timeout has to be handled
		 * @param time current time
		 * @param param extra parameter from the schedule timer.
		 */
		int handle_timeout(const ACE_Time_Value& time,const void* param);
		
		/**
		 * It sets the event identifier the handler is associated to
		 * @param new identifier
		 */
		void setIdentifier(const long& id) { m_id=id; }
	private:
		TCallBack m_callBack;
		bool m_oneShot;
		CScheduleTimer *m_timer;
		long m_id;
	};
private:
	typedef struct {
		long id;
		ACS::Time time;
		ACS::TimeInterval interval;
		EventHandler *handler;
		TCleanupFunction cleanupFunction;
		const void* parameter;
	} TRecord;
	typedef std::vector<TRecord *> TEvents;
	typedef TEvents::iterator TEventsIterator;
	typedef ACE_Event_Handler_Handle_Timeout_Upcall TUpcall;
	typedef ACE_Timer_Heap_T<ACE_Event_Handler*,TUpcall,ACE_Null_Mutex> TTimer_Heap;
	typedef ACE_Thread_Timer_Queue_Adapter<TTimer_Heap> TThread_Timer_Queue;
	
	TThread_Timer_Queue *m_timerQueue;
	bool m_active;
	TEvents m_events;
	
	CScheduleTimer(const CScheduleTimer& src);
	void operator=(const CScheduleTimer& src);
	
	/**
	 * clear an event based on its id
	 * return false if it could not clear the event..probably missing
	 */
	bool clearEvent(const long& id);
};


};



#endif /*TIMER_H_*/
