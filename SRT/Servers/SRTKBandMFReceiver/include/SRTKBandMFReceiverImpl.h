/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __KBANDMFRECEIVERIMPL_H__
#define __KBANDMFRECEIVERIMPL_H__

#include <baciCharacteristicComponentImpl.h>
#include <baciSmartPropertyPointer.h>
#include <baciRWdouble.h>
#include <baciROdouble.h>
#include <baciRWdoubleSeq.h>
#include <baciROdoubleSeq.h>
#include <baciROlong.h>
#include <baciROlongSeq.h>
#include <baciROstring.h>
#include <baciROpattern.h>
#include <baciROuLongLong.h>
#include <acsncSimpleSupplier.h>
#include <enumpropROImpl.h>
#include <ComponentErrors.h>
#include <ReceiversErrors.h>
#include <SecureArea.h>
#include <LogFilter.h>
#include <SRTKBandMFS.h>
#include <CommonReceiverInterfaceS.h>
#include <GenericReceiverS.h>
#include <SRTKBandMFParameters.h>
#include <ReceiverSocket.h>

using namespace baci;

class SRTKBandMFReceiverImpl: public CharacteristicComponentImpl,  public virtual POA_Receivers::SRTKBandMFReceiver {

public:
    
    SRTKBandMFReceiverImpl(const ACE_CString &CompName, maci::ContainerServices *containerServices);

    virtual ~SRTKBandMFReceiverImpl(); 

    /**
     * Get the parameter from CDB and create a ReceiverSocket and a CSecureArea. 
     * Initialize the socket calling its Init method.
     *
     * @throw ComponentErrors::CDBAccessExImpl
     * @throw ComponentErrors::MemoryAllocationExImpl
     */
    virtual void initialize() throw (ComponentErrors::CDBAccessExImpl, ComponentErrors::MemoryAllocationExImpl);


    /**
     * @throw ComponentErrors::MemoryAllocationExImpl
     * @throw ComponentErrors::SocketErrorExImpl
     */
    virtual void execute() throw (
            ComponentErrors::MemoryAllocationExImpl,
            ComponentErrors::SocketErrorExImpl
    );
    

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


    /// Return a reference to a vacuum property (ROdouble) 
    virtual ACS::ROdouble_ptr vacuum() throw (CORBA::SystemException);


    /// Return a reference to lowTemperature property (ROdouble) 
    virtual ACS::ROdouble_ptr lowTemperature() throw (CORBA::SystemException);


    /// Return a reference to highTemperature property (ROdouble) 
    virtual ACS::ROdouble_ptr highTemperature() throw (CORBA::SystemException);


    /// Return a reference to status property (ROpattern) 
    virtual ACS::ROpattern_ptr status() throw (CORBA::SystemException);


    /// Return a reference to LO property (ROdoubleSeq) 
    virtual ACS::ROdoubleSeq_ptr LO() throw (CORBA::SystemException);


    /// Return a reference to number of feeds property (ROlong) 
    virtual ACS::ROlong_ptr feeds() throw (CORBA::SystemException);


    /// Return a reference to IFs property (ROlong) 
    virtual ACS::ROlong_ptr IFs() throw (CORBA::SystemException);


    /// Return a reference to polarization property (ROlongSeq) 
    virtual ACS::ROlongSeq_ptr polarization() throw (CORBA::SystemException);


    /// Return a reference to receiverName property (ROstring) 
    virtual ACS::ROstring_ptr receiverName() throw (CORBA::SystemException);


    /**
     * This method is used to turn the calibration diode on.
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx  
     */
    virtual void calOn() throw (ComponentErrors::ComponentErrorsEx);


    /**
     * This method is used to turn the calibration diode off.
     * @throw CORBA::SystemExcpetion
     * @throw ComponentErrors::ComponentErrorsEx  
     */
    virtual void calOff() throw (ComponentErrors::ComponentErrorsEx);
    
