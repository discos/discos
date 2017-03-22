#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

/* ************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                                             */
/* $Id: Configuration.h,v 1.6 2011-05-20 16:53:09 a.orlati Exp $                                                                                                                                     */
/*                                                                                                                                                */
/* This code is under GNU General Public Licence (GPL).                                                         */
/*                                                                                                                                                 */
/* Who                                                    When                 What                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)          22/12/2008         Creation                                             */

#include <IRA>
#include <maciContainerServices.h>
#include <ComponentErrors.h>
#include <ManagmentDefinitionsC.h>

using namespace IRA;

/**
 * This class implements the component configurator. The data inside this class are initialized at the startup from the
 * configuration database and then are used (read) inside the component.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
  */
class CConfiguration {
public:	

	typedef struct {
		Management::TScanAxis axis;
		IRA::CString servoName;
		Antenna::TsubScanGeometry antennaGeometry;
		double beamDevitionFactor;
	} TMinorServoAxis;

	typedef struct {
		IRA::CString alias;
		IRA::CString backend;
		bool noData;
		IRA::CString dataPath;
	} TAvailableBackends;

	/**
	 * Constructor 
	*/
	CConfiguration();
	
	/**
	 * Destructor
	*/
	~CConfiguration();
	/**
     * This member function is used to configure component by reading the configuration parameter from the CDB.
	 * This must be the first call before using any other function of this class.
	 * @throw ComponentErrors::CDBAccessExImpl
	 * @param Services pointer to the container services object
	*/
	void init(maci::ContainerServices *Services) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);
	
	/**
	 * This function is called in order to read a table in the CDB that stores a procedure file. 
	 * @param procedureFile file name, inside the CDB, that must be loaded
	 * @param names list of the procedures names (output)
	 * @param args for each procedure, indicates the number of arguments of the procedure (output)
	 * @param bodies this array stores the list of commands for each procedure. The Nth position contains the commands of the Nth procedure name. It must be freed by the caller.
	 * @param services pointer to the container services object
	 * @throw ComponentErrors::CDBAccessExImpl
	 * @throw ComponentErrors::IRALibraryResourceExImpl
	*/
	void readProcedures(maci::ContainerServices *services,const IRA::CString& procedureFile,ACS::stringSeq& names,ACS::longSeq& args,ACS::stringSeq *&bodies) throw (
			ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl);
	
	/**
	 * @return the name of the interface of the antenna boss component
	*/
	const IRA::CString& getAntennaBossComponent() const { return m_antennaBossComp; }
	/**
	 * @return the name of the interface of the observatory component
	 */
	const IRA::CString& getObservatoryComponent() const { return m_observatoryComp; }
	/**
	 * @return the name of the interface of the receivers boss component.
	*/
	const IRA::CString& getReceiversBossComponent() const { return m_receiversBossComp; }
	/**
	 * @return the name of the interface of the  boss component od minor servos.
	*/
	const IRA::CString& getMinorServoBossComponent() const { return m_minorServoBossComp; }
	/**
	 * @return the name of the interface of the  boss component of the active surface
	*/
	const IRA::CString& getActiveSurfaceBossComponent() const { return m_activeSurfaceBossComp; }
	/**
	 * @return the name of the interface of the custom logger component.
	*/
	const IRA::CString& getCustomLoggerComponent() const { return m_custoLoggerComp; }
	/**
	 * @return the name of the interface of the weather station component.
	*/
	const IRA::CString& getWeatherStationComponent() const { return m_weatherComp; }
	/**
	 * @return the name of the instance of the default backend component.
	*/	
	const IRA::CString& getDefaultBackendInstance() const { return m_defaultBackendInst; }
	/**
	 * @return the name of the instance of the default data receiver.
	 */
	const IRA::CString& getDefaultDataReceiverInstance() const { return m_defaultDataReceiverInst; }
	/**
	 * @return the number of microseconds that the log filter will cache a log message
	*/
	long getRepetitionCacheTime() const { return m_repetitionCacheTime; }
	/**
	 * @return the number of microseconds that the log filter will take as expiration time for a log message
	*/
	long getRepetitionExpireTime() const { return m_repetitionExpireTime; }
	/**
	 * @return the path to the directory that stores the schedule files.
	 */
	const IRA::CString& getScheduleDirectory() const { return m_schedDir; }
	/**
	 * @return the path to the directory where to store the data files.
	 */
	const IRA::CString& getDataDirectory() const;
	/**
	 * @return the path to the directory where to store the system data file or auxiliary data or files not coming from a schedule
	 */
	const IRA::CString& getSystemDataDirectory() const { return m_systemDataDir; }
	/**
	 * @return the path to the directory where to store the system stores the log files
	 */
	const IRA::CString& getLogDirectory() const { return m_logDir; }
	/**
	 * @return the path to the directory where to store the system stores the system log files
	 */
	//const IRA::CString& getSystemLogDirectory() const { return m_systemLogDir; }
	/**
	 * @return the sleep time between two execution of the schedule executor thread (microseconds)
	 */ 
	long getScheduleExecutorSleepTime() const { return m_scheduleExecutorSleepTime; }
	
	/**
	 * @return the time (microseconds) gap between two transition of the calibration diode.
	 */ 
	long getTsysGapTime() const { return m_TsysGapTime; }
	
	/**
	 * @return the number of meaningful decimal digits when performing ftrack computation, i represents the precision of the local oscillators
	 */
	long getFTrackDigits() const { return m_fTrackDigits; }

	/**
	 * @return the location, inside the CDB of the procedures files
	 */ 	
	const IRA::CString& getProceduresLocation() const { return m_proceduresLocation; }

	/**
	 * @return the default procedures file that will be loaded at the component startup
	 */ 		
	const IRA::CString& getDefaultProceduresFile() const { return m_defaultProceduresFile; }

	/**
	 * @return the default project code
	 */
	const IRA::CString& getDefaultProjectCode() const { return m_defaultProjectCode; }

	/**
	 * @return if the new project code has to be checked for existance or not
	 */
	const bool getCheckProjectCode() const { return m_checkProjectCode; }

	/**
	 * return the name of the servo mapped to the given axis
	 */
	IRA::CString getServoNameFromAxis(const Management::TScanAxis& axis) const;

	/**
	 * return axis based on the the name of the servo
	 */
	Management::TScanAxis getAxisFromServoName(const IRA::CString& servoName) const;

	double getBDFfromAxis(const Management::TScanAxis& axis) const;

	Antenna::TsubScanGeometry getScanGeometryFromAxis(const Management::TScanAxis& axis) const;

	/**
	 * check the availability of a backend from its name or an alias
	 * @param name name of the backend or an alias
	 * @param backend it return the name of the backend i.e its component instance
	 * @param pos current position - if available - in the internal backend list (index)
	 * @return true if the backend has been verified to be available in the configuration
	 */
	bool getAvailableBackend(const IRA::CString& name,IRA::CString& backend,long &pos) const;

	void setCurrentBackend(const long& pos)
	{
		m_currentBackendIndex=pos;
	}

	/**
	 * @return the string that describes the path (with trailing '/') where the schedule reports must be saved
	 */
	const IRA::CString& getScheduleReportPath() const { return m_scheduleReportPath; }

	/**
	 * @return the string that describes the path (with trailing '/') where the schedule are copied for backup
	 */
	const IRA::CString& getScheduleBackuptPath() const { return m_scheduleBackuptPath; }

	/**
	 * @return the string that is the full name of the recording lock file
	 */
	const IRA::CString& getRecordingLockFile() const { return m_recordingLockFile;}

