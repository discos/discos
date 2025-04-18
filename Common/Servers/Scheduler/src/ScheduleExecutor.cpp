#include "ScheduleExecutor.h"
#include <LogFilter.h>
#include <libgen.h>

using namespace baci;

#define BCKEND_CONFIG (m_currentScan.backendProc!=_SCHED_NULLTARGET)
#define NORMAL_RECORDING BCKEND_CONFIG && (m_currentScan.duration>0.0)
#define NO_RECORDING m_currentScan.duration<=0.0
#define DRY_RUN (m_currentScan.backendProc==_SCHED_NULLTARGET) && (m_currentScan.duration>0.0)
 

CScheduleExecutor::CScheduleExecutor(const ACE_CString& name,CCore *param, 
			const ACS::TimeInterval& responseTime,const ACS::TimeInterval& sleepTime) : ACS::Thread(name,responseTime,sleepTime)
{
	AUTO_TRACE("CSchedulerExecutor::CSchedulerExecutor()");
	m_core=param;
}

CScheduleExecutor::~CScheduleExecutor()
{
	AUTO_TRACE("CScheduleExecutor::~CScheduleExecutor()");
	cleanSchedule(false);
}

void CScheduleExecutor::onStart()
{
	AUTO_TRACE("CScheduleExecutor::onStart()");
}

void CScheduleExecutor::onStop()
{
	 AUTO_TRACE("CScheduleExecutor::onStop()");
}

