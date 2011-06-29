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

#include <acsContainerServices.h>
#include <ManagementErrors.h>
#include <AntennaBossC.h>
#include <FitsWriterC.h>
#include <ReceiversBossC.h>
#include <GenericBackendC.h>
#include <ScheduleTimer.h>
#include <Site.h>
#include <DateTime.h>
#include <acsncSimpleConsumer.h>
#include <SP_parser.h>
#include "Configuration.h"
#include "Schedule.h"

using namespace maci;

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
	CCore(ContainerServices *service,CConfiguration *conf);

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
	 * @param lineNumber line inside the schedule to start from
	 * @throw ComponentErrors::MemoryAllocationExImpl
	 * @throw ManagementErrors::ScheduleErrorExImpl
	 * @throw ComponentErrors::AlreadyRunningExImpl
	 * @throw ManagementErrors::SubscanErrorExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	*/
	void startSchedule(const char* scheduleFile,const long& lineNumber) throw (ManagementErrors::ScheduleErrorExImpl,
			ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ManagementErrors::SubscanErrorExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
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
	 * This function reports about the scan number currently executed. This is not thread safe because
	 * it is almost atomic.
	 * @return the line (that corresponds to the scan ID) inside the main schedule file
	 */
	const DWORD& getScanNumber();
	
	/**
	 * This function reports the name of the file of the running schedule. This is not thread safe because
	 * it is almost atomic.
	 */ 
	const IRA::CString& getScheduleName();
	
	/**
	 * This function reports the current status of the component. This is not thread safe but is 
	 * almost atomic.
	 */
	const Management::TSystemStatus& getStatus() const { return m_schedulerStatus; }
	
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
			ManagementErrors::TargetIsNotVisibleExImpl,ManagementErrors::TsysErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,ManagementErrors::AntennaScanErrorExImpl);
	
	/**
	 * It parses a human readable command and executes it
	 * @throw ManagementErrors::CommandLineErrorExImpl 
	 * @param cmd command to be executed
	 * @return the string with command answer
	 */ 
	IRA::CString command(const IRA::CString& cmd) throw (ManagementErrors::CommandLineErrorExImpl);
	
	/**
	 * It changes the current device, first it checks if the corresponding section is existent in the current backend (default backend if no schedule is running, the schedule backend if a schedule runs).
	 * The the section configuration is read and eventually a new beamsize is computed also using the current configuration of the receiver.
	 * @param deviceID identifier of the section (device) of the current backend. If negative, the current value is not changed.
	 */ 
	void setDevice(const long& deviceID) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,
			ComponentErrors::OperationErrorExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
	
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
	inline bool isTracking() const { return m_isTracking; }
	
	/**
	 * This is not thread safe but we can consider it almost atomic.
	 * It clear the tracking flag, issued when a new scan is commanded in order to prevent the scheduler to consider the tracking when it is not the case
	 */
	inline void clearTracking() { m_isTracking=false; }
	
	/**
	 * This is not thread sape but it is almost atomic.
	 * @return the current active device
	 */
	inline long getCurrentDevice() const { return m_currentDevice; }
	
	/**
	 * This function loads a new procedures file from the CDB. The parser is directly informed on the new procedures.
	 * @param proceduresFile name of the table that stores the procedures to be loaded
	 * @throw ManagementErrors::ProcedureFileLoadingErrorExImpl
	*/
	void loadProcedures(const IRA::CString& proceduresFile) throw (ManagementErrors::ProcedureFileLoadingErrorExImpl);

private:

	/**
	 * pointer to the configuration object
	 */
	CConfiguration* m_config;
	/**
	 * pointer to the container serivices
	 */
	ContainerServices * m_services;
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
	
	
	#include "Core_Common.h"
		
	#include "Core_Resource.h"
	
	#include "Core_Extra.h" 
		 	
};


#endif /*CORE_H_*/
