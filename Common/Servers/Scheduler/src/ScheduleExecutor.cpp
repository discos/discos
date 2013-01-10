#include "ScheduleExecutor.h"
#include <LogFilter.h>
#include <libgen.h>

using namespace baci;

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
	if (m_stopMe) {
		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"SCHEDULE_STOPPED_BY_USER"));
		cleanSchedule(false); //it also sets m_stopMe to false again;
		return;
	}
	if (m_goAhead) {
		// check that the antenna reference if still good and in case try to take a valid reference
		try {
			m_core->loadAntennaBoss(m_antennaBoss,m_antennaBossError);
		}
		catch (ComponentErrors::CouldntGetComponentExImpl &ex) {
			ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_WARNING,"ANTENNA_BOSS_NOT_AVAILABLE"));
			// we take no action....the problem will be managed during scan execution....
		}
		switch (m_stage) {
			case INIT: { // run once at schedule start, it runs the init procedure, in case of errors..schedule execution is continued
				IRA::CString procName;
				ACS::stringSeq proc;
				if (!m_schedule->getInitProc(procName,proc)) { //get the procedure
					//if init proc fails...warn but go ahead
					_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::runLoop()");
					dummy.setReason((const char *)m_schedule->getLastError());
					dummy.log(LM_WARNING);
					m_core->changeSchedulerStatus(Management::MNG_WARNING); 
				}
				else if (procName!=_SCHED_NULLTARGET) {  
					m_goAhead=false;
					try {
						m_core->injectProcedure(procName,proc,&procedureCallBack,this);
						m_stage=STARTTIME_WAIT;
						break; // if the injection was correct we need to wait for the procedure termination...so exit from the cycle.
					}
					catch (ManagementErrors::ProcedureErrorExImpl& ex) {
						ex.log(LM_WARNING);
						m_core->changeSchedulerStatus(Management::MNG_WARNING); 
						// m_goAhead=true; the callback procedure should be called anyway, also in case of errors
					}
				}
				m_stage=STARTTIME_WAIT;
			}
			case STARTTIME_WAIT: {
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
			 	 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"SCHEDULE_IS_WAITING_LST: %s",(const char *)out));
			 	 	}
					m_stage=SCAN_SELECTION;
					break;
			 	}
			 	else {
			 		m_stage=SCAN_SELECTION;
			 	}				
			}
			case SCAN_SELECTION: { //select the next scan to be done.....in case of errors the schedule is stopped.			
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
				ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"NEXT_SUBSCAN_IS: %s",(const char *)m_schedule->getIdentifiers(m_scheduleCounter)));
				m_subScanDone=false;
				m_stage=SCAN_CHECK;
			}
			case SCAN_CHECK: {// checks the scan if it feasible to be done in time...if not the previous stage is executed again. In case of error the scan is aborted
				bool ok;
				try {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"CHECKING_THE_SCAN"));
					ok=CCore::checkScan(m_schedule->getSchedMode(),m_currentScan,m_currentScanRec,m_antennaBoss.in(),m_antennaBossError);
				}
				catch (ACSErr::ACSbaseExImpl& ex) {
					_ADD_BACKTRACE(ManagementErrors::SubscanErrorExImpl,impl,ex,"CScheduleExecutor::runLoop()");
					impl.setSubScanID(m_scheduleCounter);
					impl.setReason("cannot check scan against telescope");
					m_core->changeSchedulerStatus(Management::MNG_FAILURE);
					impl.log(LM_ERROR);
					cleanScan();
					break;
				}
				if (!ok) {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_WARNING,"SUBSCAN_SKIPPED: %s",(const char *)m_schedule->getIdentifiers(m_scheduleCounter)));
					// 1) if the whole schedule has been parsed...or it is the first iteration
					if (m_startSubScan==m_scheduleCounter) {
						// 1) this is not the first iteration (m_scansCounter==-1) and no scan performed during the repetition (m_scanCounter>0)
						if (m_scansCounter==0) { 
							ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"NO_SCANS_COULD_BE_DONE"));
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
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"REPETITION_COMPLETED"));
						m_scansCounter=0; // during the new repetition the number of done scans is reset
						m_scheduleRewound=false;
						m_repetition++;
						if (m_schedule->getSchedMode()==CSchedule::LST) { //if the schedule is sequential or timetagged is run continuosly
							if (m_schedule->getSchedReps()>0) { // if negative the schedule is repeated continuosly
								if (m_repetition>=(DWORD)m_schedule->getSchedReps()) { // END OF SCHEDULE
									ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"ALL_REPETITIONS_COMPLETED"));
									cleanSchedule(false);
									break;
								}
							}
						}
					}
					if (m_firstSubScan==0) { // if this is the first call: the first scan executed in the schedule
						m_firstSubScan=m_scheduleCounter;
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"FIRST_SUBSCAN_IS: %s",(const char *)m_schedule->getIdentifiers(m_scheduleCounter)));
					}
					m_stage=SCAN_PREPARATION;
				}
			}
			case SCAN_PREPARATION: {  // command the scan to the telescope...in case of error the current scan is aborted
				try {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"COMMAND_SCAN_TO_THE_TELESCOPE"));
					CCore::doScan(m_currentScan,m_currentScanRec,m_antennaBoss.in(),m_antennaBossError);
				}
				catch (ACSErr::ACSbaseExImpl& Ex) {
					_ADD_BACKTRACE(ManagementErrors::SubscanErrorExImpl,impl,Ex,"CScheduleExecutor::runLoop()");
					impl.setSubScanID(m_scheduleCounter);
					impl.setReason("cannot command scan to the telescope");
					m_core->changeSchedulerStatus(Management::MNG_FAILURE);
					impl.log(LM_ERROR);
					cleanScan();
					break;
				}
				m_core->clearTracking();
				m_stage=WRITING_INITIALIZATION;
			}
			case WRITING_INITIALIZATION: { //prepare the data transfer, it configures the backend and the writer. In case of error the scan is aborted.
				try {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"PREPARE_DATA_ACQUISITION"));
					prepareFileWriting(m_currentScan);
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
				ACS::stringSeq preProc;
				if (m_currentScan.preScan!=_SCHED_NULLTARGET) {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"PRESCAN_PROCEDURE_IS_NOT_NULL"));
					if (!m_schedule->getPreScanProcedureList()->getProcedure(m_currentScan.preScan,preProc)) {
						_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::runLoop()");
						dummy.setReason((const char *)m_schedule->getLastError());
						m_core->changeSchedulerStatus(Management::MNG_WARNING);
						dummy.log(LM_WARNING); 
					}
					else if (preProc.length()==0) { //if the procedure is empty...no need to execute it.
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"PRESCAN_PROCEDURE_IS_EMPTY"));
						m_stage=SCAN_START;
					}
					else {
						if (m_currentScan.preScanBlocking) {
							ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"BLOCKING_PRESCAN_PROCEDURE"));
							m_goAhead=false;
							m_stage=SCAN_START;
							try {
								m_core->injectProcedure(m_currentScan.preScan,preProc,&procedureCallBack,this);
								break; //procedure injection is ok.....wait for it to finish
							}
							catch (ManagementErrors::ProcedureErrorExImpl& ex) {
								ex.log(LM_WARNING);
								m_core->changeSchedulerStatus(Management::MNG_WARNING); 
								// m_goAhead=true; the callback procedure should be called anyway, also in case of errors
							}
						}
						else {  // scheduler is not asked to wait for the procedure termination...inject the procedure and go on.
							ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"NON-BLOCKING_PRESCAN_PROCEDURE"));
							try {
								m_core->injectProcedure(m_currentScan.preScan,preProc,&procedureCallBack,this);
							}
							catch (ManagementErrors::ProcedureErrorExImpl& ex) {
								ex.log(LM_WARNING);
								m_core->changeSchedulerStatus(Management::MNG_WARNING); 
							}
							m_stage=SCAN_START;
						}
					}
				}
				else {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"PRESCAN_PROCEDURE_IS_NULL"));
				}
				m_stage=SCAN_START;	 
			}
			case SCAN_START: { // It starts the data acquisition......In case of error the current scan is aborted				
				bool start=false;
				if (m_currentScan.ut==0) {
					if (m_core->isTracking()) {
						start=true;
					}
					else {
						break;
					}
				}
				else {
					start=true;
				}
				if (start) {
					ACS::stringSeq layoutProc;
					try {
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
						startRecording(m_currentScan,m_currentScanRec,layoutProc);
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
				m_stage=SCHEDULE_STOP;
			}
			case SCHEDULE_STOP: { // schedule the scan stop..in case of error. the schedule is aborted
				if (m_streamStarted) {
					m_lastScheduledTime=m_startRecordTime+(unsigned long long)(m_currentScan.duration*10000000); // this is the stop time in 100 ns.
					IRA::CString out;
					IRA::CIRATools::timeToStr(m_lastScheduledTime,out);
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"RECORDING_STOP_TIME_IS: %s",(const char *)out));
					if (!m_core->addTimerEvent(m_lastScheduledTime,&stopRecordingEventHandler,static_cast<void *>(this))) {
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
					if  (CCore::checkRecording(m_writer.in(),m_writerError)) {
						break;
					}
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_NOTICE,"SUBSCAN_DONE"));
				}
				m_stage=POST_SCAN_PROC;
			}
			case POST_SCAN_PROC: { //Executes the post scan procedure..in case of error nothing is done. We try to keep it up.
				ACS::stringSeq postProc;
				if (m_currentScan.postScan!=_SCHED_NULLTARGET) {
					ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"POSTSCAN_PROCEDURE_IS_NOT_NULL"));
					if (!m_schedule->getPostScanProcedureList()->getProcedure(m_currentScan.postScan,postProc)) {
						_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::runLoop()");
						dummy.setReason((const char *)m_schedule->getLastError());
						m_core->changeSchedulerStatus(Management::MNG_WARNING);
						dummy.log(LM_WARNING); 
					}
					else if (postProc.length()==0) { //if the procedure is empty...no need to execute it.
						ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"POSTSCAN_PROCEDURE_IS_EMPTY"));
						m_stage=SCAN_SELECTION;
					}
					else {
						if (m_currentScan.postScanBlocking) {
							m_goAhead=false;
							m_stage=SCAN_SELECTION;
							ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"BLOCKING_POSTSCAN_PROCEDURE"));
							try {
								m_core->injectProcedure(m_currentScan.postScan,postProc,&procedureCallBack,this);
								break; //procedure injection is ok.....wait for it to finish
							}
							catch (ManagementErrors::ProcedureErrorExImpl& ex) {
								ex.log(LM_WARNING);
								m_core->changeSchedulerStatus(Management::MNG_WARNING); 
								// m_goAhead=true; the callback procedure should be called anyway, also in case of errors
							}
						}
						else {  // scheduler is not asked to wait for the procedure termination...inject the procedure and go on.
							ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"NON-BLOCKING_PRESCAN_PROCEDURE"));
							try {
								m_core->injectProcedure(m_currentScan.postScan,postProc,&procedureCallBack,this);
							}
							catch (ManagementErrors::ProcedureErrorExImpl& ex) {
								ex.log(LM_WARNING);
								m_core->changeSchedulerStatus(Management::MNG_WARNING); 
							}							
						}
					}
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
	if (m_active) {
		m_schedule->getIdentifiers(m_scheduleCounter,scanID,subScanID);
	}
	else {
		scanID=subScanID=0;
	}
}

