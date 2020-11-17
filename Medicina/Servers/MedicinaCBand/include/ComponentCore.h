#ifndef _COMPONENTCORE_H_
#define _COMPONENTCORE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public License (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 03/08/2011     Creation                                         */

#include "Configuration.h"
#include <ReceiverControl.h>
#include <LocalOscillatorInterfaceC.h>
#include <ReceiversErrors.h>
#include <ManagmentDefinitionsC.h>
#include "Commons.h"
#include "MixerOperator.h"

/**
 * This class contains the code of almost all the features  of the component
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
  */
class CComponentCore {

	private:

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

public:
    /**
     * Constructor
     */
    CComponentCore();

    /**
     * Destructor
     */
    virtual ~CComponentCore();

        /* *** COMPONENT SERVICE *** */

    /**
     * This method initializes the object
     * @param service pointer to container services object provided by the container
     */
    virtual void initialize(maci::ContainerServices* services);

    /**
     * This method prepares the object for execution.
     * @return the pointer to the configuration class
     */
    virtual CConfiguration const * const execute() throw (ComponentErrors::CDBAccessExImpl,ComponentErrors::MemoryAllocationExImpl,ComponentErrors::SocketErrorExImpl);

    /**
     * This function is responsible to free all allocated resources
     */
    virtual void cleanup();

    /**
     * It activate the receiver, in other words it allows to setup the default configuration and to make sure the LNA are turned on and selects the appropriate
     * C receiver ( LOW - HIGH ).
     * @details Throw an exception if mode is not valid or it got some init errors.
     */
    void activate(const char *mode) throw (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl,ReceiversErrors::NoRemoteControlErrorExImpl,
            ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * It deactivates the receiver.
     */
    void deactivate() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);
    
    /**
     * This method resumes the whole status of the component. It set the <i>componentStatus</i> member variable.
     */
    void updateComponent();
   
    /**
     * This is getter method. No need to make it thread safe......
     * @return the current status word
     */
    DWORD getStatusWord() const  { return  m_statusWord; }

     /**
     * @return the status flag of the component
     */
    const Management::TSystemStatus& getComponentStatus();


        /* *** MODE *** */

     /**
     * It allows to change the operating mode of the receiver. If the mode does not correspond to a valid mode an error is thrown.
     * @param  mode mode code as a string
     */
    void setMode(const char * mode) throw  (ReceiversErrors::ModeErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,
            ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl);

    /**
     * It returns the current operating mode of the receiver.
     * @return output string
     */
    const IRA::CString getSetupMode();


        /* *** LOs *** */

