#ifndef _RECVBOSSCORE_H_
#define _RECVBOSSCORE_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  21/07/2010      Creation                                                  */
/* Andrea Orlati(aorlati@ira.inaf.it)  28/02/2011      For computation is now considered the real band, given by the receiver band and the backend filter */
/* Andrea Orlati(aorlati@ira.inaf.it)  8/04/2013       changes to fit the new K band, dual feed in Medicina  */
/* Andrea Orlati(aorlati@ira.inaf.it)  18/06/2013     changes in order to make the length of sequence properties equal to IFs*feeds */
/* Marco Buttu (mbuttu@oa-cagliari.inaf.it) 25/05/2015 Added external mark control  */

#ifdef COMPILE_TARGET_MED

#include <ManagmentDefinitionsC.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <ReceiversErrors.h>
#include <ReceiversBossS.h>
#include <ReceiversDefinitionsS.h>
#include <IRA>
#include <acsncSimpleSupplier.h>
#include "Configuration.h"
#define _RECVBOSSCORE_MAX_IFS 4

#elif COMPILE_TARGET_NT

#include <ManagmentDefinitionsC.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <ReceiversErrors.h>
#include <ReceiversBossS.h>
#include <ReceiversDefinitionsS.h>
#include <IRA>
//only noto implementation needs a reference to the TotalPower backend component
//that is used for calibration mark control
#include <BackendsProxy.h>
#include <acsncSimpleSupplier.h>
#include "Configuration.h"
#define _RECVBOSSCORE_MAX_IFS 4

#else

#include <ManagmentDefinitionsC.h>
#include <GenericReceiverS.h>
#include <DewarPositionerC.h>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <ReceiversErrors.h>
#include <IRA>
#include "Configuration.h"
#include <ReceiversBossS.h>
#include <acsncSimpleSupplier.h>

#endif

class CRecvBossCore {
public:
	/**
	 * Constructor. Default Constructor.
	 * @param service pointer to the container services.
	 * @param conf pointer to the class CConfiguration tat conatains all the configuration regarding the component
	 * @param me pointer to the component itself
	*/
	CRecvBossCore();

	/** 
	 * Destructor.
	*/ 
	virtual ~CRecvBossCore();
	
	/**
	 * This function starts the boss core  so that it will available to accept operations and requests.
	 * @throw ComponentErrors::IRALibraryResourceExImpl
	 * @throw ComponentErrors::CDBAccessExImpl
	*/
	virtual void execute() throw (ComponentErrors::IRALibraryResourceExImpl,ComponentErrors::CDBAccessExImpl);
	
	/**
	 * This function initializes the boss core, all preliminary operation are performed here.
	*/
	virtual void initialize(maci::ContainerServices* services,CConfiguration *config,acscomponent::ACSComponentImpl *me) throw (ComponentErrors::UnexpectedExImpl);
	
	/** 
	 * This function performs all the clean up operation required to free al the resources allocated by the class
	*/
	virtual void cleanUp();
	
