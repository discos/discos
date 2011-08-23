#ifndef __SRT7GHZIMPL_H_
#define __SRT7GHZIMPL_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public License (GPL).                                                          */
/*                                                                                                               */
/* Who                                when                     What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 02/08/2011       		   Creation                                          */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciROdoubleSeq.h>
#include <baciROlongSeq.h>
#include <baciROstring.h>
#include <baciROlong.h>
#include <baciROdouble.h>
#include <enumpropROImpl.h>
#include <SP_parser.h>
#include <IRA>
#include <SRT7GHzS.h>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>
#include "ComponentCore.h"
#include "MonitorThread.h"


/** 
 * @mainpage 7GHz receiver component Implementation
 * @date 23/08/2011
 * @version 1.0.0
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>
 * @remarks Last compiled under ACS 8.0.2
 * @remarks compiler version is 3.4.6
 * @remarks the case there are connection problems with the control boards it is not very satisfactory and should be reviewed even if this implies a review as the receiverControl library is designed..
 *                      At the moment the component does not react when a connection error is detected it just takes note of the event (changing the status word).  The component should be allowed to
 *                      close the connection, delete the object and recreate it, but at the moment a lock is taken inside the library and the componeNT has no way to check is there sone one using the library at the moment,
 *                      so a delete could lead to a segmentation-fault. The fact a ReceiverControl is protected by a inner lock instead of the component mutex iis due to the fact that some operation of the library
 *                      could take much time and this could have caused critical delays on the component.
 * @todo in the taper computation the correct numbers for SRT telescope must be inserted, moreover we have to clarify if the taper lookup table is a property of the receiver or
 *              it is a property of the focus.
 *              The code to drive the synthesizer must be completed
 *              Ask Buttu to move the FetValue enum and structure into the ReceiverControl class definition
 *              environmentTemperature must be implemented yet because the corresponding call in ReceiverControl class is missing, ask Buttu
 *              vacuumPumpFault and EXT_MARK_ENABLE are missing as well, ask Buttu
 *              Add in the Receivers interface the attribute componentStatus: Management::TSystemStatus that is a summary of the component status: Error,Warning, ok
 *
*/