void CScheduleExecutor::initialize(maci::ContainerServices *services,const double& dut1,const IRA::CSite& site)
{ 
	 m_services=services;
	 m_currentWriterInstance="";
	 m_currentBackendInstance="";
	 m_currentBackendProcedure="";
	 Management::FitsWriter::_nil();
	 m_backend=Backends::GenericBackend::_nil();
	 m_antennaBoss=Antenna::AntennaBoss::_nil();
	 m_antennaBossError=false;
	 m_backendError=false;
	 m_writerError=false;
	 m_streamPrepared=m_streamStarted=m_streamConnected=false;
	 m_scanStarted=false;
	 m_scheduleRewound=false;
	 m_site=site;
	 m_dut1=dut1;
	 m_active=false;
	 m_scheduleLoaded=false;
	 m_scheduleName="";
	 m_projectCode=m_core->m_config->getDefaultProjectCode();
	 m_scheduleCounter=0;
	 m_lastScanID=0;
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
}
 
void CScheduleExecutor::startSchedule(const char* scheduleFile,const char * subScanidentifier) throw (
 		ManagementErrors::ScheduleErrorExImpl, ManagementErrors::AlreadyRunningExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::CouldntGetComponentExImpl,
 		ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl)
{
 	baci::ThreadSyncGuard guard(&m_mutex);
 	if (m_active) {
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
 	}
 	else {
 		fullPath=m_core->m_config->getScheduleDirectory()+projectCode+"/";
 	}
 	try {
 		m_schedule=new CSchedule(fullPath,schedule);
 	}
 	catch (std::bad_alloc& ex) {
 		_EXCPT(ComponentErrors::MemoryAllocationExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		m_core->changeSchedulerStatus(Management::MNG_FAILURE);
 		throw dummy;
 	}
 	if (!m_schedule->readAll(true)) {
 		_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		dummy.setReason((const char *)m_schedule->getLastError());
 		delete m_schedule;
 		m_schedule=NULL;
 		m_core->changeSchedulerStatus(Management::MNG_FAILURE);
 		throw dummy;
 	}
 	if (!m_schedule->isComplete()) {
 		_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::startSchedule()");
 		dummy.setReason((const char *)m_schedule->getLastError());
 		delete m_schedule;
 		m_schedule=NULL;		
 		m_core->changeSchedulerStatus(Management::MNG_FAILURE);
 		throw dummy;		
 	}
 	m_scheduleLoaded=true;
 	try {
 		m_core->loadAntennaBoss(m_antennaBoss,m_antennaBossError); 
 	}
 	catch (ComponentErrors::CouldntGetComponentExImpl& ex) {
 		delete m_schedule;
 		m_schedule=NULL;
 		m_scheduleLoaded=false;
 		m_core->changeSchedulerStatus(Management::MNG_FAILURE);
 		throw ex;
 	}
 	m_scheduleCounter=m_schedule->getSubScanCounter(subScanidentifier)-1; //need to point before the first scan in the schedule, the first scan has counter==1
 	m_core->changeLogFile((const char *)m_schedule->getBaseName()); //  (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ManagementErrors::LogFileErrorExImpl);
 	m_active=true;
	//save the scan number selected by user as start scan
 	m_startSubScan=m_scheduleCounter+1;
	m_scansCounter=-1;
 	m_scheduleName=schedule;
 	if (projectCode!=".") {
 		m_projectCode=projectCode;
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::startSchedule()",(LM_NOTICE,"PROJECT: %s",(const char *)m_projectCode));
 	}
 	// if the schedule is properly started the status of the scheduler is considered now to be ok.
 	m_core->resetSchedulerStatus();
 	m_stage=INIT;  // put the executor in the init stage.
 	m_goAhead=true; // make sure the executor is not suspended
 	m_stopMe=false;
 	m_haltMe=false;
 	m_scheduleRewound=false;
 	ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::startSchedule()",(LM_NOTICE,"SCHEDULE_STARTED_FROM_SCAN: %s %u",(const char *)schedule,m_startSubScan));
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
 
Backends::GenericBackend_ptr CScheduleExecutor::getBackendReference()
{
	baci::ThreadSyncGuard guard(&m_mutex);
	if (CORBA::is_nil(m_backend)) {
		return Backends::GenericBackend::_nil();
	}
	else {
		Backends::GenericBackend_var tmp;
		tmp=Backends::GenericBackend::_duplicate(m_backend);
		return tmp._retn();
	}
}

//************************PRIVATE*********************************** 

void CScheduleExecutor::stopRecording()
{
	baci::ThreadSyncGuard guard(&m_mutex); // not called directly by the thread runLoop, which is synchronized, but called by the timer handler, so an explicit sync is required.
	m_lastScheduledTime=0;
	if (!CORBA::is_nil(m_backend)) {
		try {
			if (m_streamStarted) {
				m_backend->sendStop();
				m_streamStarted=false;
				ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::stopRecording()",(LM_NOTICE,"RECORDING_STOPPED"));
			}
		}
		catch (...) {
			m_scanStopError=true;
			m_backendError=true;
		}
	}
	m_subScanDone=true;
	m_scansCounter++;
}

void CScheduleExecutor::cleanScan()
{
	m_core->cancelTimerEvent(m_lastScheduledTime);
	// clean up backend and writer
	try {
		CCore::disableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
		//disableFileWriting();
		closeBackend();
		closeWriter();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		m_streamStarted=m_streamPrepared=m_streamConnected=m_scanStarted=false;
		//ex.log(LM_WARNING);
	}
	m_currentWriterInstance="";
	m_currentBackendInstance="";
	m_currentBackendProcedure="";
	m_stage=SCAN_SELECTION;
	m_subScanDone=false;
	m_scanStopError=false;
	ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanScan()",(LM_NOTICE,"SCAN_ABORTED: %s",(const char *)m_schedule->getIdentifiers(m_scheduleCounter)));
	//now setup the timer to wake the thread up in order to try to start the next scan
	if (m_schedule->getSchedMode()==CSchedule::LST) {
		// in case of LST based schedule...we can proceed directly to next scan selection because the next scan will never start before its time stamp
	}
	else {
		// in case of SEQ or TIMETTAGGED...in case of error we want to wait before going to the next scan, in order to give time to the system to recover from the
		// error...so we wait from now to the duration of the present scan
		TIMEVALUE now;
		IRA::CIRATools::getTime(now);
		m_lastScheduledTime=now.value().value+(unsigned long long)(m_currentScan.duration*10000000); // this is the stop time in 100 ns.
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
		//disableFileWriting();
		CCore::disableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
		closeBackend();
		closeWriter();
	}
	catch (ACSErr::ACSbaseExImpl& ex) {
		m_streamStarted=m_streamPrepared=m_streamConnected=m_scanStarted=false;
		//ex.log(LM_WARNING);
	}
	// get rid of the schedule file......
	if (m_scheduleLoaded) {
		if (m_schedule!=NULL) { 
			delete m_schedule;
			m_schedule=NULL;
		}
		m_scheduleLoaded=false;
	}
	//release the AntennaBoss	
	m_core->unloadAntennaBoss(m_antennaBoss);
	m_currentWriterInstance="";
	m_currentBackendInstance="";
	m_currentBackendProcedure="";
	m_scheduleCounter=0;
	m_lastScanID=0;
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
	m_writerError=false;
	m_backendError=false;
	if (error) {
		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanSchedule()",(LM_NOTICE,"SCHEDULE_HALTED"));
	}
	else {
		if (m_active) {
			ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanSchedule()",(LM_NOTICE,"END_OF_SCHEDULE"));
		}
	}
	m_active=false;
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
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanSchedule()",(LM_DEBUG,"LST_SCAN_START: %s",(const char *)out));
 		IRA::CIRATools::timeToStr(rec.ut,out);
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::cleanSchedule()",(LM_DEBUG,"UT_SCAN_START: %s",(const char *)out));
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

void CScheduleExecutor::openWriter(const IRA::CString& wrt) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (wrt!=m_currentWriterInstance) {
		closeWriter(); //throw ComponentErrors::CouldntReleaseComponentExImpl and ComponentErrors::UnexpectedExImpl
		try {	
 			m_writer=m_services->getComponent<Management::DataReceiver>((const char *)wrt);
 			m_writer->reset();
 			m_currentWriterInstance=wrt;
 			m_writerError=false;
 		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CScheduleExecutor::openWriter()");
			Impl.setComponentName((const char*)wrt);
			throw Impl;
		}
	 	catch (...) {
	 		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CScheduleExecutor::openWriter()");
	 		throw impl;
	 	}		
	}
}

void CScheduleExecutor::closeWriter() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
 	try {
 		if (!CORBA::is_nil(m_writer)) {
 			m_services->releaseComponent((const char*)m_writer->name());
 			m_writer=Management::DataReceiver::_nil();	
 			m_currentWriterInstance="";
 			m_writerError=false;
 		}
 	}
 	catch (maciErrType::CannotReleaseComponentExImpl& ex) {
 		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CScheduleExecutor::closeWriter()");				
 		Impl.setComponentName((const char *)m_currentWriterInstance);
		m_writer=Management::DataReceiver::_nil();	
		m_currentWriterInstance="";
		m_writerError=false;
 		throw Impl;
 		
 	}
 	catch (...) {
		m_writer=Management::DataReceiver::_nil();	
		m_currentWriterInstance="";
		m_writerError=false;
 		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CScheduleExecutor::closeWriter()");
 		throw impl;
 	}
}
 
void CScheduleExecutor::openBackend(const IRA::CString& bck) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	if (bck!=m_currentBackendInstance) {
		closeBackend(); //throw ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl
		try {	
 			m_backend=m_services->getComponent<Backends::GenericBackend>((const char *)bck);
 			m_currentBackendInstance=bck;
 			m_backendError=false;
 		}
		catch (maciErrType::CannotGetComponentExImpl& ex) {
			_ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,Impl,ex,"CScheduleExecutor::openBackend()");
			Impl.setComponentName((const char*)bck);
			throw Impl;		
		}
	 	catch (...) {
	 		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CScheduleExecutor::openBackend()");
	 		throw impl;
	 	}			
	}
}