void CScheduleExecutor::runLoop()
{
	baci::ThreadSyncGuard guard(&m_mutex); // keep the mutex for thread execution
	Antenna::TTrackingParameters *primary,*secondary;
	Receivers::TReceiversParameters *recvPar;
	MinorServo::MinorServoScan *servoPar;
	if (m_stopMe) {
		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"SCHEDULE_STOPPED_BY_USER"));
		cleanSchedule(false); //it also sets m_stopMe to false again;
		return;
	}
	if (m_goAhead) {
		switch (m_stage) {
			case INIT: { // run once at schedule start, it runs the init procedure, in case of errors..schedule execution is continued
				//CannotClosePendingTask
				IRA::CString procName;
				if (!m_schedule->getInitProc(procName)) { //get the procedure
					//if init proc fails...warn but go ahead
					_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::runLoop()");
					dummy.setReason((const char *)m_schedule->getLastError());
					dummy.log(LM_WARNING);
					m_core->changeSchedulerStatus(Management::MNG_WARNING); 
				}
				else if (procName!=_SCHED_NULLTARGET) {  
					m_goAhead=false;
					try {
						m_core->executeProcedure(procName,&procedureCallBack,this);
						m_stage=STARTTIME_WAIT;
						break; // if the injection was correct we need to wait for the procedure termination...so exit from the cycle.
					}
					catch (ManagementErrors::ScheduleProcedureErrorExImpl& ex) {
						ex.log(LM_WARNING);
						m_core->changeSchedulerStatus(Management::MNG_WARNING); 
						// m_goAhead=true; the callback procedure should be called anyway, also in case of errors
					}
				}
				m_stage=STARTTIME_WAIT;
			}
			case STARTTIME_WAIT: {
				//printf("START TIME WAIT\n");
			 	if (m_schedule->getSchedMode()==CSchedule::TIMETAGGED) {
			 	 	TIMEVALUE currentUT;	
			 	 	IRA::CDateTime now;
			 		ACS::TimeInterval lstStartTime=m_schedule->getSchedStartTime();
			 	 	IRA::CIRATools::getTime(currentUT); // get the current time
			 	 	now.setDateTime(currentUT,m_dut1);  // transform the current time in a CDateTime object
			 	 	m_lastScheduledTime=CCore::getUTFromLST(now,now,lstStartTime,m_site,m_dut1); //get the corresponding ut time starting from the lst
			 	 	if (!m_core->addTimerEvent(m_lastScheduledTime,&restartEventHandler,static_cast<void *>(this))) {
						_EXCPT(ComponentErrors::TimerErrorExImpl,dummy,"CScheduleExecutor::runLoop()");
						dummy.setReason("The event for the start of the schedule could not be configured");
						m_core->changeSchedulerStatus(Management::MNG_FAILURE);
						dummy.log(LM_ERROR);
						cleanSchedule(true);
						break;
					}
			 	 	else {
				 	 	m_goAhead=false;
				 	 	IRA::CString out;
				 	 	IRA::CIRATools::intervalToStr(lstStartTime,out);
			 	 		CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",
			 	 		  (LM_NOTICE,"Waiting for LST: %s",(const char *)out));
			 	 	}
					m_stage=SCAN_SELECTION;
					break;
			 	}
			 	else {
			 		m_stage=SCAN_SELECTION;
			 	}				
			}
			case SCAN_SELECTION: { //select the next scan to be done.....in case of errors the schedule is stopped.	
				//printf("SCAN_SELECTION\n");
				if (m_haltMe) { //asked to close at the end of the scan...so close exactly at the beginning of the next one
					m_stopMe=true;
					break;
				}
				try {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"LOOK_FOR_NEXT_SCAN"));
					getNextScan(m_scheduleCounter,m_currentScan,m_currentScanRec);
				}
				catch (ManagementErrors::ScheduleErrorExImpl& ex) { //Fatal Error.......
					m_core->changeSchedulerStatus(Management::MNG_FAILURE);
					ex.log(LM_ERROR);
					cleanSchedule(true);
					break;
				}
				if (m_firstSubScan!=0) { // if this is not the first iteration,  we store when the schedule is started form the beginning of the file
					if (m_currentScan.rewind) m_scheduleRewound=m_currentScan.rewind;
				}
				m_scheduleCounter=m_currentScan.counter;
				//printf("scan selection: %u\n",m_scheduleCounter);
				CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",
				  (LM_NOTICE,"Next subscan: %s",(const char *)m_schedule->getIdentifiers(m_scheduleCounter)));
				m_subScanDone=false;
				m_stage=SCAN_CHECK;
			}
			case SCAN_CHECK: {// checks the scan if it feasible to be done in time...if not the previous stage is executed again. In case of error the scan is aborted
				double minEl,maxEl;
				bool ok;
				//printf("SCAN_CHECK\n");
				try {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"CHECKING_THE_SCAN"));
					m_schedule->getElevationLimits(minEl,maxEl);
					primary=static_cast<Antenna::TTrackingParameters *>(m_currentScanRec.primaryParameters);
					secondary=static_cast<Antenna::TTrackingParameters *>(m_currentScanRec.secondaryParameters);
					servoPar=static_cast<MinorServo::MinorServoScan *>(m_currentScanRec.servoParameters);
					recvPar=static_cast<Receivers::TReceiversParameters *>(m_currentScanRec.receieversParsmeters);
					ok=m_core->checkScan(m_currentScan.ut,primary,secondary,servoPar,recvPar,minEl,maxEl);
				}
				catch (ACSErr::ACSbaseExImpl& ex) {
					_ADD_BACKTRACE(ManagementErrors::SubscanErrorExImpl,impl,ex,"CScheduleExecutor::runLoop()");
					impl.setSubScanID(m_scheduleCounter);
					impl.setReason("scan cannot be validated");
					m_core->changeSchedulerStatus(Management::MNG_FAILURE);
					//impl.log(LM_ERROR);
					CUSTOM_EXCPT_LOG(impl,LM_ERROR);
					cleanScan();
					break;
				}
				if (!ok) {
					CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",
					  (LM_WARNING,"Skipping subscan: %s",(const char *)m_schedule->getIdentifiers(m_scheduleCounter)));
					// 1) if the whole schedule has been parsed...or it is the first iteration
					if (m_startSubScan==m_scheduleCounter) {
						// 1) this is not the first iteration (m_scansCounter==-1) and no scan performed during the repetition (m_scanCounter>0)
						if (m_scansCounter==0) { 
							CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",
							  (LM_NOTICE,"No scans done, current schedule will be aborted"));
							cleanSchedule(false);
							break;
						}
					}	
					if (m_scansCounter==-1) m_scansCounter=0; // this is not the first iteration any more
					m_stage=SCAN_SELECTION;  //if the scan is late or below the horizon....go immediately back to the previous stage
					break;
				} 
				else { 
					if (m_scansCounter==-1) m_scansCounter=0; // this is not the first iteration any more
					if ((m_firstSubScan<=m_scheduleCounter) && (m_scheduleRewound)) { //check if one repetition is complete.....
						CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",
						  (LM_NOTICE,"Repetition completed, restarting from the beginning"));
						m_scansCounter=0; // during the new repetition the number of done scans is reset
						m_scheduleRewound=false;
						m_repetition++;
						if (m_schedule->getSchedMode()==CSchedule::LST) { //if the schedule is sequential or timetagged is run continuosly
							if (m_schedule->getSchedReps()>0) { // if negative the schedule is repeated continuosly
								if (m_repetition>=(DWORD)m_schedule->getSchedReps()) { // END OF SCHEDULE
									CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",
									  (LM_NOTICE,"All repetitions are completed, schedule will be halted"));
									cleanSchedule(false);
									break;
								}
							}
						}
					}
					if (m_firstSubScan==0) { // if this is the first call: the first scan executed in the schedule
						m_firstSubScan=m_scheduleCounter;
						CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",
						  (LM_NOTICE,"Starting from subscan: %s",(const char *)m_schedule->getIdentifiers(m_scheduleCounter)));
					}
					m_stage=SCAN_PREPARATION;
				}
			}
			case SCAN_PREPARATION: {  // command the scan to the telescope...in case of error the current scan is aborted
				//printf("SCAN_PREPARATION\n");
				try {
					// clears all subsystem tracking flags
					m_core->clearTracking();
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"COMMAND_SCAN_TO_THE_TELESCOPE"));
					primary=static_cast<Antenna::TTrackingParameters *>(m_currentScanRec.primaryParameters);
					secondary=static_cast<Antenna::TTrackingParameters *>(m_currentScanRec.secondaryParameters);
					servoPar=static_cast<MinorServo::MinorServoScan *>(m_currentScanRec.servoParameters);
					recvPar=static_cast<Receivers::TReceiversParameters *>(m_currentScanRec.receieversParsmeters);
					Management::TSubScanConfiguration subConf=*(m_currentScanRec.subScanConfiguration);
					m_core->doScan(m_currentScan.ut,primary,secondary,servoPar,recvPar,subConf);
					m_closeScanTimer=0;
				}
				catch (ACSErr::ACSbaseExImpl& Ex) {
					_ADD_BACKTRACE(ManagementErrors::SubscanErrorExImpl,impl,Ex,"CScheduleExecutor::runLoop()");
					impl.setSubScanID(m_scheduleCounter);
					impl.setReason("cannot command scan to the telescope");
					m_core->changeSchedulerStatus(Management::MNG_FAILURE);
					CUSTOM_EXCPT_LOG(impl,LM_ERROR);
					//impl.log(LM_ERROR);
					cleanScan();
					break;
				}
				m_stage=WRITING_INITIALIZATION;
			}
			case WRITING_INITIALIZATION: { //prepare the data transfer, it configures the backend and the writer. In case of error the scan is aborted.
				//printf("WRITING_INIT\n");
				try {
					//if ((m_currentScan.backendProc!=_SCHED_NULLTARGET) && (m_currentScan.duration>0.0))  {
					//printf("PREPARING FILE WRITING\n");
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"PREPARE_DATA_ACQUISITION"));
					prepareFileWriting(/*m_currentScan*/);
					//}
				}
				catch (ACSErr::ACSbaseExImpl& Ex) {
					_ADD_BACKTRACE(ManagementErrors::SubscanErrorExImpl,impl,Ex,"CScheduleExecutor::runLoop()");
					impl.setSubScanID(m_scheduleCounter);
					impl.setReason("cannot configure data acquisition");
					m_core->changeSchedulerStatus(Management::MNG_FAILURE);
					impl.log(LM_ERROR);
					cleanScan();
					break;
				}
				m_stage=PRE_SCAN_PROC;
			}
			case PRE_SCAN_PROC: { // It executed the pre scan procedure. In case of error the scheduled is continued	
				//printf("PRE_SCAN_PROC\n");
				ACS::stringSeq preProc;
				//WORD preProcArgs;
				if (m_currentScan.preScan!=_SCHED_NULLTARGET) {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"PRESCAN_PROCEDURE_IS_NOT_NULL"));
					/*if (!m_schedule->getPreScanProcedureList()->getProcedure(m_currentScan.preScan,preProc,preProcArgs)) {
						_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::runLoop()");
						dummy.setReason((const char *)m_schedule->getLastError());
						m_core->changeSchedulerStatus(Management::MNG_WARNING);
						dummy.log(LM_WARNING); 
					}
					else if (preProc.length()==0) { //if the procedure is empty...no need to execute it.
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"PRESCAN_PROCEDURE_IS_EMPTY"));
						m_stage=SCAN_START;
					}
					else {*/
					if (m_currentScan.preScanBlocking) {
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"BLOCKING_PRESCAN_PROCEDURE"));
						m_goAhead=false;
						m_stage=SCAN_START;
						try {
							m_core->executeProcedure(m_currentScan.preScan,&procedureCallBack,this);
							break; //procedure injection is ok.....wait for it to finish
						}
						catch (ManagementErrors::ScheduleProcedureErrorExImpl& ex) {
							ex.log(LM_WARNING);
							m_core->changeSchedulerStatus(Management::MNG_WARNING);
							// m_goAhead=true; the callback procedure should be called anyway, also in case of errors
						}
					}
					else {  // scheduler is not asked to wait for the procedure termination...inject the procedure and go on.
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"NON-BLOCKING_PRESCAN_PROCEDURE"));
						try {
							m_core->executeProcedure(m_currentScan.preScan,&procedureCallBack,this);
						}
						catch (ManagementErrors::ScheduleProcedureErrorExImpl& ex) {
							ex.log(LM_WARNING);
							m_core->changeSchedulerStatus(Management::MNG_WARNING);
						}
						m_stage=SCAN_START;
					}
				}
				else {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"PRESCAN_PROCEDURE_IS_NULL"));
				}
				m_stage=SCAN_START;	 
			}
			case SCAN_START: { // It starts the data acquisition......In case of error the current scan is aborted				
				//printf("SCAN_START\n");
				bool start=false;
				if (m_currentScan.ut==0) {
					if (m_core->isTracking()) {
						start=true;
					}
					else {
						break;
					}
				}
				else { //m_currentScan.ut>0
					TIMEVALUE currentUTime;
					IRA::CIRATools::getTime(currentUTime); // get the current time
					// This check permits to skip (not recording) current subscan in case the scheduled start time is already elapsed.
					// This happens, for example, when the previous backend configuration is too slow and the scan could not start on time
					if (m_currentScan.ut<currentUTime.value().value) {  // if the forseen time for the start recording is already elapsed....we give up the current recording
						m_stage=STOP_SCHEDULING; // this will skip the start recording command, the next stage (STOPSCHEDULING) does nothing if the start was not done
						//ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_WARNING,"sub scan preparation is late"));
						CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_WARNING,"Subscan preparation is late, skipping subscan: %s",
						  (const char *)m_schedule->getIdentifiers(m_scheduleCounter)));
						CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"Waiting for next subscan...."));
						break;
					}
					start=true;
				}
				if (start) {
					ACS::stringSeq layoutProc;
					IRA::CString fullSubscanFileName;
					IRA::CString fullScanFolder;
					try {
						//printf("Scan duration: %lf\n",m_currentScan.duration);
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"STARTING_DATA_RECORDING"));
						if (m_schedule->getLayoutList()) {
							if (!m_schedule->getLayoutList()->getScanLayout(m_currentScan.layout,layoutProc)) { // get the scan layout definition
								_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::runLoop()"); // if we have an error we keep going, just log the error.....
								dummy.setReason((const char *)m_schedule->getLastError());
								m_core->changeSchedulerStatus(Management::MNG_WARNING);
								dummy.log(LM_WARNING);
								layoutProc.length(0); // important to to set the procedure  length to zero so that nothing is passed to data receiver
							}
						}
						else {
							ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"NO_LAYOUT_FILE_DEFINED"));
							layoutProc.length(0);
						}
						if (NORMAL_RECORDING)  { 
							  //printf("START RECORDING...normal\n");		
							  m_startRecordTime=m_core->startRecording(m_currentScan.ut,m_currentScan.scanid,m_currentScan.subscanid,m_schedule->getScanTag(),
							  m_config->getDataDirectory(), m_currentScan.suffix,m_schedule->getObserverName(), m_schedule->getProjectName(),							 
							  m_schedReport.getSchedule(),m_schedReport.getLogFileName(),m_currentScan.layout,layoutProc,fullSubscanFileName,fullScanFolder);	  
							  if (fullScanFolder!="") {
								  m_schedReport.addScanPath(fullScanFolder);
							  }
						}
						else if (NO_RECORDING) { //this is the case a 0.0 is set as integration time
							m_startRecordTime=0;	
							CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"No recording required"));
						}
						// DRY_RUN
						else { // this is the case a backend is not selected but a integration time greater than zero is given
							TIMEVALUE cUTime;
							IRA::CIRATools::getTime(cUTime); // get the current time
							m_startRecordTime=cUTime.value().value;								
							CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"Dry recording started"));						
							CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"Running for: %lf seconds",m_currentScan.duration));
						}
						//startRecording(m_currentScan,m_currentScanRec,layoutProc);
						m_scanStopError=false;
					}
					catch (ACSErr::ACSbaseExImpl& Ex) {
						_ADD_BACKTRACE(ManagementErrors::SubscanErrorExImpl,impl,Ex,"CScheduleExecutor::runLoop()");
						impl.setSubScanID(m_scheduleCounter);
						impl.setReason("cannot start data acquisition");
						m_core->changeSchedulerStatus(Management::MNG_FAILURE);
						impl.log(LM_ERROR);
						cleanScan();
						break;
					}					
				}
				m_stage=STOP_SCHEDULING;
			}
			case STOP_SCHEDULING: { // schedule the scan stop..in case of error. the schedule is aborted
				//printf("STOP_SCHEDULING\n");
				if (m_core->isStreamStarted()) {
					m_lastScheduledTime=m_startRecordTime+(unsigned long long)(m_currentScan.duration*10000000); // this is the stop time in 100 ns.
					IRA::CString out;
					IRA::CIRATools::timeToStr(m_lastScheduledTime,out);
					CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"Recording will stop at: %s",(const char *)out));
					if (!m_core->addTimerEvent(m_lastScheduledTime,&stopRecordingEventHandler,static_cast<void *>(this))) {
						_EXCPT(ComponentErrors::TimerErrorExImpl,dummy,"CScheduleExecutor::runLoop()");
						dummy.setReason("The scan stop event could not be scheduled");
						m_core->changeSchedulerStatus(Management::MNG_FAILURE);
						dummy.log(LM_ERROR);
						cleanSchedule(true);
						break;
					}
				}
				else if (m_startRecordTime>0) { //dry run.....
					m_lastScheduledTime=m_startRecordTime+(unsigned long long)(m_currentScan.duration*10000000); // this is the stop time in 100 ns.
					IRA::CString out;
					IRA::CIRATools::timeToStr(m_lastScheduledTime,out);
					if (!m_core->addTimerEvent(m_lastScheduledTime,&stopDryRecordingEventHandler,static_cast<void *>(this))) {
						_EXCPT(ComponentErrors::TimerErrorExImpl,dummy,"CScheduleExecutor::runLoop()");
						dummy.setReason("The scan stop event could not be scheduled");
						m_core->changeSchedulerStatus(Management::MNG_FAILURE);
						dummy.log(LM_ERROR);
						cleanSchedule(true);
						break;
					}
				} 
				else {
					m_subScanDone=true;
				}
				m_stage=SCAN_EXECUTION;
			}
			case SCAN_EXECUTION: { //it waits for the scan completion...in case of error the scan is forcibly aborted
				//printf("SCAN_EXECTION\n");
				if (!m_subScanDone) {
					break;
				}
				else {
					//stores the current scan id as it is the last scan id for the next subscan.......
					m_lastScanID=m_currentScan.scanid;
					if (m_scanStopError) {
						_EXCPT(ManagementErrors::SubscanErrorExImpl,impl,"CScheduleExecutor::runLoop()");
						impl.setSubScanID(m_scheduleCounter);
						impl.setReason("cannot stop data acquisition");
						m_core->changeSchedulerStatus(Management::MNG_FAILURE);
						impl.log(LM_ERROR);
						cleanScan();
						break;
					}
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"SUBSCAN_DONE"));
				}
				m_stage=POST_SCAN_PROC;
			}
			case POST_SCAN_PROC: { //Executes the post scan procedure..in case of error nothing is done. We try to keep it up.
				//printf("POST_SCAN_PROC\n");
				if (m_currentScan.postScan!=_SCHED_NULLTARGET) {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"POSTSCAN_PROCEDURE_IS_NOT_NULL"));
					if (m_currentScan.postScanBlocking) {
						m_goAhead=false;
						m_stage=SCAN_CLOSEUP;
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"BLOCKING_POSTSCAN_PROCEDURE"));
						try {
							m_core->executeProcedure(m_currentScan.postScan,&procedureCallBack,this);
							break; //procedure injection is ok.....wait for it to finish
						}
						catch (ManagementErrors::ScheduleProcedureErrorExImpl& ex) {
							ex.log(LM_WARNING);
							m_core->changeSchedulerStatus(Management::MNG_WARNING);
							// m_goAhead=true; the callback procedure should be called anyway, also in case of errors
						}
					}
					else {  // scheduler is not asked to wait for the procedure termination...inject the procedure and go on.
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"NON-BLOCKING_PRESCAN_PROCEDURE"));
						try {
							m_core->executeProcedure(m_currentScan.postScan,&procedureCallBack,this);
						}
						catch (ManagementErrors::ScheduleProcedureErrorExImpl& ex) {
							ex.log(LM_WARNING);
							m_core->changeSchedulerStatus(Management::MNG_WARNING);
						}
					}
				}
				m_stage=SCAN_CLOSEUP;
			}
			case SCAN_CLOSEUP : {
				//printf("SCAN_CLOSEUP\n");
				// trying to clean up and close the current scan commanded to the telescope
				try {
					m_closeScanTimer=m_core->closeScan(false);
					IRA::CString outstr;
					IRA::CIRATools::timeToStr(m_closeScanTimer,outstr);
					//printf("tempo di chiusura: %s\n",(const char*)outstr);
				}
				catch (ACSErr::ACSbaseExImpl& ex) {
					_ADD_BACKTRACE(ManagementErrors::CloseTelescopeScanErrorExImpl,impl,ex,"CScheduleExecutor::runLoop()");
					impl.log(LM_ERROR);
					m_core->changeSchedulerStatus(Management::MNG_FAILURE);
					cleanScan();
					break;
				}
				m_stage=CLOSEUP_WAIT;
				break;
			}
			case CLOSEUP_WAIT : {
				//printf("CLOSEUP_WAIT\n");
				if (m_closeScanTimer!=0) {
					TIMEVALUE now;
					IRA::CIRATools::getTime(now);
					if (m_closeScanTimer>now.value().value) {
						break;
					}
					m_closeScanTimer=0;
				}
				m_stage=RECORDING_FINALIZE;
			}
			case RECORDING_FINALIZE: {
				//printf("RECORDING_FINALIZE\n");
				// wait for the recorder to consume all the data in its cache
				try {
					if (NORMAL_RECORDING)  {
						if (m_core->checkRecording()) {
							break;
						}
					}
				}
				catch (ACSErr::ACSbaseExImpl& ex) {
					ex.log(LM_WARNING);
					m_core->changeSchedulerStatus(Management::MNG_WARNING);
				}
				m_stage=SCAN_SELECTION;
				break;
			}
			default : {
				// do nothing
			}
		}
	}
}

