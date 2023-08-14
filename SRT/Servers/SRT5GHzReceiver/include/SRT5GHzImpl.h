#ifndef __SRT5GHZIMPL_H_
#define __SRT5GHZIMPL_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public License (GPL).                                                          */
/*                                                                                                               */
/* Who                                when                     What                                              */
/* Andrea Orlati(andrea.orlati@inaf.it) 10/08/2023       		   Creation                                          */



#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#else
#define STRING2(X) #X
#define STRING(X) STRING2(X) 
#pragma message ("C++ version is " STRING(__cplusplus))
#endif

#include <Cplusplus11Helper.h>

#define _CPLUSPLUS11_PORTING_

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
C11_IGNORE_WARNING("-Wmisleading-indentation")
C11_IGNORE_WARNING("-Wcatch-value=")
#include <baciCharacteristicComponentImpl.h>
C11_IGNORE_WARNING_POP

#include <baciSmartPropertyPointer.h>
#include <baciROdoubleSeq.h>
#include <baciROlongSeq.h>
#include <baciROstring.h>
#include <baciROlong.h>
#include <baciROdouble.h>

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wsequence-point")
#include <enumpropROImpl.h>
C11_IGNORE_WARNING_POP

//C11_IGNORE_WARNING_PUSH
//C11_IGNORE_WARNING("-Wdeprecated-declarations")
#include <SP_parser.h>
//C11_IGNORE_WARNING_POP
#include <IRA>
#include <SRT5GHzS.h>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>
#include "ComponentCore.h"
#include "MonitorThread.h"

#undef _CPLUSPLUS11_PORTING_


/** 
 * @mainpage 5GHz receiver component Implementation
 * @date 10/08/2023
 * @version 1.0.0
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>
 * @remarks the case there are connection problems with the control boards it is not very satisfactory and should be reviewed 
 *          even if this implies a review as the receiverControl library is designed..
 *          At the moment the component does not react when a connection error is detected it just takes note of the event 
 *          (changing the status word).  The component should be allowed to close the connection, delete the object and recreate it, 
 *          but at the moment a lock is taken inside the library and the component has no way to check is there sone one using 
 *          the library at the moment, so a delete could lead to a segmentation-fault. The fact a ReceiverControl is protected 
 *          by a inner lock instead of the component mutex is due to the fact that some operation of the library
 *          could take much time and this could have caused critical delays on the component.
 *
*/

/**
 * @author <a href=mailto:andrea.orlati@inaf.it>Orlati Andrea</a>
 * Istituto di Radioastronomia, Italia
 * <br> 
 */
