#ifndef SRTACUIMPL_H
#define SRTACUIMPL_H

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: SRTMountImpl.h,v 1.7 2011-06-01 18:24:44 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                when            What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  23/11/2010      Creation                                                  */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROdouble.h>
#include <baciROlong.h>
#include <baciROuLongLong.h>
#include <enumpropROImpl.h>
#include <baciROdoubleSeq.h>
#include <baciROlongSeq.h>
#include <baciROpattern.h>
#include <SRTMountS.h>
#include "CommandSocket.h"
#include "StatusSocket.h"
#include "CommonData.h"
#include "WatchDog.h"

using namespace baci;

/** 
 * @mainpage SRT ACU component documentation
 * @date 23/11/2010
 * @version 0.1.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 7.0.2
 * @remarks gcc version 4.1.2
*/

/**
 * This class implements the Antenna::SRTMount CORBA interface and the ACS CharacteristicComponent.  
 * All exception that comes from the run-time interation with clients are at logged with LM_DEBUG priority.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class SRTMountImpl: public CharacteristicComponentImpl,
				    public virtual POA_Antenna::SRTMount,
				    public ActionImplementator
{

public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	SRTMountImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~SRTMountImpl(); 

	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters, builds up connection. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	 * 		  @arg \c ComponentErrors::MemoryAllocation
	*/
	virtual void initialize() throw (ACSErr::ACSbaseExImpl);

	/**
 	 * Called after <i>initialize()</i> to tell the component that it has to be ready to accept incoming functional calls any time. 
	 * Must be implemented as a synchronous (blocking) call. In this class the default implementation only logs the COMPSTATE_OPERATIONAL
	 * @throw ACSErr::ACSbaseExImpl
	 *    @arg \c ComponentErrors::CDBAccess
	 *    @arg \c ComponentErrors::SocketError
	*/
	virtual void execute() throw (ACSErr::ACSbaseExImpl);
	
	/** 
	 * Called by the container before destroying the server in a normal situation. This function takes charge of releasing all resources.
	*/
	virtual void cleanUp();
	
	/** 
	 * Called by the container in case of error or emergency situation. This function tries to free all resources even though there is no
	 * warranty that the function is completely executed before the component is destroyed.
	*/	
	virtual void aboutToAbort();
	
	/**
     * Returns a reference to the azimuth property Implementation of IDL interface.
	 * @return pointer to read-only double property azimuth
	*/
	virtual ACS::ROdouble_ptr azimuth() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the elevation property Implementation of IDL interface.
	 * @return pointer to read-only double property elevation
	*/
	virtual ACS::ROdouble_ptr elevation() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the azimuthError property implementation of IDL interface.
	 * @return pointer to read-only double property azimuthError
	*/
	virtual ACS::ROdouble_ptr azimuthError() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the elevationError property implementation of IDL interface.
	 * @return pointer to read-only double property elevationError
	*/
	virtual ACS::ROdouble_ptr elevationError() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the azMode property implementation of IDL interface.
	 * @return pointer to read-only TModes property azimuthMode
	*/
	virtual Antenna::ROTCommonModes_ptr azimuthMode() throw (CORBA::SystemException);
	
	/**
     * Returns a reference to the elMode property implementation of IDL interface.
	 * @return pointer to read-only TModes property elevationMode
	*/
	virtual Antenna::ROTCommonModes_ptr elevationMode() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the time property implementation of IDL interface.
	 * @return pointer to read-only uLongLong property time
	*/	
	virtual ACS::ROuLongLong_ptr time() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the status line status property implementation of IDL interface.
	 * @return pointer to read-only TStatus property status
	*/	
	virtual Antenna::ROTStatus_ptr statusLineStatus() throw (CORBA::SystemException);

	/** 
	 * Returns a reference to the control line status property implementation of IDL interface.
	 * @return pointer to read-only TStatus property status
	*/	
	virtual Antenna::ROTStatus_ptr controlLineStatus() throw (CORBA::SystemException);

	/** 
	 * Returns a reference to the IDL implementation of the program track positions property.
	 * @return pointer to read-only pattern property programTrackPositions
	*/	
	virtual ACS::ROlong_ptr programTrackPositions() throw (CORBA::SystemException);

	/** 
	 * Returns a reference to the section property implementation of IDL interface.
	 * @return pointer to read-only TSection property section
	*/	
	virtual Antenna::ROTSections_ptr section() throw (CORBA::SystemException);
		
	/**
    * Returns a reference to the azimuth rate property implementation of IDL interface. This property is
	 * not read from the ACU but is derived by computing the antenna velocity between two consecutive position.
	 * @return pointer to read-only double property azimuthRate
	*/
	virtual ACS::ROdouble_ptr azimuthRate() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the elevation rate property implementation of IDL interface. This property is
	 * not read from the ACU but is derived by computing the antenna velocity between two consecutive position.
	 * @return pointer to read-only double property elevationRate
	*/
	virtual ACS::ROdouble_ptr elevationRate() throw (CORBA::SystemException);
	
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
	 * Returns a reference to the deltaTime property implementation of IDL interface. This property is a fake because
	 * the Medicina Antenna Control Unit does not support this feature.
	 * @return pointer to read-only double property deltaTime.
	*/
	virtual ACS::ROdouble_ptr deltaTime() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the commandedAzimuth property implementation of IDL interface.
	 * @return pointer to read-only double property commandedAzimuth.
	*/
	virtual ACS::ROdouble_ptr commandedAzimuth() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the commandedElevation property implementation of IDL interface.
	 * @return pointer to read-only double property commandedElevation.
	*/
	virtual ACS::ROdouble_ptr commandedElevation() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the mountStatus property implementation of IDL interface.
	 * @return pointer to read-only TMountStatus property mountStatus
	*/
	virtual Management::ROTSystemStatus_ptr mountStatus() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the commandedAzimuthRate property implementation of IDL interface.
	 * @return pointer to read-only double property commandedAzimuthRate
	*/	
	virtual ACS::ROdouble_ptr commandedAzimuthRate() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the commandedElevationRate property implementation of IDL interface.
	 * @return pointer to read-only double property commandedElevationRate
	*/	
	virtual ACS::ROdouble_ptr commandedElevationRate() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the elevationOffset property implementation of IDL interface.
	 * @return pointer to read-only double property elevationOffset
	*/	
	virtual ACS::ROdouble_ptr commandedAzimuthOffset() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the commandedElevationOffset property implementation of IDL interface.
	 * @return pointer to read-only double property commandedElevationOffset
	*/	
	virtual ACS::ROdouble_ptr commandedElevationOffset() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the cableWrapperPosition property implementation of IDL interface.
	 * @return pointer to read-only double property cableWrapperPosition
	*/	
	virtual ACS::ROdouble_ptr cableWrapPosition() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the cableWrapTrackingError property implementation of IDL interface.
	 * @return pointer to read-only double property cableWrapTrackingError
	*/	
	virtual ACS::ROdouble_ptr cableWrapTrackingError() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the cableWrapperRate property implementation of IDL interface.
	 * @return pointer to read-only double property cableWrapperRate
	*/	
	virtual ACS::ROdouble_ptr cableWrapRate() throw (CORBA::SystemException);		
	
	/** 
	 * Returns a reference to the azimuthTrackingError property implementation of IDL interface.
	 * @return pointer to read-only double property azimuthTrackingError
	*/
	virtual ACS::ROdouble_ptr azimuthTrackingError() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the elevationTrackingError property implementation of IDL interface.
	 * @return pointer to read-only double property elevationTrackingError
	*/
	virtual ACS::ROdouble_ptr elevationTrackingError() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the generalStatus property implementation of IDL interface.
	 * @return pointer to read-only pattern property generalStatus
	*/
	virtual ACS::ROpattern_ptr generalStatus() throw (CORBA::SystemException);

	/** 
	 * Returns a reference to the azimuthStatus property implementation of IDL interface.
	 * @return pointer to read-only pattern property azimuthStatus
	*/
	virtual ACS::ROpattern_ptr azimuthStatus() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the elevationStatus property implementation of IDL interface.
	 * @return pointer to read-only pattern property elevationStatus
	*/
	virtual ACS::ROpattern_ptr elevationStatus() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the motorsPosition property implementation of IDL interface.
	 * @return pointer to read-only double sequence property motorsPosition
	*/
	virtual ACS::ROdoubleSeq_ptr motorsPosition() throw (CORBA::SystemException);

	/** 
	 * Returns a reference to the motorsSpeed property implementation of IDL interface.
	 * @return pointer to read-only double sequence property motorsSpeed
	*/
	virtual ACS::ROdoubleSeq_ptr motorsSpeed() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the motorsTorque property implementation of IDL interface.
	 * @return pointer to read-only double sequence property motorsTorque
	*/	
	virtual ACS::ROdoubleSeq_ptr motorsTorque() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the motorsUtilization property implementation of IDL interface.
	 * @return pointer to read-only double sequence property motorsUtilization
	*/		
	virtual ACS::ROdoubleSeq_ptr motorsUtilization() throw (CORBA::SystemException);
	
	/** 
	 * Returns a reference to the motorsStatus property implementation of IDL interface.
	 * @return pointer to read-only long sequence property motorsStatus
	*/	
	virtual ACS::ROlongSeq_ptr motorsStatus() throw (CORBA::SystemException);
	
	/**
	* Action dispatcher function. This function is called whenever an asynchronous request has to be handled. This function is inherited from ActionImplementator
	* Interface.
	* @param function Action funtion to be invoked.
	* @param cob owner of the action.
	* @param callbackID ID of the callback to be notified.
	* @param descIn callback descriptor (passed by client).
	* @param value action data (e.g. value to be set).
	* @param completion error handing structure.
	* @param descOut callback descriptor which will be passed to client
	* @return the request to be performed by BACI
	*      @arg \c reqNone - Do nothing (action will be kept in queue).
	*	   @arg \c reqInvokeWorking - Invoke Callback::working.
	*      @arg \c reqInvokeDone - Invoke Callback::done and destroy callback.
	*	   @arg \c reqDestroy - Destroy callback (callback should have been called already by function).
	*/
	ActionRequest invokeAction(int function,BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,BACIValue *value,Completion& completion,CBDescOut& descOut);
	
	/**
	 * Implementation of asynchronous Mount::unstow() method. This is the function that actually runs the unstow action and, 
 	 * when completed, invokes the callback installed by the client when it requested the action.
 	 * @param cob owner of the action.
 	 * @param callbackID ID of the callback to be notified.
 	 * @param descIn callback descriptor (passed by client).
 	 * @param value_p action data (e.g. value to be set).
 	 * @param completion error handing structure.
 	 *		@arg \c ComponentErrors::NoErrorCompletion
	 *		@arg \c AntennaErrors::StoppedByUser
 	 * @param descOut Callback descriptor which will be passed to client.
 	 * @return the request to be performed by BACI
 	 * 		@arg \c reqNone - Do nothing (action will be kept in queue).
 	 *		@arg \c reqInvokeWorking - Invoke Callback::working.
 	 *		@arg \c reqInvokeDone - Invoke Callback::done and destroy callback.
 	 *		@arg \c reqDestroy - Destroy callback (callback should have been called already by function).
 	 */
	ActionRequest unstowAction(BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,BACIValue* value_p,Completion& completion,CBDescOut& descOut);
	
	/**
	 * Implementation of asynchronous Mount::stow() method. This is the function that actually runs the unstow action and, 
 	 * when completed, invokes the callback installed by the client when it requested the action.
 	 * @param cob owner of the action.
 	 * @param callbackID ID of the callback to be notified.
 	 * @param descIn callback descriptor (passed by client).
 	 * @param value_p action data (e.g. value to be set).
 	 * @param completion error handling structure.
	 *		@arg \c ComponentErrors::NoErrorCompletion
	 *		@arg \c AntennaErrors::StoppedByUser
 	 * @param descOut Callback descriptor which will be passed to client.
 	 * @return the request to be performed by BACI
 	 * 		@arg \c reqNone - Do nothing (action will be kept in queue).
 	 *		@arg \c reqInvokeWorking - Invoke Callback::working.
 	 *		@arg \c reqInvokeDone - Invoke Callback::done and destroy callback.
 	 *		@arg \c reqDestroy - Destroy callback (callback should have been called already by function).
 	 */
	ActionRequest stowAction(BACIComponent* cob,const int& callbackID,const CBDescIn& descIn,BACIValue* value_p,Completion& completion,CBDescOut& descOut);

	/** 
	 * This action will subtract the antenna stow pin. This method just registers the request in the asyncronous queue, together with the associated 
	 * callback and returns control immediatly. The actual action will be invoked asyncronously by the asynchonous call manager by calling <i>unstowAtion()</i>. 
	 * The given callback is used to inform the caller when the action is performed. Since this operation can take long time the antenna is put in busy state.
 	 * @param cb Callback when action has finished.
 	 * @param desc Callback used for holding information on timeout periods.
 	*/
	void unstow(ACS::CBvoid_ptr cb,const ACS::CBDescIn &desc) throw (CORBA::SystemException);

	/** This action will insert the antenna stow pin and put the antenna in survival position. This method just registers the request in the asyncronous queue, 
	 * together with the associated callback and returns control immediatly. The actual action will be invoked asyncronously by the asynchonous call manager 
	 * by calling <i>stowAtion()</i>. The given callback is used to inform the caller when the action is performed. Since this operation can take long time
	 * the antenna is put in busy state.
 	 * @param cb Callback when action has finished.
 	 * @param desc Callback used for holding information on timeout periods.
 	*/
	void stow(ACS::CBvoid_ptr cb,const ACS::CBDescIn &desc) throw (CORBA::SystemException);
	
	/**
	 * This method implements the stop command. When this command is issued all pending operations (long jobs) are interrupted, the antenna status
	 * is restored to ready. Then, both axes are put in <i>Antenna::ACU_STOP</i> mode.
	 * @throw ComponentErrors::ComponentErrorsEx
	 *       @arg \c ComponentErrors::Timeout
	 *       @arg \c ComponentErrors::SocketError
	 * @throw AntennaErrors::AntennaErrorsEx
	 * 		 @arg \c AntennaErrors::Nak
	 *       @arg \c AntennaErrors::Connection
	*/
	void stop() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx);
	
	/** 
	 * It allows to presets a new position for the antenna provided that the current mode is the <i>ACU_PRESET</i> and the <i> controlLineStatus</i> is not <i>ACU_BUSY</i>.
	 * The position coordinates are given for azimuth (0..360) and elevation (hardware limit of the mount). 
	 * The sector used to reach the commanded position will be decided following the shortest path policy or according the setting done by the previous call to
	 * <i>forceSection()</i>..
	 * @throw ComponentErrors::ComponentErrorsEx
	 *    @arg \c ComponentErrors::Timeout
	 * 	  @arg \c ComponentErrors::ValueOutofRange
	 *    @arg \c ComponentErrors::PropertyError
	 *    @arg \c ComponentErrors::SocketError
	 * @throw AntennaErrors::AntennaErrorsEx 
	 * 	  @arg \c AntennaErros::Nak
	 *    @arg \c AntennnaErros::Connection
	 *    @arg \c AntennaErrors::AntennaBusy	 	 	 
	      @arg \c AntennaErrors::OperationNotPermitted
	 * @param az position azimuth (degree)
	 * @param el position elevation (degree)
   */	
	void preset(CORBA::Double az,CORBA::Double el)  throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx);
	  
	/** 
	 * It allows to apply new rates into the ACU provided that the current mode is the <i>ACU_RATE</i>. The commanded velocites are
	 * checked to be beyond the hardware limits otherwise an exception is thrown. The <i>controlLineStatus</i> must not be <i>ACU_BUSY</i>.
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 		 @arg \c ComponentErrors::Timeout
	 * 		 @arg \c ComponentErrors::ValueOutofRange
	 *       @arg \c ComponentErrors::SocketError
	 *       @arg \c ComponentErrors::PropertyError
	 * @throw AntennaErrors::AntennaErrorsEx
	 * 		 @arg \c AntennaErros::Nak
	 *       @arg \c AntennnaErros::Connection
	 *  	 @arg \c AntennaErrors::AntennaBusy	       
	 * @param azRate azimuth rates (degrees per seconds)
	 * @param elRate elevation rates (degree per seconds)
   */	
	void rates(CORBA::Double azRate,CORBA::Double elRate)
	  throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx);
	  
	/**
	 * This method is used to add a new position to the ACU tracking curve if the current mode is the <i>ACU_PROGRAMTRACK</i>. 
	 * When one or more position are loaded the ACU will begin to interpolate a  spline around them in order to better fit
	 * the tracking curve. A far better performace will be achived if a least four points are supllied in advance. If a point in the 
	 * past (smaller time value than the last one) arrives it is neglected; on the other hand if a sample with a time tag in the future arrives
	 * the position of the last sample is kept. The sample must be spaced at minimum by 0.05 seconds; the lenght of the position stack is
	 * at maximum 2500. Any position is checked for range limits before they can be transfered to the ACU: elvation is always kept inside
	 * the hardware limits (configured in the CDB), whilst azimuth must always be inside the range 0..360 for computations and the all the range (as reported in the CDB) is exploited. 
	 * The sector used to reach the commanded point will be decided following the shortest path policy.  
 	 * @throw ComponentErrors::ComponentErrorsEx
	 * 		 @arg \c ComponentErrors::Timeout
	 * 		 @arg \c ComponentErrors::ValueOutofRange
	 *       @arg \c ComponentErrors::SocketError
	 * 		 @arg \c ComponentErrors::PropertyError
	 * @thorw AntennaErrors::AntennaErrorsEx
	 *       @arg \c AntennnaErros::Connection
	 * 		 @arg \c AntennaErrors::AntennaBusy	 	 	 
	 * 		 @srg \c AntennaErrors::OperationNotPermitted
	 * 		 @arg \c AntennaErros::Nak 	 
	 * @param az azimuth position of the sample loaded into the ACU (degrees, 0..360)
	 * @param el elevation position of the sample loaded into the ACU (degrees, elMin..elMax)
	 * @param time tag of time (number of 100 ns ticks since 1582-10-15 00:00:00). The ACU will try to reach the sky position a the right time.
	 * @param restart if true the component will flush the positions stack before loading the new position.  
	*/  
	void programTrack(CORBA::Double az,CORBA::Double el,ACS::Time time,CORBA::Boolean restart) 
	  throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx);
	
	/** This changes the modes for both azimuth and elevation axes. Even if it allows to gives different modes for azimuth and elevation this
	 * cace will result in an error condition. If it suceeds, it also resets the offsets.
	 * @throw ComponentErrors::OperationError
	 * 		@arg \c ComponentErrors::Timeout
	 * 		@arg \c AntennnaErros::Nak	 
	 * 		@arg \c AntennnaErros::Connection
	 * 	    @arg \c AntennnaErros::AntennaBusy	 	 	 
	 * 		@arg \c ComponentErrors::SocketError
	 * @throw ComponentErrors::ValidationError
	 * 		@arg \c ComponentErrors::OperationNotPermitted
	 * @throw AntennaErrors::AntennaErrorsEx
	 * @param azMode This the mode that should have to be commanded for the azimuth axis
	 * @param elMode This the mode that should have to be commanded for the elevation axis
	*/
	void changeMode(Antenna::TCommonModes azMode,Antenna::TCommonModes elMode) 
	  throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx,AntennaErrors::AntennaErrorsEx);

	/**
	 * This will set the ACU time according to the argument. 
 	 * @throw ComponentErrors::ComponentErrorsEx
	 * 		@arg \c ComponentErrors::Timeout
	 * 		@arg \c ComponentErrors::Socket</a>
	 * @throw AntennaErrors::AntennaErrorsEx
	 * 		@arg \c AntennaErrors::Nak	 
	 * 		@arg \c AntennaErrors::Connection  	 
	 * @param now The ACU time will be set to this value, in 100 ns units since 1582-10-15 00:00:00.
	*/
	void setTime(ACS::Time now) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx);
	
	/* 
	 * This will set the position offsets to mount. The current commanded offsets
	 * can be read from the <i>commandedAzimuthOffset</i> and <i>commandedElevationOffset</i> properties.
	 * @thorw ComponentErrors::ComponentErrorsEx
	 * 	   @arg \c ComponentErrors::Timeout	 
	 *     @arg \c ComponentErrors::SocketError
	 * 	   @arg \c ComponentErrors::PropertyError
	 *     @arg \c ComponentErrors::ValidationError
	 * @thorw AntennaErrors::AntennaErrorsEx
	 * 	   @arg \c AntennaErrors::Connection      
	 * 	   @arg \c AntennaErrors::OperationNotPermitted
	 * @param azOff position offset to be added to azimuth coordinates (degrees)
	 * @param elOff position offset to be added to elevation coordinates (degrees)
	 */
	void setOffsets(CORBA::Double azOff,CORBA::Double elOff) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx);
	
	/**
	 * This sets the time offset. This method is a fake because Medicina ACU does not have such a feature. 
	 * In practice it does nothing and it is here to mantain compatibility with the component interface.
	 * @param delta new time offsets in milliseconds.
	 * @thorw ComponentErrors::ComponentErrorsEx
	 * @thorw AntennaErrors::AntennaErrorsEx
	*/ 
	void setDeltaTime(CORBA::Double delta) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx);

	/**
	 * This method gets all encoders coordinates and offsets from the mount in one time.
	 * @param time this field returns exactly the time the data was obtained from the mount
	 * @param azimuth the value read from the azimuth encoder in degrees (offset included)
	 * @param elevation  the value read from the elevation encoder in degrees (offset included)
	 * @param azOffset the instant offset applied to the azimuth
	 * @param elOffset the instant offset applied to the elevation
	 * @param section returns the section in whihc the mount is at present.
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 		@arg \c ComponentErrors::SocketErrorExImpl
	 * 		@arg \c ComponentErrors::TimeoutExImpl
	 * @throw AntennaErrors::AntennaErrorsEx
	 * 		@arg \c AntennaErrors::ConnectionExImpl 
	 */
	void getEncoderCoordinates(ACS::Time_out time,CORBA::Double_out azimuth,CORBA::Double_out elevation,
	  CORBA::Double_out azOffset,CORBA::Double_out elOffset,Antenna::TSections_out section) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx);

	/**
	 * This method can be used to get the tracking errors of the antenna in a single shot, without accessing the attributes <i>azimuthError</i>
	 * and <i>elevationError</i> in separated times.
	 * @param time this field returns exactly the time the data was obtained from the mount
	 * @param azError error in azimuth in degrees
	 * @param elError error iin elevation in degrees
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 		@arg \c ComponentErrors::SocketErrorExImpl
	 * 		@arg \c ComponentErrors::TimeoutExImpl
	 * @throw AntennnaErrors::AntennaErrorsEx
	 * 		@arg \c AntennaErrors::ConnectionExImpl 
	 */
	void getAntennaErrors(ACS::Time_out time,CORBA::Double_out azError,CORBA::Double_out elError) throw (
			CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx);

	/**
	 * This method ensures that the next commanded point will be reached in the given section of the azimuth range. If the point is not reachable from the requested section
	 * this command is bypassed and the tracking point is commanded anyway.
	 * @param section It could be <i>ACU_CW</i>,<i>ACU_CCW</i> or <i>ACU_NEUTRAL</i>. In the latter case the commanded point
	 * will be reached by the mount following the shortest route. 
	 */
	void forceSection(Antenna::TSections section) throw (CORBA::SystemException);
	
	/**
	 * This method computes the real azimuth coordinate that could be loaded into the ACU with respect the harware limits, the present
	 * antenna position and the preferred section.
	 * @param destination the 0..360 degrees azimuth coordinate.
	 * @param section preferred section
	 * @return hardware contraints maps the destination into this number.
	 */
	CORBA::Double getHWAzimuth(CORBA::Double destination,Antenna::TSections section) throw(CORBA::SystemException);
	
	/**
	 * This methods resets/acknowledges all active errors
	 * @throw ComponentErrors::ComponentErrorsEx
	 * 		@arg \c ComponentErrors::Timeout
	 * 		@arg \c ComponentErrors::Socket</a>
	 * @throw AntennaErrors::AntennaErrorsEx
	 * 		@arg \c AntennaErrors::Nak	 
	 * 		@arg \c AntennaErrors::Connection 
	 */
	void reset() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, AntennaErrors::AntennaErrorsEx);
	
