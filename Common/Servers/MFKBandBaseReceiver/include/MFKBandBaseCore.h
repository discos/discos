#ifndef _MFBASECOMPONENTCORE_H_
#define _MFBASECOMPONENTCORE_H_

/***********************************************************************\
 IRA Istituto di Radioastronomia                                 
 This code is under GNU General Public License (GPL).          
                                                              
 Andrea Orlati (aorlati@ira.inaf.it): Author
 Marco Buttu (mbuttu@oa-cagliari.inaf.it): 
     I made just some adaptings to use the same code with the SRTKBandMF 
     receiver. This code is inherited by SRTKBandMFCore, so I made the 
     setMode function pure virtual and I removed the updateLNA() method, 
     which now must be implemented in derived class.
     I also changed the access label of some methods and attributes from
     private to protected in order to use them in the derived class.
\***********************************************************************/

#include "MFKBandBaseConf.h"
#include <ReceiverControl.h>
#include <LocalOscillatorInterfaceC.h>
#include <ReceiversErrors.h>
#include <ManagmentDefinitionsC.h>
#include "MFKBandBaseUtils.h"

/**
 * This class contains the code of almost all the features  of the component that controls a MFKBand
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
  */
class CComponentCore {

public:

    CComponentCore();
    virtual ~CComponentCore();


    /**
     * This method initializes the object
     * @param service pointer to container services object provided by the container
     */
    virtual void initialize(maci::ContainerServices* services);


    /**
     * This method prepares the object for execution.
     * @return the pointer to the configuration class
     */
    virtual CConfiguration const * const execute() throw (
            ComponentErrors::CDBAccessExImpl,
            ComponentErrors::MemoryAllocationExImpl,
            ComponentErrors::SocketErrorExImpl,
            ReceiversErrors::ModeErrorExImpl
    );
    

    /**
     * This function is responsible to free all allocated resources
     */
    virtual void cleanup();


    /*
     * It sets the local oscillator. Only the first value is considered in this case, 
     * since the receiver has just one synthesizer. Before commanding the new value 
     * some check are done. The the correspnding signal amplitude is computed.
     * @param lo lists of values for the local oscillator (MHz), one for each IF. 
     * In that case just the first one is significant. In a -1 is passed the present value is kept.
     * @throw  ComponentErrors::ValidationErrorExImpl
     * @throw ComponentErrors::ValueOutofRangeExImpl
     * @throw ComponentErrors::CouldntGetComponentExImpl
     * @throw ComponentErrors::CORBAProblemExImpl
     * @thorw ReceiversErrors::LocalOscillatorErrorExImpl
     */
    void setLO(const ACS::doubleSeq& lo) throw (
            ComponentErrors::ValidationErrorExImpl,
            ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,
            ComponentErrors::CORBAProblemExImpl,
            ReceiversErrors::LocalOscillatorErrorExImpl
    );


    /**
     * It activate the receiver, in other words it allows to setup the default configuration 
     * and to make sure the LNA are turned on.
     */
    void activate() throw (
            ReceiversErrors::ModeErrorExImpl,
            ComponentErrors::ValidationErrorExImpl,
            ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,
            ComponentErrors::CORBAProblemExImpl,
            ReceiversErrors::LocalOscillatorErrorExImpl,
            ReceiversErrors::NoRemoteControlErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl
    );

    /**
     * It deactivates the receiver.
     */
    void deactivate() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It allows to compute the value of the calibration mark for any given sub bands in 
     * the IF space.
     * @param result this the sequence of computed mark values, the first entry correspond to 
     * first sub band and so on....
     * @param, resFreq the sequence reports the initial observed sky frequency (MHz), the  
     * first entry correspond to first sub band and so on....
     * @param resBw the sequence reports the real bandwidth observed (MHz), the  first entry 
     * correspond to first sub band and so on....
     * @param freqs  list of start frequencies (MHz)
     * @param bandwidth list of the band widths (MHz)
     * @param feeds list of feed identifier, it allows to specifies form which feed the sub 
     * band comes from. In that case it is neglected since the receiver is a single feed
     * @param ifs list of IF identifier, it allows to specifies from which receiver IF the 
     * sub band comes from.
     * @param scale value to scale tsys measurement
     */
    void getCalibrationMark(
            ACS::doubleSeq& result,
            ACS::doubleSeq& resFreq,
            ACS::doubleSeq& resBw,
            const ACS::doubleSeq& freqs,
            const ACS::doubleSeq& bandwidths,
            const ACS::longSeq& feeds,
            const ACS::longSeq& ifs,
            bool& onoff,
            double &scale
     ) throw (ComponentErrors::ValidationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl);


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
     * @param waveLen wave length of the reference band, the band is transformed in a real sky 
     * observed band and the the central frequency is taken.
     */
    double getTaper(
            const double& freq,
            const double& bw,
            const long& feed,
            const long& ifNumber,
            double& waveLen
    ) throw (ComponentErrors::ValidationErrorExImpl, ComponentErrors::ValueOutofRangeExImpl);