class SRT5GHzImpl: public baci::CharacteristicComponentImpl,
				   public virtual POA_Receivers::SRT5GHz
{
public:
	
	/** 
	* Constructor.
	* @param CompName component's name. This is also the name that will be used to find the configuration data for the component in the Configuration Database.
	* @param containerServices pointer to the class that exposes all services offered by container
	*/
	SRT5GHzImpl(const ACE_CString &CompName,maci::ContainerServices *containerServices);

	/**
	 * Destructor.
	*/
	virtual ~SRT5GHzImpl();

	/** 
	 * Called to give the component time to initialize itself. The component reads in configuration files/parameters or 
	 * builds up connection to devices or other components. 
	 * Called before execute. It is implemented as a synchronous (blocking) call.
	 * @throw ACSErr::ACSbaseExImpl
	*/
	virtual void initialize();

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
	 * It must be called to switch the receiver to operative mode. when called the default configuration and mode is loaded. Regarding this
	 * implementation calling this method corresponds to a call to <i>setMode("NORMAL")</i>.
     * @param setup_mode the setup mode (KKG, CCB, LLP, PLP, ecc.)
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	 virtual void activate(const char * setup_mode);

	/**
	 * It must be called to switch off the receiver.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	 virtual void deactivate();
		
	/**
	 * This method is used to turn the calibration diode on.
	 * @throw CORBA::SystemExcpetion
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	void calOn();

	/**
	 * This method is used to turn the calibration diode off.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	void calOff();
	

    /**
     * This method is used to turn the external calibration diode on.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void externalCalOn();

    /**
     * This method is used to turn the external calibration diode off.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void externalCalOff();

	/**
	 * This method allows to set local oscillator. In this implementation only the first value is considered.
	 * @param lo the list contains the values in MHz for the local oscillator
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	void setLO(const ACS::doubleSeq& lo);
	
	/**
	 * This method allows to set the operating mode of the receiver. In that implementation the receiver does not have special modes so a call to this method
	 * will lead to Configuration exception.
	 * @param mode string identifier of the operating mode
	 * @throw CORBA::SystemException
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 * @throw ComponentErrors::ComponentErrorsEx
	 */
	void setMode(const char * mode);
	
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
	 * @param skyFreq for each sub band it returns the real observed frequency(MHz), included detector, receiver IF  and Local Oscillator.
	 * @param skyBw for each sub band it returns the real observed bandwidth(MHz), included detector bandwidth , receiver IF bandwidth
	 * @param onoff true if the calibration diode is turned on.
	 * @param scaleFactor this is a value to be applied as scale factor during system temperature computation
	 * @return the list of the noise calibration value in Kelvin degrees.
	 */
    virtual ACS::doubleSeq * getCalibrationMark(const ACS::doubleSeq& freqs, const ACS::doubleSeq& bandwidths, const ACS::longSeq& feeds, const ACS::longSeq& ifs,
    		ACS::doubleSeq_out skyFreq,ACS::doubleSeq_out skyBw,CORBA::Boolean_out onoff,CORBA::Double_out scaleFactor);

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
    virtual CORBA::Long getFeeds(ACS::doubleSeq_out X,ACS::doubleSeq_out Y,ACS::doubleSeq_out power);
    

    /**
     * It is called to get the all the receiver output information in one call.
     * An output is identified by providing the feed and the IF identifier. It can process any number of requests at a time.
     * @param feeds is a list that stores the corresponding feed of the output we are asking for
     * @param ifs is a list that identifies which IFs of the feed we are interested in, usually 0..<i>IFs</i>-1
     * @param freq used to return the start frequency of the band provided by the output  the oscillator
     * (if present) is not  added (MHz)
     * @param bw used to return the total provided bandwidth. (MHz)
     * @param pols it specifies the polarization of the receiver output, since ACS does not support for enum
     * sequences the correct value must be matched against the <i>Receivers::TPolarization</i> enumeration.
     * @param LO it gives (if present) the value of the local oscillator (MHz).
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     * @throw CORBA::SystemException
     */
    virtual void getIFOutput(
            const ACS::longSeq& feeds,
            const ACS::longSeq& ifs,
            ACS::doubleSeq_out freqs,
            ACS::doubleSeq_out bw,
            ACS::longSeq_out pols,
            ACS::doubleSeq_out LO
    );

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
    virtual CORBA::Double getTaper(CORBA::Double freq,CORBA::Double bandWidth,CORBA::Long feed,CORBA::Long ifNumber,CORBA::Double_out waveLen) ;
    
	/**
	 * This method is called in order to turn the LNA On.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
    virtual void turnLNAsOn();

	/**
	 * This method is called in order to turn the LNA Off.
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
    virtual void turnLNAsOff();

    /**
     * it turns the vacuum sensor on
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnVacuumSensorOn();

    /**
     * it turns the vacuum sensor on
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnVacuumSensorOff();
    
	/**
	 * It allows to turn the antenna unit on
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	virtual void turnAntennaUnitOn();

	/**
	 *  It allows to turn the antenna unit off
	 * @throw CORBA::SystemException
	 * @throw ComponentErrors::ComponentErrorsEx
	 * @throw ReceiversErrors::ReceiversErrorsEx
	 */
	virtual void turnAntennaUnitOff();

	/**
	 * Returns a reference to the mode property implementation of the IDL interface.
	 * @return pointer to read-only string property
	 * @throw CORBA::SystemException
	*/
    virtual ACS::ROstring_ptr mode();

	/**
	 * Returns a reference to the LO property implementation of the IDL interface.
	 * @return pointer to read-only double sequence property
	 * @throw CORBA::SystemException	 
	*/	 
	virtual ACS::ROdoubleSeq_ptr LO();

	/**
	 * Returns a reference to the feeds property implementation of the IDL interface.
	 * @return pointer to read-only long property
	 * @throw CORBA::SystemException	 
	*/	 
	virtual ACS::ROlong_ptr feeds();

	/**
	 * Returns a reference to the IFs property implementation of the IDL interface.
	 * @return pointer to read-only long property
	 * @throw CORBA::SystemException	 
	*/	 	
	virtual ACS::ROlong_ptr IFs();
	
	/**
	 * Returns a reference to the initialFrequency property implementation of the IDL interface.
	 * @return pointer to read-only doubleSeq property
	 * @throw CORBA::SystemException	 
	*/	 	
	virtual ACS::ROdoubleSeq_ptr initialFrequency();

	/**
	 * Returns a reference to the bandWidth property implementation of the IDL interface.
	 * @return pointer to read-only doubleSeq property
	 * @throw CORBA::SystemException	 
	*/	 	
	virtual ACS::ROdoubleSeq_ptr bandWidth();

	/**
	 * Returns a reference to the polarization property implementation of the IDL interface.
	 * @return pointer to read-only long sequence property
	 * @throw CORBA::SystemException	 
	*/	 
	virtual ACS::ROlongSeq_ptr polarization();	
	
	/**
    * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only pattern property
	 * @throw CORBA::SystemException
	*/
	virtual ACS::ROpattern_ptr status();

	/**
	 * Returns a reference to the vacuum property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr vacuum();

	/**
	 * Returns a reference to the Vd_1 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr Vd_1();

	/**
	 * Returns a reference to the Vd_2 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr Vd_2();

	/**
	 * Returns a reference to the Id_1 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr Id_1();

	/**
	 * Returns a reference to the Id_2 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr Id_2();

	/**
	 * Returns a reference to the Vg_1 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr Vg_1();

	/**
	 * Returns a reference to the Vg_2 property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr Vg_2();

	/**
	 * Returns a reference to the cryoTemperatureCoolHead property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr cryoTemperatureCoolHead();

	/**
	 * Returns a reference to the cryoTemperatureCoolHeadWindow property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr cryoTemperatureCoolHeadWindow();

	/**
	 * Returns a reference to the cryoTemperatureLNA property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr cryoTemperatureLNA();

	/**
	 * Returns a reference to the cryoTemperatureLNAWindow property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr cryoTemperatureLNAWindow();

	/**
	 * Returns a reference to the environmentTemperature property implementation of the IDL interface.
	 * @return pointer to read-only double property
	 * @throw CORBA::SystemException	 
	*/
    virtual ACS::ROdouble_ptr environmentTemperature();

	/**
    * Returns a reference to the status property Implementation of IDL interface.
	 * @return pointer to read-only ROTSystemStatus property status
	 * @throw CORBA::SystemException	 
	*/
	virtual Management::ROTSystemStatus_ptr receiverStatus();

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
