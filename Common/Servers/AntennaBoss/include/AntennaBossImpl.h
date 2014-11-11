#ifndef _ANTENNABOSSIMPL_H
#define _ANTENNABOSSIMPL_H

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*  									         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                when                     What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it) 20/11/2007       Creation                                                  */
/*  Andrea Orlati(aorlati@ira.inaf.it) 20/08/2008      Added the command line parsing                           */
/*  Andrea Orlati(aorlati@ira.inaf.it) 09/04/2010      Added wrappers function: track, moon, elevationScan,azimuthScan                 */
/*  Andrea Orlati(aorlati@ira.inaf.it) 13/09/2010      Major update during several weeks, compliant to all fetures of the IDL interface, and all capabilities implemented  */
/*  Andrea Orlati(aorlati@ira.inaf.it) 10/10/2011      Given implementation of the the method getFluxes() and attribute targetFlux*/
/*  Andrea Orlati(aorlati@ira.inaf.it) 15/02/2012      Given implementation of the the method getApparentCoordinates */
/*  Andrea Orlati(aorlati@ira.inaf.it) 08/02/2013     implemented again the  command method*/
/*  Andrea Orlati(aorlati@ira.inaf.it) 22/02/2013     new implementation of checkScan()  method */
/*  Andrea Orlati(aorlati@ira.inaf.it) 12/06/2014     implementation of the radialVelocity method */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROdouble.h>
#include <baciROstring.h>
#include <enumpropROImpl.h>
#include <Definitions.h>
#include <AntennaDefinitionsS.h>
#include <AntennaBossS.h>
#include <SP_parser.h>
#include <ManagementErrors.h>
#include "BossCore.h"
#include <SecureArea.h>
#include "WorkingThread.h"
#include "WatchingThread.h"

/** 
 * @mainpage AntennaBoss component Implementation 
 * @date 12/06/2014
 * @version 1.6.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 8.2.0
 * @remarks compiler version is 4.1.2
*/

using namespace baci;

_SP_WILDCARD_CLASS(ReferenceFrame_WildCard,"UNDEF");
_SP_WILDCARD_CLASS(VradDefinition_WildCard,"UNDEF");
_SP_WILDCARD_CLASS(VRad_WildCard,"nan");

class ReferenceFrame_converter
{
public:
	Antenna::TReferenceFrame strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		Antenna::TReferenceFrame frame;
		if (!CBossCore::mapReferenceFrame(str,frame)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"ReferenceFrame_converter::strToVal()");
			ex.setExpectedType("Reference frame");
			throw ex;
		}
		return frame;
	}
	char *valToStr(const Antenna::TReferenceFrame& val) {
		IRA::CString frame;
		CBossCore::mapReferenceFrame(val,frame);
		char *c=new char[frame.GetLength()+1];
		strcpy(c,(const char*)frame);
		return c;
	}
};

class VradDefinition_converter
{
public:
	Antenna::TVradDefinition strToVal(const char * str) throw (ParserErrors::BadTypeFormatExImpl) {
		Antenna::TVradDefinition def;
		if (!CBossCore::mapVelocityDefinition(str,def)) {
			_EXCPT(ParserErrors::BadTypeFormatExImpl,ex,"VradDefinition_converter::strToVal()");
			ex.setExpectedType("Radial velocity definition");
			throw ex;
		}
		return def;
	}
	char *valToStr(const Antenna::TVradDefinition& val) {
		IRA::CString def;
		CBossCore::mapVelocityDefinition(val,def);
		char *c=new char[def.GetLength()+1];
		strcpy(c,(const char*)def);
		return c;
	}
};


