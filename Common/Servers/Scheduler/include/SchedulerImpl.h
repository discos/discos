#ifndef SCHEDULERIMPL_H_
#define SCHEDULERIMPL_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                when                     What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 18/12/2008       Creation                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  29/05/2012      Added command changeLogFile */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <enumpropROImpl.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <baciROlong.h>
#include <SchedulerS.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <Definitions.h>
#include "Configuration.h"
#include "Core.h"
#include <IRA>
#include <baciROdoubleSeq.h>

/** 
 * @mainpage Scheduler component Implementation 
 * @date 08/04/2013
 * @version 1.13.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 8.2.0
 * @remarks compiler version is 4.1.2
*/

//using namespace baci;
using namespace IRA;
/**
 * This class is the implementation of the Scheduler component.  
 * @author <a href=mailto:a.orlati@ira.inaf.it>Orlati Andrea</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class SchedulerImpl: public baci::CharacteristicComponentImpl,
				       public virtual POA_Management::Scheduler
{

public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component 
	*                in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	SchedulerImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~SchedulerImpl(); 

	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters or 
	 * builds up connection to devices or other components. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);

	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming 
 	 * functional calls any time. 
	 * Must be implemented as a synchronous (blocking) call. In this class the default implementation only 
	 * logs the COMPSTATE_OPERATIONAL
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void execute();
	
	/** 
	 * Called by the container before destroying the server in a normal situation. This function takes charge of 
	 * releasing all resources.
	*/
	virtual void cleanUp();
	
	/** 
	 * Called by the container in case of error or emergency situation. This function tries to free all resources 
	 * even though there is no warranty that the function is completely executed before the component is destroyed.
	*/	
	virtual void aboutToAbort();
	
	/**
     * Returns a reference to the status property implementation of IDL interface.
	 * @return pointer to read-only ROTSystemStatus property status
	*/
	virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the scheduleName property implementation of IDL interface.
	 * @return pointer to read-only ROstring property status
	*/
	virtual ACS::ROstring_ptr scheduleName() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the scanID property implementation of IDL interface.
	 * @return pointer to read-only ROlong property status
	*/
	virtual ACS::ROlong_ptr scanID() throw (CORBA::SystemException);

	/**
     * Returns a reference to the subscanID property implementation of IDL interface.
	 * @return pointer to read-only ROlong property status
	*/
	virtual ACS::ROlong_ptr subScanID() throw (CORBA::SystemException);

	/**
     * Returns a reference to the tracking property implementation of IDL interface.
	 * @return pointer to read-only ROTBoolean property tracking
	*/
	virtual Management::ROTBoolean_ptr tracking() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the currentDevice property implementation of IDL interface.
	 * @return pointer to read-only ROlong property
	*/	
	virtual ACS::ROlong_ptr currentDevice() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the projectCode property Implementation of IDL interface.
	 * @return pointer to read-only string property projectCode
	*/
	virtual ACS::ROstring_ptr projectCode() throw (CORBA::SystemException);

	/**
     * Returns a reference to the currentBackend property Implementation of IDL interface.
	 * @return pointer to read-only string property currentBackend
	*/
	virtual ACS::ROstring_ptr currentBackend() throw (CORBA::SystemException);

	/**
     * Returns a reference to the currentRecorder property Implementation of IDL interface.
	 * @return pointer to read-only string property currentRecorder
	*/
	virtual ACS::ROstring_ptr currentRecorder() throw (CORBA::SystemException);

	/**
     * Returns a reference to the restFrequency property Implementation of IDL interface.
	 * @return pointer to read-only double sequence property restFrequency
	 */
	virtual ACS::ROdoubleSeq_ptr restFrequency() throw (CORBA::SystemException);

	/**
	 * This method implements the command line interpreter. The interpreter allows to ask for services or to issue commands
	 * to the control system by human readable command lines.
	 * @throw CORBA::SystemException
	 * @param cmd string that contains the command line
	 * @param answer contains the answer to the command
	 * @return the result of the command execution
	 */
	virtual CORBA::Boolean command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException);
	
	/**
	 * This method allow to change the name of the current log file.
	 * @param fileName new file name
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
	virtual void changeLogFile(const char *fileName) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

	/**
	 * This method reports back the weather parameter read out from the weather station.
	 * @param temperature value of the external temperature (°C)
	 * @param humidity relative humidity
	 * @param pressure value of the pressure (mBar)
	 * @param wind speed of wind (no direction) in Km/s
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
	virtual void weatherParamenters (CORBA::Double_out temperature,CORBA::Double_out humidity,CORBA::Double_out pressure,CORBA::Double_out wind);

	/**
	 * This method can load a new schedule file. After a succesfull parse of the schedule the itself is started
	 * from the specified line
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @param fileName name of the file of the schedule
	 * @param startSubScan scan/subscan to start from
	 */
	virtual void startSchedule(const char * fileName,const char *startSubScan) throw (CORBA::SystemException,
			ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);
	
	/**
	 * This method performs the system temperature measurement. In order to do that it uses the currently configured backend and frontend. The measure is performed
	 * for aech of the section of the backend. The sequence of operation is: 
	 * 	@arg \c a) call the backend in order to collect the informations for each section (start frequency, bandwidth, polarization and feed).
	 *     @arg \c b) call the receiverboss in order to know the value of the calibration diode for each of the sections
	 *     @arg \c c) call the backend ion order to get the TPI and TPIzero measurements
	 *     @arg \c d) call the receiverBoss in order to turn the calibration diode on
	 *     @arg \c  e) call the backend in order to get the TPIcal  measurement
	 *     @arg \c  f) call the receiver boss in order to turn the calibration diode off
	 *     @arg \c g) compute the system temperature
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @return the system temperature in Kelvin related with each of the backend sections.
	 */
	virtual ACS::doubleSeq *systemTemperature() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);
	
	/**
	 * This method implements the cross scan operation. The cross scan is done around a previously commanded target (prerequisite) using the main drives of the telescope and 
	 * the Antenna subsystem. The operation consists of a tsys measurement and the the lon/lat On-The-Fly scans.
 	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @param coordFrame allow to select the frame along which the single scan is done
	 * @param span length of the scan
	 * @param duration how long the single subscan has to take
	 */ 
	virtual void crossScan(Management::TCoordinateFrame coordFrame,CORBA::Double span,ACS::TimeInterval duration) throw (CORBA::SystemException,
			ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

	/**
	 * Performs a peaker scan moving the subreflector or the receiver along an axis. The operation consists of system temperature measurement (the telescope is sent
	 * 3 times the current beam size off) and then a focus scan on the source. The default backend and the default data recorder are used as data source and destination respectively.
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @throw CORBA::SystemExcpetion
	 * @param axis name of the axis to scan
	 * @param span this is the overall length of the single scans (mm or arcsec according to the selected axis)
	 * @param duration this determine how long the scan has to take.
	 */
	virtual void peakerScan(const char *axis,CORBA::Double span,ACS::TimeInterval duration) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);
	
	/**
	 * This method performs a skydip with the antenna. The scan is done across the current azimuth between the two elevation limits. A system temperature measurement is
	 * done at the higher elevation limit.
 	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @param elevation1 represent2 the first limit of the elevation range to be covered
	 * @param elevation2 represent2 the second limit of the elevation range to be covered
	 * @param duration how much time the skydip has to take
	 */
	virtual void skydip(CORBA::Double elevation1,CORBA::Double elevation2,ACS::TimeInterval duration)  throw (CORBA::SystemException,
			ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);

	/**
	 * It allows to change the name of the instance of the current default backend
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @param bckInstance name of the instance 
	*/ 
	virtual void chooseDefaultBackend(const char *bckInstance) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);
	
	/**
	 * It allows to change the name of the instance of the current data recorder component
	 * @throw CORBA::SystemException
	 * @thorw ComponentErrors::ComponentErrorsEx
	 * @thorw ManagementErrors::ManagementErrorsEx
	 * @param rvcInstance name of the instance 
	 */
	virtual void chooseDefaultDataRecorder(const char *rcvInstance) throw (CORBA::SystemException,
		ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);
	
	/**
	 * This method stops the current running schedule, if any. In order to start a new schedule the current one must be stopped.
	 * @throw CORBA::SystemException
	 **/
	virtual void stopSchedule() throw (CORBA::SystemException);
	
	/**
	 * This method halts the current running schedule, if any. The schedule is stopped after that the currently running scan is completed.
	 * In order to start a new schedule the current one must be stopped.
	 * @throw CORBA::SystemException
	 **/
	virtual void haltSchedule() throw (CORBA::SystemException);
	
	/**
	 * This method will reset the property <i>status</i> to MNG_OK.
	 * @throw CORBA::SystemException 
	 */ 
	virtual void clearStatus() throw (CORBA::SystemException);
	
	/**
	 * This method will set the current device. The current device is a section of the currently used backend. When a new device is set some configuration are done.
	 * first of all the configuration of the section is read, if everything works the new beamsize is computed.The current device is the backend section also used for calibration purposes.
	 * By default the device (if not explicitly set) is the number zero, but if this method is not called there is no guarantee that the backend configuration, the beamsize, the receiver configuration,
	 * and the current device are coherent. the last set value can be read in the <i>currentDevice</i> attribute.
	 * @param deviceID identifier of the section (of the current backend). The value is checked to be inside valid sections identifiers.  
	 */
	virtual void setDevice(CORBA::Long deviceID) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx);
	
	/**
	 * This method  allows to set the current running project code. The code will be used to locate schedules files and to store data files on system disks. If the project is not present/registered in the
	 * system an error is thrown.
	 * @param code new project code. If an empty string is given the default code is adopted, the default code is part of the component configuration.
	 * @throw CORBA::SystemException
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
	virtual void setProjectCode(const char *code) throw (CORBA::SystemException,ManagementErrors::ManagementErrorsEx);

	/**
	 * This method allow to set the current value for the rest frequency
	 * @param rest sequence of value (MHz) that are the rest frequencies to be applied for each section. If one is given the same value is used for the sections.
	 */
	virtual void setRestFrequency(const ACS::doubleSeq& rest) throw (CORBA::SystemException);

	/**
	 * It allows to centre the observe frequency line in the observed band.
	 */
    virtual void fTrack(const char* dev) throw (ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);


	/**
	 * It allows to immediately start a scan that involves a primary focus axis or a subreflector axis. The scan is performed of the currently
	 * commanded source. If the axis involves a pointing offsets the telescope will try to correct it by moving the antenna by the adequate
	 * beam deviation factor.
	 * @param axis name of the axis to scan
	 * @param span total length of the scan
	 * @param duration time required by the scan to complete
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
    virtual void peaker(const char *axis,CORBA::Double span,ACS::TimeInterval duration) throw (
			ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

	/**
	 * It allows to immediately start an OTF scan along the longitude axis of the given frame.
	 * A visible and observable source must be already commanded in order to take its equatorial J2000 coordinate as
	 * center of the OTF scan.
	 * @param scanFrame frame along which doing the scan
	 * @param span total lenght of the scan (radians)
	 * @param duration time required by the scan to complete
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
    virtual void lonOTF(Antenna::TCoordinateFrame scanFrame,CORBA::Double span,ACS::TimeInterval duration) throw (
			ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

	/**
	 * It allows to immediately start an OTF scan along the latitude axis of the given frame.
	 * A visible and observable source must be already commanded in order to take its equatorial J2000 coordinate as
	 * center of the OTF scan.
	 * @param scanFrame frame along which doing the scan
	 * @param span total lenght of the scan (radians)
	 * @param duration time required by the scan to complete
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
    virtual void latOTF(Antenna::TCoordinateFrame scanFrame,CORBA::Double span,ACS::TimeInterval duration) throw (
			ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

	/**
	 * It allows to move the antenna along the elevation in order to perform a skydip scan. The scan will be set according the
	 * currently tracked source.
	 * @param el1 first elevation limit of the scan
	 * @param el2 second elevation limit of the scan
	 * @param duration duration of the scan in 10^-7 seconds, the combination of duration and span gives the scan velocity
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
    virtual void skydipOTF(CORBA::Double el1,CORBA::Double el2,ACS::TimeInterval duration) throw (
			ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start a sidereal traking over a catalog source.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 * @param targetName name of the source to track, it must be known by the system
	*/
	virtual void track(const char *targetName) throw (ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

	/**
	 *  It allows to immediately start a tracking of the moon.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	*/
	virtual void moon() throw (ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

	/**
	 * It allows to immediately start a tracking of a sidereal source, given its equatorial coordinates
	 * @param targetName name or identifier of the source
	 * @param ra right ascension in radians
	 * @param dec declination in radians
	 * @param eq reference equinox of the equatorial coordinates
	 * @param section preferred section of the azimuth cable wrap
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ManagementErrors::ManagementErrorsEx
	 */
	virtual void sidereal(const char * targetName,CORBA::Double ra,CORBA::Double dec,Antenna::TSystemEquinox eq,Antenna::TSections section) throw (
			ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

    /**
	 * It allows to immediately go to a fixed horizontal position (beampark).
	 * @param az azimuth in radians. It could be -1, in that case the current position is taken
	 * @param el elevation in radians. It could be -1, in that case the current position is taken
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx
	 */
    virtual void goTo(CORBA::Double az,CORBA::Double el) throw (ComponentErrors::ComponentErrorsEx,
    		ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

	/**
	 * It aborts the long-running operations
	 */
	virtual void abort() throw (ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

	/**
	 * It prepares the data recording.
	 * @param scanId identifier of the scan that is going to be open
	 */
    virtual void initRecording(CORBA::Long scanid) throw (ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

    /**
     * It starts the data acquisition, it also take care of stopping the data stream after the required duration.
     * @param subScanId identifier of the current subscan
     * @param duration required recording time
     */
    virtual void startRecording(CORBA::Long subScanId,ACS::TimeInterval duration) throw (ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

    /**
     * It finalized the current scan (initiated by initRecording)
     */
    virtual void terminateScan() throw (ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);

    /**
     * Called in order to get current subscan configuration
     * @param conf current subscan configruation
     */
    virtual void getSubScanConfigruation (Management::TSubScanConfiguration_out conf) throw (ComponentErrors::ComponentErrorsEx,ManagementErrors::ManagementErrorsEx,CORBA::SystemException);


private:

	baci::SmartPropertyPointer<baci::ROstring> m_pscheduleName;
	baci::SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus> > m_pstatus;
	baci::SmartPropertyPointer<baci::ROlong> m_pscanID;
	baci::SmartPropertyPointer<baci::ROlong> m_psubScanID;
	baci::SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),POA_Management::ROTBoolean> > m_ptracking;
	baci::SmartPropertyPointer<baci::ROlong> m_pcurrentDevice;
	baci::SmartPropertyPointer<baci::ROstring> m_pprojectCode;
	baci::SmartPropertyPointer<baci::ROstring> m_pcurrentBackend;
	baci::SmartPropertyPointer<baci::ROstring> m_pcurrentRecorder;
	baci::SmartPropertyPointer<baci::ROdoubleSeq> m_prestFrequency;
	CConfiguration m_config;
	CCore *m_core;
};


#endif /*SCHEDULERIMPL_H_*/
