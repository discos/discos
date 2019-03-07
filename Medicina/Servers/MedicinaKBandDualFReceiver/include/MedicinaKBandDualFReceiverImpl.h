/*******************************************************************************\
 *  Author Info
 *  ===========
 *  Marco Buttu <mbuttu@oa-cagliari.inaf.it>
 *  Andrea Orlati <orlati@ira.inaf.it>
\*******************************************************************************/

#ifndef __MEDICINAKBANDMFRECEIVERIMPL_H__
#define __MEDICINAKBANDMFRECEIVERIMPL_H__

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
#include <MedicinaKBandDualFS.h>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>
#include "MedicinaKBandDualFCore.h"
#include "MonitorThread.h"


using namespace baci;

/** 
 * @mainpage 22GHz Medicina KBand Dualfeed receiver component Implementation
 * @date 12/12/2018
 * @version 1.1.0
 * @author <a href=mailto:mbuttu@oa-cagliari.inaf.it>Marco Buttu</a>,
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>.
 * <h1>Interface Summary</h1>
 *
 * <h2>Methods</h2>
 * <ul>
 *     <li>void activate(): It must be called to switch the receiver to operative mode.</li>
 *     <li>void calOn(): This method is used to turn the calibration diode ON.</li>
 *     <li>void calOff(): This method is used to turn the calibration diode OFF.</li>
 *     <li>void externalCalOn(): This method is used to turn the external calibration diode ON.</li>
 *     <li>void externalCalOff(): This method is used to turn the external calibration diode OFF.</li>
 *     <li>void setLO(...): This method allows to set local oscillator. In this implementation 
 *     only the first value is considered.</li>
 *     <li>void setMode(...): This method allows to set the operating mode of the receiver.</li>
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
 * <h2>LNA Properties</h2>
 * <p>We use the ROdoubleSeq, one sequence for each amplifier stage
 * and channel. The first index is for the amplifier stage, and
 * the second one is for the feed. The letters L and R mean Left 
 * and Right. For a N feed receiver we have the following
 * properties:</p><br />
 * <br />
 * idL1 (idL1_1, idL1_2, ...., idL1_N)<br />
 * idR1 (idR1_1, idR1_2, ...., idR1_N)<br />
 * idL2 (idL2_1, idL2_2, ...., idL2_N)<br />
 * idR2 (idR2_1, idR2_2, ...., idR2_N)<br />
 *        |                           <br />
 * idL5 (idL5_1, idL5_2, ...., idL5_N)<br />
 * idR5 (idR5_1, idR5_2, ...., idR5_N)
 * <br />
 * <br />
 * vdL1 (vdL1_1, vdL1_2, ...., vdL1_N)<br />
 * vdR1 (vdR1_1, vdR1_2, ...., vdR1_N)<br />
 * vdL2 (vdL2_1, vdL2_2, ...., vdL2_N)<br />
 * vdR2 (vdR2_1, vdR2_2, ...., vdR2_N)<br />
 *        |                           <br />
 * vdL5 (vdL5_1, vdL5_2, ...., vdL5_N)<br />
 * vdR5 (vdR5_1, vdR5_2, ...., vdR5_N)
 * <br />
 * <br />
 * vgL1 (vgL1_1, vgL1_2, ...., vgL1_N)<br />
 * vgR1 (vgR1_1, vgR1_2, ...., vgR1_N)<br />
 * vgL2 (vgL2_1, vgL2_2, ...., vgL2_N)<br />
 * vgR2 (vgR2_1, vgR2_2, ...., vgR2_N)<br />
 *        |                           <br />
 * vgL5 (vgL5_1, vgL5_2, ...., vgL5_N)<br />
 * vgR5 (vgR5_1, vgR5_2, ...., vgR5_N)
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
class MedicinaKBandDualFReceiverImpl: public CharacteristicComponentImpl,  public virtual POA_Receivers::MedicinaKBandDualF {

public:
    
    MedicinaKBandDualFReceiverImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

    virtual ~MedicinaKBandDualFReceiverImpl(); 

    /**
     * Get the parameter from CDB and create a ReceiverSocket and a CSecureArea. 
     * Initialize the socket calling its Init method.
     *
     * @throw ACSErr::ACSbaseExImpl
     */
    virtual void initialize() throw (ACSErr::ACSbaseExImpl);


    /**
     * @throw ACSErr::ACSbaseExImpl, ComponentErrors::MemoryAllocationExImpl
     */
    virtual void execute() throw (ACSErr::ACSbaseExImpl, ComponentErrors::MemoryAllocationExImpl);
    

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
     virtual void activate(const char * setup_mode) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx);

     /**
      * It must be called to switch off the receiver
      * @throw CORBA::SystemExcpetion
      * @throw ComponentErrors::ComponentErrorsEx
      * @throw ReceiversErrors::ReceiversErrorsEx
     */
     virtual void deactivate() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx);

    /**
     * This method is used to turn the calibration diode on.
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void calOn() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx);


    /**
     * This method is used to turn the calibration diode off.
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void calOff() throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx);


    /**
     * This method is used to turn the external calibration diode on.
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void externalCalOn() throw (
            CORBA::SystemException, 
            ComponentErrors::ComponentErrorsEx, 
            ReceiversErrors::ReceiversErrorsEx);


    /**
     * This method is used to turn the external calibration diode off.
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void externalCalOff() throw (
            CORBA::SystemException, 
            ComponentErrors::ComponentErrorsEx, 
            ReceiversErrors::ReceiversErrorsEx);



    /**
     * This method allows to set local oscillator. In this implementation only the first value is considered.
     * @param lo the list contains the values in MHz for the local oscillator
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    void setLO(const ACS::doubleSeq& lo) throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );

    
    /**
     * This method allows to set the operating mode of the receiver. In that implementation the receiver 
     * does not have special modes so a call to this method will lead to Configuration exception.
     * @param mode string identifier of the operating mode
     * @throw CORBA::SystemException
     * @throw ReceiversErrors::ReceiversErrorsEx
     * @throw ComponentErrors::ComponentErrorsEx
     */
    void setMode(const char * mode) throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );
    
 
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
    ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx);
            

    virtual ACS::doubleSeq * getCalibrationMark(
            const ACS::doubleSeq& freqs, 
            const ACS::doubleSeq& bandwidths, 
            const ACS::longSeq& feeds, 
            const ACS::longSeq& ifs,
            ACS::doubleSeq_out skyFreq,
            ACS::doubleSeq_out skyBw,
            CORBA::Boolean_out onoff,
            CORBA::Double_out scaleFactor
    ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx);





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
     */    
    virtual CORBA::Long getFeeds(
            ACS::doubleSeq_out X,
            ACS::doubleSeq_out Y,
            ACS::doubleSeq_out power
            ) throw (
                CORBA::SystemException,
                ComponentErrors::ComponentErrorsEx,
                ReceiversErrors::ReceiversErrorsEx
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
     */        
    virtual CORBA::Double getTaper(
            CORBA::Double freq,
            CORBA::Double bandWidth,
            CORBA::Long feed,
            CORBA::Long ifNumber,
            CORBA::Double_out waveLen
            ) throw (
                CORBA::SystemException,
                ComponentErrors::ComponentErrorsEx,
                ReceiversErrors::ReceiversErrorsEx
    );
 

    /**
     * This method is called in order to turn the LNAs On.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnLNAsOn() throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );


    /**
     * This method is called in order to turn the LNAs Off.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnLNAsOff() throw (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );


    /**
     * It turns the vacuum sensor on
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnVacuumSensorOn() throw  (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );


    /**
     * It turns the vacuum sensor on
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnVacuumSensorOff() throw  (
            CORBA::SystemException,
            ComponentErrors::ComponentErrorsEx,
            ReceiversErrors::ReceiversErrorsEx
    );

    /**
     * It allows to turn the antenna unit on
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnAntennaUnitOn() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);

    /**
     *  It allows to turn the antenna unit off
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @throw ReceiversErrors::ReceiversErrorsEx
     */
    virtual void turnAntennaUnitOff() throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx,ReceiversErrors::ReceiversErrorsEx);


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
     * Returns a reference to the vdL1 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdL1() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdR1 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdR1() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdL2 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdL2() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdR2 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdR2() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdL3 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdL3() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdR3 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdR3() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdL4 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdL4() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdR4 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdR4() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdL5 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdL5() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vdR5 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vdR5() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idL1 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idL1() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idR1 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idR1() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idL2 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idL2() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idR2 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idR2() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idL3 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idL3() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idR3 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idR3() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idL4 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idL4() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idR4 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idR4() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idL5 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idL5() throw (CORBA::SystemException);


    /**
     * Returns a reference to the idR5 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr idR5() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgL1 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgL1() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgR1 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgR1() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgL2 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgL2() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgR2 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgR2() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgL3 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgL3() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgR3 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgR3() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgL4 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgL4() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgR4 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgR4() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgL5 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgL5() throw (CORBA::SystemException);


    /**
     * Returns a reference to the vgR5 property implementation of the IDL interface.
     * @return pointer to read-only double property
     */
    virtual ACS::ROdoubleSeq_ptr vgR5() throw (CORBA::SystemException);


    /// Return a reference to receiverName property (ROstring) 
    virtual ACS::ROstring_ptr receiverName() throw (CORBA::SystemException);


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

    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdL1;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdR1;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdL2;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdR2;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdL3;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdR3;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdL4;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdR4;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdL5;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvdR5;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidL1;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidR1;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidL2;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidR2;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidL3;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidR3;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidL4;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidR4;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidL5;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pidR5;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgL1;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgR1;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgL2;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgR2;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgL3;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgR3;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgL4;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgR4;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgL5;
    baci::SmartPropertyPointer<baci::ROdoubleSeq> m_pvgR5;

    baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureCoolHead;
    baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureCoolHeadWindow;
    baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureLNA;
    baci::SmartPropertyPointer<baci::ROdouble> m_pcryoTemperatureLNAWindow;
    baci::SmartPropertyPointer<baci::ROdouble> m_penvironmentTemperature;
    baci::SmartPropertyPointer<baci::ROstring> m_pmode;
    baci::SmartPropertyPointer < ROEnumImpl<ACS_ENUM_T(Management::TSystemStatus), \
        POA_Management::ROTSystemStatus> > m_preceiverStatus;
    baci::SmartPropertyPointer<ROstring> m_preceiverName;

    MedicinaKBandDualFCore m_core;
    CMonitorThread *m_monitor;

    void operator=(const MedicinaKBandDualFReceiverImpl &);

};


#endif