void CScheduleExecutor::getCurrentScanIdentifers(DWORD& scanID,DWORD& subScanID)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (isScheduleActive()) {
		m_schedule->getIdentifiers(m_scheduleCounter,scanID,subScanID);
	}
	else {
		scanID=subScanID=0;
	}
}

void CScheduleExecutor::initialize(maci::ContainerServices *services,const double& dut1,const IRA::CSite& site,CConfiguration* config)
{ 
	 m_services=services;
	 m_config=config;
	 m_currentBackendProcedure="";
	 Management::FitsWriter::_nil();
	 //m_scanStarted=false;
	 m_scheduleRewound=false;
	 m_site=site;
	 m_dut1=dut1;
	 //m_active=false;
	 m_scheduleLoaded=false;
	 m_scheduleName="";
	 m_projectCode=m_core->m_config->getDefaultProjectCode();
	 m_scheduleCounter=0;
	 m_lastScanID=0;
	 m_closeScanTimer=0;
	 m_goAhead=true;
	 m_stage=INIT;
	 m_firstSubScan=0;
	 m_startSubScan=0;
	 m_repetition=0;
	 m_lastScheduledTime=0;
	 m_scansCounter=-1;
	 m_subScanDone=false;
	 m_scanStopError=false;
	 m_stopMe=false;
	 m_haltMe=false;
	 m_schedReport=CScheduleReport(m_config->getLogDirectory(),m_config->getScheduleReportPath(),
			 m_config->getScheduleBackuptPath(),m_config->getRecordingLockFile());
}
 
