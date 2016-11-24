#ifndef _COMPONENTCORE_H_
#define _COMPONENTCORE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public License (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/08/2011     Creation                                           */

#include "Configuration.h"
#include <ReceiverControl.h>
#include <LocalOscillatorInterfaceC.h>
#include <ReceiversErrors.h>
#include <ManagmentDefinitionsC.h>
#include <BackendsProxy.h>
/**
 * This class contains the code of almost all the features  of the component
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
  */
class CComponentCore {
public:
    /**
     * Constructor
     */
    CComponentCore();

    /**
     * Destructor
     */
    virtual ~CComponentCore();

    /**
     * This method initializes the object
     * @param service pointer to container services object provided by the container
     */
    virtual void initialize(maci::ContainerServices* services) throw (ComponentErrors::CDBAccessExImpl);

    /**
     * This function is responsible to free all allocated resources
     */
    virtual void cleanup();

    /**
     * It sets the local oscillator. Only the first value is considered in this case, since the receiver has just one synthesizer. Before commanding the new value some check are done. The the correspnding signal
     * amplitude is computed.
     * @param lo lists of values for the local oscillator (MHz), one for each IF. In that case just the first one is significant. In a -1 is passed the present value is kept
     * @throw  ComponentErrors::ValidationErrorExImpl
     * @throw ComponentErrors::ValueOutofRangeExImpl
     * @throw ComponentErrors::CouldntGetComponentExImpl
     * @throw ComponentErrors::CORBAProblemExImpl
     * @thorw ReceiversErrors::LocalOscillatorErrorExImpl
     */
    void setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CouldntGetComponentExImpl,
            ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl);

    /**
     * It allows to change the operating mode of the receiver. If the mode does not correspond to a valid mode an error is thrown.
     * @param  mode mode code as a string
     */
    void setMode(const char * mode) throw  (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl,
            ReceiversErrors::ConfigurationExImpl);

    /**
     * It activate the selected receiver. All required operation are performed.
     */
    void activate(const char *setup) throw (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl,ReceiversErrors::NoRemoteControlErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl,ReceiversErrors::ConfigurationExImpl,ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl);

    /**
     * It deactivates the receiver.
     */
    void deactivate() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * It allows to compute the value of the calibration mark for any given sub bands in the IF space.
     * @param result this the sequence of computed mark values, the first entry correspond to first sub band and so on....
     * @param, resFreq the sequence reports the initial observed sky frequency (MHz), the  first entry correspond to first sub band and so on....
     * @param resBw the sequence reports the real bandwidth observed (MHz), the  first entry correspond to first sub band and so on....
     * @param freqs  list of start frequencies (MHz)
     * @param bandwidth list of the band widths (MHz)
     * @param feeds list of feed identifier, it allows to specifies form which feed the sub band comes from. In that case it is neglected since the receiver is a single feed
     * @param ifs list of IF identifier, it allows to specifies from which receiver IF the sub band comes from.
     */
    void getCalibrationMark(ACS::doubleSeq& result,ACS::doubleSeq& resFreq,ACS::doubleSeq& resBw,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,
            const ACS::longSeq& ifs,double& scaleFactor) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl);

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
     * @throw ComponentErrors::ValidationErrorExImpl
     * @thorw ComponentErrors::ValueOutofRangeExImpl
     */
     void getIFOutput(
             const ACS::longSeq& feeds,
             const ACS::longSeq& ifs,
             ACS::doubleSeq& freqs,
             ACS::doubleSeq& bw,
             ACS::longSeq& pols,
             ACS::doubleSeq& LO
     ) throw (ComponentErrors::ValidationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl);

    /**
     * It computes the taper given a reference band.
     * @param freq start frequency of the reference band
     * @param bw width of the reference band
     * @param feed feed number
     * @param ifNumber IF chain identifier
     * @param waveLen wave length of the reference band, the band is transformed in a real sky observed band and the the central frequency is taken
     */
    double getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (
            ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl);

    /**
     * It turns the calibration diode on.
     */
    void calOn() throw (ReceiversErrors::FocusSelectorErrorExImpl,ComponentErrors::UnexpectedExImpl);

    /**
     * It turns the calibration diode off
     */
    void calOff() throw (ReceiversErrors::FocusSelectorErrorExImpl,ComponentErrors::UnexpectedExImpl);

    /**
     * It turns the AntennaUnit on
    */
    void antennaUnitOn() throw (ReceiversErrors::AntennaUnitErrorExImpl);

    /**
     * It turns the AntennaUnit off
    */
    void antennaUnitOff() throw (ReceiversErrors::AntennaUnitErrorExImpl);

    /**
     * It checks if the Dewar power box is in remote or not
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    //void updateIsRemote() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * It checks is the status of the noise mark correspond to the commanded status, otherwise it sets the <i>NOISEMARKERROR</i> bit. It also check if the
     * external control of the noise mark has been enabled or not
     */
    //void updateNoiseMark() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * This method resumes the whole status of the component. It set the <i>componentStatus</i> member variable.
     */
    void updateComponent();

    /**
     * It checks if the local oscillator is locked properly
     */
    void checkLocalOscillator();

    /**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the vacuum in mbar
     */
    double getVacuum() const { return 0.0; }

    /**
     * This is getter method. No need to make it thread safe......
     * @return the current status word
     */
    DWORD getStatusWord() const  { return m_statusWord; }

    /**
     * It returns the feed geometry of the receiver with respect to the central one. For this implementation it is just a placeholder since there is just one feed.
     */
    long getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power);

    /**
     * It returns back the current local oscillator frequency settings.
     * @param lo output sequence
     */
    void getLO(ACS::doubleSeq& lo);

    /**
     * It returns back the current bandwidth for each IF.
     * @param bw output sequence
     */
    void getBandwidth(ACS::doubleSeq& bw);

    /**
     * It returns back the current start frequency for each IF.
     * @param sf output sequence
     */
    void getStartFrequency(ACS::doubleSeq& sf);

    /**
     * It returns back the current polarization for each IF.
     * @param pol output sequence
     */
    void getPolarization(ACS::longSeq& pol);

    /**
     * It returns the current operating mode of the receiver.
     * @return output string
     */
    const IRA::CString& getSetupMode();

    /**
     * It returns the number of IF chains
     * @return output value
     */
    const DWORD& getIFs();

    /**
     * It returns the number of feeds
     * @return output value
     */
    const DWORD& getFeeds();

    /**
     * @return the status flag of the component
     */
    const Management::TSystemStatus& getComponentStatus();

