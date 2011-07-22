#ifndef _BOSSCORE_H_
#define _BOSSCORE_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: BossCore.h,v 1.26 2011-04-19 06:53:41 c.migoni Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  24/01/2008      Creation                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  30/12/2008      Fixed a bug in getObservedEquatorials */
/* Andrea Orlati(aorlati@ira.inaf.it)  09/04/2010      added some wrappers of the function startScan */

#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <AntennaErrors.h>
#include <Site.h>
#include <acsContainerServices.h>
#include <MountC.h>
#include <EphemGeneratorC.h>
#include <SkySourceC.h>
#include <MoonC.h>
#include <PointingModelC.h>
#include <RefractionC.h>
#include <OTFC.h>
#include "Configuration.h"
#include <TimeTaggedCircularArray.h>
#include <AntennaDefinitionsS.h>
#include <ManagmentDefinitionsS.h>
#include <AntennaBossS.h>
#include <acsThread.h>
#include <acsncSimpleSupplier.h>
#include "Callback.h"
#include "SlewCheck.h"

using namespace IRA;
using namespace maci;
using namespace baci;

class AntennaBossImpl;
class CWorkingThread;
class CWatchingThread;

/**
 * This class models the Boss datasets and functionalities. 
 * @author <a href=mailto:a.orlati@ira.inaf.it>Orlati Andrea</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class CBossCore {
	friend class AntennaBossImpl;
	friend class CWorkingThread;
	friend class CWatchingThread;
	friend class CCallback;
public:
	/**
	 * Constructor. Default Constructor.
	 * @param service pointer to the continaer services.
	 * @param conf pointer to the class CConfiguration tat conatains all the configuration regarding the component
	 * @param me pointer to the component itself
	*/
	CBossCore(ContainerServices *service,CConfiguration *conf,acscomponent::ACSComponentImpl *me);

	/** 
	 * Destructor.
	*/ 
	virtual ~CBossCore();
	
	/**
	 * This function starts the boss core  so that it will available to accept operations and requests.
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @thore ComponentErrors::CouldntReleaseComponentExImpl
	*/
	virtual void execute() throw (ComponentErrors::CouldntGetComponentExImpl,
			ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntReleaseComponentExImpl);
	
	/**
	 * This function initializes the boss core, all preliminary operation are performed here.
	*/
	virtual void initialize() throw (ComponentErrors::UnexpectedExImpl);
	
	/** 
	 * This function performs all the clean up operation required to free al the resources allocated by the class
	*/
	virtual void cleanUp();
	
	/**
	 * This functions starts a new scan given its parameters. It loads an ammount of coordinates into the mount and then it allows 
	 * for the routine that is in charge to keep the trajectory up to date. This method succeeds only if the mount has
	 * already been configured in PROGRAMTRACK mode.
	 * @thorw ComponentErrors::CouldntReleaseComponentExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImp
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::OperationErrorExImpl
	 * @param targetName name of the target of the scan
	 * @param startUT time at which the stat is supposed to start, if zero or negative the scan must start as soon as possible. In that case the value could be returned back with
	 *        the expected (computed) start time.
	 * @param parameters this array stores the parameters required by the coordinate generator to compute the next coordinates.
	 */ 
	void startScan(const char* targetName,ACS::Time& startUt,const Antenna::TTrackingParameters& parameters) throw(
			ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
					ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl);
	
	
	/**
	 * This function immediately starts a sidereal tracking over a source from the interna source catalog. Internally it calls the <i>setScan()</i> method.
	 * @param targetName identifier of the source
	 * @thorw ComponentErrors::CouldntReleaseComponentExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImp
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::OperationErrorExImpl
	 */
	void track(const char *targetName) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
					ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl);
	
	/**
	 * This function immediately starts a the tracking of the moon. Internally it calls the <i>setScan()</i> method.
	 * @thorw ComponentErrors::CouldntReleaseComponentExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImp
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::OperationErrorExImpl
	 */			
	void moon() throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
			ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::OperationErrorExImpl);
			
	/**
	 * This function immediately starts a OTF scan starting from a central equatorial point. The scan is done along the elevation axis of the telescope.
	 * Internally it calls the <i>setScan()</i> method.
	 * @param cRa right ascension of the central point,J2000,radians
	 * @param cDec declination of the central point,J2000,radians
	 * @param span radians of the elevation travel (radians)
	 * @param duration duration of the scan (100ns)
	 * @thorw ComponentErrors::CouldntReleaseComponentExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImp
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::OperationErrorExImpl
	 */
	void elevationScan(const double& cRa,const double& cDec,const double& span,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntReleaseComponentExImpl,
			ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
			ComponentErrors::OperationErrorExImpl);

	/**
	 * This function immediately starts a OTF scan starting from a central equatorial point. The scan is done along the azimuth axis of the telescope.
	 * Internally it calls the <i>setScan()</i> method.
	 * @param cRa right ascension of the central point,J2000,radians
	 * @param cDec declination of the central point,J2000,radians
	 * @param span radians of the azimuth travel (radians)
	 * @param duration duration of the scan (100ns)
	 * @thorw ComponentErrors::CouldntReleaseComponentExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImp
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::OperationErrorExImpl
	 */	
	void azimuthScan(const double& cRa,const double& cDec,const double& span,const ACS::TimeInterval& duration) throw (ComponentErrors::CouldntReleaseComponentExImpl,
			ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
			ComponentErrors::OperationErrorExImpl);
	
	/**
	 * This function immediately starts a sidereal tracking over a source given its equqtorial coordinates.
	 * Internally it calls the <i>setScan()</i> method.
	 * @param sourceName name of the source.
	 * @param ra right ascension in radians
	 * @param dec declination in radians
	 * @param epoch epoch of the equtorial coordinate ("ANT_J2000 or ANT_B1950 or ANT_APPARENT")
	 * @param section azimuth section (ACU_CW or ACU_CCW or 
	 * @thorw ComponentErrors::CouldntReleaseComponentExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImp
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::OperationErrorExImpl
	 */	
	void source(const char *sourceName,const double& ra,const double& dec,const Antenna::TSystemEquinox& epoch,const Antenna::TSections& section) throw (ComponentErrors::CouldntReleaseComponentExImpl,
			ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
			ComponentErrors::OperationErrorExImpl);

	/**
	 * Immediately stops the antenna tracking and stops the mount, the ephemeris generator is also released.
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::CouldntReleaseComponentExImpl
	 * @thorw ComponentErrors::CouldntGetComponentExImpl
	*/
	void stop() throw (ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntCallOperationExImpl,
			ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This routine is used to setup the antenna before starting any observation. First of all it unstow the antenna (if required) and then it configures
	 * the standard operational mode (PROGRAMTRACK). It also synchronizes the time of the Antenna Control Unit with the system time..
	 * @param config mnemonic code of the configuration
	 * @throw ManagementErrors::ConfigurationErrorExImpl
	*/
	void setup(const char *config) throw (ManagementErrors::ConfigurationErrorExImpl);
	
	/**
	 * This routine is used to put the antenna to survival posistion. It does anything else than calling the stow() method provided
	 * by the mount.
	 * @throw ManagementErrors::ParkingErrorExImpl
	*/
	void stow() throw (ManagementErrors::ParkingErrorExImpl);
	
	/**
	 * This sets new offsets in the present generator for the given frame. If the generator has not been selected yet,
	 * the offsets are simply stored. Every time a new tracking is commanded the offsets are not reset excpet the case a new generatoris required and loaded.
	 * @param lonOff new azimuth offset in radians
	 * @param latOff new elevation offset in radians
	 * @param frame reference frame
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @thorw ComponentErrors::OperationErrorExImpl
     */
	void setOffsets(const double& lonOff,const double& latOff,const Antenna::TCoordinateFrame& frame) throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl);

	/**
	 * Sets the <i>m_enable</i> flag to false, i.e. the component will not send command to the mount any more. To enable the component again a call
	 * to <i>enable()</i> is required. 
	*/
	void disable();
	
	/**
	 * Sets the <i>m_enable</i> flag to true, i.e. the component will regularly send command to the mount. To disable the component again a call
	 * to <i>disable()</i> is required. 
	*/	
	void enable();
	
	/**
	 * Sets the current value for the BWHM
	 * @param val new value in radians. 
	*/
	void setBWHM(const double& val);
	
	/**
	 * Sets the current value for the BWHM, computing it from the taper and the wave length
	 * @param taper taper of the current receiver
	 * @param waveLen wave length of the sky frequency in meters
	 */
	void computeBWHM(const double& taper,const double& waveLen);
	
	/**
	 * Sets the current value for the vlsr. 
	*/
	void setVlsr(const double& val);	
	
	/**
	 * Sets the <i>m_correctionEnable</i> flag to true, i.e. the component when commanding a tracking curve will apply 
	 * the correction due to pointing model and refraction. Call <i>disableCorrection()</i< to disable this feature.
	*/
	void enableCorrection();
	
	/**
	 * Sets the <i>m_correctionEnable</i> flag to false, i.e. the component when commanding a tracking curve will not apply
	 * the correction due to pointing model and refraction. In that case the Raw coordinates will match the Apparetn ones. 
	 * Call <i>enableCorrection()</i> to enable this feature.
	*/	
	void disableCorrection();
	
	/**
	 * This function is used internally by the component to pass the pointer to the working thread to the core
	 * @param work pointer to the thread
	*/
	void setWorkingThread(ACS::Thread *work) {m_workingThread=work; }

	/**
	 * @return the last computed raw azimuth 
	*/ 	
	const double& getRawHorizontalAzimuth() const { return m_rawCoordinates.getLastAzimuth(); }
	
	/**
	 * @return the last computed raw elevation
	*/
	const double& getRawHorizontalElevation() const { return m_rawCoordinates.getLastElevation(); }
	
	/**
	 * @return the last observed azimuth 
	*/ 
	const double& getObservedHorizontalAzimuth() const { return m_observedHorizontals.getLastAzimuth(); }

	/**
	 * @return the last observed azimuth 
	*/ 
	const double& getObservedHorizontalElevation() const { return m_observedHorizontals.getLastElevation(); }
	
	/**
	 * @return the last observed right Ascension 
	*/ 
	const double& getObservedEquatorialRightAscension() const { return m_observedEquatorials.getLastAzimuth(); }
	
	/**
	 * @return the last observed Declination 
	*/ 
	const double& getObservedEquatorialDeclination() const { return m_observedEquatorials.getLastElevation(); }
	
	/**
	 * @return the last observed galactic longitude
	*/ 
	const double& getObservedGalacticLongitude() const { return m_observedGalactics.getLastAzimuth(); }
	
	/**
	 * @return the last observed galactic latitude 
	*/ 
	const double& getObservedGalacticLatitude() const { return m_observedGalactics.getLastElevation(); }
	
	/**
	 * @return the J2000 right ascension of the target
	 */
	const double& getTargetRightAscension() const { return m_targetRA; }
	
	/**
	 * Get all the user offset (refferred to all supported frames) with a single call.
	 */
	void getAllOffsets(double& azOff,double& elOff,double& raOff,double& decOff,double& lonOff,double& latOff) const;
	
	/**
	 * @return the J2000 right ascension of the target
	 */
	const double& getTargetDeclination() const { return m_targetDec; }
	
	/**
	 * @return the radial velocity of the target in km/s
	 */
	const double& getTargetVlsr() const { return m_targetVlsr; }
		
	/**
	 * This funtions can be called to get the observed  equatorial J2000 coordinates at any given time. This coordinates are computed starting from the 
	 * observed horizontal coordinates (@sa <i>setObservedCoordinates()</i>) 
	 * There are two cases: if the duration is smaller than the coordinate integration time (parameter from the CDB), the retruned coordinate is istantaneous, 
	 * otherwise the coordinate is the average over the duration time.
	 * In the instantaneous case, if the particular time is not present in the buffer the returned point is the result of a linear interpolation between the two nearest points. 
	 * @param time the epoch which the observed horizontal coordinates (where the equatorial ones come from) refers to.
	 * @param duration in conjunction with <i>time</i> identifies a period over which the returned coordinate in integrated.
	 * @param ra  the requested right ascension  coordinate.
	 * @param el the reqeusted declination  coordinate.
	 */
	void getObservedEquatorial(TIMEVALUE& time,TIMEDIFFERENCE& duration,double&ra,double& dec) const;
		
	/**
	 * This function is called in order to get the observed horizontal coordinates at a given epoch. if the epoch is not present in the buffer the returned point is the
	 * result of a linear interpolation between the two nearest points. 
	 * @param time the epoch which the horizontal coordinates refers to
	 * @param duration in conjunction with <i>time</i> identifies a period over which the returned coordinate in integrated. 
	 * @param az the requested azimuth coordinate
	 * @param el the reqeusted elevation coordinate
	 */ 
	void getObservedHorizontal(TIMEVALUE& time,TIMEDIFFERENCE& duration,double& az,double& el) const;
	
	/**
	 * This function is called in order to get the raw horizontal coordinates at a given epoch. If the epoch is not present in the buffer the returned point is the
	 * result of a linear interpolation between the two nearest points. 
	 * @param time the epoch which the horizontal coordinates refers to
	 * @param az the requested azimuth coordinate
	 * @param el the reqeusted elevation coordinate
	 */ 
	void getRawHorizontal(const TIMEVALUE& time,double& az,double& el) const;

	/**
	 * This function can be called to get the observed  coordinates at any given time. This coordinates are computed starting from the 
	 * observed equatorial coordinates at J2000 (@sa <i>setObservedCoordinates()</i>). If the particular time is not present in the buffer the returned point is the
	 * result of a linear interpolation between the two nearest points.
	 * @param time the epoch which the observed horizontal coordinates (whiere the equatorial ones come from) refers to.
	 * @param lng  the requested galactic longitude coordinate.
	 * @param lat the reqeusted galactic latitude coordinate.
	*/
	void getObservedGalactic(const TIMEVALUE& time,double &lng,double& lat) const;
	
	/**
	 * This function will compute the slewing time of the telescope starting from the current position to the
	 * target position. The target is described giving the tracking parameters as it was a norma scan start.
	 * It also check is the scan can be performed according the elevation limits of the telescope.
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw AntennaErrors::ScanErrorExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @param startUt time by which the scan is supposed to start, if negative or zero the start time is not considered and the scan is supposed start
	 *         as soon as possible
	 * @param targetName name of the target
	 * @param par the description of the scan that has to be checked
	 * @param slewingTime time that the telescope will take to reach the target position
	 * @return true if the telescope will be in the target position before the given epoch expires (for example the target is below the horizon), false if the
	 * telescope will not be able to get there.
	 */ 
														   
	bool checkScan(const ACS::Time& startUt,const char *targetName,const Antenna::TTrackingParameters& par,ACS::TimeInterval& slewingTime) throw (
			ComponentErrors::CouldntGetComponentExImpl,AntennaErrors::ScanErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * This funtion returns the value that descrines the type of the current ephemeris generator.
	 * @return the current ephem generator type.
	 */
	inline Antenna::TGeneratorType getGeneratorType() const { return m_generatorType; }

	/**
	 * This function returns the enable flags, this flag is true if the boss is enable to send command to the antenna. Viceversa if false the component
	 * works in a sort of simulation mode.
	 * @return a boolean value that is true if the antenna is enabled
	*/
	inline bool getEnable() const { return m_enable; } 
	
	/**
	 * @return a boolean value that indicates wether the refraction/pointing model corrections are anabled or not
	 */
	inline bool getCorrectionEnable() const { return m_correctionEnable; }
	
	/**
	 * This function returns the value of the last offset, coming from pointing model, applied to the encoders coordinate in order to get
	 *  the observed azimuth coordinate.
	 * @return the azimuth offset in radians
	*/ 
	inline double getPointingAzOffset() const { return m_pointingAzOffset; }
	
	/**
	 * This function returns the value of the last offset, coming from pointing model, applied to the encoders coordinate in order to get
	 *  the observed elevation coordinate.
	 * @return the elevation offset in radians
	*/ 
	inline double getPointingElOffset() const { return  m_pointingElOffset; }

	/**
	 * This function returns the value of the last offset, coming from refraction model, applied to the encoders coordinate in order to get
	 *  the observed elevation coordinate.
	 * @return the elevation offset in radians
	*/ 
	inline double getRefractionOffset() const { return m_refractionOffset; }
	
	/**
	 * This function returns the value of the current azimuth offset, 
	 * @return the azimuth offset in radians
	*/ 
	inline double getAzimuthOffset() const { return (m_offsetFrame==Antenna::ANT_HORIZONTAL)?m_longitudeOffset:0.0; }

	/**
	 * This function returns the value of the current elevation offset, 
	 * @return the elevation offset in radians
	*/ 
	inline double getElevationOffset() const { return (m_offsetFrame==Antenna::ANT_HORIZONTAL)?m_latitudeOffset:0.0; }
	
	/**
	 * This function returns the value of the current rightAscension offset, 
	 * @return the rightAscension offset in radians
	*/ 
	inline double getRightAscensionOffset() const { return (m_offsetFrame==Antenna::ANT_EQUATORIAL)?m_longitudeOffset:0.0; }
	
	/**
	 * This function returns the value of the current declination offset, 
	 * @return the declination offset in radians
	*/ 
	inline double getDeclinationOffset() const { return (m_offsetFrame==Antenna::ANT_EQUATORIAL)?m_latitudeOffset:0.0; }
	
	/**
	 * This function returns the value of the current longitude offset, 
	 * @return the longitude offset in radians
	*/ 
	inline double getLongitudeOffset() const { return (m_offsetFrame==Antenna::ANT_GALACTIC)?m_longitudeOffset:0.0; }

	/**
	 * This function returns the value of the current latitude offset, 
	 * @return the latitude offset in radians
	*/ 
	inline double getLatitudeOffset() const { return (m_offsetFrame==Antenna::ANT_GALACTIC)?m_latitudeOffset:0.0; }
	
	/**
	 * This functions returns the name of the current target, if it exists.
	 * @return a string containig the source identifier 
	*/
	inline const CString& getTargetName() const { return m_targetName; }
	
	/**
	 * This function returns the status of the Antenna subsystem; this indicates if the system is working correctly or there are some
	 * possible problems or a failure has been encoutered. This flag takes also into consideration the status of the Boss. 
	 */
	inline const Management::TSystemStatus& getStatus() const { return m_status; }
	
	/**
	 * @return the HPBW in radians currently in use. The default is zero, that means the component will be in <i>MNG_WARNING</i>
	 * status.  
	 */
	inline const double& getBWHM() const { return m_BWHM; }
	
	void getAllattributes();
	
	/**
	 * This is a wrapper function for the <i>setOffsets()</i> function when the horizontal frame is implicit.
	 * @param azOff azimuth offset (rad)
	 * @param elOff elevation offset (rad)
	 */
	void setHorizontalOffsets(const double& azOff,const double& elOff) throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl);
	
	/**
	 * This is a wrapper function for the <i>setOffsets()</i> function when the equatorial frame is implicit.
	 * @param raOff right ascension offset (rad)
	 * @param decOff declination offset (rad)
	 */
	void setEquatorialOffsets(const double& raOff,const double& decOff) throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl);
	
	/**
	 * This is a wrapper function for the <i>setOffsets()</i> function when the galactic frame is implicit.
	 * @param lonOff galactic longitude offset (rad)
	 * @param latOff galactic latitude offset (rad)
	 */
	void setGalacticOffsets(const double& lonOff,const double& latOff) throw(ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::OperationErrorExImpl);
	
	