    /**
     * It sets the local oscillator. System has 2 synth. Before commanding the new value some check are done. The the correspnding signal amplitude is computed.
     * @param lo lists of values for the local oscillator (MHz), one for each IF. 2 Values are processed. If a -1 is passed the present value is kept
     * @throw  ComponentErrors::ValidationErrorExImpl
     * @throw ComponentErrors::ValueOutofRangeExImpl
     * @throw ComponentErrors::CouldntGetComponentExImpl
     * @throw ComponentErrors::CORBAProblemExImpl
     * @thorw ReceiversErrors::LocalOscillatorErrorExImpl
     */
    void setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CouldntGetComponentExImpl,
            ComponentErrors::CORBAProblemExImpl,ReceiversErrors::LocalOscillatorErrorExImpl);
    /**
     * It returns back the current local oscillator frequency settings.
     * @param lo output sequence
     */
    void getLO(ACS::doubleSeq& lo);

    /**
     * I checks if the local oscillator is locked properly
     */
    void checkLocalOscillator() throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::CouldntGetAttributeExImpl);

        /* *** IFs *** */

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
     * It returns the feed geometry of the receiver with respect to the central one. For this implementation it is just a placeholder since there is just one feed.
     */
    long getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power);


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
     * It returns the number of IF chains
     * @return output value
     */
    const DWORD getIFs();

    /**
     * It returns the number of feeds
     * @return output value
     */
    const DWORD getFeeds();

        /* *** TAPER *** */

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


        /* *** CAL MARK *** */

    /**
     * It allows to compute the value of the calibration mark for any given sub bands in the IF space.
     * @param result this the sequence of computed mark values, the first entry correspond to first sub band and so on....
     * @param, resFreq the sequence reports the initial observed sky frequency (MHz), the  first entry correspond to first sub band and so on....
     * @param resBw the sequence reports the real bandwidth observed (MHz), the  first entry correspond to first sub band and so on....
     * @param freqs  list of start frequencies (MHz)
     * @param bandwidth list of the band widths (MHz)
     * @param feeds list of feed identifier, it allows to specifies form which feed the sub band comes from. In that case it is neglected since the receiver is a single feed
     * @param ifs list of IF identifier, it allows to specifies from which receiver IF the sub band comes from.
     * @param true if the calibration diode is turned on.
     */
    void getCalibrationMark(ACS::doubleSeq& result,ACS::doubleSeq& resFreq,ACS::doubleSeq& resBw,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,
            const ACS::longSeq& ifs,bool& onoff,double& scaleFactor) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl);

    /**
     * It turns the calibration diode on.
     */
    void calOn() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ComponentErrors::ValidationErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * It turns the calibration diode off
     */
    void calOff() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ComponentErrors::ValidationErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);

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

    /**
     * It checks is the status of the noise mark correspond to the commanded status, otherwise it sets the <i>NOISEMARKERROR</i> bit. It also check if the
     * external control of the noise mark has been enabled or not
     */
    void updateNoiseMark() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


        /* *** LNAs *** */

    /**
     * It allows to turn LNA on
     */
    void lnaOn() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * It allows to turn LNA off
     */
    void lnaOff() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * It reads and updates from the LNA control board the current values of current and voltage of gate and drain of the transistors
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateLNAControls() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


        /* *** VACUUM *** */

     /**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the vacuum in mbar
     */
    double  getVacuum() const { return m_vacuum; }

    /**
     * It reads and updates from the control board the current value of the vacuum
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateVacuum() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

 	 /**
 	 * @brief Turn on vacuum pump
 	 */	
	 void vacuumPumpOn() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

	 /**
 	 * @brief Turn off vacuum pump
 	 */	
	 void vacuumPumpOff() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);
	 
    /**
     * It check if the vacuum pump is on and check is the status is fault or not (<i>VACUUMPUMPFAULT</i>)
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateVacuumPump() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);


    /**
     * It checks if the vacuum valve is opened or not
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateVacuumValve() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

	/**
 	 * @brief Turn on vacuum valve with delay
 	 * @todo check how it works
 	 */	
	 void openVacuumValve(const char * p_mode, double p_delay) throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);
	 
	 /**
	 * @brief Close vaccum valve
	 */
	 void closeVacuumValve() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * Allows to set the "default_value" for the vacuum characteristic. In principle it is possible to read it directly from CDB, but I found it more
     * comfortable to get it directly from the characteristic itself.
     */
    inline void setVacuumDefault(const double& val) { m_vacuumDefault=val; }
       
     /**
     * It turns on the sensor for vacuum measurement.
     */
    void vacuumSensorOn() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);
    
    /**
     * It turns off the sensor for vacuum measurement.
     */
    void vacuumSensorOff() throw (ReceiversErrors::NoRemoteControlErrorExImpl,ReceiversErrors::ReceiverControlBoardErrorExImpl);

        /* *** TEMPERATURE *** */        
   
     /**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the vertex temperature
     */
    BoardValue getEnvironmentTemperature() const { return m_environmentTemperature; }

    /**
     * It reads and updates from the control board the current vertex temperature
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateEnvironmentTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the vertex temperature
     */
	BoardValue getShieldTemperature() const { return m_shieldTemperature; }

	 /**
     * It reads and updates from the control board the current shield temperature
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateShieldTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

/**
     * This is getter method. No need to make it thread safe......
     * @return the current value of the vertex temperature
     */
	BoardValue getLnaTemperature() const { return m_lnaTemperature; }

	 /**
     * It reads and updates from the control board the current shield temperature
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateLnaTemperature() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

	/*
	* @brief Cold head on
	*/
	void coldHeadOn() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);
	 
	 /**
	 *@brief Cold head off
	 */
	 void coldHeadOff() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

     /**
     * It checks if the cool head is turned on or not
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateColdHead() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

        /* *** LOCAL REMOTE *** */

    /**
     * It checks if the Dewar power box is in remote or not
     * @throw ReceiversErrors::ReceiverControlBoardErrorExImpl
     */
    void updateIsRemote() throw (ReceiversErrors::ReceiverControlBoardErrorExImpl);

    /**
     * This is getter method. In this case, since it makes use of some class members that could be changed by other methods it is advisable to protect this method with the class mutex.
     * @param control name of the parameter that must be returned
     * @param ifs Intermediate frequency identifier, it permits to select which amplification chain we are interested in
     * @return a specific value of from the transistor control parameters
     */
    double getFetValue(const IRA::ReceiverControl::FetValue& control,const DWORD& ifs);


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


    void setComponentStatus(const Management::TSystemStatus& status) { if (status>m_componentStatus) m_componentStatus=status;  }

    /**
     * This function will set the a status bit. It may be considered thread safe due to its definition
     */
    inline void setStatusBit(TStatusBit bit) { m_statusWord |= 1 << bit; }

    /**
     * This function will unset (clear) a status bit. It may be considered thread safe due to its definition
     */
    inline void clearStatusBit(TStatusBit  bit) { m_statusWord &= ~(1 << bit); }

    /**
     * This function check is a bit is set or not. It may be considered thread safe due to its definition
     */
    inline bool checkStatusBit(TStatusBit bit) { return m_statusWord & (1 << bit); }

private:

    /**@brief LO device and data collector */        

    CConfiguration m_configuration;
    maci::ContainerServices* m_services;
    BACIMutex m_mutex;

    IRA::ReceiverControl *m_control; // this object is thread safe        
    ACS::doubleSeq m_startFreq;
    ACS::doubleSeq m_bandwidth;
    ACS::longSeq m_polarization;
    ACS::doubleSeq m_localOscillatorValue;  /**< Frequency Value */
    IRA::CString m_setupMode;   /**< Setup mode string */

    MixerOperator m_mixer;  /**< LOs manager */
    
    bool m_calDiode;

    double m_vacuum;
    double m_vacuumDefault;
    
    BoardValue m_environmentTemperature;
    BoardValue m_shieldTemperature;
    BoardValue m_lnaTemperature;
        
    IRA::ReceiverControl::FetValues m_fetValues;
    
    DWORD m_statusWord;    
    bool m_ioMarkError; /**< m_ioMarkError is a flag used to know if we already got an IO error. See mantis issue n.0000236 */
    Management::TSystemStatus m_componentStatus;

};


#endif