void CScheduleExecutor::startSchedule(const char* scheduleFile,const char * subScanidentifier) throw (
 		ManagementErrors::ScheduleErrorExImpl, ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::CouldntGetComponentExImpl,
 		ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl,ManagementErrors::ScheduleNotExistExImpl,
 		ManagementErrors::CannotClosePendingTaskExImpl,ManagementErrors::ScheduleProjectNotMatchExImpl)
{
 	baci::ThreadSyncGuard guard(&m_mutex);
 	bool projectChanged;
 	if (isScheduleActive()) {
 		_EXCPT(ManagementErrors::AlreadyRunningExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		throw dummy;
 	}
 	char *passedSchedule=new char [strlen(scheduleFile)+1]; // make a copy because the man pages states that the input string may be changed in functions basename() and dirname()
 	strcpy(passedSchedule,scheduleFile);
 	char *tmp=dirname(passedSchedule);
 	IRA::CString projectCode=IRA::CString(tmp); 
 	strcpy(passedSchedule,scheduleFile);
 	tmp=basename(passedSchedule);
 	IRA::CString schedule=IRA::CString(tmp);
 	delete [] passedSchedule;
 	IRA::CString fullPath;
 	if (projectCode==".") { //no project given......use the current project code
 		fullPath=m_core->m_config->getScheduleDirectory()+m_projectCode+"/";
 		projectCode=m_projectCode;
 		projectChanged=false;
 	}
 	else {
 		fullPath=m_core->m_config->getScheduleDirectory()+projectCode+"/";
 		projectChanged=(projectCode==m_projectCode);
 	}
 	//this also means that the project directory exists, so for discos the project is legal 
 	if (!IRA::CIRATools::fileExists(fullPath+schedule)) { 
 		_EXCPT(ManagementErrors::ScheduleNotExistExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		throw dummy;
 	}
 
 	try {
 		m_schedule=new CSchedule(fullPath,schedule);
 	}
 	catch (std::bad_alloc& ex) {
 		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		m_core->changeSchedulerStatus(Management::MNG_FAILURE);
 		throw dummy;
 	}
 	m_schedule->setConfiguration(m_config);
 	if (!m_schedule->readAll(true)) {
 		_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		dummy.setReason((const char *)m_schedule->getLastError());
 		delete m_schedule;
 		m_schedule=NULL;
 		//m_core->changeSchedulerStatus(Management::MNG_FAILURE);
 		throw dummy;
 	}
 	if (!m_schedule->isComplete()) {
 		_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		dummy.setReason((const char *)m_schedule->getLastError());
 		delete m_schedule;
 		m_schedule=NULL;		
 		//m_core->changeSchedulerStatus(Management::MNG_FAILURE);
 		throw dummy;		
 	}
 	if (m_schedule->getProjectName()!=projectCode) {
 		_EXCPT(ManagementErrors::ScheduleProjectNotMatchExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		dummy.setProject((const char *)projectCode);
 		delete m_schedule;
 		m_schedule=NULL;
 		CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::startSchedule()",
 		  (LM_WARNING,"Please make sure the project of the schedule match with the currently active project"));
 		throw dummy;
 	}
	m_projectCode=projectCode;
 	if (projectChanged) {
 		CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::startSchedule()",(LM_NOTICE,"New active project: %s",(const char *)m_projectCode));
 	}
 	m_scheduleLoaded=true;
	ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::startSchedule()",(LM_NOTICE,"Stopping all pending tasks"));
 	try {
 		m_core->closeScan(true);
 	}
 	catch (ACSErr::ACSbaseExImpl& ex) {
 		_ADD_BACKTRACE(ManagementErrors::CannotClosePendingTaskExImpl,impl,ex,"CScheduleExecutor::startSchedule()");
 		m_core->changeSchedulerStatus(Management::MNG_FAILURE);
 		throw impl;
 	} 	 	
 	m_scheduleCounter=m_schedule->getSubScanCounter(subScanidentifier)-1; //need to point before the first scan in the schedule, the first scan has counter==1
 	m_schedReport.addScheduleName(m_schedule->getFileName());
 	if (m_schedule->getScanList()) {
 		m_schedReport.addAuxScheduleFile(m_schedule->getScanList()->getFileName());
 	}
 	if (m_schedule->getPreScanProcedureList()) {
 		// Pre or Post proc are the same....they mirror....the file is exactly the same
 		m_schedReport.addAuxScheduleFile(m_schedule->getPreScanProcedureList()->getFileName());
 	}
 	if (m_schedule->getBackendList()) {
 		m_schedReport.addAuxScheduleFile(m_schedule->getBackendList()->getFileName());
 	}
 	if (m_schedule->getLayoutList()) {
 		m_schedReport.addAuxScheduleFile(m_schedule->getLayoutList()->getFileName());
 	}

 	//m_core->_changeLogFile((const char *)logfile_name.str().c_str()); //  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl);
    m_core->_changeLogFile((const char *)m_schedReport.getLogFileName()); //  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl);
 	// load the procedures associated to the schedule
 	m_core->loadProceduresFile(m_schedule->getPreScanProcedureList());
 	//m_active=true;
	//save the scan number selected by user as start scan
 	m_startSubScan=m_scheduleCounter+1;
	m_scansCounter=-1;
 	m_scheduleName=schedule;
 
 	// if the schedule is properly started the status of the scheduler is considered now to be ok.
 	m_core->clearStatus();
 	m_stage=INIT;  // put the executor in the init stage.
 	m_goAhead=true; // make sure the executor is not suspended
 	m_stopMe=false;
 	m_haltMe=false;
 	m_scheduleRewound=false;

	if (!m_schedReport.activate()) {
		IRA::CString msg;
		msg.Format("Error in schedule reporting: %s",(const char *)m_schedReport.getLastError());
		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::startSchedule()",(LM_WARNING,"%s",(const char *)msg));
	}
 	CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::startSchedule()",
 	  (LM_NOTICE,"Schedule will start from subscan: %s %u",(const char *)schedule,m_startSubScan));
 	ACS::Thread::resume();  // resume the thread execution.... 	
}

void CScheduleExecutor::stopSchedule(bool force)
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (force) {
		m_stopMe=true;
	}
	else {
		m_haltMe=true;
	}
}
 
