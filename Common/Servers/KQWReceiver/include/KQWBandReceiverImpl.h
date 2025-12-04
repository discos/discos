/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Andrea Orlati <orlati@inaf.it>
\*******************************************************************************/

#ifndef __KQWBANDRECEIVERIMPL_H__
#define __KQWBANDRECEIVERIMPL_H__

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <Cplusplus11Helper.h>

C11_IGNORE_WARNING_PUSH
C11_IGNORE_WARNING("-Wdeprecated-declarations")
C11_IGNORE_WARNING("-Wmisleading-indentation")
C11_IGNORE_WARNING("-Wcatch-value=")
C11_IGNORE_WARNING("-Wsequence-point")
#include <baciCharacteristicComponentImpl.h>
#include <enumpropROImpl.h>
C11_IGNORE_WARNING_POP

#include <baciSmartPropertyPointer.h>
#include <baciROdoubleSeq.h>
#include <baciROlongSeq.h>
#include <baciROstring.h>
#include <baciROlong.h>
#include <baciROdouble.h>
#include <SP_parser.h>
#include <IRA>
#include <KQWBandS.h>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>
#include "ComponentCore.h"
#include "MonitorThread.h"


using namespace baci;

/** 
 * @mainpage KQW bands receiver component implementation
 * @date 2024/08/06
 * @version 0.1
 * @author <a href=mailto:andrea.orlati@inaf.it>Andrea Orlati</a>.
 * <h1>Interface Summary</h1>
 *
 * <h2>Methods</h2>
 * <ul>
 *     <li>void activate(): It must be called to switch the receiver to operative mode.</li>
 *     <li>void calOn(): This method is used to turn the calibration diode ON.</li>
 *     <li>void calOff(): This method is used to turn the calibration diode OFF.</li>
 *     <li>void externalCalOn(): This method is used to turn the external calibration diode ON.</li>
 *     <li>void externalCalOff(): This method is used to turn the external calibration diode OFF.</li>
 *     <li>void setMode(...): This method allows to set the operating mode of the receiver.</li>
 *     <li>void setLO(...): ... do nothing</li>
 *     <li>ACS::doubleSeq * getCalibrationMark(...): This method is called when the values of 
 *     the calibration mark of the receiver are required.</li>
 *     <li>CORBA::Long getFeeds(...): This method is called in order to know the geometry of the receiver.</li>
 *     <li>CORBA::Double getTaper(...): This method is called in order to know the taper of the receiver.</li>
 *     <li>void turnLNAsOn(): This method is called in order to turn the LNAs ON.</li>
 *     <li>void turnLNAsOff(): This method is called in order to turn the LNAs OFF.</li>
 *     <li>void turnVacuumSensonOn(): It turns the vacuum sensor ON.</li>
 *     <li>void turnVacuumSensonOff(): It turns the vacuum sensor OFF.</li>
 * </ul>
 *
 *
 * <h2>Dewar Properties</h2>
 * <ul>
 *     <li>mode: the receiver operating mode</li>
 *     <li>LO: reports the current value of the local oscillator of the current receiver. 
 *     Generally one LO for each IFs.</li>
 *     <li>feeds: reports the number of feeds of the current receiver</li>
 *     <li>IFs: reports the number of Intermediate Frequencies available for each feed</li>
 *     <li>initialFrequency: a sequence of double values; each value corresponds to the start 
 *     frequency (MHz) of IF of the receiver</li>
 *     <li>bandWidth: a sequence of double values; each value corresponds to the band width (MHz) 
 *     of IF of the receiver</li>
 *     <li>polarization: reports the polarization configured in each IF available.</li>
 *     <li>status: a status pattern</li>
 *     <li>vacuum: dewar vacuum</li>
 * </ul>
 */
class KQWBandReceiverImpl: public CharacteristicComponentImpl,  public virtual POA_Receivers::KQWBand {

public:
    
    KQWBandReceiverImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

    virtual ~KQWBandReceiverImpl(); 

    /**
     * Get the parameter from CDB and create a ReceiverSocket and a CSecureArea. 
     * Initialize the socket calling its Init method.
     *
     * @throw ACSErr::ACSbaseExImpl
     */
    virtual void initialize();


    /**
     * @throw ACSErr::ACSbaseExImpl, ComponentErrors::MemoryAllocationExImpl
     */
    virtual void execute();
    

    /** 
     * Called by the container before destroying the server in a normal situation. 
     * This function takes charge of releasing all resources.
     */
     virtual void cleanUp();
    

    /** 
     * Called by the container in case of error or emergency situation. 
     * This function tries to free all resources even though there is no warranty that the function 
     * is completely executed before the component is destroyed.
     */ 
    virtual void aboutToAbort();