    /** It turns the calibration diode on. */
    void calOn() throw (
            ReceiversErrors::NoRemoteControlErrorExImpl,
            ComponentErrors::ValidationErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /** It turns the calibration diode off */
    void calOff() throw (
            ReceiversErrors::NoRemoteControlErrorExImpl,
            ComponentErrors::ValidationErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /** It turns the external calibration diode on. */
    void externalCalOn() throw (
            ReceiversErrors::NoRemoteControlErrorExImpl,
            ComponentErrors::ValidationErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /** It turns the external calibration diode off. */
    void externalCalOff() throw (
            ReceiversErrors::NoRemoteControlErrorExImpl,
            ComponentErrors::ValidationErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /** It turns on the sensor for vacuum measurement. */
    void vacuumSensorOn() throw (
            ReceiversErrors::NoRemoteControlErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /** It turns off the sensor for vacuum measurement. */
    void vacuumSensorOff() throw (
            ReceiversErrors::NoRemoteControlErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl
    );


    /** It allows to turn LNA on */
    void lnaOn() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /** It allows to turn LNA off */
    void lnaOff() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It reads and updates from the control board the current value of the vacuum
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateVacuum() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It check if the vacuum pump is on and check is the status is fault or not (<i>VACUUMPUMPFAULT</i>)
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateVacuumPump() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It checks if the vacuum valve is opened or not
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateVacuumValve() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It reads and updates from the control board the current cryo temperature measured near the cool head
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateCryoCoolHead() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It reads and updates from the control board the current cryo temperature measured near the cool head window
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateCryoCoolHeadWin() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It reads and updates from the control board the current cryo temperature measured near the LNA
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateCryoLNA() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It reads and updates from the control board the current cryo temperature measured near the LNA window
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateCryoLNAWin() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It reads and updates from the control board the current vertex temperature
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateVertexTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It checks if the Dewar power box is in remote or not
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateIsRemote() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It checks if the cool head is turned on or not
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    virtual void updateCoolHead() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It checks is the status of the noise mark correspond to the commanded status, 
     * otherwise it sets the <i>NOISEMARKERROR</i> bit. It also check if the
     * external control of the noise mark has been enabled or not
     */
    virtual void updateNoiseMark() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * This method resumes the whole status of the component. It set the <i>componentStatus</i> member variable.
     */
    void updateComponent();


    /** I checks if the local oscillator is locked properly */
    void checkLocalOscillator() throw (ComponentErrors::CORBAProblemExImpl, ComponentErrors::CouldntGetAttributeExImpl);


    /**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the vacuum in mbar
     */
    double getVacuum() const { return m_vacuum; }


    /**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the cryogenic temperature at cool head in °K
     */
    CConfiguration::BoardValue getCryoCoolHead() const { return m_cryoCoolHead; }


    /**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the cryogenic temperature at cool head window in °K
     */
    CConfiguration::BoardValue getCryoCoolHeadWin() const { return m_cryoCoolHeadWin; }


    /**
     * This is getter method. No need to make it thread safe......
      * @return the current value of the cryogenic temperature at LNA in °K
     */
    CConfiguration::BoardValue getCryoLNA() const { return m_cryoLNA; }


    /**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the cryogenic temperature at LNA  window in °K
     */
    CConfiguration::BoardValue getCryoLNAWin() const { return m_cryoLNAWin; }


    /**
     * This is getter method. No need to make it thread safe......
     * @return the current vertex temperature in °K
     */
    CConfiguration::BoardValue getVertexTemperature() const { return m_envTemperature; }


    /**
     * This is getter method. No need to make it thread safe......
     * @return the current status word
     */
    DWORD getStatusWord() const  { return  m_statusWord; }


    /**
     * This is getter method. In this case, since it makes use of some class members that 
     * could be changed by other methods it is advisable to protect this method with the class mutex.
     * @param control name of the parameter that must be returned
     * @param ifs Intermediate frequency identifier, it permits to select which amplification chain 
     * we are interested in
     * @return a specific value of from the transistor control parameters
     */
    double getFetValue(const IRA::ReceiverControl::FetValue& control, const DWORD& ifs);


    /**
     * It returns the feed geometry of the receiver with respect to the central one. 
     * For this implementation it is just a placeholder since there is just one feed.
     */
    long getFeeds(ACS::doubleSeq& X, ACS::doubleSeq& Y, ACS::doubleSeq& power);


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


    /**
     * Allows to set the "default_value" for the vacuum characteristic. In principle it is possible 
     * to read it directly from CDB, but I found it more
     * comfortable to get it directly from the characteristic itself.
     */
    inline void setVacuumDefault(const double& val) { m_vacuumDefault=val; }


    /** 
     * This is a pure virtual function, so you must write it in your derived class.
     * It allows to change the operating mode of the receiver. 
     * If the mode does not correspond to a valid mode an error is thrown.
     * @param  mode mode code as a string
     * @throw ReceiversErrors::ModeErrorExImpl,
     * @throw ComponentErrors::ValidationErrorExImpl,
     * @throw ComponentErrors::ValueOutofRangeExImpl,
     * @throw ComponentErrors::CouldntGetComponentExImpl,
     * @throw ComponentErrors::CORBAProblemExImpl,
     * @throw ReceiversErrors::LocalOscillatorErrorExImpl
     */
    virtual void setMode(const char * mode) throw (
            ReceiversErrors::ModeErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl,
            ComponentErrors::ValidationErrorExImpl,
            ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,
            ComponentErrors::CORBAProblemExImpl,
            ReceiversErrors::LocalOscillatorErrorExImpl
    ) = 0; // Pure Virtual Function


protected:

	typedef double (*TConverter)(double voltage);

    /** Obtain a valid reference to the local oscillator device */
    void loadLocalOscillator()  throw (ComponentErrors::CouldntGetComponentExImpl);


    /** Used to free the reference to the local oscillator device */
    void unloadLocalOscillator();


    /************************ CONVERSION FUNCTIONS **************************/
    // Convert the voltage value of the vacuum to mbar
 	/*virtual double voltage2mbar(double voltage) { return voltage; }


    // Convert the voltage value of the temperatures to Kelvin
	virtual  double voltage2Kelvin(double voltage) {
   	return voltage;
   }
    // Convert the voltage value of the temperatures to Celsius (Sensor B57703-10K)
	virtual double voltage2Celsius(double voltage) { return voltage; } */
    /*{ return -5.9931 * pow(voltage, 5) + 40.392 * pow(voltage, 4) - 115.41 * pow(voltage, 3) + 174.67 * pow(voltage, 2) - 174.23 * voltage + 112.79; }*/

    // Convert the ID voltage value to the mA value
	/*virtual double currentConverter(double voltage) { return voltage; }


    // Convert the VD and VG voltage values using a right scale factor
	virtual double voltageConverter(double voltage) { return voltage; }*/
   /********************** END CONVERSION FUNCTIONS ***********************/

	
	TConverter voltage2mbar;
	TConverter voltage2Kelvin;
	TConverter voltage2Celsius;
	TConverter currentConverter;
	TConverter voltageConverter;

    enum TStatusBit {
        LOCAL=0,
        VACUUMSENSOR=1,
        VACUUMPUMPSTATUS=2,
        VACUUMPUMPFAULT=3,
        VACUUMVALVEOPEN=4,
        COOLHEADON=5,
        COMPRESSORFAULT=6,
        NOISEMARK=7,
        NOISEMARKERROR=8,
        EXTNOISEMARK=9,
        CONNECTIONERROR=10,
        UNLOCKED=11
    };


    /** This function will set the a status bit. It may be considered thread safe due to its definition */
    inline void setStatusBit(TStatusBit bit) { m_statusWord |= 1 << bit; }


    /** This function will unset (clear) a status bit. It may be considered thread safe due to its definition */
    inline void clearStatusBit(TStatusBit  bit) { m_statusWord &= ~(1 << bit); }


    /** This function check is a bit is set or not. It may be considered thread safe due to its definition */
    inline bool checkStatusBit(TStatusBit bit) { return m_statusWord & (1 << bit); }


    CConfiguration m_configuration;
    IRA::ReceiverControl *m_control; // This object is thread safe
    ACS::longSeq m_polarization;
    BACIMutex m_mutex;

    ACS::doubleSeq m_startFreq;
    ACS::doubleSeq m_bandwidth;
    IRA::CString m_setupMode;
    double m_vacuum;
    CConfiguration::BoardValue m_envTemperature;
    CConfiguration::BoardValue m_cryoCoolHead;
    CConfiguration::BoardValue m_cryoCoolHeadWin;
    CConfiguration::BoardValue m_cryoLNA;
    CConfiguration::BoardValue m_cryoLNAWin;
    double m_vacuumDefault;
    bool m_calDiode;


private:

    maci::ContainerServices* m_services;
    Receivers::LocalOscillator_var m_localOscillatorDevice;
    bool m_localOscillatorFault;
    double m_localOscillatorValue;
    IRA::ReceiverControl::FetValues m_fetValues;
    DWORD m_statusWord;
    // m_ioMarkError is a flag used to know if we already got an IO
    // error. See mantis issue n.0000236
    bool m_ioMarkError;    
    
    Management::TSystemStatus m_componentStatus;

    void setComponentStatus(const Management::TSystemStatus& status) { if (status>m_componentStatus) m_componentStatus=status;  }

    double linearFit(double *X,double *Y,const WORD& size,double x);
};


#endif
