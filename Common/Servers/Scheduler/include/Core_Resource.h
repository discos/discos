#ifndef CORE_RESOURCE_H_
#define CORE_RESOURCE_H_

// $Id: Core_Resource.h,v 1.8 2011-04-15 09:06:59 a.orlati Exp $

#define RESOURCE_INIT m_antennaBoss=Antenna::AntennaBoss::_nil(); \
									m_receiversBoss=Receivers::ReceiversBoss::_nil();	\
									m_defaultBackend=Backends::GenericBackend::_nil(); \
									m_antennaNC=NULL; \
									m_isTracking=false; \
									m_antennaBossError=m_receiversBossError=m_defaultBackendError=false; \
									m_currentDevice=0; 
									
#define RESOURCE_EXEC ACS_NEW_SIMPLE_CONSUMER(m_antennaNC,Antenna::AntennaDataBlock,Antenna::ANTENNA_DATA_CHANNEL,antennaNCHandler,static_cast<void*>(this)); \
					  m_antennaNC->consumerReady(); \
					  ACS_LOG(LM_FULL_INFO, "Core::execute()", (LM_INFO,"ANTENNA_NC_READY")); \
					  m_defaultBackendInstance=m_config->getDefaultBackendInstance(); \
					  try { \
					  	m_parser=new CParser<CCore>(this,10,true);\
					  } \
					  catch (...) { \
						_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CCore::execute()"); \
						throw dummy; \
	                  } \
	                  if (!m_timer.init()) { \
		                _EXCPT(ComponentErrors::TimerErrorExImpl,dummy,"CCore::initialize()"); \
		                 dummy.setReason("Timer could not be initialized"); \
		                throw dummy; \
	                  }

#define RESOURCE_CLEANUP if (m_antennaNC!=NULL) m_antennaNC->disconnect(); \
						 m_antennaNC=NULL; \
						 if (m_parser) { \
						    delete m_parser; \
						 } \
						 ACS_LOG(LM_FULL_INFO,"CCore::cleanUp()",(LM_INFO,"PARSER_DESTROYED")); \




/**
 * Thread sync mutex for timer resource
 */
BACIMutex m_timerMutex;
/**
 * This is the internal timer used to schedule event at precise instants
*/
IRA::CScheduleTimer m_timer;
/**
 * Parser that executes the command line parsing
 */
SimpleParser::CParser<CCore> * m_parser;
/**
 * AntennaBoss reference
 */
Antenna::AntennaBoss_var m_antennaBoss;
/**
 * true if an error was detected when communicating to the antenna boss
 */
bool m_antennaBossError;
/**
 * Receivers boss reference
*/
Receivers::ReceiversBoss_var m_receiversBoss;
/**
 * true if an error was detected when communicating to the receivers boss
 */
bool m_receiversBossError;
/**
 * Name of the instance of the backend currently loaded as default backend
 */
IRA::CString m_defaultBackendInstance;
/**
 * Reference to the default backend....used to compute tsys in case a schedule is not running 
 */
Backends::GenericBackend_var m_defaultBackend;
/**
 * true if an error was detected when communicating to the default backend
 */
bool m_defaultBackendError;
/**
 * pointer to the antenna notification channel
*/
nc::SimpleConsumer<Antenna::AntennaDataBlock> *m_antennaNC;
/*
 * Tracking status of the telescope it resumes all the subsystem tracking flags
 */
bool m_isTracking;
/**
 * Stores the ID of the section of the current backend selected as current active device.
 */
long m_currentDevice;
/**
 * used to get a reference to the antenna boss component.
 * @param ref the pointer to the antenna boss component
 * @param errorDetected this flag is true if the reference seems not valid any more and the reference should not be considered valid any more
 */
void loadAntennaBoss(Antenna::AntennaBoss_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl);
/**
 * used to free the reference to the antenna boss component
 * @param ref reference to the antenna boss component
 */
void unloadAntennaBoss(Antenna::AntennaBoss_var& ref) const;
/**
 * used to get a reference to the receivers boss component.
 * @param ref reference to the receivers boss component
 * @param errorDetected this flag is true if the reference seems not valid any more and the reference should not be considered valid any more 
*/
void loadReceiversBoss(Receivers::ReceiversBoss_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl);
/**
 * used to free the reference to the receivers boss component
 */
void unloadReceiversBoss(Receivers::ReceiversBoss_var& ref) const;
/**
 * used to get a reference to the default backend component. 
 */
void loadDefaultBackend() throw (ComponentErrors::CouldntGetComponentExImpl);
/**
 * used to free the reference to the default backend component
 */
void unloadDefaultBackend();

/**
 * This method is a wrapper of the inject method of the internal parser. It is used to inject a procedure into the parser for its execution
*/
void injectProcedure(const IRA::CString& name,const ACS::stringSeq& proc,_SP_CALLBACK(callBack),const void *param) throw (ManagementErrors::ProcedureErrorExImpl);

/**
 * Allows to schedule an event at a precise time using the internal timer
 * @param time time to be scheduled
 * @param parameter pointer to the parameter that will be passed to the event handler
 * @param handler pointer to the handler function
 * @return false if the event could not be scheduled
 */
bool addTimerEvent(const ACS::Time& time,IRA::CScheduleTimer::TCallBack handler,void *parameter);

/**
 * Allows to delete the timer event associated to a specific time
 * @param scheduledTime time associated to the event that has to be canceled, the argument is zeroed.
 */
void cancelTimerEvent(ACS::Time& scheduledTime);

/**
 * It will cancel all the configured events
 */
void cancelAllTimerEvents();


#endif /*CORE_RESOURCE_H_*/
