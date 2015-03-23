#ifndef CORE_H_
#define CORE_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: Core.h,v 1.24 2011-06-05 14:45:31 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  22/12/2008      Creation                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  18/11/2009      Added the implementation of procedrure "command"                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  20/09/2010      Added command waitOnSource */
/* Andrea Orlati(aorlati@ira.inaf.it)  29/05/2012      Added command changeLogFile */
/* Andrea Orlati(aorlati@ira.inaf.it)  08/04/2014     computeScanAxis() re-implemented, it now relies of AntennaBoss.*/
/* Andrea Orlati(aorlati@ira.inaf.it)  08/04/2014     added initRecording(), StartRecording(), StopRecording and TerminateRecording().*/
/* Andrea Orlati(aorlati@ira.inaf.it)  08/04/2014     implemented the skidip scan.*/


#include <acsContainerServices.h>
#include <ManagementErrors.h>
#include <AntennaBossC.h>
#include <FitsWriterC.h>
#include <ReceiversBossC.h>
#include <MinorServoBossC.h>
#include <GenericBackendC.h>
#include <CustomLoggerC.h>
#include <GenericWeatherStationC.h>
#include <ActiveSurfaceBossC.h>
#include <ScheduleTimer.h>
#include <Site.h>
#include <DateTime.h>
#include <acsncSimpleConsumer.h>
#include <SP_parser.h>
#include "Configuration.h"
#include "Schedule.h"

#define MINOR_SERVO_AVAILABLE m_config->getMinorServoBossComponent()!=""

//using namespace maci;
using namespace IRA;

class CScheduleExecutor;