	void calOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
			ComponentErrors::UnexpectedExImpl);
	
	void calOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
			ComponentErrors::UnexpectedExImpl);
	
	void externalCalOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
			ComponentErrors::UnexpectedExImpl);
	
	void externalCalOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
			ComponentErrors::UnexpectedExImpl);
	

	
	void setLO(const ACS::doubleSeq& lo) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::SocketErrorExImpl,ComponentErrors::CORBAProblemExImpl,
			ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl);
	
	void AUOn() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
					ComponentErrors::UnexpectedExImpl);

	void AUOff() throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
					ComponentErrors::UnexpectedExImpl);

	/**
	 * It allows to setup the starting position for the derotator
	 * @param pos initial position in degrees
	 * @throw ReceiversErrors::NoDewarPositioningExImpl
	 * @throw ReceiversErrors::NoDerotatorAvailableExImpl
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ReceiversErrors::DewarPositionerCommandErrorExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ReceiversErrors::DewarPositionerNotConfiguredExImpl
	 */
	void setDerotatorPosition(const double& pos) throw (ReceiversErrors::NoDewarPositioningExImpl,
			ReceiversErrors::NoDerotatorAvailableExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
			ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
			ReceiversErrors::DewarPositionerNotConfiguredExImpl);

	/**
	 * Used to read the position of the derotator at a given epoch
	 * @param epoch reference time
	 * @return the position of the derotator (degrees)
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ReceiversErrors::DewarPositionerCommandErrorExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 */
	double getDerotatorPosition (const ACS::Time& epoch) throw (ComponentErrors::CouldntGetComponentExImpl,
			ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

	/**
	 * This method is a wrap a call to the derotator setup, allowing to activate the derotation.
	 * If the derotator is not supported by the current receiver an error is risen.
	 * @param confe specified the tracking mode of the derotator, if <i>RCV_UNDEF_DERATOCONF</i> is given the present value is kept
	 * @param rewinding specifies the rewind mode in case the derotator reaches its final limit, if i<i>RCV_UNDEF_REWIND</i> is given, the present value is kept
	 * @param feeds number of feeds to be rewind in case the automatic rewind is set, if -1 current value is kept, default value is 1;
	 * @throw ReceiversErrors::NoDewarPositioningExImpl
	 * @throw ReceiversErrors::NoDerotatorAvailableExImpl
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ReceiversErrors::DewarPositionerSetupErrorExImpl
	 */
    /*void derotatorMode(const Receivers::TDerotatorConfigurations& mode,const Receivers::TRewindModes& rewinding,const long& feeds) throw (
    		ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,ComponentErrors::ValidationErrorExImpl,
    		ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,
    		ReceiversErrors::DewarPositionerSetupErrorExImpl);*/

    /**
     * This method is a wrap a call to the derotator setup, allowing to prepare the derotation. If the derotator is not supported by
     * the current receiver an error is risen.
	 * @param conf specified the tracking mode of the derotator, if <i>RCV_UNDEF_DERATOCONF</i> is given the present value is kept
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw ReceiversErrors::NoDewarPositioningExImpl
	 * @throw ReceiversErrors::NoDerotatorAvailableExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ReceiversErrors::DewarPositionerSetupErrorExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 */
    void derotatorSetConfiguration(const Receivers::TDerotatorConfigurations& conf) throw (
		ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
		ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
		ComponentErrors::UnexpectedExImpl);

    /**
     * This method allows to change the configuration of the dewar positioner when a rewinding is needed.
     * @param rewind new rewinding mode
	 * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw ReceiversErrors::NoDewarPositioningExImpl
	 * @throw ReceiversErrors::NoDerotatorAvailableExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ReceiversErrors::DewarPositionerSetupErrorExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ReceiversErrors::DewarPositionerNotConfiguredExImpl
     */
    void derotatorSetRewindingMode(const Receivers::TRewindModes& rewind) throw (
    		ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
    		ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
    		ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl);

    /**
     * This method specifies how many steps to rewind when the auto rewinding mode is configured (@sa <i>derotatorSetRewindingMode</i>).
     * @param steps number of steps
     * @throw ComponentErrors::ValidationErrorExImpl
	 * @throw ReceiversErrors::NoDewarPositioningExImpl
	 * @throw ReceiversErrors::NoDerotatorAvailableExImpl
	 * @throw ComponentErrors::CouldntGetComponentExImpl
	 * @throw ReceiversErrors::DewarPositionerSetupErrorExImpl
	 * @throw ComponentErrors::CORBAProblemExImpl
	 * @throw ComponentErrors::UnexpectedExImpl
	 * @throw ReceiversErrors::DewarPositionerNotConfiguredExImpl
     */
    void derotatorSetAutoRewindingSteps(const long& steps) throw (
    		ComponentErrors::ValidationErrorExImpl,ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
    		ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerSetupErrorExImpl,ComponentErrors::CORBAProblemExImpl,
    		ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl);

    /**
     * It disables the derotator. It is implicitly called also by the <i>park()</i> method.
     * @throw ReceiversErrors::NoDewarPositioningExImpl
     * @throw ReceiversErrors::NoDerotatorAvailableExImpl
     * @throw ComponentErrors::ValidationErrorExImpl
     * @throw ComponentErrors::CouldntGetComponentExImpl
     * @throw ReceiversErrors::DewarPositionerParkingErrorExImpl
     * @throw ComponentErrors::CORBAProblemExImpl
     * @throw ComponentErrors::UnexpectedExImpl
     */
     void derotatorPark() throw (ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
      ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerParkingErrorExImpl,
      ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

     void derotatorRewind(const long& steps) throw (ComponentErrors::ValidationErrorExImpl,
       ReceiversErrors::NoDewarPositioningExImpl,ReceiversErrors::NoDerotatorAvailableExImpl,
       ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl,
       ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl,ReceiversErrors::DewarPositionerNotConfiguredExImpl);

     /**
      * Return back the present configuration of the dewar positioner
      */
     void getDewarParameter(Receivers::TDerotatorConfigurations& mod,double& pos) throw (
       ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,ComponentErrors::UnexpectedExImpl);

	/**
	 * This method will configure the component that controls the receiver actually identified by the given code
	 * @param code identifier code of the receiver
	 */
	void setupReceiver(const char * code) throw (ManagementErrors::ConfigurationErrorExImpl);

	void setMode(const char * mode) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,ReceiversErrors::ModeErrorExImpl,
			ComponentErrors::UnexpectedExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::CORBAProblemExImpl);
	
	void park()  throw (ManagementErrors::ParkingErrorExImpl);
	
	void startScan(ACS::Time& startUT,const Receivers::TReceiversParameters& param,const Antenna::TRunTimeParameters& antennaInfo) throw(
			ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
			ComponentErrors::UnexpectedExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl);

	void closeScan(ACS::Time& timeToStop) throw (ReceiversErrors::DewarPositionerCommandErrorExImpl,ComponentErrors::CORBAProblemExImpl,
			ComponentErrors::UnexpectedExImpl);

	long getFeeds(ACS::doubleSeq& X,ACS::doubleSeq& Y,ACS::doubleSeq& power) throw (ComponentErrors::ValidationErrorExImpl,
			ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl);
	
	double getTaper(const double& freq,const double& bw,const long& feed,const long& ifNumber,double& waveLen) throw (ComponentErrors::ValidationErrorExImpl,
			ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl);
	
	void getCalibrationMark(ACS::doubleSeq& result,ACS::doubleSeq& resFreq,ACS::doubleSeq&resBw,const ACS::doubleSeq& freqs,const ACS::doubleSeq& bandwidths,const ACS::longSeq& feeds,
			const ACS::longSeq& ifs,bool& onoff,double& scale) throw (ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,
					ReceiversErrors::UnavailableReceiverOperationExImpl,ComponentErrors::UnexpectedExImpl);
	
	void getIFOutput(const ACS::longSeq& feeds,const ACS::longSeq& ifs,ACS::doubleSeq& freqs,ACS::doubleSeq&  bw,ACS::longSeq& pols,ACS::doubleSeq& LO)  throw (
			ComponentErrors::ValidationErrorExImpl,ComponentErrors::ValueOutofRangeExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverOperationExImpl,
			ComponentErrors::UnexpectedExImpl);

	void getLO(ACS::doubleSeq& lo) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl);
	
	void getInitialFrequency(ACS::doubleSeq& iFreq) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl);
	
	void getBandWidth(ACS::doubleSeq& bw) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl);
	
	void getPolarization(ACS::longSeq& pol) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl);
	
	void getFeeds(long& feeds) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl);
	
	void getIFs(long& ifs) throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl);
	
	const IRA::CString& getRecvCode();
	
	const IRA::CString& getOperativeMode() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl);

	const Management::TSystemStatus& getStatus();

	void  updateRecvStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,ReceiversErrors::UnavailableReceiverAttributeExImpl);

	void updateDewarPositionerStatus() throw (ComponentErrors::CouldntGetComponentExImpl,ComponentErrors::CORBAProblemExImpl,
			ComponentErrors::OperationErrorExImpl,ReceiversErrors::DewarPositionerCommandErrorExImpl);

	void publishData() throw (ComponentErrors::NotificationChannelErrorExImpl);