//************************PRIVATE*********************************** 

void CScheduleExecutor::stopRecording()
{
	baci::ThreadSyncGuard guard(&m_mutex); // not called directly by the thread runLoop, which is synchronized, but called by the timer handler, so an explicit sync is required.
	m_lastScheduledTime=0;
	try {
		m_core->stopDataTransfer();
	}
	catch (...) {
		m_scanStopError=true;
	}
	m_subScanDone=true;
	m_scansCounter++;
}

void CScheduleExecutor::stopDryRecording()
{
	baci::ThreadSyncGuard guard(&m_mutex); // not called directly by the thread runLoop, which is synchronized, but called by the timer handler, so an explicit sync is required.
	m_lastScheduledTime=0;
	m_subScanDone=true;
	m_scansCounter++;
}

void CScheduleExecutor::cleanScan()
{
	ACS::Time waitForStop=0;
	m_core->cancelTimerEvent(m_lastScheduledTime);
	// clean up backend and writer
	try {
		m_core->disableDataTransfer();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		//m_scanStarted=false;
		ex.log(LM_WARNING);
	}
	try {
		waitForStop=m_core->closeScan(false); // close the current scan in all subsystems
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_WARNING);
	}
	m_closeScanTimer=0;
	m_currentBackendProcedure="";
	m_stage=SCAN_SELECTION;
	m_subScanDone=false;
	m_scanStopError=false;
	CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanScan()",
	  (LM_NOTICE,"Aborted subscan: %s",(const char *)m_schedule->getIdentifiers(m_scheduleCounter)));

	//now setup the timer to wake the thread up in order to try to start the next scan
	if (m_schedule->getSchedMode()==CSchedule::LST) {
		// in case of LST based schedule...we can proceed directly to next scan selection because the next scan will never start before its time stamp
	}
	else {
		// in case of SEQ or TIMETTAGGED...in case of error we want to wait before going to the next scan, in order to give time to the system to recover from the
		// error...so we wait from now to the duration of the present scan
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		if (waitForStop>0) {
			ACS::Time temp=now.value().value+(unsigned long long)(m_currentScan.duration*10000000);
			if (waitForStop>temp) {
				m_lastScheduledTime=waitForStop;
			}
			else {
				m_lastScheduledTime=temp;
			}
		}
		else {
			m_lastScheduledTime=now.value().value+(unsigned long long)(m_currentScan.duration*10000000); // this is the stop time in 100 ns.
		}
		m_goAhead=false;
		if (!m_core->addTimerEvent(m_lastScheduledTime,&restartEventHandler,static_cast<void *>(this))) {
			cleanSchedule(true);
		}		
	}
}

