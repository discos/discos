#ifndef CORE_GETTER_H_
#define CORE_GETTER_H_

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
 * This function reports the current status of the component. This is not thread safe but is
 * almost atomic.
 */
const Management::TSystemStatus& getStatus() const { return m_schedulerStatus; }

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

/**.
 * @param dv returns back  the current active data receiver
 */
void  getCurrentDataReceiver(IRA::CString& dv);

/*
 * Used to know the currently used backend.
 */
void getCurrentBackend(IRA::CString& bck);

/**
 * This is not thread safe but is almost atomic.
 * @return true if the telescope is tracking or not
 */
bool isTracking() const;


#endif /* CORE_GETTER_H_ */
