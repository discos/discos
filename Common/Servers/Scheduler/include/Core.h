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
	 * @throw ComponentErrors::UnexpectedExImpl
	*/
	virtual void execute() throw (ComponentErrors::TimerErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::MemoryAllocationExImpl,
	  ManagementErrors::ProcedureFileLoadingErrorExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * This function initializes the core, all preliminary operation are performed here.
	*/
	virtual void initialize();
	
	/** 
	 * This function performs all the clean up operation required to free al the resources allocated by the class
	*/
	virtual void cleanUp();
	
	 /**
	  * It clears all the alarms from the scheduler
	  */
	 void clearStatus();

	/**
	 * This is a macro operation, it performs a cross scan over a previously commanded source.....
	 */
	void crossScan(const Antenna::TCoordinateFrame& scanFrame,const double& span,const ACS::TimeInterval& duration) throw (ManagementErrors::NotAllowedDuringScheduleExImpl,
			ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
			ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,ManagementErrors::AntennaScanErrorExImpl,
			ComponentErrors::TimerErrorExImpl,ManagementErrors::TelescopeSubScanErrorExImpl,ManagementErrors::TargetOrSubscanNotFeasibleExImpl,
			ManagementErrors::AbortedByUserExImpl,ManagementErrors::RecordingAlreadyActiveExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl);
	
	/**
	 * This is a macro operation, it performs a focus scan over the previously commanded source
	 */
	void peakerScan(const char *axis,const double& span,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
			ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::TsysErrorExImpl,ComponentErrors::OperationErrorExImpl,
			ManagementErrors::UnsupportedOperationExImpl,ManagementErrors::MinorServoScanErrorExImpl,ComponentErrors::TimerErrorExImpl,
			ManagementErrors::AbortedByUserExImpl,ManagementErrors::NotAllowedDuringScheduleExImpl,ManagementErrors::RecordingAlreadyActiveExImpl,
			ComponentErrors::ValidationErrorExImpl,ManagementErrors::TelescopeSubScanErrorExImpl);

	/**
	 * this is a macro operation, it performs a skydip scan from the current azimuth position
	 */
	void skydip(const double& el1,const double& el2,const ACS::TimeInterval& duration) throw (
			ManagementErrors::NotAllowedDuringScheduleExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,
			ComponentErrors::OperationErrorExImpl,ComponentErrors::ComponentNotActiveExImpl,ComponentErrors::CORBAProblemExImpl,
			ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
			ManagementErrors::AntennaScanErrorExImpl,ComponentErrors::TimerErrorExImpl,ManagementErrors::TelescopeSubScanErrorExImpl,
			ManagementErrors::TargetOrSubscanNotFeasibleExImpl,ManagementErrors::AbortedByUserExImpl,
			ManagementErrors::RecordingAlreadyActiveExImpl,ManagementErrors::CloseTelescopeScanErrorExImpl);

	/**
	 * It parses a human readable command and executes it
	 * @param cmd command to be executed
	 * @param answer string that contains the answer to the command
	 * @return the result of the command execution
	 */ 
	bool command(const IRA::CString& cmd,IRA::CString& answer);

	#include "Core_Getter.h"

	#include "Core_Operations.h"

private:

	/**
	 * pointer to the configuration object
	 */
	 CConfiguration* m_config;
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

	#include "Core_Basic.h"
		 	
};


#endif /*CORE_H_*/
