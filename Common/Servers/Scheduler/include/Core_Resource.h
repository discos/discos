#ifndef CORE_RESOURCE_H_
#define CORE_RESOURCE_H_

// Declaration and management of the resources....

#define RESOURCE_INIT m_antennaBoss=Antenna::AntennaBoss::_nil(); \
									m_receiversBoss=Receivers::ReceiversBoss::_nil();	\
									m_minorServoBoss=MinorServo::MinorServoBoss::_nil(); \
									m_activeSurfaceBoss=ActiveSurface::ActiveSurfaceBoss::_nil(); 	\
									m_defaultBackend=Backends::GenericBackend::_nil(); \
									m_defaultDataReceiver=Management::DataReceiver::_nil(); \
									m_customLogger=Management::CustomLogger::_nil(); \
									m_weatherStation=Weather::GenericWeatherStation::_nil(); \
									m_antennaNC=NULL; \
									m_minorServoNC=NULL; \
									m_isAntennaTracking=m_isMinorServoTracking=m_isReceiversTracking=false; \
									m_clearTrackingTime=0; \
									m_antennaBossError=m_receiversBossError=m_minorServoBossError=m_defaultBackendError=m_defaultDataReceiverError=m_customLoggerError=\
									m_activeSurfaceBossError=m_weatherStationError=false; \
									m_currentDevice=0;\
									m_streamStarted=m_streamPrepared=m_streamConnected=m_dataTransferInitialized=false; \
									m_restFrequency.length(0); \
									m_scanID=0; m_subScanID=0; \
									m_abortCurrentOperation=false; \
									m_subScanEpoch=0; \
									m_subScanStarted=false; \
									m_antennaRTime=new Antenna::TRunTimeParameters; m_antennaRTime->targetName=""; \
									m_antennaRTime->axis=Management::MNG_NO_AXIS; \
									m_servoRunTime=new MinorServo::TRunTimeParameters; m_servoRunTime->scanAxis="";

									
#define RESOURCE_EXEC ACS_NEW_SIMPLE_CONSUMER(m_antennaNC,Antenna::AntennaDataBlock,Antenna::ANTENNA_DATA_CHANNEL,antennaNCHandler,static_cast<void*>(this)); \
					  m_antennaNC->consumerReady(); \
					  ACS_LOG(LM_FULL_INFO, "Core::execute()", (LM_INFO,"ANTENNA_NC_READY")); \
					  if (MINOR_SERVO_AVAILABLE) {	\
		      			  ACS_NEW_SIMPLE_CONSUMER(m_minorServoNC,MinorServo::MinorServoDataBlock,MinorServo::MINORSERVO_DATA_CHANNEL,minorServoNCHandler,static_cast<void*>(this)); \
		      			  m_minorServoNC->consumerReady(); \
		      			  ACS_LOG(LM_FULL_INFO, "Core::execute()", (LM_INFO,"MINOR_SERVO_NC_READY")); \
					  } \
					  else { m_isMinorServoTracking=true; } \
					  ACS_NEW_SIMPLE_CONSUMER(m_receiversNC,Receivers::ReceiversDataBlock,Receivers::RECEIVERS_DATA_CHANNEL,receiversNCHandler,static_cast<void*>(this)); \
					  m_receiversNC->consumerReady(); \
					  ACS_LOG(LM_FULL_INFO, "Core::execute()", (LM_INFO,"RECEIVERS_NC_READY")); \
					  m_defaultBackendInstance=m_config->getDefaultBackendInstance(); \
					  m_defaultDataReceiverInstance=m_config->getDefaultDataReceiverInstance(); \
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
						 if (m_minorServoNC!=NULL) m_minorServoNC->disconnect(); \
						 m_minorServoNC=NULL; \
						 if (m_receiversNC!=NULL) m_receiversNC->disconnect(); \
						 m_receiversNC=NULL; \
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
 * Minor Servo boss reference
 */