void CScheduleExecutor::cleanSchedule(bool error)
{
	//make sure the timer has no more events
	m_core->cancelTimerEvent(m_lastScheduledTime); // => m_lastScheduledTime=0;
	// clean up backend and writer
	try {
		m_core->disableDataTransfer();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		//m_scanStarted=false;
		ex.log(LM_WARNING);
	}
	try {
		m_core->endSchedule();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_WARNING);
	}
	try {
		m_core->closeScan(false);
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_WARNING);
	}
	try {
		m_core->defaultlogFile();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		ex.log(LM_WARNING);
	}
	m_closeScanTimer=0;
	// get rid of the schedule file......
	if (m_scheduleLoaded) {
		if (m_schedule!=NULL) { 
			delete m_schedule;
			m_schedule=NULL;
			m_scheduleName="";
		}
		m_scheduleLoaded=false;
	}
	m_currentBackendProcedure="";
	m_scheduleCounter=0;
	m_lastScanID=0;
	m_closeScanTimer=0;
	m_stage=LIMBO;
	m_firstSubScan=0;
	m_startSubScan=0;
	m_repetition=0;
	m_scansCounter=-1;
	m_subScanDone=false;
	m_scanStopError=false;
	m_goAhead=false;
	m_stopMe=false;
	m_haltMe=false;
	if (error) {
		CUSTOM_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanSchedule()",(LM_NOTICE,"Schedule halted"));
	}
	else {
		if (isScheduleActive()) {
			ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanSchedule()",(LM_NOTICE,"End of the schedule"));
		}
	}
	if (!m_schedReport.deactivate()) {
		IRA::CString msg;
		msg.Format("Error in schedule reporting: %s",(const char *)m_schedReport.getLastError());
		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanSchedule()",(LM_WARNING,"%s",(const char *)msg));
	}
	//m_active=false;
	// suspend me
	ACS::Thread::suspend();
}