void CScheduleExecutor::closeBackend() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl)
{
	try {
		if (!CORBA::is_nil(m_backend)) {
			m_services->releaseComponent((const char*)m_backend->name());
			m_backend=Backends::GenericBackend::_nil();	
 			m_currentBackendInstance="";
 			m_backendError=false;
 		}
 	}
 	catch (maciErrType::CannotReleaseComponentExImpl& ex) {
 		_ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,Impl,ex,"CScheduleExecutor::closeBackend()");	
 		Impl.setComponentName((const char *)m_currentBackendInstance);
		m_backend=Backends::GenericBackend::_nil();	
		m_currentBackendInstance="";
		m_backendError=false;
 		throw Impl;
 	}
 	catch (...) {
 		_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CScheduleExecutor::closeBackend())");
		m_backend=Backends::GenericBackend::_nil();	
		m_currentBackendInstance="";
		m_backendError=false; 		
 		throw impl;
 	}
}

/*void CScheduleExecutor::configureBackend(const std::vector<IRA::CString>& procedure) throw (ManagementErrors::ProcedureErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl)
{
	char *answer;
	unsigned i;
	if (!CORBA::is_nil(m_backend)) {
		for (i=0;i<procedure.size();i++) {
			try {
				answer=m_backend->command((const char *)procedure[i]);
				CORBA::string_free(answer);
			}
			catch (ManagementErrors::CommandLineErrorEx& ex) {
				ManagementErrors::CommandLineErrorExImpl exImpl(ex);
				_ADD_BACKTRACE(ManagementErrors::ProcedureErrorExImpl,impl,ex,"CScheduleExecutor::configureBackend()");
				impl.setCommand((const char *)procedure[i]);
				impl.setLine(i);
				impl.setProcedure("Backend Configuration");				
				impl.setErrorMessage(exImpl.getErrorMessage());				
				throw impl;
			}
			catch (CORBA::SystemException& ex) {
				_EXCPT(ComponentErrors::CORBAProblemExImpl,impl,"CScheduleExecutor::configureBackend()");
				impl.setName(ex._name());
				impl.setMinor(ex.minor());
				m_backendError=true;
				throw impl;
			}
			catch (...) {
				m_backendError=true;
				_EXCPT(ComponentErrors::UnexpectedExImpl,impl,"CScheduleExecutor::configureBackend()");
				throw impl;
			}			
		}
	}
}*/

