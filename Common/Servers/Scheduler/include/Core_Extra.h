#ifndef CORE_EXTRA_H_
#define CORE_EXTRA_H_

#define EXTRA_INIT m_scanID=0;

long m_scanID;

/**
 * Make the current thread wait for the given number of seconds. It can be used stop execution.
 * @param seconds number of seconds to wait, expressed as integer number plus fraction of seconds.
*/
void wait(const double& seconds) const;

/**
 * make the current thread wait until a given time
 * @param time time to wait for
 */
void waitUntil(const ACS::Time& time) throw (ComponentErrors::TimerErrorExImpl);

/**
 * timer event handler user to implement <i>waitUntil()</i> function
 */
static void waitUntilHandler(const ACS::Time& time,const void *par);

/**
 * The implementation of No Operation command. It does nothing
 */
void nop() const;

/**
 * Make the current thread wait for the tracking flag to be true. It can be used to stop the schedule execution until the telescope is tracking
 */
void waitOnSource() const;

/**
 * 
 */
int status() const;

/**
 * initialize the writing of the data
 * @param scaid identifier of the scan
 */
void initRecording(const long& scanid) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl);

/**
 * Start the recording at the given time
 * @param startTime epoch when the recording is started
 * @param subScanId identifier of the subscan
 */
void startRecording(const ACS::Time& startTime,const long& subScanId) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl);

/**
 * Starts the recording immediately. It is used to be called by RAL
 * @param subScanId identifier of the subscan
 */
void startRecording(const long& subScanId) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::ComponentNotActiveExImpl,
		ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl,
		ManagementErrors::DataTransferSetupErrorExImpl);

/**
 * Immediately stops the data recording
 */
void stopRecording() throw (ComponentErrors::OperationErrorExImpl,ManagementErrors::BackendNotAvailableExImpl);

/**
 * disable the recording
 */
void terminateRecording() throw (ComponentErrors::OperationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

#endif