/**
 * This class is the implementation of the AntennaBoss component. This component is the manager of all the Antenna
 * package and it is capable to lead a single subscan both in tracking and OnTheFly mode.  
 * @author <a href=mailto:a.orlati@ira.inaf.it>Orlati Andrea</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class AntennaBossImpl: public CharacteristicComponentImpl,
				       public virtual POA_Antenna::AntennaBoss
{
public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	AntennaBossImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~AntennaBossImpl(); 

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
	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	
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
     * Returns a reference to the theoreticalAzimuth property Implementation of IDL interface.
	 * @return pointer to read-only double property theoreticalAzimuth
	*/
	virtual ACS::ROdouble_ptr rawAzimuth() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the theoreticalElevation property Implementation of IDL interface.
	 * @return pointer to read-only double property theoreticalElevation
	*/
	virtual ACS::ROdouble_ptr rawElevation() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the observedAzimuth property Implementation of IDL interface.
	 * @return pointer to read-only double property observedAzimuth
	*/
	virtual ACS::ROdouble_ptr observedAzimuth() throw (CORBA::SystemException);

	/**
     * Returns a reference to the observedElevation property Implementation of IDL interface.
	 * @return pointer to read-only double property observedElevation
	*/
	virtual ACS::ROdouble_ptr observedElevation() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the observedRightAscension property Implementation of IDL interface.
	 * @return pointer to read-only double property observedRightAscension
	*/
	virtual ACS::ROdouble_ptr observedRightAscension() throw (CORBA::SystemException);

	/**
     * Returns a reference to the observedDeclination property Implementation of IDL interface.
	 * @return pointer to read-only double property observedDeclination
	*/
	virtual ACS::ROdouble_ptr observedDeclination() throw (CORBA::SystemException);

	/**
     * Returns a reference to the observedGalLongitude property Implementation of IDL interface.
	 * @return pointer to read-only double property observedGalLongitude
	*/
	virtual ACS::ROdouble_ptr observedGalLongitude() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the observedGalLatitude property Implementation of IDL interface.
	 * @return pointer to read-only double property observedGalLatitude
	*/
	virtual ACS::ROdouble_ptr observedGalLatitude() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the target property Implementation of IDL interface.
	 * @return pointer to read-only string property target
	*/
	virtual ACS::ROstring_ptr target() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the targetRightAscension property Implementation of IDL interface.
	 * @return pointer to read-only double property targetRightAscension
	*/	
	virtual ACS::ROdouble_ptr targetRightAscension() throw (CORBA::SystemException);

	/**
     * Returns a reference to the targetRightAscension property Implementation of IDL interface.
	 * @return pointer to read-only double property targetRightAscension
	*/	
	virtual ACS::ROdouble_ptr targetDeclination() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the targetVrad property implementation of IDL interface.
	 * @return pointer to read-only double property targetVrad
	*/
	virtual ACS::ROdouble_ptr targetVrad() throw (CORBA::SystemException);

	/**
     * Returns a reference to the vradReferenceFrame property Implementation of IDL interface.
	 * @return pointer to read-only ROTReferenceFrame property vradReferenceFrame
	*/
	virtual Antenna::ROTReferenceFrame_ptr vradReferenceFrame() throw (CORBA::SystemException);

	/**
     * Returns a reference to the vradDefinition property Implementation of IDL interface.
	 * @return pointer to read-only ROTVradDefinition property vradDefinition
	*/	
	virtual Antenna::ROTVradDefinition_ptr vradDefinition() throw (CORBA::SystemException);

	/**
     * Returns a reference to the targetFlux property implementation of IDL interface.
	 * @return pointer to read-only double property targetFlux
	*/
	virtual ACS::ROdouble_ptr targetFlux() throw (CORBA::SystemException);

	/**
     * Returns a reference to the generatorType property Implementation of IDL interface.
	 * @return pointer to read-only ROTGeneratorType property generatorType
	*/
	virtual Antenna::ROTGeneratorType_ptr generatorType() throw (CORBA::SystemException); 
	
	/**
     * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only ROTSystemStatus property status
	*/
	virtual Management::ROTSystemStatus_ptr status() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the enable property implementation of IDL interface.
	 * @return pointer to read-only ROTBoolean  property enabled
	*/
	virtual Management::ROTBoolean_ptr enabled() throw (CORBA::SystemException);

	/**
     * Returns a reference to the correctionEnabled property implementation of IDL interface.
	 * @return pointer to read-only ROTBoolean  property correctionEnabled
	*/
	virtual Management::ROTBoolean_ptr correctionEnabled() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the pointingAzimuthCorrection property implementation of IDL interface.
	 * @return pointer to read-only double property pointingAzimuthCorrection
	*/
	virtual ACS::ROdouble_ptr pointingAzimuthCorrection() throw (CORBA::SystemException);

	/**
     * Returns a reference to the pointingElevationCorrection property implementation of IDL interface.
	 * @return pointer to read-only double property pointingElevationCorrection
	*/
	virtual ACS::ROdouble_ptr pointingElevationCorrection () throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the refractionCorrection property implementation of IDL interface.
	 * @return pointer to read-only double property refractionCorrection
	*/	
	virtual ACS::ROdouble_ptr refractionCorrection() throw (CORBA::SystemException);

	/**
     * Returns a reference to the azimuthOffset property implementation of IDL interface.
	 * @return pointer to read-only double property azimuthOffset
	*/	
	virtual ACS::ROdouble_ptr azimuthOffset() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the elevationOffset property implementation of IDL interface.
	 * @return pointer to read-only double property elevationOffset
	*/	
	virtual ACS::ROdouble_ptr elevationOffset() throw (CORBA::SystemException);

	/**
     * Returns a reference to the rightAscensionOffset property implementation of IDL interface.
	 * @return pointer to read-only double property rightAscensionOffset
	*/	
	virtual ACS::ROdouble_ptr rightAscensionOffset() throw (CORBA::SystemException);

	/**
     * Returns a reference to the declinationOffset property implementation of IDL interface.
	 * @return pointer to read-only double property declinationOffset
	*/	
	virtual ACS::ROdouble_ptr declinationOffset() throw (CORBA::SystemException);

	/**
     * Returns a reference to the longitudeOffset property implementation of IDL interface.
	 * @return pointer to read-only double property longitudeOffset
	*/	
	virtual ACS::ROdouble_ptr longitudeOffset() throw (CORBA::SystemException);

	/**
     * Returns a reference to the latitudeOffset property implementation of IDL interface.
	 * @return pointer to read-only double property latitudeOffset
	*/	
	virtual ACS::ROdouble_ptr latitudeOffset() throw (CORBA::SystemException);

	/**
     * Returns a reference to the FWHM property implementation of IDL interface.
	 * @return pointer to read-only double property FWHM
	*/	
	virtual ACS::ROdouble_ptr FWHM() throw (CORBA::SystemException);

	/**
     * Returns a reference to the 	waveLength property implementation of IDL interface.
	 * @return pointer to read-only double property 	waveLength
	*/
	virtual ACS::ROdouble_ptr 	waveLength() throw (CORBA::SystemException);
		
	/**
	 * This method is used to stow the antenna.
	 * @throw CORBA::SystemExcpetion
	 * @throw ManagementErrors::ParkingErrorEx  
	 */
	void park() throw (CORBA::SystemException,ManagementErrors::ParkingErrorEx);
	
	/**
	 * This method will be used to configure the mount before starting an observation
	 * param config mnemonic code of the required configuration
	 * @throw CORBA::SystemException
	 * @throw ManagementErrors::ConfigurationErrorEx
	 */
	void setup(const char *config) throw (CORBA::SystemException,ManagementErrors::ConfigurationErrorEx);
	
	/**
	 * This method stops the mount. It also chease the any tracking activities of this component, if there are any. 
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	*/
	void stop() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);
	
	/**
	 * This method causes the enable flag to be set in false, that means the component is not fully operational any more and many
	 * functionalities will not be available.
	 * @throw CORBA::SystemException 
	*/
	void disable() throw (CORBA::SystemException);
	
	/**
	 * This method causes the enable flag to be set in true, that means the component is  fully operational.
	 * @throw CORBA::SystemException 
	*/	
	void enable() throw (CORBA::SystemException);
	
	/**
	 * This methods sets the attribute <i>correctionEnabled</i> to true. That means that the instrumental correction 
	 * will be applied.
	 * @throw CORBA::SystemException 
	 */
	void correctionEnable() throw (CORBA::SystemException);
	
	/**
	 * This methods sets the attribute <i>correctionEnabled</i> to false. That means that the instrumental correction 
	 * will not be applied.
	 * @throw CORBA::SystemException 
	 */	
	void correctionDisable() throw (CORBA::SystemException);
	
	/**
	 * This method sets the value for the FWHM.
	 * @throw CORBA::SystemException
	 * @param value the new value in radians.
	 * òparam waveLen corresponding wave length in meters
	 */
	void setFWHM(CORBA::Double value,CORBA::Double waveLen) throw (CORBA::SystemException);
	
	/**
	 * This method force the <i>FWHM</i> to be computed starting from the current taper and sky frequency.
	 * @throw CORBA::SystemException
	 * @param taper current taper in db
	 * @param waveLength current wave length of the sky frequency in meters. 
	 */
	void computeFWHM(CORBA::Double taper,CORBA::Double waveLength) throw (CORBA::SystemException);

	/**
	 * This method allows to compute the flux based on observing frequency multiple times. The FWHM must be set in order to perform the computation.
	 * Also the current generator must be able to estimate the flux of the current target.
	 * @param freqs list of observing frequencies
	 *  @param fluxes list of the fluxes corresponding to the given frequencies. If the computation could not be done, for some reasons, 1.0 is returned
	 */
    void getFluxes (const ACS::doubleSeq& freqs,ACS::doubleSeq_out fluxes) throw (CORBA::SystemException);

	/**
	 * It will change the current radial velocity, frame and definition for the current target.
	 * @throw CORBA::SystemException
	 * @param vrad radial velocity, given in Km/sec. If the definition is redshift it if the value of Z.
	 * @param vref reference frame
	 * @param vdef velocity definition
	 */
	void radialVelocity(CORBA::Double vrad, Antenna::TReferenceFrame vref, Antenna::TVradDefinition vdef ) throw (CORBA::SystemException);
	
	/**
	 *  This method starts a new scan that could be any of the possible antenna movement.  It loads an ammount of coordinates into the mount and then it starts the thread that is 
	 * in charge to keep the  tracking trajectory up to date. This method succeeds only if the mount has already 
	 * been configured in PROGRAMTRACK mode.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx
	 * @param startUt time from which the scan have to start. If negative the scan is started as soon as possible. In that case the argument could return back the computed effective start UT time.
	 * @param parameters this structure stores the parameters required by the ephemeris generator to computes new coordinates.
	 * @param secondary this the secondary tracking structure. It may be used according to the generaor given with the primary structure.
	*/	
	void startScan(ACS::Time& startUT,const Antenna::TTrackingParameters& parameters,const Antenna::TTrackingParameters& secondary) throw (CORBA::SystemException,
			AntennaErrors::AntennaErrorsEx,ComponentErrors::ComponentErrorsEx);
	
	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start a sidereal traking over a catalog source.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx 
	 * @param targetName name of the source to track, it must be known by the system
	*/ 
	virtual void track(const char *targetName) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);
			
	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start a tracking of the moon.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx 
	*/ 
	virtual void moon() throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);

	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start a skydip scan
	 * @param el1 sets the first edge of the elevation range, a negative means take the default value
	 * @param el2 sets the second edge of the elevation range, a negative means take the default value
	 * @param duration duration of the scan
	 * @return the expected start time of the scan, if a the target is not visible a zero is returned
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx
	*/
	virtual ACS::Time skydipScan(CORBA::Double el1,CORBA::Double el2,ACS::TimeInterval duration) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);
		
	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start a traking of a sidereal source, given its equatorial coordinates
	 * @param targetName name or identifier of the source
	 * @param ra right ascension in radians
	 * @param dec declination in radians
	 * @param eq reference equinox of the equatorial coordinates
	 * @param section preferred section of the azimuth cable wrap
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx  
	 */
	virtual void sidereal(const char * targetName,CORBA::Double ra,CORBA::Double dec,Antenna::TSystemEquinox eq,Antenna::TSections section) throw (
			ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);

	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately go offsource along a given frame. The Offset is always done in longitude a part the case
	 * of horizontal frame. In that case if the elevation is above a cut off limit the offset is done in latitude.
	 * @param frame frame involved for the offset
	 * @param skyOffset total sky offset (radians)
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx  
	 */
    virtual void goOff(Antenna::TCoordinateFrame frame,CORBA::Double skyOffset) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);

	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately go to a fixed horizontal position.
	 * @param az azimuth in radians
	 * @param el elevation in radians
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx
	 */
    virtual void goTo(CORBA::Double az,CORBA::Double el) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);

	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start an OTF scan along the longitude axis of the given frame. A visible and observable source must be already commanded
	 * in order to take its equtorial J200 coordinate as center of the OTF scan.
	 * @param scanFrame frame along which doing the scan
	 * @param span total lenght of the scan (radians) 
	 * @param duration time required by the scan to complete
	 * @return the expected start time of the scan, if a the target is not visible a zero is returned
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx  
	 */	
    virtual ACS::Time lonOTFScan(Antenna::TCoordinateFrame scanFrame,CORBA::Double span,ACS::TimeInterval duration) throw (
			ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);
	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start an OTF scan along the latitude axis of the given frame. A visible and observable source must be already commanded
	 * in order to take its equtorial J200 coordinate as center of the OTF scan.
	 * @param scanFrame frame along which doing the scan
	 * @param span total lenght of the scan (radians) 
	 * @param duration time required by the scan to complete
	 * @return the expected start time of the scan, if a the target is not visible a zero is returned
 	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx  
	 */	
    virtual ACS::Time latOTFScan(Antenna::TCoordinateFrame scanFrame,CORBA::Double span,ACS::TimeInterval duration) throw (
			ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);
    
	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start an On The Fly scan over a central point. The central point is
	 * given in J2000 equatorial frame and the scan is done along the elevation axis of the telescope.
	 * @param cRa right ascension of the central point (radians)
	 * @param cDec declination of the central point (radians)
	 * @param span distance that the telescope travels during the scan
	 * @param duration duration of the scan, the combinaiton of duration and span gives the scan velocity
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx 
	 */ 
	//void elevationScan(CORBA::Double cRA,CORBA::Double cDec,CORBA::Double span,ACS::TimeInterval duration) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,
			//CORBA::SystemException);
			
	/**
	 * This is a wrapper of the <i>startScan()</i> function. It allows to immediately start an On The Fly scan over a central point. The central point is
	 * given in J2000 equatorial frame and the scan is done along the azimuth axis of the telescope.
	 * @param cRa right ascension of the central point (radians)
	 * @param cDec declination of the central point (radians)
	 * @param span distance that the telescope travels during the scan
	 * @param duration duration of the scan, the combinaiton of duration and span gives the scan velocity
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx 
	 */ 
	//void azimuthScan(CORBA::Double cRA,CORBA::Double cDec,CORBA::Double span,ACS::TimeInterval duration) throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,
			//CORBA::SystemException); 
	
	/**
	 * This function is used internally to compute the slewing time of the telescope starting from the 
	 * current position to the target position.
	 * @throw ComponentErrors::ComponentErrrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx
	 * @throw CORBA::SystemException 
	 * @param targetName name of the target of the scan 
	 * @param startUt this is the time before which the telescope is supposed to reach the target
	 * @param parameters this structure describes the target
	 * @param secondary this the secondary tracking structure. It may be used according to the generator given with the primary structure.
	 * @param slewingTime time that the telescope will take to reach the target position
	 * @param minElLimit  elevation lower limit to check the visibility of the target
	 * @param maxElLimit elevation upper  limit to check the visibility of the target
	 * @return true if the telescope will be in the target position before the given epoch expires and the target is above the horizon, false if the
	 * telescope will not be able to get there.
	 */ 
	bool checkScan(ACS::Time startUt,const Antenna::TTrackingParameters& parameters,const Antenna::TTrackingParameters& secondary,ACS::TimeInterval_out slewingTime,CORBA::Double minElLimit,CORBA::Double maxElLimit) throw (
			ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);
	
	/**
	 * This method try to reset all the pending error conditions of the Antenna subsystem
	 * @throw ComponentErrors::ComponentErrrorsEx
	 * @throw AntennaErrors::AntennaErrorsEx
	 * @throw CORBA::SystemException 
	*/
	void reset() throw (ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx,CORBA::SystemException);

	/**
	 * This method implements the command line interpreter. the interpreter allows to ask for services or to issue commands
	 * to the sub-system by human readable command lines.
	 * @param cmd string that contains the command line
	 * @param answer string that contains the answer to the command.
	 * @return true if the operation is successful, false otherwise
	 * @throw CORBA::SystemException
	 */
	virtual CORBA::Boolean command(const char *cmd,CORBA::String_out answer) throw (CORBA::SystemException);
	//virtual char * command(const char *cmd) throw (CORBA::SystemException,ManagementErrors::CommandLineErrorEx);
	
	/**
	 * This method allows the user to set the offsets for the given frame. The offset are considered only if a tracking has already
	 * been started, otherwise they are stored for next scan.  
	 * The longitude offset will be corrected for the cosine of latitude.
	 * @param lonOff new offsets for azimuth (radians)
	 * @param latOff new offsets for elevation (radians)
	 * @param frame reference frame
	 * @throw CORBA::SystemException
	 * @throw AntennaErrors::AntennaErrorsEx 
     * @throw ComponentErrors::ComponentErrorsEx
	 *    @arg  \c ComponentErrors::UnexpectedExImp
	 *    @arg  \c ComponentErrors::CORBAProblemExImpl 
	*/
	virtual void setOffsets(CORBA::Double lonOff,CORBA::Double latOff,Antenna::TCoordinateFrame frame) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx);

	/**
	 * This function is used internally. It permits to access the generator currently used by the boss in order to produce new coordinates.
	 * A client can read the computed (apparent) coordinates by getting the componet which has the returned CURL..
	 * @throw CORBA::SystemException
	 * @param type it indicates the type of the generator, useful when narrowing to the real generator.
	 * @return the string that represents the CURL to the generator currently in use. It must be freed by the caller.  
	*/
	char *getGeneratorCURL(Antenna::TGeneratorType_out type) throw (CORBA::SystemException);
	
	/**
	 * Is is used to return the raw coordinates commanded to the mount for the given timestamp.
	 * @throw CORBA::SystemException
	 * @param time the given timestamp to which the returned coordinates refer to
	 * @param az the returned raw azimuth in radians
	 * @param el the returned raw elevation in radians 
	*/
	void getRawCoordinates(ACS::Time time,CORBA::Double_out az,CORBA::Double_out el) throw (CORBA::SystemException);
	
	/**
	 * called to get apparent coordinates as they come from ephemeris generators.
	 * @throw CORBA::SystemException
	 * @param time the given timestamp  the returned coordinates refer to
	 * @param az apparent azimuth in radians
	 * @param el apparent elevation in radians
	 * @param ra apparent right ascension in radians
	 * @param dec apparent declination in radians
	 * @param jepoch julian epoch the equatorial position refers to
	 * @parm lon galactic longitude converted from the returned apparent equatorial position
	 * @parm lat galactic latitude converted from the returned apparent equatorial position
	 */
	void getApparentCoordinates (ACS::Time time,CORBA::Double_out az,CORBA::Double_out el,CORBA::Double_out ra,CORBA::Double_out dec,CORBA::Double_out jepoch,CORBA::Double_out lon,
			CORBA::Double_out lat) throw (CORBA::SystemException);

	/**
	 * It is used to return the observed  equatorial coordinates for the given timestamp and interval.
	 * @throw CORBA::SystemException
	 * @param time the given timestamp to which the returned coordinates refer to
	 * @param duration time interval over which the coordinates are averaged
	 * @param ra the returned observed right ascension in radians
	 * @param el the returned observed declination in radians 
	*/
	void getObservedEquatorial(ACS::Time time,ACS::TimeInterval duration,CORBA::Double_out ra,CORBA::Double_out dec) throw (CORBA::SystemException);

	/**
	 * It is used to return the observed  galactic coordinates for the given timestamp.
	 * @throw CORBA::SystemException
	 * @param time the given timestamp to which the returned coordinates refer to
	 * @param duration time interval over which the coordinates are averaged
	 * @param longitude the returned observed longitude in radians
	 * @param latitude the returned observed latitude in radians 
	*/
	void getObservedGalactic(ACS::Time time,ACS::TimeInterval duration,CORBA::Double_out longitude,CORBA::Double_out latitude) throw (CORBA::SystemException);
	
	/**
	 * It is used to returns the observed horizontal coordinates for the given timestamp.
	 * @throw CORBA::SystemException
	 * @param time the given timestamp to which the returned coordinates refer to
	 * @param duration time interval over which the coordinates are averaged
	 * @param ra the returned observed azimuth in radians
	 * @param el the returned observed elevation in radians 
	*/
	void getObservedHorizontal(ACS::Time time,ACS::TimeInterval duration,CORBA::Double_out az,CORBA::Double_out el) throw (CORBA::SystemException);
	
	/**
	 * It is used internally to returns the user offsets in all supported frames
	 * @throw CORBA::SystemException
	 * @param raOff the returned offset azimuth in radians
	 * @param elOff the returned offset elevation in radians
	 * @param raOff the returned offset right ascension in radians
	 * @param decOff the returned offset declination in radians
	 * @param lonOff the returned offset longitude in radians
	 * @param latOff the returned offset latitude in radians  
	*/
	void getAllOffsets(CORBA::Double_out azOff,CORBA::Double_out elOff,CORBA::Double_out raOff,CORBA::Double_out decOff,CORBA::Double_out lonOff,CORBA::Double_out latOff) throw (CORBA::SystemException);
	
	/**
	 * It can be called to know which is the axis the antenna is currently performing the scan
	 * @param axis returned identfier of the axis
	 */
	void getScanAxis (Management::TScanAxis_out axis) throw (CORBA::SystemException);
		