void CScheduleExecutor::prepareFileWriting(const CSchedule::TRecord& rec) throw (
		ManagementErrors::ScheduleErrorExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ManagementErrors::CommandLineErrorExImpl,ManagementErrors::ProcedureErrorExImpl)
{
	std::vector<IRA::CString> command;
 	IRA::CString bckInstance,outputFileName;
 	// check if we need to close up previous transfers
 	//error recovery.....give a try to fix things up, so we try to close in order to restart from the scratch
	/*if (m_backendError || m_writerError) {
 		try {
 			CCore::disableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
 			//disableFileWriting();
 		}
 		catch (...) {
 			m_streamStarted=m_streamPrepared=m_streamConnected=m_scanStarted=false;
 		}
 		if (m_backendError) {
 			try {
 				closeBackend();
 			}
 			catch (...) {
 			}
 		}
 		if (m_writerError) {
 			try {
 				closeWriter();
 			}
 			catch (...) {
 			}
 		}
 		m_currentBackendProcedure="";
 	}
	else*/
 	// we have 4 cases: 1) start of a scan 2) first scan 3) first iteration after an error 4) middle of a scan.
 	// cases 2 and 3 are the same as the scheduler is reset, so the procedure (if not NULL) is retrieved, the components loaded, the backend configured and the transfer enabled
 	// case 1 : the transfer is disabled, the procedure (if not NULL) retrieved, if it is different from the previous one , component are loaded(if necessary), bck configured and transfer enabled again
 	// case 4: we are in this case if the currentBackendProcedure is the same of the current subscan...so notghing to do
 	if (m_lastScanID!=0) {  //if this is the first scan...nothing to do
		// otherwise if current scanid is different from the previous one, or the current scan is consequence of a schedule rewind (to deal with the case just one scan is present in the schedule and it will be executed continuously)
		if ((m_lastScanID!=m_currentScan.scanid) || (rec.rewind)) {
			// stop  // we need to stop previous scan before starting a new one.
 			CCore::disableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
		}
	}
 	if (rec.backendProc!=_SCHED_NULLTARGET) { // if the writing has been disabled
 	 	if (rec.backendProc!=m_currentBackendProcedure) {
 	 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::prepareFileWriting()",(LM_DEBUG,"NEW_BACKEND_PROCEDURE"));
 	 		if (!m_schedule->getBackendList()->getBackend(rec.backendProc,bckInstance,command)) {
 	 			_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::prepareFileWriting()");
 	 			dummy.setReason((const char *)m_schedule->getLastError());
 	 			throw dummy;
 	 		}
 	 		openBackend(bckInstance); // if the case it also close the current one, otherwise it does nothing
 	 		openWriter(rec.writerInstance); // if the case it also close the current one, otherwise it does nothing
 	 		CCore::configureBackend(m_backend.in(),m_backendError,command);
 	 		m_currentBackendProcedure=rec.backendProc;
 	 	}
 	 	CCore::enableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_streamConnected);
 	}
 	else {
 		m_currentBackendProcedure=_SCHED_NULLTARGET;
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"BACKEND_PROCEDURE_IS_NULL"));
 		return;
 	}


 	/*if (rec.backendProc==_SCHED_NULLTARGET) { // if the writing has been disabled
 		//disableFileWriting();
 		CCore::disableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
 		m_currentBackendProcedure=rec.backendProc;
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"BACKEND_PROCEDURE_IS_NULL"));
 		return;
 	}
 	else if (rec.backendProc==m_currentBackendProcedure) { // same procedure as before
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"SAME_BACKEND_PROCEDURE"));
 		if (rec.writerInstance!=m_currentWriterInstance) {
 			ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"DIFFERENT_DATA_RECORDER"));
 			ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"DISABLING_DATA_RECORDING"));
 			CCore::disableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
 			closeWriter();
 			openWriter(rec.writerInstance);
 			ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"ENABLING_DATA_RECORDING"));
 			CCore::enableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamPrepared,m_streamConnected); 
 		}
 		else {  // same procedure and same writer....let's check if a stopScan is required
 			if (m_lastScanID!=0) {  //if this is the first scan...nothing to do
 				// otherwise if current scanid is different from the previous one, or the current scan is consequence of a schedule rewind (to deal with the case just one scan is present in the schedule and it will be executed continuously)
 				if ((m_lastScanID!=m_currentScan.scanid) || (rec.rewind)) {
 					// stop  // we need to stop previous scan before starting a new one.
 					CCore::stopScan(m_writer.in(),m_writerError,m_scanStarted); // ComponentErrors::OperationErrorExImpl
 				}
 			}
 		}
 	}
 	else { //different procedure 
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"DIFFERENT_BACKEND_PROCEDURE"));
 		if (!m_schedule->getBackendList()->getBackend(rec.backendProc,bckInstance,command)) {
 			_EXCPT(ManagementErrors::ScheduleErrorExImpl,dummy,"CScheduleExecutor::prepareFileWriting()");
 			dummy.setReason((const char *)m_schedule->getLastError());
 			throw dummy;
 		}
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"DISABLING_DATA_RECORDING"));
 		CCore::disableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamStarted,m_streamPrepared,m_streamConnected,m_scanStarted);
 		//disableFileWriting();
 		openBackend(bckInstance); // if the case it also close the current one, otherwise it does nothing
 		CCore::configureBackend(m_backend.in(),m_backendError,command);
 		if (rec.writerInstance!=m_currentWriterInstance) {
 			closeWriter();
 			openWriter(rec.writerInstance);
 		}
 		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::runLoop()",(LM_DEBUG,"ENABLING_DATA_RECORDING"));
 		CCore::enableDataTransfer(m_backend.in(),m_backendError,m_writer.in(),m_writerError,m_streamPrepared,m_streamConnected);
 		//enableFileWriting();
 		m_currentBackendProcedure=rec.backendProc;
 	}*/
}