    /**
     * It must be called to switch the receiver to operative mode. 
     * When called the default configuration and mode is loaded. Regarding this
     * implementation calling this method corresponds to a call to <i>setMode("NORMAL")</i>.
     * @param setup_mode the setup mode (KKG, CCB, LLP, PLP, ecc.)
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
     virtual void activate(const char * setup_mode);
    

     /**
      * It must be called to switch off the receiver
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
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void calOff();

    /**
     * This method is used to turn the external calibration diode on.
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void externalCalOn();

    /**
     * This method is used to turn the external calibration diode off.
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void externalCalOff();

    /**
     * This method do nothing because the LP receiver has not a local oscillator
     * @param lo the list contains the values in MHz for the local oscillator
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void setLO(const ACS::doubleSeq& lo);

    
    /**
     * This method allows to set the operating mode of the receiver. In that implementation the receiver 
     * does not have special modes so a call to this method will lead to Configuration exception.
     * @param mode string identifier of the operating mode
     * @throw CORBA::SystemException
     * @throw ReceiversErrors::ReceiversErrorsEx
     * @throw ComponentErrors::ComponentErrorsEx
     */
    void setMode(const char * mode);
 
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
     * This method is called when the values of the calibration mark of the receiver are required. 
     * A value is returned for every provided sub bands. The sub bands are defined by giving the 
     * feed number, the polarization, the initial frequency and the bandwidth.
     * @param freqs for each sub band this is the list of the starting frequencies (in MHz). 
     * The value is compared and adjusted to the the real initial frequency of the receiver.
     * @param bandwidths for each sub band this is the width in MHz. The value is compared and 
     * adjusted to the the real band width of the receiver.
     * @param feeds for each sub band this if the feed number. In that case zero is the only 
     * allowed value.
     * @param ifs for each sub band this indicates the proper IF
     * @param skyFreq for each sub band it returns the real observed frequency(MHz), included 
     * detector, receiver IF  and Local Oscillator.
     * @param skyBw for each sub band it returns the real observed bandwidth(MHz), included 
     * detector bandwidth , receiver IF bandwidth
     * @param scaleFactor multiplication factor for tsys computation
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     * @return the list of the noise calibration value in Kelvin degrees.
     */
    virtual ACS::doubleSeq * getCalibrationMark(
            const ACS::doubleSeq& freqs, 
            const ACS::doubleSeq& bandwidths, 
            const ACS::longSeq& feeds, 
            const ACS::longSeq& ifs,
            ACS::doubleSeq_out skyFreq,
            ACS::doubleSeq_out skyBw,
            CORBA::Boolean_out onoff,
            CORBA::Double_out scaleFactor
    );


    /**
     * This method is called in order to know the geometry of the receiver. The geometry is given 
     * along the X and Y axis where the central feed is the origin of the axis. The relative power 
     * (normalized to one) with respect to the central feed is also given.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     * @param X the positions relative to the central beam of the feeds along the X axis (radiants)
     * @param Y the positions relative to the central beam of the feeds along the Y axis (radiants) 
     * @param power the relative power of the feeds
     * @return the number of feeds
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */    
    virtual CORBA::Long getFeeds(
            ACS::doubleSeq_out X,
            ACS::doubleSeq_out Y,
            ACS::doubleSeq_out power
            );

    /**
     * This method is called in order to know the taper of the receiver.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     * @param freq starting frequency of the detector in MHz. The value is compared and adjusted 
     * to the the real initial frequency of the receiver.
     * @param bandWidth bandwidth of the detector n MHz. The value is compared and adjusted to 
     * the the real band width of the receiver.
     * @param feed feed id the detector is attached to
     * @param ifNumber Number of the IF, given the feed
     * @param waveLen corresponding wave length in meters
     * @return the value of the taper in db
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */        
    virtual CORBA::Double getTaper(
            CORBA::Double freq,
            CORBA::Double bandWidth,
            CORBA::Long feed,
            CORBA::Long ifNumber,
            CORBA::Double_out waveLen
            );
 
    /**
     * This method is called in order to turn the LNAs On.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnLNAsOn();

    /**
     * This method is called in order to turn the LNAs Off.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnLNAsOff();

    /**
     * It turns the vacuum sensor on
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnVacuumSensorOn();

    /**
     * It turns the vacuum sensor on
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
     * This method is called in order to se the L band filter.
     * @param filter_id the filter ID:
     *     ID 1 -> all band filter, 1300-1800 no filter
     *     ID 2 -> 1320-1780 MHz
     *     ID 3 -> 1350-1450 MHz (VLBI)
     *     ID 4 -> 1300-1800 MHz (band-pass)
     *     ID 5 -> 1625-1715 MHz (VLBI)
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ManagementErrors::ConfigurationErrorEx
     */    
   /* virtual void setLBandFilter(CORBA::Long filter_id) throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );*/