private:
	SmartPropertyPointer<ROstring> m_ptarget;
	SmartPropertyPointer<ROdouble> m_prawAzimuth;
	SmartPropertyPointer<ROdouble> m_prawElevation;
	SmartPropertyPointer<ROdouble> m_pobservedAzimuth;
	SmartPropertyPointer<ROdouble> m_pobservedElevation;
	SmartPropertyPointer<ROdouble> m_pobservedRightAscension;
	SmartPropertyPointer<ROdouble> m_pobservedDeclination;
	SmartPropertyPointer<ROdouble> m_pobservedGalLongitude;
	SmartPropertyPointer<ROdouble> m_pobservedGalLatitude;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Antenna::TGeneratorType),
	  POA_Antenna::ROTGeneratorType> > m_pgeneratorType;
	SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),
	  POA_Management::ROTSystemStatus> > m_pstatus;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),
	  POA_Management::ROTBoolean>  > m_penabled;
	SmartPropertyPointer<ROdouble> m_ppointingAzimuthCorrection;
	SmartPropertyPointer<ROdouble> m_ppointingElevationCorrection;
	SmartPropertyPointer<ROdouble> m_prefractionCorrection;
	SmartPropertyPointer<ROdouble> m_pFWHM;
	SmartPropertyPointer<ROdouble> m_pwaveLength;
	SmartPropertyPointer<ROdouble> m_ptargetFlux;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TBoolean),
	  POA_Management::ROTBoolean>  > m_pcorrectionEnabled;
	SmartPropertyPointer<ROdouble> m_ptargetRightAscension;
	SmartPropertyPointer<ROdouble> m_ptargetDeclination;
	SmartPropertyPointer<ROdouble> m_ptargetVrad;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Antenna::TReferenceFrame), POA_Antenna::ROTReferenceFrame> > m_pvradReferenceFrame;
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Antenna::TVradDefinition), POA_Antenna::ROTVradDefinition> > m_pvradDefinition;
	SmartPropertyPointer<ROdouble> m_pazimuthOffset;
	SmartPropertyPointer<ROdouble> m_pelevationOffset;
	SmartPropertyPointer<ROdouble> m_prightAscensionOffset;
	SmartPropertyPointer<ROdouble> m_pdeclinationOffset;
	SmartPropertyPointer<ROdouble> m_plongitudeOffset;
	SmartPropertyPointer<ROdouble> m_platitudeOffset;
	IRA::CSecureArea<CBossCore> *m_core;
	CWorkingThread *m_workingThread;
	CWatchingThread *m_watchingThread;
	CConfiguration  m_config;
	SimpleParser::CParser<CBossCore> *m_parser;
};

#endif /*!_H*/