MinorServo::MinorServoBoss_var m_minorServoBoss;
/**
 * true if an error was detected when communicating to the minor servo boss
 */
bool m_minorServoBossError;
/**
 * Active Surface Boss reference
 */
ActiveSurface::ActiveSurfaceBoss_var m_activeSurfaceBoss;
/**
 * true if an error was detected when communicating to the active surface  boss
 */
bool m_activeSurfaceBossError;
/**
 * CustomLogger reference
 */
Management::CustomLogger_var m_customLogger;
/**
 * true if an error is detected during custom logger communication
 */
bool m_customLoggerError;
/**
 * Generic weather station reference
 */
Weather::GenericWeatherStation_var m_weatherStation;
/**
 * true if an error is detected during weather station communication
 */
bool m_weatherStationError;
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
 * name of the instance of the data receiver currently loaded
 */
IRA::CString m_defaultDataReceiverInstance;
/**
 * Reference to the default Data Receiver
 */
Management::DataReceiver_var m_defaultDataReceiver;
/**
 * true if an error was detected when communicating to the default data receiver
 */
bool m_defaultDataReceiverError;
/**
 * this members keep track of the current status/initialization the data transfer is.....
 */
bool m_streamStarted;
bool m_streamPrepared;
bool m_streamConnected;
bool m_dataTransferInitialized;
/**
 * these member keep track if a subscan has been issued to the telescope (antenna..minorServo....)
 */
//bool m_antennaSubScanStarted,m_msSubScanStarted,m_recvSubScanStarted;
/**
 * @var Last commanded scan identifier
 */
long m_scanID;
/**
 * @var Last commanded sub scan identifier
 */
long m_subScanID;

/**
 * This is starting epoch of the last subScan, it could be zero denoting the fact the scan was begun as soon as possible
 */
ACS::Time m_subScanEpoch;

/**
 * pointer to the antenna notification channel
*/
nc::SimpleConsumer<Antenna::AntennaDataBlock> *m_antennaNC;

/**
 * pointer to the receivers notification channel
*/
nc::SimpleConsumer<Receivers::ReceiversDataBlock> *m_receiversNC;

/**
 * pointer to the minor servo notification channel
*/
nc::SimpleConsumer<MinorServo::MinorServoDataBlock> *m_minorServoNC;

/*
 * Tracking status of the telescope
 */
bool m_isAntennaTracking;

/*
 * Tracking status of the minor Servo subsystem
 */
bool m_isMinorServoTracking;

/**
 * Tracking status of the receivers subsystem
 */
bool m_isReceiversTracking;

/*
 * Marks when the clear tracking has been issue, the tracking will be false for next 500msec
 */
ACS::Time m_clearTrackingTime;

/**
 * Stores the ID of the section of the current backend selected as current active device.
 */
long m_currentDevice;

/**
 * Stores the values of the rest frequencies
 */
ACS::doubleSeq m_restFrequency;
/**
 * structure used to pass back and forth runtime parameters from the receivers
 */
Receivers::TRunTimeParameters m_receiversRunTime;
/**
 * structure used to pass back and forth runtime parameters from the receivers
 */
Antenna::TRunTimeParameters_var m_antennaRTime;
/**
 * structure used to pass back and forth runtime parameters from the minor servo
 */
MinorServo::TRunTimeParameters_var m_servoRunTime;
/**
 * This flags force the component to abort all "long-lasting" operations
 */
bool m_abortCurrentOperation;
/**
 * if true a scan has been commanded to the telescope and the scan is still active (not closed)
 */
bool m_subScanStarted;
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
 * used to get a reference to the active surface  boss component.
 * @param ref the pointer to the active surface boss component
 * @param errorDetected this flag is true if the reference seems not valid any more and the reference should not be considered valid any more
 */
void loadActiveSurfaceBoss(ActiveSurface::ActiveSurfaceBoss_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl);

/**
 * used to free the reference to the active surface  boss component
 * @param ref reference to the  boss component
 */