    /**
     * This method is called in order to se the P band filter.
     * @param filter_id the filter ID:
     *     ID 1 -> all band filter, 305-410 no filter
     *     ID 2 -> 310-350 MHz
     *     ID 3 -> 305-410 MHz (band-pass filter)
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ManagementErrors::ConfigurationErrorEx
     */    
    /*virtual void setPBandFilter(CORBA::Long filter_id) throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );*/


    /**
     * This method is called in order to set the L band polarization.
     * @param polarization "L" for Linear, "C" for Circular
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ManagementErrors::ConfigurationErrorEx
     */    
    /*virtual void setLBandPolarization(const char * polarization) throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );*/


    /**
     * This method is called in order to set the P band polarization.
     * @param polarization "L" for Linear, "C" for Circular
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ManagementErrors::ConfigurationErrorEx
     */ 
    /*virtual void setPBandPolarization(const char * polarization) throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );*/


    /**
     * This method sets the RF path of the L band feed to the cold load
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ManagementErrors::ConfigurationErrorEx
     */    
    /*virtual void setLBandColdLoadPath() throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );*/


    /**
     * This method sets the RF path of the P band feed to the cold load
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ManagementErrors::ConfigurationErrorEx
     */    
    /*virtual void setPBandColdLoadPath() throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );*/


    /**
     * This method sets the RF path of the L band feed to the sky
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ManagementErrors::ConfigurationErrorEx
     */    
    /*virtual void setLBandSkyPath() throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );*/


    /**
     * This method sets the RF path of the P band feed to the sky
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ManagementErrors::ConfigurationErrorEx
     */    
    /*virtual void setPBandSkyPath() throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );*/


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
     * Returns a reference to the vdKL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vdKL();
          /**
     * Returns a reference to the vdKR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vdKR();
          /**
     * Returns a reference to the vdQL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vdQL();
          /**
     * Returns a reference to the vdQR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vdQR();
          /**
     * Returns a reference to the vdWL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vdWL();
          /**
     * Returns a reference to the vdWR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vdWR();
     
     /**
     * Returns a reference to the idKL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr idKL();
          /**
     * Returns a reference to the idKR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr idKR();
          /**
     * Returns a reference to the idQL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr idQL();
          /**
     * Returns a reference to the idQR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr idQR();
          /**
     * Returns a reference to the vdWL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr idWL();
          /**
     * Returns a reference to the idWR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr idWR();

     /**
     * Returns a reference to the vgKL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vgKL();
          /**
     * Returns a reference to the vgKR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vgKR();
          /**
     * Returns a reference to the vgQL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vgQL();
          /**
     * Returns a reference to the vgQR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vgQR();
          /**
     * Returns a reference to the vdWL property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vgWL();
          /**
     * Returns a reference to the vgWR property implementation of the IDL interface.
     * @return pointer to read-only double property
     * @throw CORBA::SystemException
     */
     virtual ACS::ROdouble_ptr vgWR();

    /*
    * @return a reference to receiverName property (ROstring) 
    * @throw CORBA::SystemException
    */
    virtual ACS::ROstring_ptr receiverName();


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

    baci::SmartPropertyPointer<baci::ROdouble> m_pvdKL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvdKR;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvdQL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvdQR;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvdWL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvdWR;
    baci::SmartPropertyPointer<baci::ROdouble> m_pidKL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pidKR;
    baci::SmartPropertyPointer<baci::ROdouble> m_pidQL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pidQR;
    baci::SmartPropertyPointer<baci::ROdouble> m_pidWL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pidWR;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvgKL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvgKR;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvgQL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvgQR;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvgWL;
    baci::SmartPropertyPointer<baci::ROdouble> m_pvgWR;

    baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureCoolHead;
    baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureCoolHeadWindow;
    baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureLNA;
    baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureLNAWindow;
    baci::SmartPropertyPointer<baci::ROdouble> m_penvironmentTemperature;
    baci::SmartPropertyPointer<baci::ROstring> m_pmode;
    baci::SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), \
        POA_Management::ROTSystemStatus> > m_preceiverStatus;
    baci::SmartPropertyPointer<ROstring> m_preceiverName;

    CComponentCore m_core;
    CMonitorThread *m_monitor;

    void operator=(const KQWBandReceiverImpl &);

};


#endif