private:
	
	/** Component shared store area */
	IRA::CSecureArea<CCommonData> m_commonData;
	/** Elevation position property */
	SmartPropertyPointer<ROdouble> m_pelevation;
	/** Azimuth position property */
	SmartPropertyPointer<ROdouble> m_pazimuth;
	/** Elevation error property */
	SmartPropertyPointer<ROdouble> m_pelevationError;
	/** Azimuth error property */
	SmartPropertyPointer<ROdouble> m_pazimuthError;
	/** Azimuth mode property */
	SmartPropertyPointer<ROEnumImpl< ACS_ENUM_T(Antenna::TCommonModes),POA_Antenna::ROTCommonModes > > m_pazMode;
	/** Elevation mode property */
	SmartPropertyPointer<ROEnumImpl< ACS_ENUM_T(Antenna::TCommonModes),POA_Antenna::ROTCommonModes > > m_pelMode;
	/** Time property */	
	SmartPropertyPointer<ROuLongLong> m_ptime;
	/** control line status property */
	SmartPropertyPointer< ROEnumImpl< ACS_ENUM_T(Antenna::TStatus),POA_Antenna::ROTStatus > > m_pcontrolLineStatus;
	/** status line status property */
	SmartPropertyPointer< ROEnumImpl< ACS_ENUM_T(Antenna::TStatus),POA_Antenna::ROTStatus > > m_pstatusLineStatus;
	/** Free Program track positions */
	SmartPropertyPointer<ROlong> m_pprogramTrackPositions;	
	/** section property */
	SmartPropertyPointer<ROEnumImpl<ACS_ENUM_T(Antenna::TSections),POA_Antenna::ROTSections> > m_psection;	
	/** Elevation rate property */
	SmartPropertyPointer<ROdouble> m_pelevationRate;
	/** Azimuth rate property */
	SmartPropertyPointer<ROdouble> m_pazimuthRate;	
	/** Azimuth offset */ 
	SmartPropertyPointer<ROdouble> m_pazimuthOffset;
	/** Elevation offset */	
	SmartPropertyPointer<ROdouble> m_pelevationOffset;
	/** Delta Time property */
	SmartPropertyPointer<ROdouble> m_pdeltaTime;
	/** Commanded azimuth  property */
	SmartPropertyPointer<ROdouble> m_pcommandedAzimuth;	
	/** Commanded elevation property */
	SmartPropertyPointer<ROdouble> m_pcommandedElevation;	
	/** Mount status property */
	SmartPropertyPointer< ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus),POA_Management::ROTSystemStatus> >m_pmountStatus; 
	SmartPropertyPointer<ROdouble> m_pcommandedAzimuthRate;
	SmartPropertyPointer<ROdouble> m_pcommandedElevationRate;
	SmartPropertyPointer<ROdouble> m_pcommandedAzimuthOffset;
	SmartPropertyPointer<ROdouble> m_pcommandedElevationOffset;
	SmartPropertyPointer<ROdouble> m_pcableWrapPosition;
	SmartPropertyPointer<ROdouble> m_pcableWrapTrackingError;
	SmartPropertyPointer<ROdouble> m_pcableWrapRate;
	/** Elevation tracking error property */
	SmartPropertyPointer<ROdouble> m_pelevationTrackingError;
	/** Azimuth tracking error property */
	SmartPropertyPointer<ROdouble> m_pazimuthTrackingError;

	SmartPropertyPointer<ROpattern> m_pgeneralStatus;
	SmartPropertyPointer<ROpattern> m_pazimuthStatus;
	SmartPropertyPointer<ROpattern> m_pelevationStatus;	
	SmartPropertyPointer<ROdoubleSeq> m_pmotorsPosition;
	SmartPropertyPointer<ROdoubleSeq> m_pmotorsSpeed;
	SmartPropertyPointer<ROdoubleSeq> m_pmotorsTorque;
	SmartPropertyPointer<ROdoubleSeq> m_pmotorsUtilization;
	SmartPropertyPointer<ROlongSeq> m_pmotorsStatus;
		
	/** Component configuration data */
	CConfiguration m_compConfiguration;
	/**
	 *  socket used to issue commands to the ACU
	 */
	CCommandSocket m_commandSocket;
	/**
	 * socket used to receive status message frames from the ACU
	 */
	CStatusSocket m_statusSocket;
	/**
	 * pointer to the watch dog thread
	 */
	CWatchDog *m_watchDog;
	
	/**
	 * Parameter data structure for the wath dog
	 */
	CWatchDog::TThreadParameter m_watchDogParam;	
};

#endif /*!_H*/