protected:
    /**
     * Obtain a valid reference to the local oscillator device
     */
    void loadLocalOscillator()  throw (ComponentErrors::CouldntGetComponentExImpl);

    /**
     * used to free the reference to the local oscillator device
     */
    void unloadLocalOscillator();
private:

    enum TStatusBit {
        NOISEMARK=0,
        NOISEMARKERROR=1,
        UNLOCKED=2,
        LOERROR=3,
        ANTENNAUNIT=4,
        AUERROR=5
    };

    CConfiguration m_configuration;
    maci::ContainerServices* m_services;
    BACIMutex m_mutex;
    Receivers::LocalOscillator_var m_localOscillatorDevice;
    bool m_localOscillatorFault;
    double m_localOscillatorValue;
    ACS::doubleSeq m_startFreq;
    ACS::doubleSeq m_bandwidth;
    ACS::longSeq m_polarization;
    IRA::CString m_setupMode;
    IRA::CString m_setup;
    /*double m_vacuum;
    double m_vacuumDefault;*/
    DWORD m_statusWord;
    Backends::TotalPower_proxy m_totalPower_proxy;

    Management::TSystemStatus m_componentStatus;

    /**
     * This method prepares the object for execution.
     * @param configuration to be loaded
     */
    virtual void loadConf(const IRA::CString& conf) throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::SocketErrorExImpl);


    void setComponentStatus(const Management::TSystemStatus& status) { if (status>m_componentStatus) m_componentStatus=status;  }

    /**
     * This function will set the a status bit. It may be considered thread safe due to its definition
     */
    inline void setStatusBit(TStatusBit bit) { m_statusWord |= 1 << bit; }

    /**
     * This function will unset (clear) a status bit. It may be considered thread safe due to its definition
     */
    inline void clearStatusBit(TStatusBit bit) { m_statusWord &= ~(1 << bit); }

    /**
     * This function check is a bit is set or not. It may be considered thread safe due to its definition
     */
    inline bool checkStatusBit(TStatusBit bit) { return m_statusWord & (1 << bit); }

    double linearFit(double *X,double *Y,const WORD& size,double x);

};


#endif