/**
 * This class models the Scheduler datasets and functionalities. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Orlati Andrea</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CCore {
	friend class CScheduleExecutor;
public:
	/**
	 * Constructor. Default Constructor.
	 * @param service pointer to the continaer services.
	 * @param conf pointer to the class CConfiguration tat conatains all the configuration regarding the component
	*/
	CCore(maci::ContainerServices *service,CConfiguration *conf);

	/** 
	 * Destructor.
	*/ 
	virtual ~CCore();
	
	/**
	 * This function starts the core  so that it will available to accept operations and requests.
	 * @throw ComponentErrors::TimerErrorExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @thorw ManagementErrors::ProcedureFileLoadingErrorExImpl
	*/
	virtual void execute() throw (ComponentErrors::TimerErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::MemoryAllocationExImpl,ManagementErrors::ProcedureFileLoadingErrorExImpl);
	
	/**
	 * This function initializes the core, all preliminary operation are performed here.
	*/
	virtual void initialize();
	
	/** 
	 * This function performs all the clean up operation required to free al the resources allocated by the class
	*/
	virtual void cleanUp();

	/**
	 * This method loads a schedule file and starts the execution of the schedule form the given line.
	 * This function is thread safe.
	 * @param scheduleFile string that reports the file name of the schedule to load
	 * @param startSubScan subscan of the schedule to start from
	 * @throw ComponentErrors::MemoryAllocationExImpl
	 * @throw ManagementErrors::ScheduleErrorExImpl
	 * @throw ComponentErrors::AlreadyRunningExImpl
	 * @throw ManagementErrors::SubscanErrorExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ManagementErrors::LogFileErrorExImpl
	*/
	void startSchedule(const char* scheduleFile,const char * startSubScan) throw (ManagementErrors::ScheduleErrorExImpl,ManagementErrors::AlreadyRunningExImpl,
ComponentErrors::MemoryAllocationExImpl,ManagementErrors::SubscanErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl);
	
	 /**
	  * It clears all the alarms from the scheduler
	  */
	 void resetSchedulerStatus();
	
	/**
	 * This method stops the current schedule, if one is runnig. If no schedule is active at tehe moment it takes
	 * no action.
	 * This function is thread safe.
	 */
	void stopSchedule();
	
	/**
	 * This method halts the current schedule, if one is runnig. If no schedule is active at the moment it takes
	 * no action. The active schedule is halted after the currently running scan is completed.
	 * This function is thread safe.
	 */
	void haltSchedule();	
	
	/**
	 * This function reports about the sub scan counter currently executed. This is not thread safe because
	 * @param cc the counter  of the currently executed subscan inside the main schedule file
	 */
	void getScanCounter(DWORD& cc);
	
	/**
	 * This function reports the identifiers of the current scan and subscan
	 * @param scanID currently running scan
	 * @param subScanID currently running subscan
	 */
	void getCurrentIdentifiers(DWORD& scanID,DWORD& subScanID);

	/**
	 * This function reports the name of the file of the running schedule.
	 * @param name name of the schedule
	 */ 
	void getScheduleName(IRA::CString& name);

	/**
	 * This function reports the code of the running project
	 * @param code project code
	 */
	void getProjectCode(IRA::CString& code);
	
	/**
	 * This function reports the current status of the component. This is not thread safe but is 
	 * almost atomic.
	 */
	const Management::TSystemStatus& getStatus() const { return m_schedulerStatus; }
	
	/**
	 * Allows to change the name of the current log file.
	 * @param fileName new file name.
	 * @param ComponentErrors::CouldntGetComponentExImpl
	 * @param ComponentErrors::CORBAProblemExImpl
	 * @param ManagementErrors::LogFileErrorExImpl
	 */
	void changeLogFile(const char *fileName) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl);

	/**
	 * Allows to read out from weather station.
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ManagementErrors::WeatherStationErrorExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 */
	void getWeatherStationParameters(double &temp,double& hum,double& pres, double& wind)  throw (ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::WeatherStationErrorExImpl,ComponentErrors::CORBAProblemExImpl);

	/**
	 * It computes the system temperature.
	 * @param ComponentErrors::CouldntGetComponentExImpl
	 * @param ComponentErrors::CORBAProblemExImpl
	 * @param ComponentErrors::OperationErrorExImpl 
	 * @param ComponentErrors::CouldntReleaseComponentExImpl
	 * @param  tsys list of the tsys given as output, one for each channel of the currently selected backend.
	 * */
	void callTSys(ACS::doubleSeq& tsys) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl,
			ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * This is a macro operation, it performs a cross scan over a previously commanded source.....
	 */
	void crossScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (ManagementErrors::NotAllowedDuringScheduleExImpl,
			ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
			ManagementErrors::TargetIsNotVisibleExImpl,ManagementErrors::TsysErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
			ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::TimerErrorExImpl);
	
	/**
	 * This is a macro operation, it performs a focus scan over the previously commanded source
	 */
	void focusScan(const double& span,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
			ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::TsysErrorExImpl,ComponentErrors::OperationErrorExImpl,
			ManagementErrors::UnsupportedOperationExImpl,ManagementErrors::MinorServoScanErrorExImpl,ComponentErrors::TimerErrorExImpl);

	/**
	 * this is a macro operation, it performs a skydip scan from the current azimuth position
	 */
	void skydip(const double& el1,const double& el2,const ACS::TimeInterval& duration)  throw (ManagementErrors::NotAllowedDuringScheduleExImpl,
			ComponentErrors::CouldntGetComponentExImpl,ManagementErrors::TsysErrorExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl, ComponentErrors::CORBAProblemExImpl,
			ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
			ComponentErrors::TimerErrorExImpl);

	/**
	 * It parses a human readable command and executes it
	 * @param cmd command to be executed
	 * @param answer string that contains the answer to the command
	 * @return the result of the command execution
	 */ 
	bool command(const IRA::CString& cmd,IRA::CString& answer);
	
	/**
	 * It changes the current device, first it checks if the corresponding section is existent in the current backend (default backend if no schedule is running, the schedule backend if a schedule runs).
	 * The the section configuration is read and eventually a new beamsize is computed also using the current configuration of the receiver.
	 * @param deviceID identifier of the section (device) of the current backend. If negative, the current value is not changed.
	 */ 
	void setDevice(const long& deviceID) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,
			ComponentErrors::OperationErrorExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * It allows to set a new project code. If requested by the component configuration (<i>CheckProjectCode</i>)  the project is checked to be registered in the system.
	 * If not present an error is thrown. The check consist in verifying a folder named "code" exists in <i>SchedDir</i> of the configuration.
	 * @param code new project code
	 * @throw ManagementErrors::UnkownProjectCodeErrorExImpl
	 */
	void setProjectCode(const char* code) throw (ManagementErrors::UnkownProjectCodeErrorExImpl);

	/**
	 * It allows to change the backend elected as default backend, the default backend is the device used for all operation (for example tsys) when a schedule is not running.
	 * @param bckInstance name of the instance of the backend that has to be placed as default backend 
	 */
	void chooseDefaultBackend(const char *bckInstance);
	
	/**
	 * It allows to change the component elected as default data receiver, 
	 * @param rcvInstance name of the instance of the data receiver component 
	 */
	void chooseDefaultDataRecorder(const char *rcvInstance);
	
	/**
	 * It allows to change the current status of the scheduler.
	 * @param status new status of the scheduler
	 */
	 void changeSchedulerStatus(const Management::TSystemStatus& status);
	 
	/**
	 * This is not thread safe but is almost atomic.
	 * @return true if the telescope is tracking or not
	 */
	bool isTracking() const;
	
	/**
	 * This is not thread safe but we can consider it almost atomic.
	 * It clear the tracking flag, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
	 */
	inline void clearAntennaTracking() { m_isAntennaTracking=false; }

	/**
	 * This is not thread safe but we can consider it almost atomic.
	 * It clear the tracking flag from minor servo, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
	 */
	inline void clarMinorServoTracking() { if (MINOR_SERVO_AVAILABLE) m_isMinorServoTracking=false; }
	
	/**
	 * This is not thread safe but we can consider it almost atomic.
	 * This method will cause the tracking flag to be false for a certain amount of time.
	 */
	void clearTracking();

	/**
	 * This is not thread safe but it is almost atomic.
	 * @return the current active device
	 */
	inline long getCurrentDevice() const { return m_currentDevice; }
	
	/**
	 * Not thread safe, but not a big issue
	 * @param out used to return to the caller the values of the rest frequencies
	 */
	void getRestFrequency(ACS::doubleSeq& out) const  { out=m_restFrequency; }

	/**
	 * called to set proper values for the rest frequency
	 * @param in new values
	 */
	void setRestFrequency(const ACS::doubleSeq& in);

	/**.
	 * @param dv returns back  the current active data receiver
	 */
	void  getCurrentDataReceiver(IRA::CString& dv);

	/*
	 * Used to know the currently used backend.
	 */
	void getCurrentBackend(IRA::CString& bck);

private:

	/**
	 * pointer to the configuration object
	 */
	 CConfiguration*  m_config;
	/**
	 * pointer to the container services
	 */
	maci::ContainerServices * m_services;
	/**
	 * Site information
	 */
	IRA::CSite m_site;
	/**
	 * UTC-UT1
	 */
	double m_dut1;
	/**
	 * Thread sync mutex
	 */
	BACIMutex m_mutex;
	
	/**
	 * Pointer to the thread executer thread
	 */
	CScheduleExecutor *m_schedExecuter;

	/**
	 * @var m_schedulerStatus it stores the current status of the scheduler  
	 */
	Management::TSystemStatus m_schedulerStatus;
	
	/**
	 * @var m_currentProceduresFile stored the name of the table inside CDB of procedures file currently in use
	 */
	IRA::CString m_currentProceduresFile;
	
	/**
	 * @var last weather station readout
	 */
	Weather::parameters m_weatherPar;

	/**
	 * @var last time the weather station was enquired
	 */
	ACS::Time m_lastWeatherTime;

	
	#include "Core_Common.h"
		
	#include "Core_Resource.h"
	
	#include "Core_Extra.h" 
		 	
};


#endif /*CORE_H_*/