private:

	Management::TSystemStatus m_status;
        maci::ContainerServices *m_services;
	CConfiguration *m_config;
	BACIMutex m_mutex;
	long m_feeds; // numebr of feeds;
	long m_IFs;  //number of IFs per feed
	IRA::CString m_currentOperativeMode;

#ifdef COMPILE_TARGET_MED
        #warning "Compiling RecvBossCore with Medicina target"
	/**
	 * Socket to the KBand Recevier
	 */
	IRA::CSocket m_kBandSocket;
	
	/**
	 * Socket for the Receviers configuration
	 */
	IRA::CSocket m_recvSocket;
	IRA::CSocket m_fsSocket;
	bool m_recvOpened;
	bool m_fsOpened;
	bool m_cal;
	IRA::CDBTable *m_KKCFeedTable;
	double m_LO[_RECVBOSSCORE_MAX_IFS];
	Receivers::TPolarization m_pols[_RECVBOSSCORE_MAX_IFS];
	double m_startFreq[_RECVBOSSCORE_MAX_IFS];
	double m_bandWidth[_RECVBOSSCORE_MAX_IFS];
	IRA::CString m_currentReceiver;
	long m_totalOutputs;

#elif COMPILE_TARGET_NT

        #warning "Compiling RecvBossCore with Noto target"
	void reinit() throw (ComponentErrors::IRALibraryResourceExImpl);
	void reinitCal() throw (ComponentErrors::IRALibraryResourceExImpl);

	IRA::CSocket m_fsSocket;
	//bool m_fsOpened;
	IRA::CSocket m_fsSocketCal;
	//bool m_fsCalOpened;
	bool m_fsSocketError;
	bool m_fsCalSocketError;
	double m_LO[_RECVBOSSCORE_MAX_IFS];
	Receivers::TPolarization m_pols[_RECVBOSSCORE_MAX_IFS];
	double m_startFreq[_RECVBOSSCORE_MAX_IFS];
	double m_bandWidth[_RECVBOSSCORE_MAX_IFS];
	IRA::CString m_currentReceiver;
	long m_totalOutputs;
	bool m_cal;
   Backends::TotalPower_proxy m_totalPower_proxy;
	