private:
	IRA::CString m_schedDir;
	IRA::CString m_dataDir;
	IRA::CString m_systemDataDir;
	IRA::CString m_antennaBossComp;
	IRA::CString m_observatoryComp;
	IRA::CString m_receiversBossComp;
	IRA::CString m_minorServoBossComp;
	IRA::CString m_activeSurfaceBossComp;
	IRA::CString m_defaultBackendInst;
	IRA::CString m_defaultDataReceiverInst;
	IRA::CString m_logDir;
	//IRA::CString m_systemLogDir;
	IRA::CString m_custoLoggerComp;
	IRA::CString m_weatherComp;
	long m_repetitionCacheTime;
	long m_repetitionExpireTime;
	long m_scheduleExecutorSleepTime;
	long m_TsysGapTime;
	IRA::CString m_proceduresLocation;
	IRA::CString m_defaultProceduresFile;
	IRA::CString m_defaultProjectCode;
	bool m_checkProjectCode;
	long m_minorServoMappings;
	long m_fTrackDigits;
	long m_availableBackends;
	
	TMinorServoAxis* m_axis;
	TAvailableBackends* m_backend;

	long m_currentBackendIndex;

	/**
	 * Pointer to the DB table used to load the station procedures from the CDB 
	*/
	IRA::CDBTable * m_procTable;
	
	void extractBody(const IRA::CString& body,ACS::stringSeq& commands);

	Management::TScanAxis str2Axis(const IRA::CString& axis) const;

	IRA::CString m_scheduleReportPath;
	IRA::CString m_scheduleBackuptPath;
	IRA::CString m_recordingLockFile;

};



#endif /*CONFIGURATION_H_*/