void CScheduleExecutor::startRecording(const CSchedule::TRecord& rec,const Schedule::CScanList::TRecord& scanRec,const ACS::stringSeq& layoutProc) throw (ComponentErrors::OperationErrorExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl,
		ComponentErrors::ComponentNotActiveExImpl)
{
	IRA::CString outputFileName;
	ACS::TimeInterval startLst;
	Management::TScanAxis scanAxis;
	IRA::CString baseName,path,extraPath/*,layoutName*/,targetID;
	if (rec.ut==0) {
		TIMEVALUE now;
		TIMEDIFFERENCE lst;
		IRA::CDateTime currentUT;
		IRA::CIRATools::getTime(now);
		m_startRecordTime=now.value().value;
		currentUT.setDateTime(now,m_dut1);  // transform the current time in a CDateTime object
		currentUT.LST(m_site).getDateTime(lst);
		lst.day(0);
	 	startLst=lst.value().value;             // current lst as TimeInterval	
	}
	else {
		m_startRecordTime=rec.ut;
		startLst=rec.lst;
	}
	IRA::CString out;
	IRA::CIRATools::timeToStr(m_startRecordTime,out);
	ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::startRecording()",(LM_DEBUG,"RECORDING_START_TIME: %s",(const char *)out));

	/*if (m_projectCode!="") {*/
		path=m_core->m_config->getDataDirectory()+m_projectCode+"/";
	/*}
	else {
		path=m_core->m_config->getDataDirectory();
	}*/
	baseName=CCore::computeOutputFileName(m_startRecordTime,startLst,/*m_schedule->getProjectName()*/m_projectCode,rec.suffix,extraPath);

	Antenna::TTrackingParameters *prim=static_cast<Antenna::TTrackingParameters *>(scanRec.primaryParameters);
	targetID=(const char *)prim->targetName;
	// compute the axis or direction along which the scan is performed
	scanAxis=CCore::computeScanAxis(scanRec.type,scanRec);
	if ((rec.backendProc!=_SCHED_NULLTARGET) && (rec.duration>0.0))  { // if the writing has not been disabled  and data transfer is started only if the duration is bigger than zero......
		CCore::setupDataTransfer(m_scanStarted,m_streamPrepared,m_writer.in(),m_writerError,m_backend.in(),m_backendError,m_schedule->getObserverName(),
				m_schedule->getProjectName(),baseName,path,extraPath,m_schedule->getFileName(),targetID,rec.layout,layoutProc,m_schedule->getScanTag(),m_core->getCurrentDevice(),
				rec.scanid,m_startRecordTime ,rec.subscanid,scanAxis);
		// throws  ComponentErrors::OperationErrorExImpl,ComponentErrors::UnexpectedExImpl,ManagementErrors::BackendNotAvailableExImpl,ManagementErrors::DataTransferSetupErrorExImpl
		CCore::startDataTansfer(m_backend.in(),m_backendError,rec.ut,m_streamStarted,m_streamPrepared,m_streamConnected);
	}
	else {
		ACS_LOG(LM_FULL_INFO,"CScheduleExecutor::startRecording()",(LM_NOTICE,"NO_RECORDING_REQUIRED"));
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
	CScheduleExecutor *core;
	core=static_cast<CScheduleExecutor *>(const_cast<void *>(par));
	core->stopRecording();
}

void CScheduleExecutor::restartEventHandler(const ACS::Time& time,const void *par)
{
	CScheduleExecutor *core;
	core=static_cast<CScheduleExecutor *>(const_cast<void *>(par));
	core->m_goAhead=true;	
}