/**
 * @author <a href=mailto:a.orlati@ira.inaf.it>Orlati Andrea</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class SRT7GHzImpl: public baci::CharacteristicComponentImpl,
				   public virtual POA_Receivers::SRT7GHz
{
public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	SRT7GHzImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~SRT7GHzImpl();

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
	 * It must be called to switch the receiver to operative mode. wnen called the default configuration and mode is loaded. Regarding this
	 * implementation calling this method corresponds to a call to <i>setMode("NORMAL")</i>.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	 virtual void activate() throw (ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
		
	/**
	 * This method is used to turn the calibration diode on.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	void calOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * This method is used to turn the calibration diode off.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	void calOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
	
	/**
	 * This method allows to set local oscillator. In this implementation only the first value is considered.
	 * @param lo the list contains the values in MHz for the local oscillator
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	void setLO(const ACS::doubleSeq& lo) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
	
	/**
	 * This method allows to set the operating mode of the receiver. In that implementation the receiver does not have special modes so a call to this method
	 * will lead to Configuration exception.
	 * @param mode string identifier of the operating mode
	 * @throw CORBA::SystemException
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 */
	void setMode(const char * mode) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
	
	/**
	 * This method is called when the values of the calibration mark of the receiver are required. A value is returned for every provided sub bands.
	 * The sub bands are defined by giving the feed number, the polarization, the initial frequency and the bandwidth.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 * @param freqs for each sub band this is the list of the starting frequencies (in MHz). The value is compared and adjusted to the the real
	 *               initial frequency of the receiver.
	 * @param bandwidths for each sub band this is the width in MHz. The value is compared and adjusted to the the real
	 *               band width of the receiver.
	 * @param feeds for each sub band this if the feed number. In that case zero is the only allowed value.
	 * @param ifs for each sub band this indicates the proper IF
	 * @return the list of the noise calibration value in Kelvin degrees.
	 */
    virtual ACS::doubleSeq * getCalibrationMark (const ACS::doubleSeq& freqs, const ACS::doubleSeq& bandwidths, const ACS::longSeq& feeds, const ACS::longSeq& ifs) throw (
    		CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * This method is called in order to know the geometry of the receiver. The geometry is given along the X and Y axis where the central feed is the origin
	 * the axis. The relative power (normalized to one) with respect to the central feed is also given.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 * @param X the positions relative to the central beam of the feeds along the X axis (radians)
	 * @param Y the positions relative to the central beam of the feeds along the Y axis (radians) 
	 * @param power the relative power of the feeds
	 * @return the number of feeds
	 */    
    virtual CORBA::Long getFeeds(ACS::doubleSeq_out X,ACS::doubleSeq_out Y,ACS::doubleSeq_out power) throw (CORBA::SystemException,
    		ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
    
	/**
	 * This method is called in order to know the taper of the receiver.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 * @param freq starting frequency of the detector in MHz. The value is compared and adjusted to the the real initial frequency of the receiver.
	 * @param bandWidth bandwidth of the detector n MHz. The value is compared and adjusted to the the real band width of the receiver.
	 * @param feed feed id the detector is attached to
	 * @param ifNumber Number of the IF, given the feed
	 * @param waveLen corresponding wave length in meters
	 * @return the value of the taper in db
	 */        
    virtual CORBA::Double getTaper(CORBA::Double freq,CORBA::Double bandWidth,CORBA::Long feed,CORBA::Long ifNumber,CORBA::Double_out waveLen) throw (
    		CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);
    
	/**
	 * This method is called in order to turn the LNA On.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
    virtual void turnLNAsOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * This method is called in order to turn the LNA Off.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
    virtual void turnLNAsOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

    /**
     * it turns the vacuum sensor on
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnVacuumSensorOn() throw  (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

    /**
     * it turns the vacuum sensor on
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnVacuumSensorOff() throw  (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

	/**
	 * Returns a reference to the mode property implementation of the IDL interface.
	 * @return pointer to read-only string property
	*/
    virtual ACS::ROstring_ptr mode() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the LO property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property
	*/	 
	virtual ACS::ROdoubleSeq_ptr LO() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the feeds property implementation of the IDL interface.
	 * @return pointer to read-only long property
	*/	 
	virtual ACS::ROlong_ptr feeds() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the IFs property implementation of the IDL interface.
	 * @return pointer to read-only long property
	*/	 	
	virtual ACS::ROlong_ptr IFs() throw (CORBA::SystemException);
	
	/**
	 * Returns a reference to the initialFrequency property implementation of the IDL interface.
	 * @return pointer to read-only doubleSeq property
	*/	 	
	virtual ACS::ROdoubleSeq_ptr initialFrequency() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the bandWidth property implementation of the IDL interface.
	 * @return pointer to read-only doubleSeq property
	*/	 	
	virtual ACS::ROdoubleSeq_ptr bandWidth() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the polarization property implementation of the IDL interface.
	 * @return pointer to read-only long sequence property
	*/	 
	virtual ACS::ROlongSeq_ptr polarization() throw (CORBA::SystemException);	
	
	/**
     * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only pattern property
	*/
	virtual ACS::ROpattern_ptr status() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the vacuum property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr vacuum() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the Vd_1 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr Vd_1() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the Vd_2 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr Vd_2() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the Id_1 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr Id_1() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the Id_2 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr Id_2() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the Vg_1 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr Vg_1() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the Vg_2 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr Vg_2() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the cryoTemperatureCoolHead property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr cryoTemperatureCoolHead() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the cryoTemperatureCoolHeadWindow property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr cryoTemperatureCoolHeadWindow() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the cryoTemperatureLNA property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr cryoTemperatureLNA() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the cryoTemperatureLNAWindow property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr cryoTemperatureLNAWindow() throw (CORBA::SystemException);

	/**
	 * Returns a reference to the environmentTemperature property implementation of the IDL interface.
	 * @return pointer to read-only double property
	*/
    virtual ACS::ROdouble_ptr environmentTemperature() throw (CORBA::SystemException);

	/**
     * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only ROTSystemStatus property status
	*/
	virtual Management::ROTSystemStatus_ptr receiverStatus() throw (CORBA::SystemException);

private:
	baci::SmartPropertyPointer<baci::ROdoubleSeq> m_plocalOscillator;
	baci::SmartPropertyPointer<baci::ROlong> m_pfeeds;
	baci::SmartPropertyPointer<baci::ROlong> m_pIFs;
	baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pinitialFrequency;
	baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pbandWidth;
	baci::SmartPropertyPointer<baci::ROlongSeq> m_ppolarization;
	baci::SmartPropertyPointer<baci::ROpattern> m_pstatus;
	baci::SmartPropertyPointer<baci::ROdouble> m_pvacuum;
	baci::SmartPropertyPointer<baci::ROdouble> m_pVd_1;
	baci::SmartPropertyPointer<baci::ROdouble> m_pVd_2;
	baci::SmartPropertyPointer<baci::ROdouble> m_pId_1;
	baci::SmartPropertyPointer<baci::ROdouble> m_pId_2;
	baci::SmartPropertyPointer<baci::ROdouble> m_pVg_1;
	baci::SmartPropertyPointer<baci::ROdouble> m_pVg_2;
	baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureCoolHead;
	baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureCoolHeadWindow;
	baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureLNA;
	baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureLNAWindow;
	baci::SmartPropertyPointer<baci::ROdouble> m_penvironmentTemperature;
	baci::SmartPropertyPointer<baci::ROstring> m_pmode;
	baci::SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), POA_Management::ROTSystemStatus> > m_preceiverStatus;

	CComponentCore m_core;
	CMonitorThread *m_monitor;
};



#endif