#else
        #warning "Compiling RecvBossCore with default target"
	Receivers::Receiver_var m_currentRecv;
	bool m_currentRecvError;
	IRA::CString m_currentRecvInstance;
	IRA::CString m_currentRecvCode;
	Management::TSystemStatus m_bossStatus;
	ACS::Time  m_lastStatusChange;
	ACS::doubleSeq m_lo;
	ACS::Time m_loEpoch;
	ACS::doubleSeq m_startFreq;
	ACS::Time m_starFreqEpoch;
	ACS::doubleSeq m_bandWidth;
	ACS::Time m_bandWidthEpoch;
	ACS::longSeq m_pol;
	ACS::Time m_polEpoch;
	ACS::Time m_feedsEpoch;
	ACS::Time m_IFsEpoch;
	ACS::Time m_modeEpoch;
	Management::TSystemStatus m_recvStatus;
	ACS::Time m_recvStatusEpoch;

	Management::TSystemStatus m_dewarStatus;
	bool m_dewarTracking;
	ACS::Time m_dewarStatusEpoch;

	// This also controls if the derotator has been configured
	Receivers::TDerotatorConfigurations m_updateMode;
	Receivers::TRewindModes m_rewindMode;
	long m_autoRewindSteps;
	bool m_dewarIsMoving;

	Receivers::DewarPositioner_var m_dewarPositioner;
	bool m_dewarPositionerError;



	void loadReceiver() throw (ComponentErrors::CouldntGetComponentExImpl);
	void unloadReceiver();
	void loadDewarPositioner()  throw (ComponentErrors::CouldntGetComponentExImpl);
	void unloadDewarPositioner();


	void changeBossStatus(const Management::TSystemStatus& status);

#endif

	/** This is the pointer to the notification channel */
	nc::SimpleSupplier *m_notificationChannel;

#ifdef COMPILE_TARGET_MED
	void setup(const char * code) throw(ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl);

#elif COMPILE_TARGET_NT
	void setup(const char * code) throw(ComponentErrors::SocketErrorExImpl,ComponentErrors::ValidationErrorExImpl);

#else
	void setup(const char * code) throw (ComponentErrors::CORBAProblemExImpl,ComponentErrors::ValidationErrorExImpl,ComponentErrors::CouldntGetComponentExImpl,
			ComponentErrors::UnexpectedExImpl,ComponentErrors::OperationErrorExImpl);
#endif

};

#endif