void CScheduleExecutor::getNextScan(const DWORD& counter,CSchedule::TRecord& rec,Schedule::CScanList::TRecord& scanRec) throw (ManagementErrors::ScheduleErrorExImpl)
{	
 	TIMEVALUE currentUT;
 	TIMEDIFFERENCE currentLST;	
 	IRA::CDateTime now;
 	
 	IRA::CIRATools::getTime(currentUT); // get the current time
 	now.setDateTime(currentUT,m_dut1);  // transform the current time in a CDateTime object
 	now.LST(m_site).getDateTime(currentLST);  // get the current LST time
 	currentLST.day(0);
 	rec.lst=currentLST.value().value;             // current lst as TimeInterval
 	rec.counter=counter;              			 //save the schedule line pointer
 	if (m_schedule->getSchedMode()==CSchedule::LST) {
 		if (!m_schedule->getSubScan_LST(rec)) {
 			_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::getNextScan()");
 			dummy.setReason((const char *)m_schedule->getLastError());
 			throw dummy;
 		}
 		rec.ut=CCore::getUTFromLST(now,now,rec.lst,m_site,m_dut1); //get the corresponding ut time starting from the lst
 		IRA::CString out;
 		IRA::CIRATools::intervalToStr(rec.lst,out);
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::getNextScan()",(LM_DEBUG,"LST_SCAN_START: %s",(const char *)out));
 		IRA::CIRATools::timeToStr(rec.ut,out);
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::getNextScan()",(LM_DEBUG,"UT_SCAN_START: %s",(const char *)out));
 	}
 	else if ((m_schedule->getSchedMode()==CSchedule::SEQ) || (m_schedule->getSchedMode()==CSchedule::TIMETAGGED)) {
 		if (!m_schedule->getSubScan_SEQ(rec)) {
 			_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::getNextScan()");
 			dummy.setReason((const char *)m_schedule->getLastError());
 			throw dummy;
 		}
 		rec.ut=0;
 	}
 	if (!m_schedule->getScanList()->getScan(rec.scan,scanRec)) {
 		_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::getNextScan()");
 		dummy.setReason((const char *)m_schedule->getLastError());
 		throw dummy;
 	}
}