private:
	/**
	 * Stores the sequence of observed horizontal,equatorial and galactic coorddintes, sorted by the time of observation
	*/
	CTimeTaggedCircularArray m_observedHorizontals;
	CTimeTaggedCircularArray m_observedEquatorials;
	CTimeTaggedCircularArray m_integratedObservedEquatorial;
	CTimeTaggedCircularArray m_observedGalactics;
	CTimeTaggedCircularArray m_rawCoordinates;
	ContainerServices* m_services;
	/**
	 * This object is able to read the configuration from the CDB
	*/
	CConfiguration* m_config;
	/**
	 * This object is in charge of storing the site information
	*/
	CSite m_site;
	/**
	 *  The difference between UTC and UT1
	*/
	double m_dut1;
	/**
	 * This is the reference to the antenna mount 
	*/
	Antenna::Mount_var m_mount;
	/**
	 * flag that indicates an error was relevated during communication to the mount
	 */
	bool m_mountError;
	/**
	 * This is the reference to the object that incarnate the antenna ephem generator interface
	*/
	Antenna::EphemGenerator_var m_generator;
	/**
	 * This is the reference to the poiting model component
	*/
	Antenna::PointingModel_var m_pointingModel;
	/**
	 * This is the reference to the refraction component
	 */
	Antenna::Refraction_var m_refraction;
	/**
	 * if this flag is false no action is taken by the component
	*/ 
	bool m_enable;
	/**
	 * if this flag is true the correction due to pointing model or refraction are not taken.
	*/
	bool m_correctionEnable;
	/**
	 * this field reports the last applied offset to the azimuth due to pointing model
	*/
	double m_pointingAzOffset;
	/**
	 * this field reports the last applied offset to the elevation due to pointing model
	*/	
	double m_pointingElOffset;
	/**
	 * this field reports the last applied offset to the elevation due to refraction
	*/		
	double m_refractionOffset;
	
	double m_longitudeOffset;
	double m_latitudeOffset;
	Antenna::TCoordinateFrame m_offsetFrame;
	
	/**
	 * This field  reports the type of the current generator
	 */
	Antenna::TGeneratorType m_generatorType;
	
	/**
	 * stores the name of the target, could be a name of a source or any identifier
	*/
	IRA::CString m_targetName;
	
	/**
	 * This member stores the time of the last loaded point in the ACU
	 */
	ACS::Time m_lastPointTime;
	
	/**
	 * This is the pointer to the working thred, it must be waked up when a tracking is started 
	*/
	ACS::Thread *m_workingThread;
	
	/**
	 * This represnets the status of the whole Antenna subsystem, it also includes and sammerizes the status of the boss component  
	 */
	Management::TSystemStatus m_status;
	
	/** This represents the status of the boss component. */
	Management::TSystemStatus m_bossStatus;
	
	/** This time mark the last change in the boss status, if far enough, the error that generates the change is considered cleared */
	TIMEVALUE m_lastStatusChange;
	 
	/** This is the pointer to the notification channel */
	nc::SimpleSupplier *m_notificationChannel;
	
	/** pointer to the component itself */
	acscomponent::ACSComponentImpl *m_thisIsMe;
	
	/** This variable stores the information wether the antenna is now tracking or not */
	bool m_tracking;
	
	/** This member stores the time that  the above tracking (<i>m_tracking</i>) information refers to */
	ACS::Time m_trackingTime;
	/**
	 * This marks the start time of the current equatorial coorinate integration
	 */
	ACS::Time m_integrationStartTime;
	/**
	 * value of the right ascension in the current integration
	 */
	double m_integratedRa;
	/**
	 * value of the declination in the current integration
	 */
	double m_integratedDec;
	/**
	 * number of integrated samples of the current integration
	 */
	long m_integratedSamples; 
	
	/** The beam width at half power. Given in radians. it is used to check if the telescope is tracking or not. */
	double m_BWHM;
	
	/** Reports the last encoder reads (horizontal coordinates) */ 
	double m_lastEncoderAzimuth,m_lastEncoderElevation;
	
	/** Reports the last azimuth sector in which the antenna was */ 
	Antenna::TSections m_lastAzimuthSection;
	
	/** Epoch of the last encoder read */
	ACS::Time m_lastEncoderRead;
	
	/**
	 * stores the time a new scan was started, it will be set to 0 by the working thread after 1 second later. During this period the tracking is forced to be false;
	 */
	ACS::Time m_newScanEpoch;
	
	/**
	 * These are the component used internally to check slewing times
	 */ 
	Antenna::EphemGenerator_var m_siderealGenerator;
	Antenna::EphemGenerator_var m_otfGenerator;
	Antenna::EphemGenerator_var m_moonGenerator;
	Antenna::EphemGenerator_var m_sunGenerator;
	Antenna::EphemGenerator_var m_satelliteGenerator;
	Antenna::EphemGenerator_var m_solarBodyGenerator;
	
	/**
	 * Used to compute the slewing time of the Antenna
	 */
	CSlewCheck m_slewCheck;

	CCallback *m_callbackUnstow;
	CCallback *m_callbackStow;
	CCallback *m_callbackSetup;
	
	ACS::CBvoid_ptr m_cbUnstow;
	ACS::CBvoid_ptr m_cbStow;
	ACS::CBvoid_ptr m_cbSetup;
	
	/**
	 * Reports the right ascension of the target
	 */
	double m_targetRA;
	/**
	 * Reports the declination of the target
	 */
	double m_targetDec;
	/**
	 * Reports the radial velocity of the target
	 */
	double m_targetVlsr;
	
	/** 
	 * This method loads into the mount a coordinate for a given time. It uses the commanded generator in order to provide the required coordinates.
	 * The coordinates are corrected for pointing model and refraction (if <i>m_correctionEnable</i> is set) then commanded to the mount (if the flag
	 * <i>m_enable</i> is set).
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::CouldntCallOperationExImpl
     * @throw ComponentErrors::UnexpectedExImpl
     * @throw AntennaErrors::TrackingNotStartedExImpl
     * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @param time that's the timestamp for the coordinates that should be commanded to the ACU.
	 * @param restart if true the previous statck will be cleared
	*/
	void loadTrackingPoint(const TIMEVALUE& time,bool restart) throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,
				ComponentErrors::UnexpectedExImpl,AntennaErrors::TrackingNotStartedExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This method is used internally to clear the program track curve and restart another one from the scratch. Differently from
	 * the <i>startTracking()</i> routine it does not make checks on the generator It just clears the rawHorizontals stack and load
	 * again minimum number of data points. The typical usage is when a new user offset(not a new scan) is commanded and the previously loaded
	 * trajectory must be reset.
	 * @throw ComponentErrors::OperationErrorExImpl
	 */
	void quickTracking() throw (ComponentErrors::OperationErrorExImpl);
	
	/**
	 * This function can be called to update all the observed coordinates(horizontal, equatorial, galactic). First of all it asks the mount for the encoders
	 *  coordinates. Then  these coordinates are depurated by all the effects (pointing model, refraction). The resulting coordinates are stored in a
	 *  buffer (sorted according to their timestamp). 
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	*/ 
	void updateAttributes() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,
			ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This function is called by the watching thread in order to check if the status of the Antenna subsystem is OK or not. It
	 * updates the <i>m_staus</i> variables. This check will be performed not more than once a second even if the gap between
	 * two execution of the  thread is far less.
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::CouldntGetAttributeExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 */
	void checkStatus() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetAttributeExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This member function is used by the watch dog thread in order to publish data to the notification channel. The data are
	 * posted every time a field has changed its value and at least once a second.
	 * @throw  ComponentErrors::NotificationChannelErrorExImpl if the component is not able to send data.
	*/
	void publishData() throw (ComponentErrors::NotificationChannelErrorExImpl);
	
	/**
	 * This function changes the status of the component in case of errors or when the errors are solved. 
	 * Used internally when an exception or an error is found
	*/
	void changeBossStatus(const Management::TSystemStatus& status);
	
	/** 
	 * This function is used to change the axis modes in the mount to <i>ACU_PROGRAMTRACK</i>.
	 * @throw (ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	*/ 
	void setProgramTrack() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,
			ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This is function is used to change the axis mode to <i>ACU_PRESET</i>.
	 * @throw (ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl 
	*/
	void setPreset() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,
			ComponentErrors::UnexpectedExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This function send the unstow command to the antenna. It does not wait the antenna is unstowed.
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 */
	void unstow() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This function set a preset point to the mount, it succeeds only if the <i>ACU_PRESET</i> mode was previously commanded.
	 * @param az position for the azimuth in radians
	 * @param el position fo the elevation in radians
	 * @thorw ComponentErrors::CORBAProblemExImpl
	 * @thorw ComponentErrors::CouldntCallOperationExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 */
	void preset(const double& az,const double& el) throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This function is used to keep up to date the reference to the mount component.
	 * @param ref reference to the the mount component
	 * @param errorDetected true if an error was detected during communication with mount
	 */
	void loadMount(Antenna::Mount_var& ref,bool& errorDetected) const throw (ComponentErrors::CouldntGetComponentExImpl);

	/**
	 * This function is used to keep up to date the reference to the pointing model component.
	 * @param ref reference to the the pointing model component
	 */	
	void loadPointingModel(Antenna::PointingModel_var& ref) const throw (ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * This function is used to keep up to date the reference to the refraction component.
	 * @param ref reference to the the refraction component
	 */	
	void loadRefraction(Antenna::Refraction_var& ref) const throw (ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * free the reference to the mount
	 */
	void unloadMount(Antenna::Mount_var& ref) const;

	/**
	 * free the reference to the pointing model
	 */	
	void unloadPointingModel(Antenna::PointingModel_var& ref) const;
	
	/**
	 * free the reference to the refraction component
	 */		
	void unloadRefraction(Antenna::Refraction_var& ref) const;
	
	/**
	 * This method will load all the dynamic components required for the ephemeris generation.
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @type specifies whihc generator must be loaded.
	 * @return the pointer to the loaded generator, the caller must take care of the reference of the loaded compomnent
	 */ 
	Antenna::EphemGenerator_ptr loadGenerator(const Antenna::TGeneratorType& type) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
			ComponentErrors::UnexpectedExImpl);
	
	/**
	 * This method release the the allocated generator component
	 * @param generator generator component to be released
	 */
	void freeGenerator(Antenna::EphemGenerator_ptr& generator) throw (ComponentErrors::CouldntReleaseComponentExImpl);
	
	/**
	 * This method is a wrapper of the method <i>loadGenerator()</i>, if the required generator inteface has not been laded yet, it calls the <i>loadGenrator()</i> function
	 * in order to make it available
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @type specifies whihc generator must be loaded.
	 */
	void loadInternalGenerator(const Antenna::TGeneratorType& type) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
			ComponentErrors::UnexpectedExImpl);
	
	/** 
	 * This private function will configure the scan into the given generator according the scan parameters and type.
	 * @param generator pointer to the generator component to be configured
	 * @param startUt the scan is required to start at the given time (if zero it can be reset to the estimated start Ut time).
	 * @param targetName name of the target given in input, for example and identifer to search in a catalog
	 * @param par scan parameters
	 * @param section section to forse the antenna, this is an output argument that may be significant for certain generators, also depending from what reported in <i>par.section</i>.
	 * @param ra output parameter, right ascension of the target,radians, J2000
	 * @param dec output parameter declination of the target,radians, J2000
	 * @param vlsr output radial velocity of the target, in Km/s
	 * @param sourceName output parameter, name of the target
	 * @param lonOff longitude offset, radians
	 * @param latOff latitude offsetm radians
	 * @param offFrame offset reference frame
	 */
	void prepareScan(Antenna::EphemGenerator_ptr generator,ACS::Time& startUT,const char *targetName,const Antenna::TTrackingParameters& par,Antenna::TSections& section,
			double& ra,double& dec,double& vlsr,IRA::CString& sourceName,double& lonOff,double& latOff,Antenna::TCoordinateFrame& offFrame) 
			throw (ComponentErrors::CouldntCallOperationExImpl,ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl);
};


#endif /*BOSSCORE_H_*/