    /**
     * This method allows to set local oscillator. Depending on the curretly configured receiver one or more values are considered.
     * @param lo the list contains the values in MHz for the local oscillator
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     */
    void setLO(const ACS::doubleSeq& lo) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx);

    
    /**
     * This method is called when the values of the calibration mark of the current receiver are required. 
     * A value is retruned for every subbands.
     * The subbands are defined by giving the feed numeber, the polarization the initial frequency and the bandwidth.
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @param freqs for each subband this is the list of the starting frequencies (in MHz)
     * @param bandwidths for each subband this is the width in MHz
     * @param feeds for each subband this if the feed number
     * @param ifs for each subband this indicates the proper if, given the feed
     * @return the list of the noise calibration value in Kelvin degrees.
     */
    virtual ACS::doubleSeq * getCalibrationMark(
            const ACS::doubleSeq& freqs, 
            const ACS::doubleSeq& bandwidths, 
            const ACS::longSeq& feeds, 
            const ACS::longSeq& ifs
    ) throw (CORBA::SystemException,ComponentErrors::ComponentErrorsEx); 


    /**
     * This method is called in order to know the geometry of the currently configured receiver. 
     * The geometry is given along the X and Y axis where the central feed is the origin
     * the axis. The relative power (normalized to one) with respect to the central feed is also given.    
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @param X the positions relative to the central beam of the feeds along the X axis (radians)
     * @param Y the positions relative to the central beam of the feeds along the Y axis (radians) 
     * @param power the relative power of the feeds
     * @return the number of feeds
     */    
    virtual CORBA::Long getFeeds(ACS::doubleSeq_out X, ACS::doubleSeq_out Y, ACS::doubleSeq_out power) 
        throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);
    
    /**
     * This method is called in order to know the taper of the current receiver. 
     * @throw CORBA::SystemException
     * @throw ComponentErrors::ComponentErrorsEx
     * @param freq starting frequency of the detector in MHz
     * @param bandWidth bandwidth of the detector n MHz 
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
    ) throw (CORBA::SystemException, ComponentErrors::ComponentErrorsEx);

        
   /** Turn on the LNAs of specified feeds and channels.
    * 
    * @param feed_mask must be converted to a biteset, where each bit is 
    * related to a feed. The true value of a bit means the corresponding 
    * feed is selected, so its LNAs are candidate to be turn on.
    *
    * @param channel_mask must be converted to a biteset. The true value 
    * of a bit means the corresponding channel is selected, so its LNAs 
    * will be turn on. In a dual channel representation, the bit of item 
    * 0 is for the LEFT channel and the bit of item 1 is for the RIGHT one.
    */
    virtual void turnLNAOn(CORBA::ULong feed_mask, CORBA::ULong channel_mask);


    /** Turn off the LNAs of specified feeds and channels.
     * 
     * @param feed_mask must be converted to a biteset, where each bit is 
     * related to a feed. The true value of a bit means the corresponding 
     * feed is selected, so its LNAs are candidate to be turn off.
     *
     * @param channel_mask must be converted to a biteset. The true value 
     * of a bit means the corresponding channel is selected, so its LNAs 
     * will be turn off. In a dual channel representation, the bit of item 
     * 0 is for the LEFT channel and the bit of item 1 is for the RIGHT one.
     */
     virtual void turnLNAOff(CORBA::ULong feed_mask, CORBA::ULong channel_mask);


    /** Set the receiver operating mode.
     *
     *  @param mode the operating mode of receiver. Possible values are:
     *  <ul>
     *    <li><em>SDISH</em>: Single Dish mode</li>
     *    <li><em>VLBI</em>: VLBI mode</li>
     *    <li>...</li>
     *  </ul>
     *  @throw ReceiversErrors::ConfigurationEx
     */
    virtual void setMode(const char *mode);


    /** Return the receiver operating mode as a string.
     *
     *  @return the actual operating mode of receiver. Possible values are:
     *  <ul>
     *    <li><em>SDISH</em>: Single Dish mode</li>
     *    <li><em>VLBI</em>: VLBI mode</li>
     *  </ul>
     */
    virtual char * mode(void);

    virtual ACS::ROdoubleSeq_ptr initialFrequency();

    virtual ACS::ROdoubleSeq_ptr bandWidth();

    
private:
 
    /** Structure containing the CDB parameters */
    RCDBParameters *m_cdb_ptr;
    /** Structure containing the latest value and time of properties */
    ExpireTime *m_expire_ptr;
    /** A socket link, made thread safe */
    CSecureArea<ReceiverSocket> *m_receiver_link;

    /** Dewar vacuum */
    SmartPropertyPointer<ROdouble> m_vacuum;
    /** Dewar low temperature */
    SmartPropertyPointer<ROdouble> m_lowTemperature;
    /** Dewar high temperature */
    SmartPropertyPointer<ROdouble> m_highTemperature;
    /** Receiver Status */
    SmartPropertyPointer<ROpattern> m_status;
    /** Local Oscillator */
    SmartPropertyPointer<ROdoubleSeq> m_LO;
    /** Number of Feeds */
    SmartPropertyPointer<ROlong> m_feeds;
    /** Intermediate Frequencies */
    SmartPropertyPointer<ROlong> m_IFs;
    /** Polarization */
    SmartPropertyPointer<ROlongSeq> m_polarization;
    /** Receiver Name */
    SmartPropertyPointer<ROstring> m_receiverName;
    

    void operator=(const SRTKBandMFReceiverImpl &);

};


#endif