void CScheduleExecutor::prepareFileWriting(/*const CSchedule::TRecord& rec*/) throw (
		ManagementErrors::ScheduleErrorExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ManagementErrors::BackendProcedureErrorExImpl,ManagementErrors::BackendNotAvailableExImpl)
{
	std::vector<IRA::CString> command;
 	IRA::CString bckInstance,outputFileName;
 	// we have 4 cases: 1) start of a scan 2) first scan 3) first iteration after an error 4) middle of a scan.
 	// cases 2 and 3 are the same as the scheduler is reset, so the procedure (if not NULL) is retrieved, the components loaded,
 	//         the backend configured and the transfer enabled
 	// case 1 : the transfer is disabled, the procedure (if not NULL) retrieved, if it is different from the previous one , component are loaded(if necessary), bck configured and transfer enabled again
 	// case 4: we are in this case if the currentBackendProcedure is the same of the current subscan...so nothing to do
 	if (m_lastScanID!=0) {  //if this is the first scan...nothing to do
		// otherwise if current scanid is different from the previous one, or the current scan is consequence of a schedule rewind 
		//(to deal with the case just one scan is present in the schedule and it will be executed continuously)
		if ((m_lastScanID!=m_currentScan.scanid) || (m_currentScan.rewind)) {
			// stop  // we need to stop previous scan before starting a new one.
 			m_core->disableDataTransfer();
		}
	}
 	if (BCKEND_CONFIG) { // if the writing has been disabled
 	 	if (m_currentScan.backendProc!=m_currentBackendProcedure) {
 	 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::prepareFileWriting()",(LM_DEBUG,"NEW_BACKEND_PROCEDURE"));
 	 		if (!m_schedule->getBackendList()->getBackend(m_currentScan.backendProc,bckInstance,command)) {
 	 			_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::prepareFileWriting()");
 	 			dummy.setReason((const char *)m_schedule->getLastError());
 	 			throw dummy;
 	 		}
 	 		m_core->_chooseDefaultBackend(bckInstance);  //CouldntGetComponentExImpl BackendNotAvailableExImpl
 	 		//throw ManagementErrors::BackendProcedureErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl
 	 		m_core->configureBackend(m_currentScan.backendProc,command);
 	 		m_currentBackendProcedure=m_currentScan.backendProc;
 	 	}
	 	m_core->_chooseDefaultDataRecorder(m_currentScan.writerInstance); //CouldntGetComponentExImpl ComponentErrors::UnexpectedExImpl
 	 	if (NORMAL_RECORDING) m_core->enableDataTransfer();
 	}
 	else {
 		m_currentBackendProcedure=_SCHED_NULLTARGET;
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::prepareFileWriting()",(LM_DEBUG,"BACKEND_PROCEDURE_IS_NULL"));
 		return;
 	}
}

//********************************************* STATIC METHODS  **************************************************************************

void CScheduleExecutor::procedureCallBack(const void *param,const IRA::CString& name,const bool& result)
{
	CScheduleExecutor *me=(CScheduleExecutor *)param;
	if (!result) me->m_core->changeSchedulerStatus(Management::MNG_WARNING); //logs are done by parser for runtime, by core class for errors during parsing. The status has to be changed here.
	me->m_goAhead=true;
}
void CScheduleExecutor::stopRecordingEventHandler(const ACS::Time& time,const void *par)
{
	CScheduleExecutor *me;
	me=static_cast<CScheduleExecutor *>(const_cast<void *>(par));
	me->stopRecording();
}

void CScheduleExecutor::stopDryRecordingEventHandler(const ACS::Time& time,const void *par)
{
	CScheduleExecutor *me;
	me=static_cast<CScheduleExecutor *>(const_cast<void *>(par));
	me->stopRecording();
}


void CScheduleExecutor::restartEventHandler(const ACS::Time& time,const void *par)
{
	CScheduleExecutor *core;
	core=static_cast<CScheduleExecutor *>(const_cast<void *>(par));
	core->m_goAhead=true;	
}