void unloadActiveSurfaceBoss(ActiveSurface::ActiveSurfaceBoss_var& ref) const;

/**
 * used to get a reference to the minor servo  boss component.
 * @param ref the pointer to the minor servo boss component
 * @param errorDetected this flag is true if the reference seems not valid any more and the reference should not be considered valid any more
 */
void loadMinorServoBoss(MinorServo::MinorServoBoss_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl);

/**
 * used to free the reference to the minor servo  boss component
 * @param ref reference to the minor servo  boss component
 */
void unloadMinorServoBoss(MinorServo::MinorServoBoss_var& ref) const;

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
 * used to get a reference to the custom logger component.
 * @param ref reference to the custom logger component
 * @param errorDetected this flag is true if the reference seems not valid any more and the reference should not be considered valid any more
*/
void loadCustomLogger(Management::CustomLogger_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl);
/**
 * used to free the reference to the custom logger component
 */
void unloadCustomLogger(Management::CustomLogger_var& ref) const;
/**
 * used to get a reference to the custom weather station component.
 * @param ref reference to the custom logger component
 * @param errorDetected this flag is true if the reference seems not valid any more and the reference should not be considered valid any more
*/
void loadWeatherStation(Weather::GenericWeatherStation_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl);
/**
 * used to free the reference to the weather station component
 */
void unloadWeatherStation(Weather::GenericWeatherStation_var& ref) const;
/**
 * used to get a reference to the default backend component. 
 */
void loadDefaultBackend() throw (ComponentErrors::CouldntGetComponentExImpl);
/**
 * used to free the reference to the default backend component
 */
void unloadDefaultBackend();
/**
 * used to get a reference to the default data receiver component. 
 */
void loadDefaultDataReceiver() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl);
/**
 * used to free the reference to the default data receiver component
 */
void unloadDefaultDataReceiver();
/**
 * This method is a wrapper of the runAsync method of the internal parser. It is used to execute a procedure.
 * @param name name of the procedure to be executed
 * @param callBack function to executed as soon as the procedure completes
 * @param param parameter to be passed to the call back as argument
 * @throw ManagementErrors::ScheduleProcedureErrorExImpl
*/
void executeProcedure(const IRA::CString& name,_SP_CALLBACK(callBack),const void *param) throw (ManagementErrors::ScheduleProcedureErrorExImpl);

/**
 * This function loads new procedures from the CDB. The parser is directly informed on the new procedures.
 * @param proceduresFile name of the table that stores the procedures to be loaded
 * @throw ManagementErrors::ProcedureFileLoadingErrorExImpl
*/
void loadProcedures(const IRA::CString& proceduresFile) throw (ManagementErrors::ProcedureFileLoadingErrorExImpl);

/**
 * This function loads a new procedures from a given file. The parser is directly loaded with the new procedures. The file must exists on a disk reachable by the component.
 * @param path path of the file
 * @param file name of the procedure file
 * @throw ManagementErrors::ProcedureFileLoadingErrorExImpl
*/
void loadProceduresFile(const IRA::CString path,const IRA::CString file) throw (ManagementErrors::ProcedureFileLoadingErrorExImpl);

/**
 * This function loads a new procedures from a given file. The parser is directly loaded with the new procedures.
 * @param loader pointer to the class that is in charge of reading the file, the object must be already initialized.
*/
void loadProceduresFile(Schedule::CProcedureList *loader);

/**
 * Allows to schedule an event at a precise time using the internal timer
 * @param time time to be scheduled
 * @param parameter pointer to the parameter that will be passed to the event handler
 * @param handler pointer to the handler function
 * @param cleanup pointer to the cleanup function, if required.
 * @return false if the event could not be scheduled
 */
bool addTimerEvent(const ACS::Time& time,IRA::CScheduleTimer::TCallBack handler,void *parameter,CScheduleTimer::TCleanupFunction cleanup=NULL);

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
